import os
from utils import run_cmd
from utils import debug
import subprocess

class Compiler:
	def analyze(self, input):
		debug("Analyzing firmware.")
		run_cmd([os.environ["SVF"], input["bc"], "./kern_funcs2", "./user_funcs2", "./safe_funcs2", "./create_funcs", "./dev.map"])

	def instrument(self, input):
		debug("Instrumenting firmware.")
		run_cmd([os.environ["SVF"], input["bc"], "./kern_funcs2", "./user_funcs2", "./safe_funcs2", "./create_funcs", "./dev.map", "-p", "./.policy"])
		return os.environ["P_OUT_DIR"] + "temp.bc"

	def disassemble(self, input):
		debug("Dissassembling " + input)
		bc = os.path.abspath(input)
		run_cmd(["llvm-dis", bc], out=subprocess.STDOUT)
