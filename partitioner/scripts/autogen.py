#!/usr/bin/python3
import subprocess
import buildutils
import os
import sys
import re
from pathlib import Path

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
	sizeinfo = parse_sizeinfo(files[0])

	total_partitioned_code =0
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
	except subprocess.CalledProcessError as e:
		print("Error occurred while running the command:")
		print(e.stderr)

	
	

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
		print("autogen.py ./scripts/mps2_m3.ld.overlay")
		exit()

if phase ==1:
	run_setupld(env, 20)

if phase ==2:
	run_setupld(env, get_num_compartments())

if phase ==3:
	#TODO: Run setup LD with fixup.
	print("Errrrrr")
	




