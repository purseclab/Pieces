import regex as re
from dataclasses import dataclass


@dataclass
class ControlVariable:
    name:str
    addr:int
    cxt_cs_addr:int = None


def parse_semantics(semantic_out: str):
    semantic_out = semantic_out[semantic_out.find('POS_X\n>>'):]
    semantic_out = semantic_out.split('\n\n\n')[:-1]

    semantics = {}
    for semantic in semantic_out:
        name, vars = semantic.split('\n', 1)
        var_list = re.finditer(">> [0-9a-zA-Z ]*: 0x[0-9a-fA-F]*(\n\t>> [0-9a-zA-Z ]*: 0x[0-9a-fA-F]*)?", vars)
        semantics[name] = []
        for var in var_list:
            matched = var.group(0)
            var_name = re.search(">> (.*):", matched).group(1)
            # print(var_name)
            var_addr = int(re.search(': 0x([a-fA-F0-9]+)', matched).group(1), 16)
            cxt_cs_addr = re.search("cxt cs: 0x([a-fA-F0-9]+)", matched)
            if cxt_cs_addr is not None:
                cxt_cs_addr = int(cxt_cs_addr.group(1), 16)

            semantics[name].append(ControlVariable(var_name, var_addr, cxt_cs_addr))
    return semantics
