# !/user/bin/env python
# -*- coding:utf-8 -*-
# Author: Aolin Ding
"""
python file
"""

import sys
import time
import string
import pandas as pd
import argparse

parser = argparse.ArgumentParser(description='Binary address calculator')
parser.add_argument('--mapping_results', '-mr', default='All-memory-mapping', type=str, help='Name of control model binary')
parser.add_argument('--control_model', default='All', type=str, help='control models to output results')
args = parser.parse_args()

INPUT_DIRECTORY = './memory_mapping/{}.csv'.format(args.mapping_results)
OUTPUT_DIRECTORY ='./memory_mapping/'


if __name__ == "__main__":
    controllers_df = pd.read_csv(INPUT_DIRECTORY, sep=';')
    if args.control_model != 'All':
        controllers_df = controllers_df[controllers_df['Control_model'] == args.control_model]
    f = open(OUTPUT_DIRECTORY + args.control_model + '_output.txt', 'a+')

    for idx, row in  controllers_df.iterrows():
        f.write('$$\n')
        f.write('PIDType##{}\n'.format(row['Controller_type']))
        f.write('FUNC##function_{}\n'.format(row['Function_address']))
        f.write('cur##{}\n'.format(row['Current']))
        f.write('ref##{}\n'.format(row['Reference']))
        f.write('err##{}\n'.format(row['Error']))
        f.write('out##{}\n'.format(row['Output']))
        if row['p'] != '-':
            f.write('p##{}\n'.format(row['p']))
        if row['i'] != '-':
            f.write('i##{}\n'.format(row['i']))
        if row['d'] != '-':
            f.write('d##{}\n'.format(row['d']))
        if row['ff'] != '-':
            f.write('ff##{}\n'.format(row['ff']))
        f.write('GROUNDTRUTH##{}\n'.format(row['Groundtruth']))
    f.write('$$')
    f.close()

