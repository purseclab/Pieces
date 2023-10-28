from .policy import *
import frontend 
from utils import error, debug, warn
import os
class crt(Policy):
	firmware_map = {}
	def get_safe_funcs(self):
		return 0


	def partition(self, firmware, clique):
		comp = firmware.create_compartment()
		for obj in clique["objs"]:
			comp.add(obj)

		#CRT does most stuff at compile-time using front-end.
		target_threads = []
		for obj in clique["objs"]:
			if obj in firmware.threads:
				target_threads.append(obj)

		target_funcs = []
		for thread in target_threads:
			for func in firmware.threads_reach[thread]:
				if func not in target_funcs:
					target_funcs.append(func)
			target_funcs.extend(firmware.threads_reach[thread])
		

		target_files = []
		for target in target_funcs:
			if target in firmware.funcdirmap:
				if firmware.funcdirmap[target] +"/"+ firmware.funcfilemap[target] not in target_files:
					target_files.append(firmware.funcdirmap[target] +"/"+ firmware.funcfilemap[target])
		debug("Target Functions:")
		debug(target_funcs)

		debug("Target Files:")
		debug(target_files)


		safe_funcs = []
		#Get safe functions

		#Find safe regions
		for f in target_files:
			pp_f = frontend.preprocess(f, firmware.config["fronted_flags"], firmware.config["project_root"])
			pragmas = frontend.parse_pragmas(pp_f.name)
			if pragmas:
				ranges =[]
				on = False
				for pragma in pragmas:
					if "CHECKED_SCOPE" in pragma.str:
						if "ON" in pragma.str:
							if not on:
								begin = pragma.line 
								on = True
						elif "OFF" in pragma.str:
							if on:
								end = pragma.line 
								ranges.append[[begin, end]]

						if on:
							with open(pp_f.name, 'r') as file:
								line_count = sum(1 for line in file)
								ranges.append([begin, line_count])

				debug(" For File"+ f)
				debug(ranges)
#				ast = frontend.parse_c_program(f, firmware.config["fronted_flags"], firmware.config["project_root"])
				for range in ranges:
					cursors = []
					frontend.get_cursors_in_range(frontend.get_cursor(pp_f.name), range[0], 0, range[1], 999999999999999, cursors)
					safe_funcs.extend(frontend.find_function_names_in_subset(cursors))


		debug("Safe Functions Found:" + str(safe_funcs))
		unsafe_targets = [x for x in target_funcs if x not in safe_funcs]

		if len(unsafe_targets) > 0:
			error("CRTC: Error unsafe threads found, please rewrite in checked scope")
			error(str(unsafe_targets))
			exit(1)

		possible_kernel_funcs = [x for x in clique["objs"] if x not in safe_funcs]


		if firmware not in crt.firmware_map:
			crt.firmware_map[firmware] = {}
			crt.firmware_map[firmware]["safe_funcs"] = []
			crt.firmware_map[firmware]["kernel_funcs"] = []

		for func in safe_funcs:
			if func not in crt.firmware_map[firmware]["safe_funcs"]:
				crt.firmware_map[firmware]["safe_funcs"].append(func)

		#TODO: make function
		for func in possible_kernel_funcs:
			if func not in crt.firmware_map[firmware]["kernel_funcs"]:
				crt.firmware_map[firmware]["kernel_funcs"].append(func)

	def finalizer(self, firmware):
		with open(os.environ["P_OUT_DIR"] + ".crtsafe", 'w') as f:
			f.write(str(crt.firmware_map[firmware]["safe_funcs"]))

		with open(os.environ["P_OUT_DIR"] + ".crtunsafe", 'w') as f:
			f.write(str(crt.firmware_map[firmware]["kernel_funcs"]))


#	print(frontend.parse_pragmas(firmware.funcdirmap[thread]+ "/"+ firmware.funcfilemap[thread], firmware.config["fronted_flags"], firmware.config["project_root"]))
