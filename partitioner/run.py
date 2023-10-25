#!/usr/bin/python3
import frontend
import shutil
import ec_loader
import click
import utils
import sys
from utils import debug, print_help_msg
from dotenv import load_dotenv
from llvm import Compiler
import os 

load_dotenv()
try:
	input = utils.load_config(standalone_mode=False)
	if not input:
		exit()
except Exception as e:
	print(e)
	utils.print_help_msg(utils.load_config)
	exit()

os.environ["P_OUT_DIR"] = os.path.abspath(os.environ["P_OUT_DIR"]) +"/"
os.makedirs(os.environ["P_OUT_DIR"], exist_ok=True)
input["firmware"]["bc"] =  os.path.abspath(input["firmware"]["bc"])
debug("Loading input firmware.")
compiler = Compiler()
compiler.analyze(input["firmware"])
firmware = ec_loader.Firmware(input["firmware"])
firmware.generate_cliques(input["firmware"])
firmware.merge_shared_compartments()
firmware.generate_dev_info()
firmware.sanitize()
firmware.write_partitions()
firmware.dump()
new_bin= compiler.instrument(input["firmware"])
compiler.disassemble(new_bin)
shutil.copyfile(new_bin, input["firmware"]["bc"])
