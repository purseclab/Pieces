#!/usr/bin/python3
import subprocess
import buildutils
import os
env = buildutils.load_project_meta()


directory = "autogen"
if not os.path.exists(directory):
	os.makedirs(directory)


# Define the command and arguments
command = [
    "setupLD.py",
    "-n", "20",
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

