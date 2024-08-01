# DisPatch
This project describes the symbolic execution and abstract syntax tree (AST) generation in the DisPatch paper. 

**Key idea:** From the static analysis results, we already locate the addresses and basic blocks for possible mathematical controllers. We take these start address, end address and avoid addresses as inputs to configure the [angr](https://github.com/angr/angr)  engine and run symbolic execution. We apply [anytree](https://anytree.readthedocs.io/en/latest/) to convert the symbolic execution result into an abstract syntax tree (AST) representation. Then, we identify the mathematical controller by comparing its AST representation with AST templates of standardized controller algorithms (e.g., PID).

## Installation
Virtualenv or conda is suggested to install and configure this demo environment to prevent potential package conflicts . The development environment is based on python 3.6.9. For instance,  **create** a new virtualenv to configure the installation.
- `sudo apt-get install python3-dev libffi-dev build-essential virtualenvwrapper graphviz`
- `virtualenv --python=$(which python3) angr`

To **activate** the *angr* virtualenv and install the DisPatch dependencies:
- `cd [directory of your new angr virtual environment]`
- `source bin/activate`
- `pip install -r /path/to/requirements.txt`

To **deactivate** the *angr* virtualenv:
 - `deactivate`

## Binaries
-  ARM: ./stripped_binary/copter_sbin
-  ARM: ./stripped_binary/heli_sbin
-  ARM: ./stripped_binary/plane_sbin
-  ARM: ./stripped_binary/sub_sbin

## Candidate function list and address info
The auto-search scripts will take the static analysis results as inputs to automatically generate the ASTs for candidate functions. In more details, it needs

- start_address: the instruction address of the  candidate math function blocks to start angr symbolic exection
- target_address: the instruction address to stop the symbolic execution
- avoid_address: the unneeded branches during the symbolic execution (e.g., exception conditions or non-math controller branches)
- skip_address: the addresses of unsupported instructions for angr execution (e.g., function calls)

The purpose of this configuration is to precisely guide the symbolic execution and generate valid results.

## Run the experiment
The current implementation runs the auto-search process based on the aforementioned address information. These info will be automatically collected by the candidate function filtering.

After activate *angr* virtualenv, run the experiment as follows:
- `python auto_search.py`

Note:
- the *auto_search.py* will load the csv file and use the candidate function info to configure the angr symbolic execution and generate ASTs
- All AST results will be saved in `./temp_AST_results/` as png images

We can use the same scripts for other math controllers (e.g., sqrt, PID) and control models (e.g., copter, plane, sub) by setting the arguments. For instance, we can search all the mathmatical controller (e.g., sqrt, PID) for Copter control model by using:

- `python auto_search.py --binary_dir ./stripped_binary/copter_sbin --func_list copter`

Similarly, we can search the sqrt controller for Sub control model by using:

- `python auto_search.py --binary_dir ./stripped_binary/sub_sbin --func_list sub`


## Bridging steps
In this section, we introduce how to use the candiate function filtering and execution paths (e.g., start address, end address, avoid address) in symbolic execution step by step. We take the ArduPilot copter model as an example.

1. Go to branch `patch-1` (Sri's branch)
2. Go to repository `dispatch/angr-scripts/paths_results/Copter`
3. Copy all the `_final.csv` files in different subfolders (i.e., get_i, get_p, run_xy_controller, sqrt_controller) and *merge* them into one csv file, then rename this combined csv file as `copter.csv` and put into branch `aolin` - repository `dispatch/angr-scripts/func_info_results/`
4. Copy all the static result files in different subfolders (i.e., `get_p/get_p.csv`, `get_i/get_i.csv`, `run_xy_controller/run_xy_controller.csv`, `run_z_controller/run_z_controller.csv`, `sqrt_controller/sqrt_controller.csv`) and *merge* them into one csv file, then rename this combined csv file as `copter_all.csv` and put into branch `aolin` - repository `dispatch/angr-scripts/func_info_results/all_static_results/`
5. Go to branch `aolin` and run the `auto_search.py` scripts as instructed above

> Note:
> * In step 3, we merge all the valid paths for all candidate functions into one file, which includes the function names, start_addresses, end_addresses, avoid_addresses. This combined file will be the input file for `auto_search.py` to iterate and run symbolic execution for every row
> * In step 4, we merge all the static result files into one file, which includes all the candidate function and their static info. This combined file will be another input file for `auto_search.py`, and the reason is as follows: angr can only deal with a relatively small callee function within the candidate function during the symbolic execution. We filter all the candidate function and find out those relatively small functions (which might be the subcontroller like get_p, get_d, get_ff). If they get called during symbolic execution, we *do not* skip them because angr can handle them. If there are some other relatively large functions or irrelecant internal functions being called during symbolic execution, we have to skip them because angr can not handle them. This solution is implemented in function `get_skip_addr()` of `auto_search.py`, which gives us the better execution results when facing this native limitation of angr

## Corner cases and troubleshoots
When checking all the candidate functions using auto-search scripts, there might have very few corner cases that will trigger the segmentation fault error. This error will cause program to crash and can not be handled easily by using try-except machanism in python scripts ([see reference](https://stackoverflow.com/questions/27950296/using-try-to-avoiding-a-segmentation-fault)). The solution is to delete this candidate function from the input list and rerun the auto-search scripts.

In very few conditions, the angr configuration (e.g., start address, end address, avoid address of candidate function) will cause the symbolic execution run forever, which can not be handled properly by the timeout alarm signal in auto search scripts due to the implementation limitation of angr itself. The solution is to 1) exit and rerun the experiment OR 2) delete this candidate function from input list and rerun the experiment.

## Convert memory mapping results to standard format
After we finish the memory mapping, we save the results in `memory_mapping` folder. We can run the script *standard_output.py* to convert the file `All-memory-mapping.csv` into the standard output format, which can be used for LLVM and further patching purpose.

Use the input argument `control_model` to generate the results for all or for single specific control model (e.g., copter, plane, heli, sub):

- `python standard_output.py --control_model All`
- `python standard_output.py --control_model Copter`
- `python standard_output.py --control_model Plane`
- `python standard_output.py --control_model Sub`
- `python standard_output.py --control_model Heli`

