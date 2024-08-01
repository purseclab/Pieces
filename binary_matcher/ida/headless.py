import os
import sys
import subprocess


class HeadlessIDA:
    def __init__(self, idat_path: str, binary_path: str):
        assert os.path.exists(idat_path)
        assert os.path.exists(binary_path)
        self.idat = idat_path
        self.bin = binary_path

    def create_new_database(self, timeout=300) -> int:
        cmd = f"sudo {self.idat} -B -P+ \"{self.bin}\""
        print("Running command: " + cmd)
        proc = subprocess.Popen(cmd, shell=True)
        proc.wait(timeout=timeout)
        print("Created packed IDA database")
        return proc.returncode
    
    def execute_script(self, script_path: str, recompile_db=False, timeout=300):
        assert os.path.exists(script_path)

        if recompile_db:
            cmd = f"sudo {self.idat} -c -A -S\"{script_path}\" -P+ \"{self.bin}\""
        else:
            cmd = f"sudo {self.idat} -A -S\"{script_path}\" \"{self.bin}.idb\""
        print("Running command: " + cmd)
        
        proc = subprocess.Popen(cmd, shell=True)
        proc.wait(timeout=timeout)
        return proc.returncode
    

# if __name__ == "__main__":
#     ida = HeadlessIDA("/opt/idapro-8.4/idat", "../data/copter_augdump.elf")
#     # ida.execute_script("/opt/idapro-8.4/python/3/functionRangeDump.py")
#     print(ida.execute_script("./scripts/apply_sigfile.py"))
#     print(ida.execute_script("./scripts/func_range_dump.py"))
