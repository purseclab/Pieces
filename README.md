
# BIND Semantic Signature Generator

## Prerequisites Setup

### Ubuntu Specific

Following may be necessary on Ubuntu systems:

```sh
sudo apt-get install libsqlite3-dev

cd /usr/include
sudo ln -s asm-generic/ asm
```

### LLVM 12

LLVM-12 is recommended as the version of KLEE and SVF we will 
use were tested on LLVM 12.

You can use build_llvm12.sh

Set to path for convenience:
```sh
export PATH=$(realpath llvm_12/llvm-12.0.0.obj/bin):$PATH
```

### Looper KLEE

You will need this custom version of KLEE:

```sh
git clone git@github.com:akulpillai/klee.git --branch looper
```
When building use llvm 12:
```sh
cd klee
mkdir build 
cd build
cmake -DLLVM_LIBS=$(realpath ../../llvm_12/llvm-12.0.0.obj/lib/libLLVM-12.so) -DCMAKE_BUILD_TYPE=Release ..
make -j12
```

### Build SVF Analysis
```sh
git clone git@github.com:purs3lab/ArduSVF.git
export LLVM_DIR=$(realpath llvm_12/llvm-12.0.0.obj)
cd ArduSVF
./build.sh debug
```

## Partitioner Setup

You will need the bitcode file of the firmware you want to generate signatures for.


Set the following environment variables:
```sh
export PATH=$(realpath klee/build/bin):$PATH
export SYMEX=$(realpath ArduSVF/Debug-build/bin/symex)
export SVF=$(realpath ArduSVF/Debug-build/bin/svf-ex)
```


## Run Partitioner


Make sure to update the bitcode file path and SVD file path in the following.
```sh
cd partitioner

cat << 'EOF' > conf.json
{
  "firmware": {
    "bc": "/home/akul/arducopter.bc",
    "symex_bc": "/home/akul/arducopter.bc",
    "platform": "stm32f4",
    "svd_file": "/workdisk/akul/fire2/Pieces/partitioner/cmsis-svd-data/data/STMicro/STM32F401.svd",
    "svd": "STM32F401",
    "os": "unknown"
  }
}
EOF

./init_files.sh

python3 classify.py conf.json
```


