#!/usr/bin/python3
import subprocess
import buildutils
import os
env = buildutils.load_project_meta()
NUM_DEFAULT_COMPARMENTS = 20


directory = "autogen"
if not os.path.exists(directory):
	os.makedirs(directory)

#TODO: this file should be in the pickle
script_dir = os.path.dirname(os.path.abspath(__file__))
file_path = os.path.join(script_dir, "../out/.policy")

try:
	with open(file_path, "r") as file:
		line_count = sum(1 for _ in file)
	print(f"The file '{file_path}' contains {line_count} lines.")
except FileNotFoundError:
	print(f"The file '{file_path}' was not found.")
	line_count = NUM_DEFAULT_COMPARMENTS
except Exception as e:
	print(f"An error occurred: {e}")

# Define the command and arguments
command = [
	"setupLD.py",
	"-n", str(line_count),
	"-l", env["LD_OVERLAY"],
	"-c", directory + "/autogen_data.c",
	"-H", directory + "/autogen_heap.c"
]

# Run the command
try:
	result = subprocess.run(command, check=True, text=True, capture_output=True)
	print("Command output:")
	print(result.stdout)
except subprocess.CalledProcessError as e:
	print("Error occurred while running the command:")
	print(e.stderr)

