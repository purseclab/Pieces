#-------------------------------------------------------------------------------
# elftools: elf/elffile.py
#
# ELFFile - main class for accessing ELF files
#
# Eli Bendersky (eliben@gmail.com)
# This code is in the public domain
#-------------------------------------------------------------------------------
import io
import struct
import copy
import zlib

try:
    import resource
    PAGESIZE = resource.getpagesize()
except ImportError:
    # Windows system
    import mmap
    PAGESIZE = mmap.PAGESIZE

from ..common.py3compat import BytesIO
from ..common.exceptions import ELFError
from ..common.utils import struct_parse, struct_write, elf_assert
from .structs import ELFStructs
from .sections import (
        Section, StringTableSection, SymbolTableSection,
        SUNWSyminfoTableSection, NullSection, NoteSection)
from .dynamic import DynamicSection, DynamicSegment
from .relocation import RelocationSection, RelocationHandler
from .gnuversions import (
        GNUVerNeedSection, GNUVerDefSection,
        GNUVerSymSection)
from .segments import Segment, InterpSegment, NoteSegment
from ..dwarf.dwarfinfo import DWARFInfo, DebugSectionDescriptor, DwarfConfig
from .constants import SH_FLAGS


class ELFFile(object):
    """ Creation: the constructor accepts a stream (file-like object) with the
        contents of an ELF file.

        Accessible attributes:

            stream:
                The stream holding the data of the file - must be a binary
                stream (bytes, not string).

            elfclass:
                32 or 64 - specifies the word size of the target machine

            little_endian:
                boolean - specifies the target machine's endianness

            header:
                the complete ELF file header

            e_ident_raw:
                the raw e_ident field of the header
    """
    def __init__(self, stream):
        self.stream = stream
        self._identify_file()
        self.structs = ELFStructs(
            little_endian=self.little_endian,
            elfclass=self.elfclass)
        self.header = self._parse_elf_header()

        self.stream.seek(0)
        self.e_ident_raw = self.stream.read(16)

        self._section_update = {}
        self._segment_update = {}
        self._file_stringtable_section = self._get_file_stringtable()
        self._section_name_map = None

        if self._file_stringtable_section is not None:
            # We could not set stringtable section name without stringtable section
            # Now when the chicken-and-egg issue is solved, we should set it properly
            self._file_stringtable_section.name = self._get_section_name(self._file_stringtable_section.header)

    def write_changes(self):
        """ Writes any modifications in headers
        """
        self._write_elf_header()
        for n, obj in self._section_update.items():
            self._write_section_header(n, obj.header)
        for n, obj in self._section_update.items():
            if (obj['sh_type'] == 'SHT_NOBITS' or
              obj['sh_type'] == 'SHT_NULL'):
                continue
            obj.write_data()
        for n, seg in self._segment_update.items():
            self._write_segment_header(n, seg.header)

    def num_sections(self):
        """ Number of sections in the file
        """
        return self['e_shnum']

    def get_section(self, n):
        """ Get the section at index #n from the file (Section object or a
            subclass)
        """
        if n == self['e_shstrndx']:
            return copy.copy(self._file_stringtable_section)
        if n in self._section_update:
            return copy.copy(self._section_update[n])
        section_header = self._get_section_header(n)
        return self._make_section(section_header)

    def set_section(self, n, obj):
        # Map sections to segments
        sect_to_seg = self.prepare_section_to_segment_mapping()
        # Check if it's a new section or update
        if n >= self['e_shnum']:
            raise ELFError('Adding new sections this way is not implemented')
        # Replace section definition
        old_obj = self.get_section(n)
        self._update_section_header(n, obj)
        if old_obj.name != obj.name:
            self._set_section_name(obj.header, obj.name)
            self._section_name_map = None
        if obj['sh_size'] != old_obj['sh_size']:
            # if section data length changed, prepare offsets to make sure they wont overlap
            self.linearize_sections()
        if obj['sh_size'] != old_obj['sh_size'] or obj['sh_addr'] != old_obj['sh_addr']:
            self.update_segments_to_sections_mapping(sect_to_seg)
            self._reorder_segment_headers()

    def insert_section(self, n, obj):
        # Map sections to segments
        sect_to_seg = self.prepare_section_to_segment_mapping()
        # Check if it's a new section or update
        if n > self['e_shnum']:
            raise ELFError('Adding new sections beyond existing ones is not implemented')
        # Make sure headers of all sections beyond the one added are reordered and stored in memory
        for i in range(self['e_shnum'], n, -1):
            nx_obj = self.get_section(i-1)
            # Change strings section index before update, to make
            # sure _file_stringtable_section gets updated too
            if i-1 == self['e_shstrndx']:
               self.header['e_shstrndx'] = i
            self._update_section_header(i, nx_obj)
        self.header['e_shnum'] += 1
        # Insert section definition
        self._set_section_name(obj.header, obj.name)
        self._section_name_map = None
        self._update_section_header(n, obj)
        if n+1 != self['e_shnum']:
            # if section data was added in the middle, prepare offsets to make sure they wont overlap
            self.linearize_sections()
        if n > 0:
            sect_to_seg.insert(n, sect_to_seg[n-1])
        else:
            sect_to_seg.insert(n, sect_to_seg[n])
        self.update_segments_to_sections_mapping(sect_to_seg)

    def get_section_by_name(self, name):
        """ Get a section from the file, by name. Return None if no such
            section exists.
        """
        # The first time this method is called, construct a name to number
        # mapping
        #
        secnum = self._get_section_number_by_name(name)
        return None if secnum is None else self.get_section(secnum)

    def set_section_by_name(self, name, obj):
        secnum = self._get_section_number_by_name(name)
        self.set_section(secnum, obj)

    def insert_section_at_end(self, obj):
        secnum = self['e_shnum']
        self.insert_section(secnum, obj)

    def insert_section_after(self, name, obj):
        secnum = self._get_section_number_by_name(name)
        self.insert_section(secnum+1, obj)

    def iter_sections(self):
        """ Yield all the sections in the file
        """
        for i in range(self.num_sections()):
            if i in self._section_update:
                yield self._section_update[i]
            else:
                yield self.get_section(i)

    def num_segments(self):
        """ Number of segments in the file
        """
        return self['e_phnum']

    def get_segment(self, n):
        """ Get the segment at index #n from the file (Segment object)
        """
        if n in self._segment_update:
            return copy.copy(self._segment_update[n])
        segment_header = self._get_segment_header(n)
        return self._make_segment(segment_header)

    def iter_segments(self):
        """ Yield all the segments in the file
        """
        for i in range(self.num_segments()):
            if i in self._segment_update:
                yield self._segment_update[i]
            else:
                yield self.get_segment(i)

    def address_offsets(self, start, size=1):
        """ Yield a file offset for each ELF segment containing a memory region.

            A memory region is defined by the range [start...start+size). The
            offset of the region is yielded.
        """
        end = start + size
        for seg in self.iter_segments():
            if (start >= seg['p_vaddr'] and
                end <= seg['p_vaddr'] + seg['p_filesz']):
                yield start - seg['p_vaddr'] + seg['p_offset']

    def segments_are_equivalent(self, seg1, seg2):
            return (seg1['p_type'] == seg2['p_type'] and
              seg1['p_flags'] == seg2['p_flags'] and
              seg1['p_align'] == seg2['p_align'])

    def get_segment_index_equivalent_touching_or_overlaping_address_range(self, equiv_seg, start, size=1):
        for i in range(self.num_segments()):
            seg = self.get_segment(i)
            # check if seg is equivalent
            if (not self.segments_are_equivalent(seg, equiv_seg)):
                continue
            # check if seg is within given address range
            if (start >= seg['p_vaddr'] and
              start + size <= seg['p_vaddr'] + seg['p_memsz']):
                return i
            # check if seg start touches/overlaps given address range
            if (start < seg['p_vaddr'] and
              start + size >= seg['p_vaddr']):
                return i
            # check if seg end touches/overlaps given address range
            if (start <= seg['p_vaddr'] + seg['p_memsz'] and
              start + size > seg['p_vaddr'] + seg['p_memsz']):
                return i
        return -1

    def linearize_sections(self):
        # Prepare an array of all sections, sorted by offset
        all_sections = []
        for i in range(0, self['e_shnum']):
            obj = self.get_section(i)
            all_sections.append({'i' : i, 'obj' : obj, 'seg' : -1})
        # Treat main header as a special section
        if 1:
            section_header = {'sh_offset' : 0, 'sh_size' : self['e_ehsize'], 'sh_addralign' : 0x20, 'sh_type' : 'SHT_PROGBITS'}
            obj = Section(section_header, "main_header_dummy_container", None)
            all_sections.append({'i' : -1, 'obj' : obj, 'seg' : -1})
        # Treat program header table as a special section
        if self['e_phoff'] > 0:
            section_header = {'sh_offset' : self['e_phoff'], 'sh_size' : self['e_phnum'] * self['e_phentsize'], 'sh_addralign' : 0x20, 'sh_type' : 'SHT_PROGBITS'}
            # Allocate space for additional entries, just in case we will decide to divide a segment
            if (self['e_phnum'] < self['e_shnum']): section_header['sh_size'] = self['e_shnum'] * self['e_phentsize']
            obj = Section(section_header, "program_headers_dummy_container", None)
            all_sections.append({'i' : -2, 'obj' : obj, 'seg' : -1})
        # Treat section header table as a special section
        if self['e_shoff'] > 0:
            section_header = {'sh_offset' : self['e_shoff'], 'sh_size' : self['e_shnum'] * self['e_shentsize'], 'sh_addralign' : 0x20, 'sh_type' : 'SHT_PROGBITS'}
            obj = Section(section_header, "section_headers_dummy_container", None)
            all_sections.append({'i' : -3, 'obj' : obj, 'seg' : -1})
        # Sort by offset
        all_sections = sorted(all_sections, key=lambda nx: (nx['obj']['sh_offset'],nx['obj']['sh_size']))
        # Now go through the sections and update them to make sure they don't overlap in file, and go closely after each other
        pv_end_offs = 0
        for nx in all_sections:
            i = nx['i']
            obj = nx['obj']
            if obj['sh_offset'] != pv_end_offs:
                if ((not obj.is_data_modified()) and (i >= 0) and
                  (obj['sh_type'] != 'SHT_NOBITS') and (obj['sh_type'] != 'SHT_NULL')):
                    obj.set_data(obj.data())
                obj.header['sh_offset'] = pv_end_offs
            if (i >= 0):
                self._update_section_header(i, obj)
            elif (i == -2):
                self.header['e_phoff'] = obj['sh_offset']
            elif (i == -3):
                self.header['e_shoff'] = obj['sh_offset']
            if (obj['sh_type'] != 'SHT_NOBITS'):
                pv_end_offs = obj['sh_offset'] + obj['sh_size']
            else:
                pv_end_offs = obj['sh_offset']

    def prepare_section_to_segment_mapping(self):
        sect_to_seg = [-1] * self['e_shnum']
        for nseg in range(0, self['e_phnum']):
            seg = self.get_segment(nseg)
            if (seg['p_type'] == 'PT_NULL'):
                continue
            for i in range(0, self['e_shnum']):
                obj = self.get_section(i)
                if (obj['sh_flags'] & SH_FLAGS.SHF_ALLOC) == 0:
                    continue
                if (obj['sh_type'] == 'SHT_NULL'):
                    continue
                if (seg.section_in_segment(obj)):
                    sect_to_seg[i] = nseg
        return sect_to_seg

    def update_segments_to_sections_mapping(self, sect_to_seg):
        # Mark used segments for update
        for nseg in set(sect_to_seg):
            if (nseg < 0): continue
            seg = self.get_segment(nseg)
            seg.header['p_offset'] = -1
            seg.header['p_filesz'] = 0
            seg.header['p_memsz'] = 0
            self._update_segment_header(nseg, seg)
        # Update used segments to represent the updated sections
        for i in range(0, self['e_shnum']):
            obj = self.get_section(i)
            nseg = sect_to_seg[i]
            if (nseg < 0):
                continue
            # Get the original segment
            seg = self.get_segment(nseg)
            # Check if we should update equivalent segment instead of original one
            if (seg['p_offset'] == -1):
                nseg_eq = nseg
            else:
                nseg_eq = self.get_segment_index_equivalent_touching_or_overlaping_address_range(seg, obj['sh_addr'], obj['sh_size'])
            # Update segment
            if (seg['p_offset'] == -1) or (nseg_eq < 0):
                # If the segment was not associated to any section yet, do the association
                # Also overwrite the association if we are going to add a new segment
                seg.header['p_offset'] = obj['sh_offset']
                seg.header['p_filesz'] = obj['sh_size']
                seg.header['p_vaddr'] = obj['sh_addr']
                seg.header['p_paddr'] = obj['sh_addr']
                seg.header['p_memsz'] = obj['sh_size']
                if (nseg_eq < 0):
                    nseg = self._add_segment_header(seg)
                else:
                    self._update_segment_header(nseg, seg)
            else:
                # Being here means we have a segment touching the current memory area; extend it
                nseg = nseg_eq
                seg = self.get_segment(nseg)
                if seg['p_offset'] > obj['sh_offset']:
                    seg.header['p_filesz'] += seg['p_offset'] - obj['sh_offset']
                    seg.header['p_offset'] = obj['sh_offset']
                elif seg['p_offset'] + seg['p_filesz'] < obj['sh_offset'] + obj['sh_size']:
                    seg.header['p_filesz'] = obj['sh_offset'] + obj['sh_size'] - seg['p_offset']
                if seg['p_vaddr'] > obj['sh_addr']:
                    seg.header['p_memsz'] += seg['p_vaddr'] - obj['sh_addr']
                    seg.header['p_vaddr'] = obj['sh_addr']
                    seg.header['p_paddr'] = obj['sh_addr']
                elif seg['p_vaddr'] + seg['p_memsz'] < obj['sh_addr'] + obj['sh_size']:
                    seg.header['p_memsz'] = obj['sh_addr'] + obj['sh_size'] - seg['p_vaddr']
                self._update_segment_header(nseg, seg)

    def has_dwarf_info(self):
        """ Check whether this file appears to have debugging information.
            We assume that if it has the .debug_info or .zdebug_info section, it
            has all the other required sections as well.
        """
        return bool(self.get_section_by_name('.debug_info')) or \
            bool(self.get_section_by_name('.zdebug_info'))

    def get_dwarf_info(self, relocate_dwarf_sections=True):
        """ Return a DWARFInfo object representing the debugging information in
            this file.

            If relocate_dwarf_sections is True, relocations for DWARF sections
            are looked up and applied.
        """
        # Expect that has_dwarf_info was called, so at least .debug_info is
        # present.
        # Sections that aren't found will be passed as None to DWARFInfo.
        #

        section_names = ('.debug_info', '.debug_aranges', '.debug_abbrev', '.debug_str',
                         '.debug_line', '.debug_frame',
                         '.debug_loc', '.debug_ranges')

        compressed = bool(self.get_section_by_name('.zdebug_info'))
        if compressed:
            section_names = tuple(map(lambda x: '.z' + x[1:], section_names))

        debug_info_sec_name, debug_aranges_sec_name, debug_abbrev_sec_name, debug_str_sec_name, \
            debug_line_sec_name, debug_frame_sec_name, debug_loc_sec_name, \
            debug_ranges_sec_name = section_names

        debug_sections = {}
        for secname in section_names:
            section = self.get_section_by_name(secname)
            if section is None:
                debug_sections[secname] = None
            else:
                dwarf_section = self._read_dwarf_section(
                    section,
                    relocate_dwarf_sections)
                if compressed:
                    dwarf_section = self._decompress_dwarf_section(dwarf_section)
                debug_sections[secname] = dwarf_section

        return DWARFInfo(
                config=DwarfConfig(
                    little_endian=self.little_endian,
                    default_address_size=self.elfclass // 8,
                    machine_arch=self.get_machine_arch()),
                debug_info_sec=debug_sections[debug_info_sec_name],
                debug_aranges_sec=debug_sections[debug_aranges_sec_name],
                debug_abbrev_sec=debug_sections[debug_abbrev_sec_name],
                debug_frame_sec=debug_sections[debug_frame_sec_name],
                # TODO(eliben): reading of eh_frame is not hooked up yet
                eh_frame_sec=None,
                debug_str_sec=debug_sections[debug_str_sec_name],
                debug_loc_sec=debug_sections[debug_loc_sec_name],
                debug_ranges_sec=debug_sections[debug_ranges_sec_name],
                debug_line_sec=debug_sections[debug_line_sec_name])


    def get_machine_arch(self):
        """ Return the machine architecture, as detected from the ELF header.
            Not all architectures are supported at the moment.
        """
        if self['e_machine'] == 'EM_X86_64':
            return 'x64'
        elif self['e_machine'] in ('EM_386', 'EM_486'):
            return 'x86'
        elif self['e_machine'] == 'EM_ARM':
            return 'ARM'
        elif self['e_machine'] == 'EM_AARCH64':
            return 'AArch64'
        elif self['e_machine'] == 'EM_MIPS':
            return 'MIPS'
        else:
            return '<unknown>'

    #-------------------------------- PRIVATE --------------------------------#

    def __getitem__(self, name):
        """ Implement dict-like access to header entries
        """
        return self.header[name]

    def _identify_file(self):
        """ Verify the ELF file and identify its class and endianness.
        """
        # Note: this code reads the stream directly, without using ELFStructs,
        # since we don't yet know its exact format. ELF was designed to be
        # read like this - its e_ident field is word-size and endian agnostic.
        #
        self.stream.seek(0)
        magic = self.stream.read(4)
        elf_assert(magic == b'\x7fELF', 'Magic number does not match')

        ei_class = self.stream.read(1)
        if ei_class == b'\x01':
            self.elfclass = 32
        elif ei_class == b'\x02':
            self.elfclass = 64
        else:
            raise ELFError('Invalid EI_CLASS %s' % repr(ei_class))

        ei_data = self.stream.read(1)
        if ei_data == b'\x01':
            self.little_endian = True
        elif ei_data == b'\x02':
            self.little_endian = False
        else:
            raise ELFError('Invalid EI_DATA %s' % repr(ei_data))

    def _section_offset(self, n):
        """ Compute the offset of section #n in the file
        """
        return self['e_shoff'] + n * self['e_shentsize']

    def _segment_offset(self, n):
        """ Compute the offset of segment #n in the file
        """
        return self['e_phoff'] + n * self['e_phentsize']

    def _make_segment(self, segment_header):
        """ Create a Segment object of the appropriate type
        """
        segtype = segment_header['p_type']
        if segtype == 'PT_INTERP':
            return InterpSegment(segment_header, self.stream)
        elif segtype == 'PT_DYNAMIC':
            return DynamicSegment(segment_header, self.stream, self)
        elif segtype == 'PT_NOTE':
            return NoteSegment(segment_header, self.stream, self)
        else:
            return Segment(segment_header, self.stream)

    def _get_section_header(self, n):
        """ Find the header of section #n, parse it and return the struct
        """
        if n in self._section_update:
            return self._section_update[n].header
        return struct_parse(
            self.structs.Elf_Shdr,
            self.stream,
            stream_pos=self._section_offset(n))

    def _update_section_header(self, n, obj):
        """ Update the header of section #n, by storing a new header
        """
        self._section_update[n] = obj
        if n == self['e_shstrndx']:
            self._file_stringtable_section = obj

    def _write_section_header(self, n, hdr):
        """ Writes the header of section back into file.
        """
        return struct_write(self.structs.Elf_Shdr, hdr, self.stream,
            stream_pos=self._section_offset(n))

    def _get_section_name(self, section_header):
        """ Given a section header, find this section's name in the file's
            string table
        """
        name_offset = section_header['sh_name']
        return self._file_stringtable_section.get_string(name_offset)

    def _set_section_name(self, section_header, name):
        """ Given a section header and name, stores this section's name
            in the file's string table, and updates offset in header
        """
        name_offset = self._file_stringtable_section.add_string(name)
        section_header['sh_name'] = name_offset

    def _get_section_number_by_name(self, name):
        """ Get number of a section, by name. Return None if no such
            section exists.
        """
        # The first time this method is called, construct a name to number
        # mapping
        #
        if self._section_name_map is None:
            self._section_name_map = {}
            for i, sec in enumerate(self.iter_sections()):
                self._section_name_map[sec.name] = i
        return self._section_name_map.get(name, None)

    def _make_section(self, section_header):
        """ Create a section object of the appropriate type
        """
        name = self._get_section_name(section_header)
        sectype = section_header['sh_type']

        if sectype == 'SHT_STRTAB':
            return StringTableSection(section_header, name, self.stream)
        elif sectype == 'SHT_NULL':
            return NullSection(section_header, name, self.stream)
        elif sectype in ('SHT_SYMTAB', 'SHT_DYNSYM', 'SHT_SUNW_LDYNSYM'):
            return self._make_symbol_table_section(section_header, name)
        elif sectype == 'SHT_SUNW_syminfo':
            return self._make_sunwsyminfo_table_section(section_header, name)
        elif sectype == 'SHT_GNU_verneed':
            return self._make_gnu_verneed_section(section_header, name)
        elif sectype == 'SHT_GNU_verdef':
            return self._make_gnu_verdef_section(section_header, name)
        elif sectype == 'SHT_GNU_versym':
            return self._make_gnu_versym_section(section_header, name)
        elif sectype in ('SHT_REL', 'SHT_RELA'):
            return RelocationSection(
                section_header, name, self.stream, self)
        elif sectype == 'SHT_DYNAMIC':
            return DynamicSection(section_header, name, self.stream, self)
        elif sectype == 'SHT_NOTE':
            return NoteSection(section_header, name, self.stream, self)
        else:
            return Section(section_header, name, self.stream)

    def _make_symbol_table_section(self, section_header, name):
        """ Create a SymbolTableSection
        """
        linked_strtab_index = section_header['sh_link']
        strtab_section = self.get_section(linked_strtab_index)
        return SymbolTableSection(
            section_header, name, self.stream,
            elffile=self,
            stringtable=strtab_section)

    def _make_sunwsyminfo_table_section(self, section_header, name):
        """ Create a SUNWSyminfoTableSection
        """
        linked_strtab_index = section_header['sh_link']
        strtab_section = self.get_section(linked_strtab_index)
        return SUNWSyminfoTableSection(
            section_header, name, self.stream,
            elffile=self,
            symboltable=strtab_section)

    def _make_gnu_verneed_section(self, section_header, name):
        """ Create a GNUVerNeedSection
        """
        linked_strtab_index = section_header['sh_link']
        strtab_section = self.get_section(linked_strtab_index)
        return GNUVerNeedSection(
            section_header, name, self.stream,
            elffile=self,
            stringtable=strtab_section)

    def _make_gnu_verdef_section(self, section_header, name):
        """ Create a GNUVerDefSection
        """
        linked_strtab_index = section_header['sh_link']
        strtab_section = self.get_section(linked_strtab_index)
        return GNUVerDefSection(
            section_header, name, self.stream,
            elffile=self,
            stringtable=strtab_section)

    def _make_gnu_versym_section(self, section_header, name):
        """ Create a GNUVerSymSection
        """
        linked_strtab_index = section_header['sh_link']
        strtab_section = self.get_section(linked_strtab_index)
        return GNUVerSymSection(
            section_header, name, self.stream,
            elffile=self,
            symboltable=strtab_section)

    def _get_segment_header(self, n):
        """ Find the header of segment #n, parse it and return the struct
        """
        return struct_parse(
            self.structs.Elf_Phdr,
            self.stream,
            stream_pos=self._segment_offset(n))

    def _update_segment_header(self, n, seg):
        """ Update the header of segment #n, by storing a new header
        """
        self._segment_update[n] = seg

    def _add_segment_header(self, seg):
        """ Adds a new segment to the Program Header within ELF file
        """
        n = self['e_phnum']
        self._segment_update[n] = seg
        self.header['e_phnum'] += 1
        return n

    def _reorder_segment_headers(self):
        """ Changes order of segment headers, also merges them when possible.
        """
        all_segments = []
        for seg in self.iter_segments():
            all_segments.append(seg)
        # Sort by mem address; according to ELF format spec,
        # LOAD segments have to be sorted this way
        all_segments = sorted(all_segments, key=lambda seg: (seg['p_type'],seg['p_vaddr'],seg['p_memsz']))
        self._segment_update.clear()
        self.header['e_phnum'] = 0
        seg = all_segments[0]
        for i in range(1,len(all_segments)):
            prev_seg = seg
            seg = all_segments[i]
            if (self.segments_are_equivalent(seg, prev_seg) and
              seg['p_vaddr'] == prev_seg['p_vaddr'] + prev_seg['p_memsz'] and
              seg['p_offset'] == prev_seg['p_offset'] + prev_seg['p_filesz']):
                seg.header['p_memsz'] += prev_seg['p_memsz']
                seg.header['p_vaddr'] = prev_seg['p_vaddr']
                seg.header['p_filesz'] += prev_seg['p_filesz']
                seg.header['p_offset'] = prev_seg['p_offset']
                # extend current segment to include previous one
                continue
            self._add_segment_header(prev_seg)
        self._add_segment_header(seg)

    def _write_segment_header(self, n, hdr):
        """ Writes the header of segment back into file.
        """
        return struct_write(self.structs.Elf_Phdr, hdr, self.stream,
            stream_pos=self._segment_offset(n))

    def _get_file_stringtable(self):
        """ Find the file's string table section
        """
        stringtable_section_num = self['e_shstrndx']
        if stringtable_section_num in self._section_update:
            return copy.copy(self._section_update[stringtable_section_num])
        return StringTableSection(
                header=self._get_section_header(stringtable_section_num),
                name='',
                stream=self.stream)

    def _parse_elf_header(self):
        """ Parses the ELF file header and assigns the result to attributes
            of this object.
        """
        return struct_parse(self.structs.Elf_Ehdr, self.stream, stream_pos=0)

    def _write_elf_header(self):
        """ Writes the ELF file header and assigns the result to attributes
            of this object.
        """
        return struct_write(self.structs.Elf_Ehdr, self.header, self.stream, stream_pos=0)

    def _read_dwarf_section(self, section, relocate_dwarf_sections):
        """ Read the contents of a DWARF section from the stream and return a
            DebugSectionDescriptor. Apply relocations if asked to.
        """
        self.stream.seek(section['sh_offset'])
        # The section data is read into a new stream, for processing
        section_stream = BytesIO()
        section_stream.write(self.stream.read(section['sh_size']))

        if relocate_dwarf_sections:
            reloc_handler = RelocationHandler(self)
            reloc_section = reloc_handler.find_relocations_for_section(section)
            if reloc_section is not None:
                reloc_handler.apply_section_relocations(
                        section_stream, reloc_section)

        return DebugSectionDescriptor(
                stream=section_stream,
                name=section.name,
                global_offset=section['sh_offset'],
                size=section['sh_size'])

    @staticmethod
    def _decompress_dwarf_section(section):
        """ Returns the uncompressed contents of the provided DWARF section.
        """
        # TODO: support other compression formats from readelf.c
        assert section.size > 12, 'Unsupported compression format.'

        section.stream.seek(0)
        # According to readelf.c the content should contain "ZLIB"
        # followed by the uncompressed section size - 8 bytes in
        # big-endian order
        compression_type = section.stream.read(4)
        assert compression_type == b'ZLIB', \
            'Invalid compression type: %r' % (compression_type)

        uncompressed_size = struct.unpack('>Q', section.stream.read(8))[0]

        decompressor = zlib.decompressobj()
        uncompressed_stream = BytesIO()
        while True:
            chunk = section.stream.read(PAGESIZE)
            if not chunk:
                break
            uncompressed_stream.write(decompressor.decompress(chunk))
        uncompressed_stream.write(decompressor.flush())

        uncompressed_stream.seek(0, io.SEEK_END)
        size = uncompressed_stream.tell()
        assert uncompressed_size == size, \
                'Wrong uncompressed size: expected %r, but got %r' % (
                    uncompressed_size, size,
                )

        return section._replace(stream=uncompressed_stream, size=size)
