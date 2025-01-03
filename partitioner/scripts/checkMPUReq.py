#!/usr/bin/python2.7

import sys, getopt
import math

fragmentation=0
arch = "armv7m"
 
# Function to check
# Log base 2
def Log2(x):
	return (math.log10(x) /
			math.log10(2));
def isPowerOfTwo(n):
	return (math.ceil(Log2(n)) == math.floor(Log2(n)));
sizeRegionMap= { 0:"0", 32: "REGION_32B", 64: "REGION_64B", 128: "REGION_128B", 256: "REGION_256B", 512: "REGION_512B", 1024: "REGION_1K", 2048: "REGION_2K", 4096: "REGION_4K", 8192:"REGION_8K", 16384: "REGION_16K", 32768: "REGION_32K", 65536: "REGION_64K", 131072: "REGION_128K", 262144: "REGION_256K", 524288: "REGION_512K", 1048576: "REGION_1M", 2097152:"REGION_2M", 4194304:"REGION_4M",8388608:"REGION_8M",16777216:"REGION_16M",33554432:"REGION_32M",67108864:"REGION_64M",134217728:"REGION_128M",268435456:"REGION_256M",536870912:"REGION_512M",1073741824:"REGION_1G",2147483648:"REGION_2G",4294967296:"REGION_4G"}
	

def valid(base, size):
	if size != 0:
		upSize = size
		print("Is power of two?" + str(isPowerOfTwo(size)))
		if not isPowerOfTwo(size):
				upSize = 2 ** math.ceil(Log2(size))
				print(upSize - size)
		size = upSize 
		if base %size:
			print("Target base address is not divisible!")
			div = base/size
			div +=1
			newOffset = size *div
			print(newOffset)
		if size<32:
			print("Is less than 32 bytes")
	return [base, size]
def writeCodeSections(cpatch, csections):
	i = 0
	cs = ".csection"
	for section in range(len(csections)):
				cpatch.write("  .csection"+str(i) +" : \n")
				cpatch.write("  {\n")
				cpatch.write("  . = "+ str(csections[cs + str(section)][1]) +";\n")
				cpatch.write("_scsection"+str(i)+" = .;\n")
				cpatch.write("	*(.csection"+str(i)+")\n")
				cpatch.write("	. = "+ str(csections[cs + str(section)][0] + csections[cs + str(section)][1])+";\n")
				cpatch.write("_ecsection"+str(i)+" = .;\n")
				if arch=="armv7m":
					cpatch.write("  } > FLASH \n")
				else:
					cpatch.write("  }")
				i +=1

def writeDataSections(dpatch, dsections):
	i =0
	os = ".osection"
'''
		.osection0data : /* AT ( _sidata ) */
    {
        . = ALIGN(4);
        *(.osection0data)           /* .data sections. */
        . = ALIGN(4);
        _eosection0data = .;
    } > RAM AT > FLASH

    _sosection0data = LOADADDR(.osection0data);
'''
	for section in range(len(dsections)):
				dpatch.write("  .osection"+str(i) +"data : /* AT ( _sidata ) */\n")
				dpatch.write("  { . = ALIGN(4); \n")
				dpatch.write("  *(.osection" +str(i)+"data)           /* .data sections. */\n")
				dpatch.write("  . = ALIGN(4); \n")
				dpatch.write("  _eosection"+str(i) + "data = .;\n")
				dpatch.write("} > RAM AT > FLASH\n")
				dpatch.write("_sosection"+str(i) + "data = LOADADDR(.osection"+str(i) + "data);\n")
				dpatch.write("  .osection"+str(i) +" : AT ( _sidata  + compartLMA)\n")
				dpatch.write("  {\n")
				dpatch.write("	. = "+ str(dsections[os + str(section)][1])+";\n")
				dpatch.write("	_sosection" +str(i) +" = .;\n")
				dpatch.write("	*(.osection"+str(i)+")\n")
				dpatch.write("	. = "+ str(dsections[os + str(section)][0] + dsections[os + str(section)][1])+";\n")
				dpatch.write("	_eosection" +str(i) +" = .;\n")
				if arch=="armv7m":
					dpatch.write("  }  > RAM \n")
				else:
					dpatch.write(" }")
#				dpatch.write("  }  > RAM \n")
				dpatch.write("compartLMA = compartLMA + SIZEOF(.osection"+str(i) +"); \n")
				i += 1
	dpatch.write("_edata = .; \n")


def fixup(section):
		global fragmentation
		if section[0] == 0:
			print "Returning empty section"
			return section[0]
		if section[0] < 32:
			fragmentation += (32 - section[0])
			section[0] = 32
		upSize = 2 ** math.ceil(Log2(section[0]))
		fragmentation += (int(upSize) - section[0])
		section[0] = int(upSize)
		if section[1] %section[0]:
				div = section[1]/section[0]
				div += 1
				print ("Update from: " + str(section[1])+ " to:"+ str(section[0] * div))
				fragmentation += ((section[0] * div) - section[1])
				section[1] = int(section[0] * div)
		print(section)
		return section[0]+section[1]
'''
def updateSize(codesections):
	it = sorted(codesections)
	for csec in sorted(codesections):
			[size,base] = codesections[csec]
			ind = it.index(csec)
			ind +=1
			if ind < len(it):
				nextSec=it[ind]
				[nsize,nbase] = codesections[nextSec]
				sizePad = nbase - base
				if not sizePad == size:
					print ("Updating size from" + str(size) + " to " + str(sizePad))
					codesections[csec][0] = sizePad
'''


def printSortedAndFixupSections(sections, start):
	totalSize = 0
	lc = start
	elemi = ''.join([i for i in list(sections.keys())[0] if not i.isdigit()])
	for elem in range(len(sections)):
		print(elemi + str(elem) + ":")
		print(sections[elemi + str(elem)])
		totalSize += sections[elemi + str(elem)][0]
		if not lc == -1: 
			sections[elemi + str(elem)][1] = lc 
		if not sections[elemi + str(elem)][0] == 0:
			valid(sections[elemi + str(elem)][1],sections[elemi + str(elem)][0])
			lc = fixup(sections[elemi + str(elem)]) #Location counter tells from where the next section base should begin.
			print(sections[elemi + str(elem)])
	return totalSize

def printSortedAndVerifSections(sections):
	elemi = ''.join([i for i in list(sections.keys())[0] if not i.isdigit()])
	for elem in range(len(sections)):
		 print(str(elem) + ":")
		 print(sections[elemi + str(elem)])
		 if not sections[elemi + str(elem)][0] == 0:
				valid(sections[elemi + str(elem)][1],sections[elemi + str(elem)][0])
	

def main(argv):
	global fragmentation
	global arch
	inputfile = ''
	outputfile = ''
	devFile = None
	try:
		opts, args = getopt.getopt(argv,"hi:o:l:c:d:a:",["ifile=","ofile="])
	except getopt.GetoptError:
		print 'test.py -i <inputfile> -o <outputfile>'
		sys.exit(2)
	for opt, arg in opts:
		if opt == '-h':
			print 'test.py -i <inputfile> -o <outputfile>'
			sys.exit()
		elif opt in ("-i", "--ifile"):
			inputfile = arg
		elif opt in ("-l"):
			overlay = arg
		elif opt in ("-c"):
			configFile = arg
		elif opt in ("-d"):
			devFile = arg
		elif opt in ("-a"):
			arch = arg
	print 'Input file is "', inputfile
	outputFile = overlay.replace("overlay","ld")
	outFile = open(outputFile, "w")
	secinfo = {}
	init = []
	rtmkCode = []
	rtmkData = []
	FLASH_BASE = 0x8000000
	RAM_BASE = 0x20000000
	with open(inputfile) as f:
			lines = f.readlines()
			for line in lines:
				line = line.replace("\n","")
				
				if ".rtmkcode" in line:
					for word in line.split(" "):
							word = unicode(word, "utf-8")
							if word.isdecimal():
									num = int(word)
									rtmkCode.append(num)
				if ".rtmkdata" in line:
					for word in line.split(" "):
							word = unicode(word, "utf-8")
							if word.isdecimal():
									num = int(word)
									rtmkData.append(num)

				if "csection" in line or "osection" in line:
					secinfo[line.split(" ")[0]] = []
					for word in line.split(" "):
							word = unicode(word, "utf-8")
							if word.isdecimal():
									num = int(word)
									secinfo[line.split(" ")[0]].append(num)

	devinfo = []
	if not devFile:
		devFile = "./partitioner/out/rtmk.devautogen"	
	#TODO: TODO:Fix address
	with open(devFile) as f:
		lines = f.readlines()
		for line in lines:
			devinfo.append(line)

	print(devinfo)


	# MPU Requirements dictate that 
	#	1. size is power of 2
	#	2. size is more than 32 bytes
	#	3. base address is multiple of size.
	#
	codesections = {}
	datasections = {}
	for section in secinfo:
		[size, base] = secinfo[section]

		if ("csection" in section):
			codesections[section] = [size, base]
		else:
		 	datasections[section] = [size, base]

	lumpText = [rtmkCode[0]+rtmkCode[1] - FLASH_BASE, FLASH_BASE]
	stat= open("./linker.stat", "w")
	fragmentation=0

	if arch=="armv7m":
		size = printSortedAndFixupSections(codesections, fixup(lumpText))
	
	codeFragmentation = fragmentation
	stat.write(str(size)+"\n")
	stat.write(str(codeFragmentation)+"\n")
	fragmentation  = 0
	lumpData = [rtmkData[0]+rtmkData[1] - RAM_BASE, RAM_BASE]
	if arch=="armv7m":
		size = printSortedAndFixupSections(datasections, fixup(lumpData))
	dataFragmentation = fragmentation
	stat.write(str(size)+"\n")
	stat.write(str(dataFragmentation)+"\n")
	

	printSortedAndVerifSections(codesections)
	
	outputFile = overlay.replace("overlay","ld")
	outFile = open(outputFile, "w")
	with open(overlay) as f:
		lines = f.readlines()
		for line in lines:
			if("datamarker-fixup::" in line):
					writeDataSections(outFile, datasections)
			elif("codemarker-fixup::" in line):
					writeCodeSections(outFile, codesections)
			else:
					outFile.write(line)
	prologue_string  = "#include <monitor.h> \n "
	if arch == "armv7m":
		prologue_string += "#include <arch/armv7m/arch.h> \n "
	prologue_string += "RTMK_DATA \n  SEC_INFO comp_info[] = {"

	if arch =="x64":
		global sizeRegionMap
		sizeRegionMap= { 0:"0", 32: "32", 64: "64", 128: "128", 256: "256", 512: "512", 1024: "1024", 2048: "2048", 4096: "4096", 8192:"8192", 16384: "16384", 32768: "32768", 65536: "65536", 131072: "131072", 262144: "262144", 524288: "524288", 1048576: "1048576", 2097152:"2097152", 4194304:"4194304",8388608:"8388608",16777216:"16777216",33554432:"33554432",67108864:"67108864",134217728:"134217728",268435456:"268435456",536870912:"536870912",1073741824:"1073741824",2147483648:"2147483648",4294967296:"4294967296"}	
	#CodeStart,CodeSize,DataStart,DataSize
	endstring = "}; \n"
	f = open(configFile, "w")
	f.write(prologue_string)
#print(datasections)
	i =0
	
	print(devinfo)
	for sect in range(len(codesections)):
			section = ".csection" + str(sect)
			dsection = section 
			f.write("/*"+ section + "*/")
			dsection = ".o" + section[2:]
			#For smaller firmwares there could be compartments without any data
			if dsection in datasections:
				dsection = datasections[dsection]
			else:
				dsection = [0,0]
			csection = codesections[section]
			f.write("{")
			f.write(str(csection[1]))#Offset
			f.write(",")
#			print(section +":")
#			print(hex(int(csection[1])))
#			print(hex(int(csection[0])))

			if arch == "armv7m":
				f.write(sizeRegionMap[csection[0]])#Size
			else:
				f.write(str(csection[0]))
			f.write(",")
			f.write(str(dsection[1]))
			f.write(",")
#			print(hex(int(dsection[1])))
#			print(hex(int(dsection[0])))
			if arch == "armv7m":
				f.write(sizeRegionMap[dsection[0]])
			else:
				f.write(str(dsection[0]))
			f.write(",")
			f.write(str(csection[0] + csection[1]))
			f.write(",")
			f.write(str(dsection[0] + dsection[1]))
			[start, size, end] = map(lambda x: int(x,16), devinfo[i].split(","))
			print (start)
			print(size)
			[start, size] = valid(start, size)
			end = int(start + size)
			print("After")
			print(start)
			print(size)
			if arch == "armv7m":
				f.write("," + str(start) + " , "  + sizeRegionMap[size] + " , " + str(end))
			else:
				f.write("," + str(start) + " , "  + str(size) + " , " + str(end))
			f.write("}")
			i +=1
			if i!= len(codesections):
					f.write(",")
	f.write(endstring)
	f.write("RTMK_DATA int code_base= "+ str(FLASH_BASE) + ";\n");
	if arch == "armv7m":
		f.write("RTMK_DATA int code_size= "+ str(sizeRegionMap[codesections[".csection0"][1] - FLASH_BASE]) +";\n")
	else:
		f.write("RTMK_DATA int code_size= "+ str(codesections[".csection0"][1] - FLASH_BASE) +";\n")
	f.write("RTMK_DATA int data_base= "+ str(RAM_BASE) + ";\n");
	if arch == "armv7m":
		f.write("RTMK_DATA int data_size= "+ str(sizeRegionMap[datasections[".osection0"][1] - RAM_BASE]) + ";\n")
	else:
		f.write("RTMK_DATA int data_size= "+ str(datasections[".osection0"][1] - RAM_BASE) + ";\n")


	f.write("int total_secs = sizeof(comp_info) / sizeof(comp_info[0]);")



if __name__ == "__main__":
	main(sys.argv[1:])
