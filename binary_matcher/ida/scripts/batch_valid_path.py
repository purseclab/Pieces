import os, sys, subprocess, logging

def batch_valid_path(gdl_path: str):
    assert os.path.exists(gdl_path)
    flist = []
    for i in os.listdir(gdl_path):
        if i.endswith(".gdl"):
            fl = i.split(".")[0:-1]
            fn = ""
            for l in fl:
                fn = fn + l
            flist.append(fn)
            
        for fn in flist:
            subprocess.run(f"python3 {os.path.dirname(__file__)}/valid_path.py {gdl_path}/{fn}.gdl >> {gdl_path}/{fn}.validpaths", shell=True)



if __name__ == "__main__":
    batch_valid_path(sys.argv[1])