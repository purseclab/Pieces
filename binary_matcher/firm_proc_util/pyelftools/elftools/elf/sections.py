#-------------------------------------------------------------------------------
# elftools: elf/sections.py
#
# ELF sections
#
# Eli Bendersky (eliben@gmail.com)
# This code is in the public domain
#-------------------------------------------------------------------------------
from ..common.utils import struct_parse, elf_assert, parse_cstring_from_stream
from collections import defaultdict
from .notes import iter_notes


class Section(object):
    """ Base class for ELF sections. Also used for all sections types that have
        no special functionality.

        Allows dictionary-like access to the section header. For example:
         > sec = Section(...)
         > sec['sh_type']  # section type
    """
    def __init__(self, header, name, stream):
        self.header = header
        self.name = name
        self.stream = stream
        self._data_update = None

    def __copy__(self):
        newone = type(self)(self.header.copy(), self.name, self.stream)
        newone._data_update = self._data_update
        return newone

    def data(self):
        """ The section data, either from the stream, or updated.
        """
        if self._data_update is not None:
            return self._data_update
        if self['sh_type'] == 'SHT_NOBITS':
            return b''
        self.stream.seek(self['sh_offset'])
        return self.stream.read(self['sh_size'])

    def set_data(self, data_buf):
        """ Set the updated section data.
        """
        self._data_update = data_buf
        self.header['sh_size'] = len(data_buf)

    def write_data(self):
        """ Writes the updated section data back to stream.
        """
        if self._data_update is not None:
            self.stream.seek(self['sh_offset'])
            return self.stream.write(self._data_update[:self['sh_size']])

    def is_data_modified(self):
        return self._data_update is not None

    def is_null(self):
        """ Is this a null section?
        """
        return False

    def __getitem__(self, name):
        """ Implement dict-like access to header entries
        """
        return self.header[name]

    def __eq__(self, other):
        return self.header == other.header
    def __hash__(self):
        return hash(self.header)


class NullSection(Section):
    """ ELF NULL section
    """
    def __init__(self, header, name, stream):
        super(NullSection, self).__init__(header, name, stream)

    def is_null(self):
        return True


class StringTableSection(Section):
    """ ELF string table section.
    """
    def __init__(self, header, name, stream):
        super(StringTableSection, self).__init__(header, name, stream)

    def get_string(self, offset):
        """ Get the string stored at the given offset in this string table.
        """
        if self._data_update is not None:
            s = parse_cstring_from_stream(io.BytesIO(self._data_update), offset)
        else:
            table_offset = self['sh_offset']
            s = parse_cstring_from_stream(self.stream, table_offset + offset)
        return s.decode('ascii')

    def add_string(self, s):
        """ Add the string at end of this string table.
        """
        data_buf = self.data()
        offset = len(data_buf)-1
        while data_buf[offset] == 0:
            offset -= 1
        if data_buf[offset] != 0:
            offset += 1
        data_buf = data_buf[0:offset]
        data_buf += b'\0' + s.encode('ascii') + b'\0'
        offset += 1
        if self.header['sh_size'] > len(data_buf):
            data_buf += b'\0' * (self.header['sh_size'] - len(data_buf))
        self.set_data(data_buf)
        return offset


class SymbolTableSection(Section):
    """ ELF symbol table section. Has an associated StringTableSection that's
        passed in the constructor.
    """
    def __init__(self, header, name, stream, elffile, stringtable):
        super(SymbolTableSection, self).__init__(header, name, stream)
        self.elffile = elffile
        self.elfstructs = self.elffile.structs
        self.stringtable = stringtable
        elf_assert(self['sh_entsize'] > 0,
                'Expected entry size of section %r to be > 0' % name)
        elf_assert(self['sh_size'] % self['sh_entsize'] == 0,
                'Expected section size to be a multiple of entry size in section %r' % name)
        self._symbol_name_map = None

    def num_symbols(self):
        """ Number of symbols in the table
        """
        return self['sh_size'] // self['sh_entsize']

    def get_symbol(self, n):
        """ Get the symbol at index #n from the table (Symbol object)
        """
        # Grab the symbol's entry from the stream
        if self._data_update is not None:
            entry_offset = n * self['sh_entsize']
            input_strm = io.BytesIO(self._data_update)
        else:
            entry_offset = self['sh_offset'] + n * self['sh_entsize']
            input_strm = self.stream
        entry = struct_parse(
            self.elfstructs.Elf_Sym,
            input_strm,
            stream_pos=entry_offset)
        # Find the symbol name in the associated string table
        name = self.stringtable.get_string(entry['st_name'])
        return Symbol(entry, name)

    def get_symbol_by_name(self, name):
        """ Get a symbol(s) by name. Return None if no symbol by the given name
            exists.
        """
        # The first time this method is called, construct a name to number
        # mapping
        #
        if self._symbol_name_map is None:
            self._symbol_name_map = defaultdict(list)
            for i, sym in enumerate(self.iter_symbols()):
                self._symbol_name_map[sym.name].append(i)
        symnums = self._symbol_name_map.get(name)
        return [self.get_symbol(i) for i in symnums] if symnums else None

    def iter_symbols(self):
        """ Yield all the symbols in the table
        """
        for i in range(self.num_symbols()):
            yield self.get_symbol(i)


class Symbol(object):
    """ Symbol object - representing a single symbol entry from a symbol table
        section.

        Similarly to Section objects, allows dictionary-like access to the
        symbol entry.
    """
    def __init__(self, entry, name):
        self.entry = entry
        self.name = name

    def __getitem__(self, name):
        """ Implement dict-like access to entries
        """
        return self.entry[name]


class SUNWSyminfoTableSection(Section):
    """ ELF .SUNW Syminfo table section.
        Has an associated SymbolTableSection that's passed in the constructor.
    """
    def __init__(self, header, name, stream, elffile, symboltable):
        super(SUNWSyminfoTableSection, self).__init__(header, name, stream)
        self.elffile = elffile
        self.elfstructs = self.elffile.structs
        self.symboltable = symboltable

    def num_symbols(self):
        """ Number of symbols in the table
        """
        return self['sh_size'] // self['sh_entsize'] - 1

    def get_symbol(self, n):
        """ Get the symbol at index #n from the table (Symbol object).
            It begins at 1 and not 0 since the first entry is used to
            store the current version of the syminfo table.
        """
        # Grab the symbol's entry from the stream
        if self._data_update is not None:
            entry_offset = n * self['sh_entsize']
            input_strm = io.BytesIO(self._data_update)
        else:
            entry_offset = self['sh_offset'] + n * self['sh_entsize']
            input_strm = self.stream
        entry = struct_parse(
            self.elfstructs.Elf_Sunw_Syminfo,
            input_strm,
            stream_pos=entry_offset)
        # Find the symbol name in the associated symbol table
        name = self.symboltable.get_symbol(n).name
        return Symbol(entry, name)

    def iter_symbols(self):
        """ Yield all the symbols in the table
        """
        for i in range(1, self.num_symbols() + 1):
            yield self.get_symbol(i)


class NoteSection(Section):
    """ ELF NOTE section. Knows how to parse notes.
    """
    def __init__(self, header, name, stream, elffile):
        super(NoteSection, self).__init__(header, name, stream)
        self.elffile = elffile

    def iter_notes(self):
        """ Yield all the notes in the section.  Each result is a dictionary-
            like object with "n_name", "n_type", and "n_desc" fields, amongst
            others.
        """
        return iter_notes(self.elffile, self['sh_offset'], self['sh_size'])
