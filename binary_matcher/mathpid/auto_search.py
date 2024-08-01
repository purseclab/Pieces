import sys
import time
import signal
import string
import angr
from angr.block import CapstoneInsn, CapstoneBlock
import monkeyhex
import pandas as pd
import argparse

from .ASTGenerator import ASTGenerator
from anytree import Node
from anytree.exporter import DotExporter


class TimeOutException(Exception):
    pass


def alarm_handler(signum, frame):
    raise TimeOutException()


def symbolic_execute(p, start_addr, avoid_addr, target_addr):
    state = p.factory.blank_state(addr=start_addr + 1)
    simgr = p.factory.simulation_manager(state)
    simgr.explore(find=target_addr - 1, avoid=[i + 1 for i in avoid_addr])
    print(simgr)
    return simgr


def convert_into_AST(root_expr, image_name=None):
    root = Node(root_expr)
    sym_root = ASTGenerator(root)
    sym_tree = sym_root.generate_ast(root, image_name)
    return sym_tree


def attach_node(root_expr, node_expr, position):
    root_ast = convert_into_AST(root_expr)
    node_ast = convert_into_AST(node_expr)
    new_ast = ASTGenerator(root_ast)
    new_ast.merge_ast(root_ast, [node_ast], [position], False)
    new_ast.store_ast_image(root_ast, 'get_i.png')
    return root_ast


def nothing(state):
    pass


def get_skip_addr(p, addr, size, df):
    # cfg = p.analyses.CFGFast()
    insn_bytes = p.loader.memory.load(addr, size)
    insns = []
    for cs_insn in p.arch.capstone.disasm(insn_bytes, addr):
        insns.append(CapstoneInsn(cs_insn))
    block = CapstoneBlock(addr, insns, 0, p.arch)
    skip_addr = []
    df = df[df['num_bb'] <= 5] # angr can not deal with large nested function calls
    for ins in block.insns:
        if ins.mnemonic == 'bl': # when angr observes a function call
            callee_func_addr = ins.op_str[1:]
            if callee_func_addr not in list(df['start_addr']):
                skip_addr.append(ins.address)
    return skip_addr


def find_loading_ins_constants(p, addr, size):
    '''
    Some parameters are calculated with constant values. We can track them by
    locating numerical and loading operations involving constant values.
    '''
    insn_bytes = p.loader.memory.load(addr, size)
    insns_loading_const = []
    for cs_insn in p.arch.capstone.disasm(insn_bytes, addr):
        ins = CapstoneInsn(cs_insn)
        if ins.mnemonic == 'vmov.f32' and '#' in ins.op_str[1:]:
            # loading on constants values
            insns_loading_const.append(ins.address)
    return insns_loading_const


def auto_search_ast(bin_path, func_results_path, all_static_results_path, output_path=None):
    p = angr.Project(bin_path,
                     main_opts={
                         'backend': 'elf',
                         'arch': 'ARMCortexM'
                     })
    print('The binary is being disassembled in the architecture:', p.arch)
    candidate_func_df = pd.read_csv(func_results_path)
    candidate_func_df['avoid_addr'] = candidate_func_df['avoid_addr'].apply(eval)

    all_static = pd.read_csv(all_static_results_path)

    count = 0 # number of functions
    found_path_count = 0
    active_path_count = 0
    start_time = time.time()

    for i in range(len(candidate_func_df.index)):
        start_addr = int(candidate_func_df.loc[i, 'start_addr'], 16)
        end_addr = int(candidate_func_df.loc[i, 'end_addr'], 16)
        avoid_addr = [int(x, 16) for x in candidate_func_df.loc[i, 'avoid_addr']]
        skip_addr = get_skip_addr(p, start_addr, end_addr - start_addr, all_static)
        const_op_addr = find_loading_ins_constants(p, start_addr, end_addr - start_addr)
        print("==============================================")
        print("Checking function {} at address {}".format(count, hex(start_addr)))
        print("==============================================")
        print("start address:", hex(start_addr))
        print("end address:", hex(end_addr))
        print("avoid address:", [hex(addr) for addr in avoid_addr])
        print("skip address:", [hex(addr) for addr in skip_addr])
        print("constants loading address:", [hex(addr) for addr in const_op_addr])

        for addr in skip_addr:
            p.hook(addr=addr + 1, hook=nothing, length=4)

        # handle timeout condition
        signal.signal(signal.SIGALRM, alarm_handler)
        signal.alarm(30)

        count += 1
        try:
            simgr = symbolic_execute(p, start_addr, avoid_addr, end_addr)
            found_path_count += len(simgr.found)
            active_path_count += len(simgr.active)
            if len(simgr.found) > 0:
                np = simgr.found[0]
                sym_expr =  np.regs.s16
                if output_path is None: output = None
                else: output = output_path + hex(start_addr) + '_func' + str(count) + '_found_results.png'
                convert_into_AST(sym_expr, output)
            if len(simgr.active) > 0:
                np = simgr.active[0]
                sym_expr =  np.regs.s0
                if output_path is None: output = None
                else: output = output_path + hex(start_addr) + '_func' + str(count) + '_active_results.png'
                convert_into_AST(sym_expr, output)
            print()
        except TimeOutException:
            print("Time Out!\n")
            pass
        except Exception as error_message:
            print(error_message)
            print('Incorrect Path!\n')
            pass
        finally:
            signal.alarm(0)
    end_time = time.time()
    print("The total searching takes {} seconds.".format(end_time - start_time))
    print("Run symbolic execution in {} candidate functions, found {} possible paths and {} active paths."
          .format(count, found_path_count, active_path_count))

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='Binary address calculator')
    parser.add_argument('--binary_dir', default='./stripped_binary/copter_sbin', type=str, help='Name of control model binary')
    parser.add_argument('--func_list', default='copter', type=str, help='Path of candidate function list and addresses')
    args = parser.parse_args()
    print(args)

    BINARY_DIRECTORY = args.binary_dir
    OUTPUT_DIRECTORY ='./temp_AST_results/'
    FUNCTION_DIRECTORY = './func_info_results/{}.csv'.format(args.func_list)
    ALL_STATIC_DIRECTORY = './func_info_results/all_static_results/{}_all.csv'.format(args.func_list)

    sym_expr = auto_search_ast(FUNCTION_DIRECTORY)
    # sym_expr_get_i = auto_search_ast('./func_info_results/{}_get_i.csv'.format(args.control_model))
    # sym_expr_error = auto_search_ast('./func_info_results/PID_{}_error.csv'.format(args.control_model))

