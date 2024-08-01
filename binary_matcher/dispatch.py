from firm_proc_util.firm_slicer import slice_firmware
from firm_proc_util.arm_bin2elf import armfw_bin2elf
from firm_proc_util.pyelftools.elftools.elf import elffile

from mathpid.auto_search import auto_search_ast

from ida import headless
from ida.scripts import batch_valid_path

import math_func_parser
import semantic_output_parser

import os
import regex as re
import logging
import subprocess


class DisPatch:
    def __init__(self, control_model: str, log_level=logging.INFO):
        self.control_model = control_model
        self.metadata = {}

        self._raw_bin_name = f"bin/{self.control_model}_fulldump.bin"
        self._elf_bin_name = f"data/{self.control_model}_augdump.elf"
        self._cfg_filename = f"data/{self.control_model}.cfg"
        
        self.logger = logging.getLogger(control_model)
        self.logger.setLevel(log_level)
        logging.basicConfig(level=logging.NOTSET)
        self.logger.info("DisPatch firmware initialized")

    def set_logging_level(self, log_level):
        self.logger.setLevel(log_level)

    def convert_bin2elf(self, flash_base, data_base, data_size, sect_align, raw_bin_name=None, elf_name=None):
        ''' Handle filename params '''
        if raw_bin_name is not None:
            self._raw_bin_name = raw_bin_name
        assert os.path.exists(self._raw_bin_name)
        if elf_name is not None:
            self._elf_bin_name = elf_name

        ''' Slice firmware from bootloader '''
        self.logger.info(f"Slicing firmware binary {self._raw_bin_name}")
        with open(self._raw_bin_name, "rb") as bin_file:
            minoff, maxoff, sliced_bin_bytes = slice_firmware(bin_file.read())
            self.logger.debug(f"Found minoff={minoff}, maxoff={maxoff} from raw binary")

        '''
        Convert ARM bin to elf (write to augdump.elf)
        --section .text@${FIRM_BASE}
        --section .data@${DATA_BASE}:${DATA_SZ}
        '''
        fw_baseaddr = flash_base + minoff
        elf_sect_pos = {'.text': fw_baseaddr, '.data': data_base}
        elf_sect_sizes = {'.text': 0, '.data': data_size}

        self.logger.info(f"Writing firmware binary {self._raw_bin_name} to {self._elf_bin_name}")
        armfw_bin2elf(sliced_bin_bytes, os.path.abspath(self._elf_bin_name), 0, elf_sect_pos, elf_sect_sizes, sect_align)

        '''
        Set metadata for firmware, instead of writing to another file?
        echo "BIN##./${MODEL}_augdump.elf" > ${META_CONFIG}
        echo "ARCH##Thumb" >> ${META_CONFIG}
        echo "BASEOFF##${FILE_OFF}" >> ${META_CONFIG}
        echo "BASEADDR##${FIRM_BASE}" >> ${META_CONFIG}
        '''
        with open(self._elf_bin_name, "rb") as elf:
            elfobj = elffile.ELFFile(elf)
            text_sect = elfobj.get_section_by_name('.text')
            assert not isinstance(text_sect, elffile.NullSection)
            self.metadata['baseoff'] = hex(int(text_sect.header['sh_offset']))
        self.metadata['bin'] = self._elf_bin_name
        self.metadata['arch'] = 'Thumb'
        self.metadata['baseaddr'] = hex(fw_baseaddr)
    
    def lift2llvm(self, elf_name:str=None):
        if elf_name is not None:
            self._elf_bin_name = elf_name
        assert os.path.exists(self._elf_bin_name)
        self.logger.info(f"RetDec lifting {self._elf_bin_name} to LLVM IR")
        args = f"retdec-decompiler.py --stop-after bin2llvmir {self._elf_bin_name}".split(" ")
        if self.logger.level <= logging.DEBUG:
            subprocess.run(args)
        else:
            subprocess.run(args, stdout=subprocess.DEVNULL)

    def symex(self, func_results=None, all_static_results=None):
        if func_results is None:
            func_results = f"data/{self.control_model}.csv"
        assert os.path.exists(func_results)
        if all_static_results is None:
            all_static_results = f"data/{self.control_model}_all.csv"
        assert os.path.exists(all_static_results)

        self.logger.info(f"Running symbolic execution with angr on {self._elf_bin_name}")
        auto_search_ast(self._elf_bin_name, func_results, all_static_results, None)

        # TODO: need to look into standard output stuff too, _pid.txt comes from the output files from IDA Pro

    
    def generate_config_file(self, cfg_filename=None, rav_info_filename=None, pid_info_filename=None, func_range_filename=None):
        ''' Handle filename params '''
        if cfg_filename is not None:
            self._cfg_filename = cfg_filename
        if rav_info_filename is None:
            rav_info_filename = f"data/rav_generic.txt"
        assert os.path.exists(rav_info_filename)
        if pid_info_filename is None:
            pid_info_filename = f"data/{self.control_model}_pid.txt"
        assert os.path.exists(pid_info_filename)
        if func_range_filename is None:
            func_range_filename = f"data/{self.control_model}_funcRange.txt"
        assert os.path.exists(func_range_filename)

        self.logger.info(f"Generating config file {self._cfg_filename}")
        with open(self._cfg_filename, 'w') as cfg_file:
            '''
            BIN##data/copter_augdump.elf
            ARCH##Thumb
            BASEOFF##0x...
            BASEADDR##0x...
            '''
            cfg_file.write(f"BIN##{self.metadata['bin']}\n")
            cfg_file.write(f"ARCH##{self.metadata['arch']}\n")
            cfg_file.write(f"BASEOFF##{self.metadata['baseoff']}\n")
            cfg_file.write(f"BASEADDR##{self.metadata['baseaddr']}\n")

            for math_func in math_func_parser.get_math_funcs(func_range_filename):
                name, addr, _ = math_func.split('~~', 2)
                name = name[name.find('##')+2:]
                addr = addr.replace('0x', '')
                cfg_file.write(f"MATHFUNC##function_{addr}~~{name}\n")

            with open(rav_info_filename, 'r') as rav_file:
                cfg_file.writelines(rav_file.readlines())
            with open(pid_info_filename, 'r') as pid_info_file:
                cfg_file.writelines(pid_info_file.readlines())
            with open(func_range_filename, 'r') as func_range_file:
                cfg_file.writelines(func_range_file.readlines())
        self.logger.info(f"Config file {self._cfg_filename} generated successfully")

    def recover_semantics(self, llvm_bc_filename=None, cfg_filename=None, output_filename=None):
        ''' Handle filename params '''
        if llvm_bc_filename is None:
            llvm_bc_filename = f"{self._elf_bin_name}.bc"
        assert os.path.exists(llvm_bc_filename)
        if cfg_filename is not None:
            self._cfg_filename = cfg_filename
        assert os.path.exists(self._cfg_filename)

        ''' elf, bc, and cfg must be in the same working directory '''
        assert os.path.dirname(os.path.abspath(self._elf_bin_name)) == \
            os.path.dirname(os.path.abspath(llvm_bc_filename)) == \
            os.path.dirname(os.path.abspath(self._cfg_filename))

        self.logger.info(f"Beginning semantic recovery using {self._cfg_filename}")
        self.logger.debug(f"Using files:\n-- {llvm_bc_filename}\n-- {self._cfg_filename}")

        ''' temporarily include SVF on path '''
        old_env = os.environ.copy()
        try:
            os.environ['PATH'] = f"{os.path.abspath('./SVF/build/bin')}:{os.environ['PATH']}"
            args = f"dispatch {llvm_bc_filename} --config {self._cfg_filename}"
            # status = os.system(args)
            semantic_out = subprocess.run(args, stdout=subprocess.PIPE, stderr=subprocess.PIPE, shell=True)
            if output_filename is not None:
                with open(output_filename, 'w+') as f:
                    f.write(semantic_out.stdout.decode())
        finally:
            os.environ.clear()
            os.environ.update(old_env)
        return semantic_out.stdout.decode()
    
    def perform_static_analysis(self, ida: headless.HeadlessIDA):
        if ida.create_new_database() != 0:
            return 1
        if ida.execute_script("./ida/scripts/apply_sigfile.py") != 0:
            return 2
        if ida.execute_script("./ida/scripts/func_range_dump.py") != 0:
            return 3
        if ida.execute_script("./ida/scripts/feature_extract.py") != 0:
            return 4
        try:
            subprocess.run('sudo python3 ./ida/scripts/batch_valid_path.py ./data/gdl', shell=True)
        except:
            return 5

        # if ida.execute_script("./ida/scripts/BB_extract.py") != 0:
        #     return 6

    def dispatch(self, ida: headless.HeadlessIDA, flash_base, data_base, data_size, sect_align, raw_bin_name: str=None):
        fw.convert_bin2elf(flash_base, data_base, data_size, sect_align, raw_bin_name=raw_bin_name)
        fw.lift2llvm()
        fw.perform_static_analysis(ida)
        func_range_filename = f"{fw._elf_bin_name}_functionRange.txt"
        fw.generate_config_file(func_range_filename=func_range_filename)
        semantic_out = fw.recover_semantics(cfg_filename=f"data/{fw.control_model}.cfg", output_filename="semantic_output.txt")
        return semantic_output_parser.parse_semantics(semantic_out)



if __name__ == "__main__":
    fw = DisPatch(control_model="copter", log_level=logging.DEBUG)
    ida = headless.HeadlessIDA("/opt/idapro-8.4/idat", fw._elf_bin_name)  
    fw.dispatch(ida, flash_base=0x8000000, data_base=0x30000000, data_size=0x480000, sect_align=0x10, raw_bin_name="bin/copter_fulldump.bin")


