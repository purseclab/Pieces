#!/usr/bin/python2.7

import sys, getopt

def writeCodeSections(cpatch, num):
	print("Number of compartments:" +str(num))
	for i in range(num):
			cpatch.write("  .csection"+str(i) +" : \n")
			cpatch.write("  {\n")
			cpatch.write("_scsection"+str(i)+" = .;\n")
			cpatch.write("	  . = ALIGN(4);\n")
			cpatch.write("	  *(.csection"+str(i)+")\n")
			cpatch.write("	  . = ALIGN(4);\n")
			cpatch.write("_ecsection"+str(i)+" = .;\n")
			cpatch.write("  } > FLASH \n")

def writeDataSections(dpatch, num):
	for i in range(num):
			dpatch.write("  .osection"+str(i) +" : AT ( _sidata  + compartLMA)\n")
			dpatch.write("  {\n")
			dpatch.write("	  . = ALIGN(4);\n")
			dpatch.write("	  _sosection" +str(i) +" = .;\n")
			dpatch.write("	  *(.osection"+str(i)+")\n")
			dpatch.write("	  . = ALIGN(4);\n")
			dpatch.write("	  _eosection" +str(i) +" = .;\n")
#			dpatch.write("  }  > RAM AT > FLASH \n")
			dpatch.write("  }  > RAM \n")
			dpatch.write("compartLMA = compartLMA + SIZEOF(.osection"+str(i) +"); \n")

def main(argv):
	num = 0
	heapConfig = ""
	arch = "armv7m"
	try:
		opts, args = getopt.getopt(argv,"hn:l:o:c:H:a:")
	except getopt.GetoptError:
		print("test.py -i <inputfile> -o <outputfile>")
		sys.exit(2)
	for opt, arg in opts:
		if opt == '-h':
			print("test.py -i <inputfile> -o <outputfile>")
			sys.exit()
		elif opt in ("-n"):
			print(arg)
			num = int(arg)
		elif opt in ("-l"):
			overlay = arg
		elif opt in ("-c"):
			configFile = arg
		elif opt in ("-H"):
			heapConfig = arg
		elif opt in ("-a"):
			arch = arg

	outputFile = overlay.replace("overlay","ld")
	outFile = open(outputFile, "w")
	with open(overlay) as f:
		lines = f.readlines()
		for line in lines:
			if("datamarker-fixup::" in line):
					writeDataSections(outFile, num)
			elif("codemarker-fixup::" in line):
					writeCodeSections(outFile, num)
			else:
					outFile.write(line)

	prologue_string  = "#include <monitor.h> \n"
	if arch=="armv7m":
		prologue_string += "#include <arch/armv7m/arch.h> \n"
	prologue_string += "RTMK_DATA \n SEC_INFO comp_info[] = {"
	#CodeStart,CodeSize,DataStart,DataSize
	endstring = "};"
	f = open(configFile, "w")
	f.write(prologue_string)
	for i in range(num):
			f.write("{0,0,0,0,0,0,0,0}")#Offset
			if i!= (num -1):
					f.write(",")
	f.write(endstring)
	f.write("int code_base;")
	f.write("int code_size;")
	f.write("int data_base;")
	f.write("int data_size;")

	f.write("int total_secs = sizeof(comp_info) / sizeof(comp_info[0]);")

	if not heapConfig:
		return
	ha={}
#	with open("./rtmk.ha") as f:
#		lines = f.readlines()
#		for line in lines:
#			[comp, use] = line.split(":")
#			ha[comp] = use
			
	f = open(heapConfig, "w")
	#Find all users so we divide accordingly
	users = 0

#See how many compartments use dynamic memory
	for i in range(num):
			if str(i) in ha:
					users = users + 1
	for i in range(num):
			if str(i) in ha:
				f.write("__attribute__((section(\".osection"+ str(i)+"\"))) uint8_t ucHeap"+ str(i)+"[ configTOTAL_HEAP_SIZE/"+str(users) +"]; \n")

	f.write("uint8_t* ucHeap[] = {")
	for i in range(num):
			if str(i) in ha:
				f.write("ucHeap"+ str(i))
			else:
				f.write("NULL")
			if not i== num-1:
				f.write(",")
	f.write("}; \n")

	f.write("uint8_t ucHeapInitFlag["+str(num)+"]; \n")
	f.write("size_t xNextFreeByte ["+str(num)+"]; \n")
	f.write("size_t heapSize ["+str(num)+"] = {")
	for i in range(num):
#			if str(i) in ha:
#				f.write("configTOTAL_HEAP_SIZE/"+str(users))
#				if not i == num-1:
#					f.write(",")
#			else:
				f.write("0")
				if not i == num-1:
					f.write(",")
	f.write("}; \n");

				


	

if __name__ == "__main__":
	main(sys.argv[1:])
