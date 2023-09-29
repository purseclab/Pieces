# Pieces
**Pieces** is a highly programmable language-agnostic automatic program compartmentalization framework. Pieces can be programmed to partition programs based on various criteria, including method of isolation between compartments. For queries, either use GitHub issues (preferred method), or email the authors ([Arslan Khan](mailto:khan253@purdue.edu?subject=[GitHub]%20Source%20Han%20Sans)).


## FuzzSGX dependencies:
FuzzSGX is written on top of the following tools:
1. SVF v2.2+
1. Checked-C 12 (Also provided as builtin binary)

Please see the corresponding projects for installation details. Feel free to ask any questions about the installation of any of the projects.

## Source Code Details.
The source code is organized into three big submodules:

1. Partitioner: The main static analyzer that converts a monolithic firmware to a partitioned firmware.
2. Bridge: libBridge that helps efficient communication between different compartments.
3. Monitors: Runtime to support isolation during compartments. 
4. Compiler: Example compiler with frontend changes required for Pieces.

## How to run:
Once the dependencies are built, you can invoke the framework using a simple python script as follows:
```python 
from llvm import Compiler
import ec_loader
compiler = Compiler()
compiler.analyze("./test.bc")
firmware = ec_loader.Firmware(input["firmware"])
```

We also provide a front-end application that can be used as a command line tool.

## Supporting a new project:
TODO: Will add this soon too. 

## Citing this work.
Pieces is built using a bunch of works and many more are on the way..

If you like or use our work. Please cite us using:
1. [EC](https://ieeexplore.ieee.org/document/10179285)
```@INPROCEEDINGS{10179285,
  author={Khan, Arslan and Xu, Dongyan and Tian, Dave Jing},
  booktitle={2023 IEEE Symposium on Security and Privacy (SP)}, 
  title={EC: Embedded Systems Compartmentalization via Intra-Kernel Isolation}, 
  year={2023},
  volume={},
  number={},
  pages={2990-3007},
  doi={10.1109/SP46215.2023.10179285}}

```
2. [CRT-C](https://ieeexplore.ieee.org/document/10179388)
```
@INPROCEEDINGS{10179388,
  author={Khan, Arslan and Xu, Dongyan and Tian, Dave Jing},
  booktitle={2023 IEEE Symposium on Security and Privacy (SP)}, 
  title={Low-Cost Privilege Separation with Compile Time Compartmentalization for Embedded Systems}, 
  year={2023},
  volume={},
  number={},
  pages={3008-3025},
  doi={10.1109/SP46215.2023.10179388}}

```




Note: I am working on pushing code in the following few weeks.
