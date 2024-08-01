import idautils
import os
import idc
import idaapi
from idaapi import Form, Choose
from glob import glob
import csv
import fileinput
import logging


logging.basicConfig(filename="BB_extract.log", filemode='w', level=logging.NOTSET)


def all_csv_files(path: str):
    EXT = "*.csv"
    all_csv_files = [file
                 for path, subdir, files in os.walk(path)
                 for file in glob(os.path.join(path, EXT))]
    return all_csv_files


def all_valid_paths(path: str):
    EXT = idaapi.get_input_file_path() + "/gdl/*.validpaths"
    all_valid_path = [file
                 for path, subdir, files in os.walk(path)
                 for file in glob(os.path.join(path, EXT))]
    return all_valid_path


def bb_addr(csv_files, res, path):
    dicty = {}

    try:
        with open(csv_files) as csv_file:
            csv_reader = csv.reader(csv_file, delimiter=',')
            line_count = 0
            for row in csv_reader:
                if line_count == 0:
                    line_count += 1
                else:
                    dicty[row[1]] = bb_lst(row[1])
    except Exception as e:
        logging.error("could not open csv(s)")

    valid_path_list = all_valid_paths(path)
    logging.debug(valid_path_list)

    final_opt = []
    with open(csv_files) as csv_filex:
        csv_readerx = csv.reader(csv_filex, delimiter=',')
        line_count1 = 0
        for rowx in csv_readerx:
            if line_count1 == 0:
                line_count1 += 1
            else:
                for i in valid_path_list:
                    if rowx[0] in i:
                        with open(i) as valid_path:
                            reader = csv.reader(valid_path, delimiter=',')
                            path_lst = []
                            for row in reader:
                                if "VALID_PATH" in (', '.join(row)):
                                    y = (', '.join(row))
                                    path_lst.append(y.split(":")[1].split("[")[1].split("]")[0].split(","))
                                vld_path_lst = [list(t) for t in set(tuple(element) for element in path_lst)]
                                for x2 in vld_path_lst:
                                    num_nodes = len(dicty[rowx[1]])
                                    avoid_addr = []
                                    for i2 in range(num_nodes):
                                        if i2 not in [int(i.split("'")[1]) for i in x2] :
                                            avoid_addr.append(dicty[rowx[1]][i2])
                                    final_opt.append([rowx[0], x2, rowx[1], rowx[2], avoid_addr])
    
    [res.append(x) for x in final_opt if x not in res]


def bb_lst(a):
    f = idaapi.get_func(int(a,base=16))
    func = idaapi.FlowChart(f)
    lst = []
    for bb in func:
        lst.append(hex(bb.start_ea))
    return lst


if __name__ == "__main__":
    idc.auto_wait()

    try:
        # csv_files = all_csv_files()
        # res = []
        # for x in csv_files:
        #     bb_addr(x, res)
        res = []

        logging.debug(idaapi.get_input_file_path() + '_all.csv')
        try:
            bb_addr(idaapi.get_input_file_path() + '_all.csv', res)
        except Exception as e:
            logging.error(e.with_traceback())


        PATH = os.path.dirname(idaapi.get_input_file_path())

        writefile = idaapi.get_input_file_path() + '_final.csv'
        logging.debug(writefile)
        fo = open(writefile, 'w', newline='')
        spamwriter = csv.writer(fo)
        spamwriter.writerow(["name", "valid_path", "start_addr", "end_addr", "avoid_addr"])
        spamwriter.writerows(res)
    except Exception as e:
        logging.error(e.with_traceback())
        idaapi.qexit(69)

    idaapi.qexit(0)