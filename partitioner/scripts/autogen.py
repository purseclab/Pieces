#!/usr/bin/python3.8
import subprocess
import buildutils
import os
import sys
import re
import math
from pathlib import Path

DEBUG_FLAG = False
NUM_DEFAULT_COMPARMENTS = 20

def find_sizeinfo_file():
	return [str(file) for file in Path(".").rglob("sizeinfo")]

def parse_sizeinfo(file_path):
	sections = {}
	total_size = 0
	
	with open(file_path, 'r') as file:
		for line in file:
			match = re.match(r'^(\S+)\s+(\d+)\s+(\d+)?$', line.strip())
			if match:
				section_name = match.group(1)
				size = int(match.group(2))
				addr = int(match.group(3)) if match.group(3) else None
				
				sections[section_name] = {"size": size, "addr": addr}
			elif line.startswith("Total"):
				total_size = int(line.split()[-1])
	
	return {"sections": sections, "total_size": total_size}

def get_num_compartments():
	script_dir = os.path.dirname(os.path.abspath(__file__))
	file_path = os.path.join(script_dir, "../out/.policy")

	try:
		with open(file_path, "r") as file:
			line_count = sum(1 for _ in file)
	except FileNotFoundError:
		print("BUGON: Wrong phase discovery?")
		line_count = NUM_DEFAULT_COMPARMENTS
	except Exception as e:
		print(f"An error occurred: {e}")
	return line_count

def find_phase(env):
	#Linker script is not given
	if "LD_OVERLAY" not in env:
		return 0

	#Firmware is not partitioned
	script_dir = os.path.dirname(os.path.abspath(__file__))
	file_path = os.path.join(script_dir, "../out/.policy")
	if not os.path.exists(file_path):
		return 1

	#Firmware objects no instrumented
	#TODO: What if more files?
	files = find_sizeinfo_file()
	if (len(files) == 0):
		return 1
	sizeinfo = parse_sizeinfo(files[0])

	total_partitioned_code =0
	if DEBUG_FLAG:
		print(sizeinfo)

	for section in sizeinfo["sections"].keys():
		if "csection" in section:
			total_partitioned_code += sizeinfo["sections"][section]["size"]

	if total_partitioned_code == 0:
		return 2
	else:
		return 3

def run_setupld(env, num_comps):
	command = [
		"setupLD.py", #For the path issue
		"-n", str(num_comps),
		"-l", env["LD_OVERLAY"],
		"-c", directory + "/autogen_data.c",
		"-H", directory + "/autogen_heap.c"
	]

	# Run the command
	try:
		result = subprocess.run(command, check=True, text=True, capture_output=True)
		
		if DEBUG_FLAG:
			print("STDOUT:", result.stdout)
			print("STDERR:", result.stderr)
	except subprocess.CalledProcessError as e:
		print("Error occurred while running the command:")
		print(e.stderr)

def next_power_of_2(n):
    return 1 if n == 0 else 2**math.ceil(math.log2(n))

def get_sections_line_index(lines):
    for index, line in enumerate(lines):
        if "SECTIONS" in line:
            return index
    return -1  # Return -1 if SECTIONS is not found

def Log2(x):
	return (math.log10(x) /
			math.log10(2))

def isPowerOfTwo(n):
    return (math.ceil(Log2(n)) == math.floor(Log2(n)))

def adjust_sections(linker_script, section_info):

	cprev_sec_end_addr = 0
	oprev_sec_end_addr = 0
	new_file_lines = []

	with open(linker_script, 'r') as file:
		lines = file.readlines()
    
	sections_start = get_sections_line_index(lines)
    
	index = 0
	while index < len(lines):

		# Phase 1: Copy everything until sections start
		line = lines[index]
		new_file_lines.append(line)
		
		if index <= sections_start:
			index += 1
			continue
		
		# Phase 2: This means that we are inside SECTIONS
		if  "csection" in line or "osection" in line and ":" in line: 
			# Handle MPU address constraints logic
			text = lines[index]
			if ":" in text:
				extracted_section_text = text.split(":")[0].split()[0]
			else:
				index += 1
				continue

			if "{" in lines[index+1] :
				index = index+1
				new_file_lines.append(lines[index])

            # This is where we should handle our location counter logic
			size = section_info[extracted_section_text]["size"]
			addr = section_info[extracted_section_text]["addr"]
			orig_addr = addr

			if "csection" in line:
				if addr < cprev_sec_end_addr:
					addr = cprev_sec_end_addr
			elif "osection" in line:
				if addr < oprev_sec_end_addr:
					addr = oprev_sec_end_addr


			if size != 0:
                # Ensure size is a power of 2 and at least 32 bytes (Constraint 1 and 2)
				if size < 32 or not isPowerOfTwo(size):
					new_size = max(next_power_of_2(size), 32)
					if DEBUG_FLAG:
						print(f"Adjusting size of {extracted_section_text}: {size} -> {new_size}")
					size = new_size
            
                # Ensure base address is a multiple of the new size (Constraint 3)
				if addr % size != 0:
					new_addr = ((addr // size)+1) * size
					if DEBUG_FLAG:
						print(f"Adjusting address of {extracted_section_text}: {addr} -> {new_addr}")
					addr = new_addr
        
			start_location_counter = "\t\t. = " + str(addr) + " ;\n"
			new_file_lines.append(start_location_counter)

			end_addr = addr + size
			end_location_counter = "\t\t. = " + str(end_addr) + " ;\n"
            
			if DEBUG_FLAG:
				print(f"section: {extracted_section_text}")
				print(f"orig_address: {orig_addr}")
				print(f"new_addr: {addr}")
				print(f"end_addr: {end_addr}")
				print(f"cprev_sec_end_addr: {cprev_sec_end_addr}")
				print(f"oprev_sec_end_addr: {oprev_sec_end_addr}")

			if "csection" in line:
				cprev_sec_end_addr = end_addr
			elif "osection" in line:
				oprev_sec_end_addr = end_addr

			while("}" not in lines[index]):
				index = index +1
				new_file_lines.append(lines[index])
            
			new_file_lines.insert(len(new_file_lines) - 2, end_location_counter)

		index += 1

	with open(linker_script, 'w') as new_file:
		new_file.writelines(new_file_lines)

def fix_mpu_reqs(env):
	size_info_file = find_sizeinfo_file()[0] # Fix find_sizeinfo_file func to return the file path
	sizeinfo = parse_sizeinfo(size_info_file)
	linker_file = env["LD_OVERLAY"].replace("overlay","ld")  #Fix ld file path

	adjust_sections(linker_file, sizeinfo["sections"])


directory = "autogen"
env = buildutils.load_project_meta()
phase = find_phase(env)
print("We are in phase:" + str(phase))


if phase ==0:
	if len(sys.argv) > 1:
		print("Command-line arguments provided:", sys.argv[1:])
		env = {}
		env["LD_OVERLAY"] = sys.argv[1]
		buildutils.save_project_meta(env)
		if not os.path.exists(directory):
			os.makedirs(directory)
	else:
		print("Project not initialized, please specify the overlay file")
		print("autogen.py ./scripts/mps2_m3.overlay")
		exit()

if phase ==1:
	run_setupld(env, 20)

if phase ==2:
	run_setupld(env, get_num_compartments())

if phase ==3:
	linker_file = env["LD_OVERLAY"].replace("overlay","ld")
	fix_mpu_reqs(env)
	if 'LD_MOD_TIME' in env:
		current_mod_time = os.path.getmtime(linker_file)
		if env["LD_MOD_TIME"] == current_mod_time:
			print("Linker script has not been modified!!!")
		else:
			print("Linker script has been modified after phase 3. Run from start again.")
		exit()

	fix_mpu_reqs(env)
	env["LD_MOD_TIME"] = current_mod_time = os.path.getmtime(linker_file)
	print("Generated new linkerscript.")
	print("Changed metadata: ", env)
	buildutils.save_project_meta(env)

	




