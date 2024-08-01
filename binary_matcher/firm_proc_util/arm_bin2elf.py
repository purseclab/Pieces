#!/usr/bin/env python3
# -*- coding: utf-8 -*-

"""
Binary firmware blob with ARM code to ELF converter. 

TODO: Consider using lief instead of pyelftools here, much simpler to modify an ELF with lief

"""

from __future__ import print_function
import argparse
import os
import re
from ctypes import *

from .pyelftools.elftools.elf import elffile

# import elftools.elf.elffile
# import elftools.elf.sections

#TODO, Vector table checker?
def getEntryPoint(data_buf):
   #return 0x80a0ab9
   return 0x8004000

def armfw_bin2elf(po, fwpartfile):
   print("{}: Memory base address set to 0x{:08x}".format(po.fwpartfile,po.baseaddr))
   print("{}: Searching for sections".format(po.fwpartfile))
   # List of section
   # > .text: idx 0
   # > .data: idx 1
   # > .shstrtab: idx 2
   sect2idx = {".text":0, ".data":1, ".shstrtab":2}
   sectname = ".data"
   if (not sectname in po.section_pos):
      sect_pos += sect_len
      if (sect_pos % sect_align) != 0:
         sect_pos += sect_align - (sect_pos % sect_align)
      po.section_pos[sectname] = sect_pos
   else:
      sect_pos = po.section_pos[sectname]

   if (not sectname in po.section_size):
      fwpartfile.seek(0, os.SEEK_END)
      sect_len = fwpartfile.tell() - sect_pos
      po.section_size[sectname] = sect_len
   else:
      sect_len = po.section_size[sectname]

   # Prepare list of sections in the order of position
   sections_order = []
   for sortpos in sorted(set(po.section_pos.values())):
      # First add sections with size equal zero
      for sectname, pos in po.section_pos.items():
         if pos == sortpos:
            if sectname in po.section_size.keys():
               if (po.section_size[sectname] < 1):
                  sections_order.append(sectname)
      # The non-zero sized section should be last
      for sectname, pos in po.section_pos.items():
         if pos == sortpos:
            if sectname not in sections_order:
               sections_order.append(sectname)

   # Prepare list of section sizes
   sectpos_next = po.addrspacelen # max size is larger than bin file size due to uninitialized sections (bss)
   for sectname in reversed(sections_order):
      sectpos_delta = sectpos_next - po.section_pos[sectname]
      if (sectpos_delta < 0): sectpos_delta = 0xffffffff - po.section_pos[sectname]
      if sectname in po.section_size.keys():
         if (po.section_size[sectname] > sectpos_delta):
            po.section_size[sectname] = sectpos_next - po.section_pos[sectname]
      else:
         po.section_size[sectname] = sectpos_delta
      sectpos_next = po.section_pos[sectname]
      print(" >> " + sectname + ": Pos: " + str(hex(po.section_pos[sectname])) + " / size: " + str(hex(po.section_size[sectname])))

   # Copy an ELF template to destination file name
   if len(os.path.dirname(__file__)) > 0:
      po.tmpltfile = os.path.dirname(__file__) + "/arm_bin2elf_template.elf"
   elf_templt = open(po.tmpltfile, "rb")
   elf_fh = open(po.elffile, "wb")

   n = 0
   while (1):
      copy_buffer = elf_templt.read(1024 * 1024)
      if not copy_buffer:
         break
      n += len(copy_buffer)
      if not po.dry_run:
         elf_fh.write(copy_buffer)
   elf_templt.close()
   elf_fh.close()
   if (po.verbose > 1):
      print("{}: ELF template '{:s}' copied to '{:s}', {:d} bytes".format(po.fwpartfile,po.tmpltfile,po.elffile,n))

   # Prepare array of addresses
   sections_address = {}
   sections_align = {}
   for sectname in sections_order:
      # add section address to array; since BIN is a linear mem dump, addresses are the same as file offsets
      sections_address[sectname] = po.section_pos[sectname]
      sect_align = (po.expect_sect_align << 1)
      while (sections_address[sectname] % sect_align) != 0: sect_align = (sect_align >> 1)
      sections_align[sectname] = sect_align
      if (po.verbose > 0):
         print("{}: Section '{:s}' memory address set to 0x{:08x}, alignment 0x{:02x}".format(po.fwpartfile,sectname,sections_address[sectname],sect_align))

   # Update entry point in the ELF header
   print("{}: Updating entry point and section headers".format(po.fwpartfile))
   if not po.dry_run:
      elf_fh = open(po.elffile, "r+b")
   else:
      elf_fh = open(po.tmpltfile, "rb")
   elfobj = elffile.ELFFile(elf_fh)


   # section header config 
   # example of  section_header = {'sh_offset' : 0, 'sh_size' : self['e_ehsize'], 'sh_addralign' : 0x20, 'sh_type' : 'SHT_PROGBITS'}
   for sectname in sections_order:
      sect = elfobj.get_section_by_name(sectname)
      if sect is None:
         assert(0)

      print("{}: Preparing ELF section '{:s}' from binary pos 0x{:08x}".format(po.fwpartfile,sectname,po.section_pos[sectname]))
      sect.header['sh_addr'] = sections_address[sectname]
      sect.header['sh_addralign'] = sections_align[sectname]

      if sectname == ".shstrtab":
         sect.header['sh_size'] = po.section_size[sectname]
      elif sectname == ".data":
         sect.set_data(b'')
         sect.header['sh_type'] = 'SHT_NOBITS'
         sect.header['sh_size'] = po.section_size[sectname]

         # segment update
         seg = elfobj.get_segment(sect2idx[sectname])
         seg.header['p_vaddr'] = sections_address[sectname]
         seg.header['p_paddr'] = sections_address[sectname]
         seg.header['p_memsz'] = po.section_size[sectname]
         seg.header['p_filesz'] = 0
         seg.header['p_offset'] = sect.header['sh_offset']
         elfobj._update_segment_header(sect2idx[sectname], seg)

      elif sectname == ".text":
         data_buf = fwpartfile.read()
         po.section_size[sectname] = len(data_buf)
         if po.section_size[sectname] == 0:
            assert(0)
         sect.set_data(data_buf)
         sect.header['sh_size'] = po.section_size[sectname]
         elfobj.header['e_entry'] = getEntryPoint(data_buf)

         # TODO consider to update the below if there is problem with offset for .text
         #seg = elfobj.get_segment(sect2idx[sectname])
         #print(seg.header)
         #assert(0)
         #seg.header['p_vaddr'] = sections_address[sectname]
         #seg.header['p_paddr'] = sections_address[sectname]
         #seg.header['p_memsz'] = po.section_size[sectname]
         #seg.header['p_filesz'] = po.section_size[sectname]
         #print(sect.header['sh_offset'])
         #assert(0)
         #seg.header['p_offset'] = sect.header['sh_offset']
         #elfobj._update_segment_header(sect2idx[sectname], seg)
      else:
         print("[Error]: Sectname: " + sectname)
         assert(0)

      elfobj.set_section_by_name(sectname, sect)

   print("{}: Writing changes to '{:s}'".format(po.fwpartfile,po.elffile))
   elfobj.write_changes()
   elf_fh.close()

def parse_section_param(s):
    """ Parses the section parameter argument.
    """
    sect={ "pos": {}, "len": {}, }
    arg_m = re.search('(?P<name>[0-9A-Za-z._-]+)(@(?P<pos>[Xx0-9A-Fa-f]+))?(:(?P<len>[Xx0-9A-Fa-f]+))?', s)
    # Convert to integer, detect base from prefix
    if arg_m.group("pos") is not None:
        sect["pos"][arg_m.group("name")] = int(arg_m.group("pos"),0)
    if arg_m.group("len") is not None:
        sect["len"][arg_m.group("name")] = int(arg_m.group("len"),0)
    return sect


def main():
  """ Main executable function.

  Its task is to parse command line options and call a function which performs requested command.
  """
  # Parse command line options

  parser = argparse.ArgumentParser(description=__doc__.split('.')[0])

  parser.add_argument("-p", "--fwpartfile", type=str, required=True,
          help="Executable ARM firmware binary module file")

  parser.add_argument("-o", "--elffile", type=str,
          help="Output ELF file name (default is fwpartfile with elf extension appended)")

  parser.add_argument("-t", "--tmpltfile", type=str, default="arm_bin2elf_template.elf",
          help="Template ELF file to use header fields from (default is \"%(default)s\")")

  parser.add_argument('-l', '--addrspacelen', default=0x2000000, type=lambda x: int(x,0),
          help='Set address space length; influences size of last section (defaults to 0x%(default)X)')

  parser.add_argument('-b', '--baseaddr', default=0x1000000, type=lambda x: int(x,0),
          help='Set base address; first section will start at this memory location (defaults to 0x%(default)X)')

  parser.add_argument("-s", "--section", action='append', metavar='SECT@POS:LEN', type=parse_section_param,
          help="Set section position and/or length; can be used to override " \
           "detection of sections; setting section .ARM.exidx will influence " \
           ".text and .data, moving them and sizing to fit one before and one " \
           "after the .ARM.exidx. Parameters are: " \
           "SECT - a text name of the section, as defined in elf template; multiple sections " \
           "can be cloned from the same template section by adding index at end (ie. .bss2); " \
           "POS - is a position of the section within input file (not a memory address!); " \
           "LEN - is the length of the section (in both input file and memory, unless its " \
           "uninitialized section, in which case it is memory size as file size is 0)")

  parser.add_argument("--dry-run", action="store_true",
          help="Do not write any files or do permanent changes")

  parser.add_argument("-v", "--verbose", action="count", default=0,
          help="Increases verbosity level; max level is set by -vvv")

  subparser = parser.add_mutually_exclusive_group()

  subparser.add_argument("-e", "--mkelf", action="store_true",
          help="make ELF file from a binary image")

  po = parser.parse_args()

  po.expect_func_align = 2
  po.expect_sect_align = 0x10
  # For some reason, if no "--section" parameters are present, argparse leaves this unset
  if po.section is None:
      po.section = []
  # Flatten the sections we got in arguments
  po.section_pos = {}
  po.section_size = {}
  for sect in po.section:
      po.section_pos.update(sect["pos"])
      po.section_size.update(sect["len"])

  po.basename = os.path.splitext(os.path.basename(po.fwpartfile))[0]
  if len(po.fwpartfile) > 0 and (po.elffile is None or len(po.elffile) == 0):
      po.elffile = po.basename + ".elf"

  if po.mkelf:
     print("{}: Opening for conversion to ELF".format(po.fwpartfile))
     fwpartfile = open(po.fwpartfile, "rb")
     armfw_bin2elf(po,fwpartfile)
     fwpartfile.close()
     print(">> Conversion Done!")

  else:
    raise NotImplementedError('Unsupported command.')

if __name__ == "__main__":
   main()



def armfw_bin2elf(fw_bytes, elf_filepath, addrspacelen, section_pos: dict, section_size: dict, expect_sect_align):
    # print("{}: Memory base address set to 0x{:08x}".format(fw_filepath, bin_baseaddr))
    # print("{}: Searching for sections".format(fw_filepath))

    # List of section
    # > .text: idx 0
    # > .data: idx 1
    # > .shstrtab: idx 2
    sect2idx = {".text":0, ".data":1, ".shstrtab":2}
    sectname = ".data"

    if (not sectname in section_pos):
        sect_pos += sect_len
        if (sect_pos % sect_align) != 0:
            sect_pos += sect_align - (sect_pos % sect_align)
        section_pos[sectname] = sect_pos
    else:
        sect_pos = section_pos[sectname]

    # fw_fh = open(fw_filepath, "rb")
    if (not sectname in section_size):
        # fw_fh.seek(0, os.SEEK_END)
        # sect_len = fw_fh.tell() - sect_pos
        sect_len = len(fw_bytes) - sect_pos
        section_size[sectname] = sect_len
    else:
        sect_len = section_size[sectname]

    # Prepare list of sections in the order of position
    sections_order = []
    for sortpos in sorted(set(section_pos.values())):
        # First add sections with size equal zero
        for sectname, pos in section_pos.items():
            if pos == sortpos:
                if sectname in section_size.keys():
                    if (section_size[sectname] < 1):
                        sections_order.append(sectname)
        # The non-zero sized section should be last
        for sectname, pos in section_pos.items():
            if pos == sortpos:
                if sectname not in sections_order:
                    sections_order.append(sectname)

    # Prepare list of section sizes
    sectpos_next = addrspacelen # max size is larger than bin file size due to uninitialized sections (bss)
    for sectname in reversed(sections_order):
        sectpos_delta = sectpos_next - section_pos[sectname]
        if (sectpos_delta < 0): sectpos_delta = 0xffffffff - section_pos[sectname]
        if sectname in section_size.keys():
            if (section_size[sectname] > sectpos_delta):
                section_size[sectname] = sectpos_next - section_pos[sectname]
        else:
            section_size[sectname] = sectpos_delta
        sectpos_next = section_pos[sectname]
        # print(" >> " + sectname + ": Pos: " + str(hex(section_pos[sectname])) + " / size: " + str(hex(section_size[sectname])))

    # Copy an ELF template to destination file name
    if len(os.path.dirname(__file__)) > 0:
        tmpltfile = os.path.dirname(__file__) + "/arm_bin2elf_template.elf"
    elf_templt = open(tmpltfile, "rb")
    elf_fh = open(elf_filepath, "wb")

    n = 0
    while (1):
        copy_buffer = elf_templt.read(1024 * 1024)
        if not copy_buffer:
            break
        n += len(copy_buffer)
        # if not dry_run:
        #    elf_fh.write(copy_buffer)
        elf_fh.write(copy_buffer)

    elf_templt.close()
    elf_fh.close()
    # if (verbose > 1):
    #    print("{}: ELF template '{:s}' copied to '{:s}', {:d} bytes".format(fwpartfile,tmpltfile,elf_filepath,n))

    # Prepare array of addresses
    sections_address = {}
    sections_align = {}
    for sectname in sections_order:
        # add section address to array; since BIN is a linear mem dump, addresses are the same as file offsets
        sections_address[sectname] = section_pos[sectname]
        sect_align = (expect_sect_align << 1)
        while (sections_address[sectname] % sect_align) != 0: sect_align = (sect_align >> 1)
        sections_align[sectname] = sect_align
        # if (verbose > 0):
        #    print("{}: Section '{:s}' memory address set to 0x{:08x}, alignment 0x{:02x}".format(fwpartfile,sectname,sections_address[sectname],sect_align))

    # Update entry point in the ELF header
    # print("{}: Updating entry point and section headers".format(fw_fh))
    # if not dry_run:
    #    elf_fh = open(elf_filepath, "r+b")
    # else:
    #    elf_fh = open(tmpltfile, "rb")
    elf_fh = open(elf_filepath, "r+b")
    elfobj = elffile.ELFFile(elf_fh)


    # section header config 
    # example of  section_header = {'sh_offset' : 0, 'sh_size' : self['e_ehsize'], 'sh_addralign' : 0x20, 'sh_type' : 'SHT_PROGBITS'}
    for sectname in sections_order:
        sect = elfobj.get_section_by_name(sectname)
        if sect is None:
            assert(0)

        # print("{}: Preparing ELF section '{:s}' from binary pos 0x{:08x}".format(fw_fh,sectname,section_pos[sectname]))
        sect.header['sh_addr'] = sections_address[sectname]
        sect.header['sh_addralign'] = sections_align[sectname]

        if sectname == ".shstrtab":
            sect.header['sh_size'] = section_size[sectname]
        elif sectname == ".data":
            # sect.data = b''
            sect.set_data(b'')
            sect.header['sh_type'] = 'SHT_NOBITS'
            sect.header['sh_size'] = section_size[sectname]

            # segment update
            seg = elfobj.get_segment(sect2idx[sectname])
            seg.header['p_vaddr'] = sections_address[sectname]
            seg.header['p_paddr'] = sections_address[sectname]
            seg.header['p_memsz'] = section_size[sectname]
            seg.header['p_filesz'] = 0
            seg.header['p_offset'] = sect.header['sh_offset']
            elfobj._update_segment_header(sect2idx[sectname], seg)

        elif sectname == ".text":
            # data_buf = fw_fh.read()
            section_size[sectname] = len(fw_bytes)
            if section_size[sectname] == 0:
                assert(0)
            # sect.data = data_buf
            sect.set_data(fw_bytes)
            sect.header['sh_size'] = section_size[sectname]
            elfobj.header['e_entry'] = getEntryPoint(fw_bytes)

            # TODO consider to update the below if there is problem with offset for .text
            #seg = elfobj.get_segment(sect2idx[sectname])
            #print(seg.header)
            #assert(0)
            #seg.header['p_vaddr'] = sections_address[sectname]
            #seg.header['p_paddr'] = sections_address[sectname]
            #seg.header['p_memsz'] = po.section_size[sectname]
            #seg.header['p_filesz'] = po.section_size[sectname]
            #print(sect.header['sh_offset'])
            #assert(0)
            #seg.header['p_offset'] = sect.header['sh_offset']
            #elfobj._update_segment_header(sect2idx[sectname], seg)
        else:
            print("[Error]: Sectname: " + sectname)
            assert(0)

        elfobj.set_section_by_name(sectname, sect)

    # print("{}: Writing changes to '{:s}'".format(fw_filepath,elf_filepath))
    elfobj.write_changes()
    elf_fh.close()
