# !/user/bin/env python
# -*- coding:utf-8 -*-
# Author: Aolin Ding
"""
This is a temporary checking script for the correctness of results
"""

import pandas as pd

model_df = pd.read_csv('./All-memory-mapping.csv', dtype = 'str')
for k in range(2, 11):
    print(model_df.iloc[k])
    # k + 22 -> copter compares with plane
    # k + 33 -> copter compares with sub
    # k + 11 -> copter compares with heli
    for i in range(24):
        try:
            if model_df.iloc[k, i][0] == '8' and model_df.iloc[k+11, i][0] == '8':
                diff = int(model_df.iloc[k, i], 16) - int(model_df.iloc[k+11, i], 16)
                print(i, '   ', diff)
            else:
                print('*')
        except:
            print()
