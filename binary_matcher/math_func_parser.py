import os
import regex as re

def get_math_funcs(func_range_filename: str):
    assert os.path.exists(func_range_filename)
    math_func_regex = 'sinf|cosf|atanf|tanf'
    #unknown_func_regex = '(null)?sub_[0-9a-fA-F]+'
    with open(func_range_filename, 'r') as f:
        for line in f.readlines():
            # first index will always be 11 ('FUNCRANGE##'), but for readability used line.find('##')+2
            func = line[line.find('##')+2:line.find('~~')]
            if re.match(math_func_regex, func):
                yield line