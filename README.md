# Pieces
**Pieces** is a highly programmable language-agnostic automatic program compartmentalization framework. Pieces can be programmed to partition programs based on various criteria, including method of isolation between compartments. For queries, either use GitHub issues (preferred method), or email the authors ([Arslan Khan](mailto:khan253@purdue.edu?subject=[GitHub]%20Source%20Han%20Sans)).


## Dependencies:
Pieces is written on top of the following tools:
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
1. First install the listed dependencies. The installation is standard except for SVF, which will require adding a new SVF tool, which is provided at  ```./partitioner/llvm/SVF/Example/```. Either create a new tool, or simply replace the base example tool.
2. After installation you should tell the partitioner about the path of SVF in ```./partitioner/.env```
3. Build your project to bitcode. (Please ping the authors if you require any help on this). Alternatively, we provide bitcode for FreeRTOS for running purposes.
4. Invoke the run utility.
```
cd partitioner
./run.py ./rules/lwip.json
```
The output should look like:
```bash
WARN:Merging for shared data:pxCurrentTCB
WARN:Users:
WARN:	Clique: background_clique
WARN:	test_etsan
WARN:	Clique: background_clique
WARN:	vTaskSwitchContext
WARN:Merging for shared data:xSchedulerRunning
WARN:Users:
WARN:	Clique: background_clique
WARN:	vTaskEndScheduler
WARN:	Clique: background_clique
WARN:	prvAddNewTaskToReadyList
WARN:Merging for shared data:uxCriticalNesting
WARN:Users:
WARN:	Clique: background_clique
WARN:	vPortEnterCritical
WARN:	Clique: background_clique
WARN:	prvTaskExitError
WARN:Merging for shared data:SystemCoreClock
WARN:Users:
WARN:	Clique: background_clique
WARN:	SystemCoreClockUpdate
WARN:	Clique: background_clique
WARN:	vPortSetupTimerInterrupt
Number of compartments: 17
Instrumenting firmware.
Dissassembling ./out/temp.bc
```
run.py can give you an overview of how to use pieces programmatically as well. Pieces provides program analyses, including symbolic execution, which can be used for analyzing firmware, compartmentalization, and much more. A simple python script is shown below:
```python 
from llvm import Compiler
import ec_loader
compiler = Compiler()
compiler.analyze(input["firmware"])
firmware = ec_loader.Firmware(input["firmware"])
```

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
