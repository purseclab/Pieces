
import idc
import idaapi
import idautils

import os
import csv
import sys
import json
import logging


logging.basicConfig(filename="testing.log", filemode='w', level=logging.NOTSET)


def get_all_function_features() -> list:
    features = []
    for segment_ea in idautils.Segments():
        for function_ea in idautils.Functions(segment_ea, idc.get_segm_end(segment_ea)):
            features.append(extract_function_features(function_ea))
    return features
            


def extract_function_features(function_ea) -> dict:
    function_end_ea = idc.get_func_attr(function_ea, idc.FUNCATTR_END)
    
    features_dict = {}
    features_dict['name'] = idaapi.get_func_name(function_ea)
    features_dict['start_addr'] = function_ea
    features_dict['end_addr'] = function_end_ea
    features_dict['num_inst'] = sum(1 for _ in idautils.FuncItems(function_ea))
    features_dict['num_bb'] = idaapi.FlowChart(idaapi.get_func(function_ea)).size
    features_dict['add'] = 0
    features_dict['sub'] = 0
    features_dict['mul'] = 0
    features_dict['div'] = 0
    features_dict['branch'] = 0
    features_dict['loop'] = 0

    # if PROCESSOR_NAME.startswith('ARM'):
    ea = function_ea
    while ea <= function_end_ea:
        instr_mnem = idc.print_insn_mnem(ea)
        if instr_mnem.startswith('VADDPL.F32') or instr_mnem.startswith('FADD') or instr_mnem.startswith('VADD.F32'):
            features_dict['add'] += 1
        elif instr_mnem.startswith('VSUBPL.F32') or instr_mnem.startswith('FSUB') or instr_mnem.startswith('VSUB.F32'):
            features_dict['sub'] += 1
        elif instr_mnem.startswith('VMULPL.F32') or instr_mnem.startswith('FMUL') or instr_mnem.startswith('VMUL.F32'):
            features_dict['mul'] += 1
        elif instr_mnem.startswith('VDIVPL.F32') or instr_mnem.startswith('FDIV') or instr_mnem.startswith('VDIV.F32'):
            features_dict['div'] += 1
        elif instr_mnem.startswith('VMRS') or instr_mnem.startswith('B'):
            features_dict['branch'] += 1
        elif instr_mnem.startswith('B'): ## TODO: this has to be a bug, this condition can never be met because of the preceeding condition ^^^
            inst = idautils.DecodeInstruction(ea)
            assert isinstance(inst, idaapi.insn_t)
            for operand in inst.ops:
                assert isinstance(operand, idaapi.op_t)
                if operand.type is not idaapi.o_void and operand.addr < ea:
                    features_dict['loop'] += 1
        else:
            pass

        ea = idc.next_head(ea)

    return features_dict


# replaces process_func.py, matches controller features with JSON heuristics
def matches_ctrl_features(features: dict, ctrl_heuristics: dict) -> bool:
    for feat in features:
        try:
            if features[feat] < ctrl_heuristics[f'{feat}_min'] or features[feat] > ctrl_heuristics[f'{feat}_max']:
                return False
        except KeyError:
            continue
    return True
    

def generate_func_gdl(function_features: dict):
    idaapi.gen_flow_graph(f"data/gdl/{function_features['name']}", function_features['name'], None, 
                          function_features['start_addr'], function_features['end_addr'], idaapi.CHART_GEN_GDL)       


if __name__ == "__main__":
    idc.auto_wait()

    # line = str(fun_dict['name']) + ', ' + str(fun_dict['start_addr']) + ', ' + str(fun_dict['end_addr']) + ', ' + str(fun_dict['num_inst']) + ', ' + str(fun_dict['num_bb']) + ', ' + str(fun_dict['div']) + ', ' + str(fun_dict['add']) + ', ' + str(fun_dict['sub']) + ', ' + str(fun_dict['mul']) + ', ' + str(fun_dict['branch']) + ', ' + str(fun_dict['loop'])
    # w.write(line + '\n')
    # "GenFuncGdl(\"" + row[0] + "\" , \"" + row[0] + "\" ," + row[1] +  "," + row[2] + " ,CHART_GEN_GDL);"

    try:
        with open('./data/p_ctrl.json', 'r') as ctrl_json:
            p_ctrl_heuristics = json.load(ctrl_json)
            logging.debug(f"P controller heuristics: {p_ctrl_heuristics}")
        with open('./data/p2_ctrl.json', 'r') as ctrl_json:
            p2_ctrl_heuristics = json.load(ctrl_json)
            logging.debug(f"P2 controller heuristics: {p2_ctrl_heuristics}")
        with open('./data/i_ctrl.json', 'r') as ctrl_json:
            i_ctrl_heuristics = json.load(ctrl_json)
            logging.debug(f"I controller heuristics: {i_ctrl_heuristics}")
        with open('./data/pid_controller.json', 'r') as ctrl_json:
            pid_ctrl_heuristics = json.load(ctrl_json)
            logging.debug(f"PID controller heuristics: {pid_ctrl_heuristics}")
        with open('./data/pidff_controller.json', 'r') as ctrl_json:
            pidff_ctrl_heuristics = json.load(ctrl_json)
            logging.debug(f"PIDFF controller heuristics: {pidff_ctrl_heuristics}")
        
        all_functions = get_all_function_features()
        with open(f"{idaapi.get_input_file_path()}_all.csv", "w") as f:
            writer = csv.DictWriter(f, fieldnames=all_functions[0].keys())
            writer.writeheader()
        
            for function_features in all_functions:
                logging.debug(f"{function_features}")
                is_controller_function = \
                    matches_ctrl_features(function_features, p_ctrl_heuristics) or \
                    matches_ctrl_features(function_features, p2_ctrl_heuristics) or \
                    matches_ctrl_features(function_features, i_ctrl_heuristics) or \
                    matches_ctrl_features(function_features, pid_ctrl_heuristics) or \
                    matches_ctrl_features(function_features, pidff_ctrl_heuristics)
                if is_controller_function:
                    logging.debug(f"Matching controller: {function_features['name']}")
                    # generate GDLs
                    generate_func_gdl(function_features)
                    # setup BB extract
                    # feature merge
                    writer.writerow(function_features)
            
    except Exception as e:
        logging.error(e)
        idaapi.qexit(2)
    finally:
        idaapi.qexit(0)
    
    idaapi.qexit(0)
