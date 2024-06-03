import os
import re
from utils import run_cmd
from utils import debug, bcolors, colorize
import subprocess
import re
import pandas as pd
import pprint
import matplotlib.pyplot as plt
import linecache
import z3
import glob
from z3 import *
import cxxfilt

class SymEx:
	bv_math_funs = ["bvmul", "bvadd", "bvsub", "bvudiv", "bvurem", "bvsdiv", "bvsrem", "bvsmod", "bvneg", "bvabs"]
	debug_info = {}
	ardu_config_types = ["class.AP_ParamT", "struct.ardupilot_indication_SafetyState"]

	def filter_queries_with_math(self, summaries):
		math_q = []
		for summary in summaries:
			current = summary[0] #Vector to ref
			if self.has_math(current):
				math_q.append(current)
		return math_q

	def has_math(self, query):
		ret_val = False
		if query.decl().name() in self.bv_math_funs:
			ret_val =  True
		for arg in query.children():
			if (self.has_math(arg)):
				ret_val = True

		return ret_val

	def filter_math(self, query):
		ret_list = []
		if query.decl().name() in self.bv_math_funs:
			ret_list = ret_list + [query]
		else:
			for child in query.children():
					ret_list = ret_list + self.filter_math(child)

		return ret_list

	def super_simple(self, query):
		if (len(self.debug_info) ==0):
			#Load debug info
			with open(os.environ["P_OUT_DIR"] +"symex_debug_info.csv", 'r') as file:
				current = ""
				for line in file:
					if ":" not in line:
						current = line.strip()
						self.debug_info[current] = []
					else:
						[name,offset,type] = line.split(",")
#name,offset,type
						self.debug_info[current].append((name.split(":")[-1].strip(), int(offset.split(":")[-1].strip()), type.split(":")[-1].strip()))
				



		simple = cxxfilt.demangle(str(z3.simplify(query)))
		current_concat = []
		current_concat_idx = []
		match = 0
		ss= ""
		concat_gate =False
		for token in re.split(r'\s+|\n+', simple):
			if "Concat" in token:
				concat_gate = True
				sub_tokens = token.split("Concat")
				if (len(sub_tokens) == 2):
					ss = ss+ sub_tokens[0]
					token = sub_tokens[1]
			
					
			if concat_gate:
				clean_token = token.replace("Concat(", "").replace(",", "")
				name = clean_token.split("[")[0].replace("(", "")
				index = clean_token.split("[")[1].split("]")[0]
				current_concat.append(name)
				current_concat_idx.append(int(index))
				while ")" in token and match > 0:
					token = token.replace(")","",1)
					match -= 1
				while "(" in token:
					token = token.replace("(", "", 1)
					match += 1
				if match ==0:
					concat_gate = False
					#Lets concat all subvectors
					base_vec = current_concat[0]
					idx = current_concat_idx[-1]
					for subs in current_concat:
						if subs == base_vec:
							continue
						else:
							print("BUG ON: Sequence not in order.")
							print(subs)
							print(base_vec)
							break
					if base_vec in self.debug_info:
						di = self.debug_info[base_vec]
						for elem in di:
#import pdb; pdb.set_trace()
							if (idx == elem[1]):
									base_vec = elem[0]
									for cfg in self.ardu_config_types:
										if (cfg in elem[2]):
												base_vec = colorize(colorize(colorize(base_vec, bcolors.BOLD), bcolors.MAGENTA), bcolors.UNDERLINE)
#TODO check for type
					
					ss = ss + base_vec

					if len(token.split("]")) == 2:
						ss = ss + token.split("]")[1]
					current_concat = []
					current_concat_idx = []

					concat_gate = False
			else:
				ss = ss+ token


		ret_val = ""
		for s in ss:
			if s in ["+", "-", "/", "*"]:
				ret_val = ret_val + " " + s + " "
			else:
				ret_val = ret_val + s


		#simplify negative
		ret_val = ret_val.replace("+ 4294967295 *", "-") 
		return ret_val


	def extract_conjuncts(self, expr):
		if is_and(expr):
			conjuncts = []
			for arg in expr.children():
				conjuncts.extend(extract_conjuncts(arg))
			return conjuncts
		else:
			return [expr]
	def parse_callgrind_file(self, file_path):
		fun_cov = {}
		with open(file_path, 'r') as f:
			lines = f.readlines()

		data = []
		for line in lines:
			line = line.strip()
			if line.startswith('positions:'):
				position_type = line.split(':')[1].strip()
				continue
			if line.startswith('events:'):
				events = line.split(':')[1].strip().split(',')
				continue
			if line.startswith('summary:'):
				break
			if line.startswith('fn='):
				fun = line[3:]
				continue
			match = re.match(r'[a-zA-Z]', line)

			if match:
				continue

			line = line.strip()
			if line:
				numbers = [int(num) for num in line.strip().split()]
				instr,line, Icov, Forks, Ireal, Itime, I, UCdist, Rtime, States, Iuncov, Q, Qiv, Qv, Qtime = numbers
				if I > 0:
					if fun not in fun_cov:
						fun_cov[fun] = []
					fun_cov[fun].append(instr)


		return fun_cov

	def get_trace(self, id):
		asm_file = os.environ["P_OUT_DIR"] + id + "/assembly.ll"
		pp = pprint.PrettyPrinter(indent=4)
		fun_cov = self.parse_callgrind_file(os.environ["P_OUT_DIR"] + id +"/run.istats")
		pp.pprint(fun_cov)
		for fun in fun_cov:
			print(fun + ":")
			first_line = 99999999999999999999999
			for line in fun_cov[fun]:
					if line<first_line:
						first_line = line

			lonely_bb = []
			print("Lines Not Covered:")
			current = first_line
			line = linecache.getline(asm_file, current).strip()
#			import pdb; pdb.set_trace()
			while (True):
					current += 1 #We miss first line, but its ok
					line = linecache.getline(asm_file, current).strip()
					if not line:
						continue
					if line[0] == ";": #Comment 
						continue
					if line.split(" ")[0][-1]== ":":
						current_bb  = line.split(" ")[0]
						continue

					if "}" == line[-1]:
						break
					if "define" == line.split(" ")[0] or "declare" == line.split(" ")[0]:
						break
					if current not in fun_cov[fun]:
						print(str(current) +":"+"  "+ linecache.getline(asm_file, current).strip())
						if current_bb not in lonely_bb:
							lonely_bb.append(current_bb)

			print("Uncovered Basic Blocks:")
			print(lonely_bb)

		return fun_cov


	def get_last_trace(self):
		return self.get_trace("klee-last")

	def generate_summary(self, firmware, function):
		debug("Generating Summary")
		summary = ""
		with open(os.environ["P_OUT_DIR"] +"obj_list", 'w') as file:
			for obj in firmware.pdg[function]:
				file.write(obj + "\n")

		run_cmd([os.environ["SYMEX"], "--symbolics=obj_list", function , firmware.symex_bc])
		with open(os.environ["P_OUT_DIR"] +"klee-last/test000001.smt2", 'r') as file:
			summary = file.read()

		matching_files = glob.glob(os.environ["P_OUT_DIR"] +"klee-last/test*.smt2")
		summaries = []
		for file in matching_files:
			with open(file, 'r') as query: 
				summary = query.read()
				summaries.append(z3.parse_smt2_string(summary))

		return summaries


	def get_old_summary(self, id):
		with open(os.environ["P_OUT_DIR"] + id + "/test000001.smt2", 'r') as file:
			summary = file.read()

		matching_files = glob.glob(os.environ["P_OUT_DIR"] +"klee-last/test*.smt2")
		summaries = []
		for file in matching_files:
			with open(file, 'r') as query:
				summary = query.read()
				summaries.append(z3.parse_smt2_string(summary))

		return summaries

	def get_addends(self, query):
		ident = 0
		addends = []
		current = ""
		for char in query:
			if char == "(":
				ident += 1
			elif char == ")":
				ident -= 1
			elif char == "+" and ident ==0:
				addends.append(current)
				current = ""
				continue

			current = current + char

		if current:
			addends.append(current)
		return addends

	
			
			

