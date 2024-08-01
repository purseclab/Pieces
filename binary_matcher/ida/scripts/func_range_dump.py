########################################################
# This program is to generate function (and its code)  #
# ranges of a binary with the help of IDA Pro 7.5      #
# Author: Jizhou Chen JZchen@purdue.edu                #
# Version: 10/23/2021                                  #
########################################################

import idc
import idaapi
import idautils

MAX_DATA_CLUSTER_COUNT = 500	#code discovered after MAX_DATA_CLUSTER_COUNT lines of data at the end of .text section will be treated as data

def isSwitch(ea):		#check if instruction at ea is swtich statement
	comp = idc.GetDisasm(ea).split()
	if comp[0].startswith("TBB") or comp[0].startswith("TBH"): return True
	if comp[0].startswith("LDR") and comp[1].startswith("PC"): return True
	return False


if __name__ == "__main__":
	idc.auto_wait()

	mode = "w"
	with open(idaapi.get_input_file_path()+"_functionRange.txt", mode) as list:
		text_end_ea = idaapi.get_segm_by_name(".text").end_ea
		for segment_ea in idautils.Segments():
			for function_ea in idautils.Functions(segment_ea, idc.get_segm_end(segment_ea)):
				if function_ea > text_end_ea: continue	#only print code info in text section
				data_cluster_count = 0		#counts number of lines in a data block
				func_size = idaapi.calc_func_size(idaapi.get_func(function_ea))		#length of function
				thumb_val = idc.get_sreg(function_ea,'T')
				if thumb_val == 1: mode = "THUMB"
				else: mode = "ARM"
				list.write("FUNCRANGE##"+idaapi.get_func_name(function_ea)+"~~"+str(hex(function_ea))+"~~"+str(hex(function_ea+func_size - 1))+"~~"+mode+"\n")
				if mode == "w": mode = "a"
				for (start_ea, end_ea) in idautils.Chunks(function_ea):
					code_end = False		#flag indicating if the end of current code block has been encountered
					reading_jump_table = False	#flag indicating if the data being read belongs to switch
					last_head = start_ea
					#list.write(hex(start_ea)[:-1]+" - ")
					for head in idautils.Heads(start_ea, end_ea):
						if idaapi.is_data(idaapi.get_full_flags(head)): data_cluster_count = data_cluster_count + 1
						if idaapi.is_code(idaapi.get_full_flags(last_head)) and idaapi.is_data(idaapi.get_full_flags(head)):	#from code to data
							if data_cluster_count >= MAX_DATA_CLUSTER_COUNT: last_head = head; continue
							if isSwitch(last_head):		#current "data" is jump table, considered as code. Don't end code part.
								reading_jump_table = True
							else:			#from code to data -> end code part
							#	list.write(hex(head-1)[:-1]+"\n")
								code_end = True
						elif idaapi.is_data(idaapi.get_full_flags(last_head)) and idaapi.is_code(idaapi.get_full_flags(head)): #from data to code
							if data_cluster_count >= MAX_DATA_CLUSTER_COUNT: last_head = head; continue		#keep going, tread code as data
							else: data_cluster_count = 0
							if reading_jump_table == True:	#end of jump table
								reading_jump_table = False
							else:		#start code part
							#	list.write("\t"+hex(head)[:-1]+" - ")
								code_end = False
							
						last_head = head
				
					#if code_end == False:		#if the last inst is code, close the last code part
					#	list.write(hex(function_ea+func_size - 1)[:-1]+"\n")
							
	print ("done!")
	idaapi.qexit(0)