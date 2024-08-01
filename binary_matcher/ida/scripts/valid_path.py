#read gdl file which created by IDAPro IDC script
#get the node[], name[], and how many calls in this node
#get the relation among all nodes
#According to the main function control flow graph to find out all library functions between start point (start EIP) and end point (end EIP)


import sys
import time  #for time

start_time = time.time()

node_val = []

#read only pid function file to get the control flow graph 

mfile = open(sys.argv[1])
m_line = mfile.readline()
m_node_list = {}   #store all call functions in the node
m_name_list = []   #store all node name, there are two situtions: some node belong to one function which have name, some node just small block which no name, we just name them 'block'
m_edge_list = []   #the relation among all nodes
arithmetic_inst_set = ['addss', 'subss', 'mulss', 'divss','addsd', 'subsd', 'mulsd', 'divsd']
mov_inst_set = ['movss', 'movsd']
compare_inst_set = ['xorpd', 'ucomisd', 'ucomiss', 'xorps']

arithmetic_instructions_x86 = ["add", "xadd", "adc", "inc", "sub", "sbb", "dec", "neg", "aaa", "aas", "aam", "aad", "daa", "das", "mul", "imul", "div", "idiv"]
#logical_instructions_x86 = ["and", "tes", "or", "xor", "not"]
arithmetic_instructions_fp_x86 = ["fabs", "fadd", "faddp", "fchs", "fdiv", "fdivp", "fdivr", "fdivrp", "fiadd", "fidiv", "fidivr", "fimul", "fisub", "fisubr", "fmul", "fmulp", "fprem", "fprem1", "frndint", "fscale", "fsqrt", "fsub", "fsubp", "fsubr", "fsubrp", "fxtract"]
#
#
####ARM
#call_instructions_arm = ["BL", "BLX"]
arithmetic_instructions_arm = ["ADD", "ADDS", "SUB", "SUBS", "RSB", "MUL", "UMULL", "SMLAL", "SDIV", "UDIV"]
arithmetic_instructions_fp_arm =["VABS", "VNEG", "VSQRT", "VADD", "VSUB", "VDIV", "VMUL", "VMLA", "VMLS", "VNMUL", "VNMLA", "VNMLS", "VFNMA", "VFNMS", "VRINT", 
								"VABS.F32", "VNEG.F32", "VSQRT.F32", "VADD.F32", "VSUB.F32", "VDIV.F32", "VMUL.F32", "VMLA.F32", "VMLS.F32", "VNMUL.F32", "VNMLA.F32", "VNMLS.F32", "VFNMA.F32", "VFNMS.F32", "VRINTF32"]
#logical_instructions_arm = ["AND", "ORR", "ORREQ", "ANDS", "EOR", "EORS", "BIC", "ORN", "ORNS"]
branch_instructions_arm = ["B", "BL", "BX", "BLX", "BXJ"]  ## pop and ldr  https://community.arm.com/processors/b/blog/posts/branch-and-call-sequences-explained
mov_inst_set = ['MOV', 'VMOV'] 
compare_inst_set = ['VCMP', 'VCMPE']



arithmetic_node = {}
mov_node = {}
compare_node = {}
while m_line:
	m_nNode = m_line.find('node')   #find out the word 'node', every block start with 'node'
	if m_nNode == 0:
		#get the name of node
		m_nQuote_1 = m_line.find('"', m_nNode)
		m_nQuote_2 = m_line.find('"', m_nQuote_1+1)
		node_index = m_line[m_nQuote_1+1:m_nQuote_2]
		arithmetic_sub_node = []
		mov_sub_node = []
		compare_sub_node = []
		while m_line:
			m_line = m_line.strip()   #remove the space from begin and end
			#print(m_line)
			if 'node' in m_line:
				tmp = m_line.strip()
			if 'PC' in m_line or 'LR' in m_line or 'VPOP' in m_line:
				node_val.append(tmp[16])
			m_space = m_line.find(' ')
			if m_space >= 0:
				instruction = m_line[0:m_space]
				if instruction in arithmetic_instructions_fp_arm:
					arithmetic_sub_node.append(instruction)
				#if instruction in compare_inst_set:
				#	compare_sub_node.append(instruction)
				#if instruction in mov_inst_set:
				#   mov_sub_node.append(instruction)
										
			m_nBracket = m_line.find('}')
			if m_nBracket > 0:
				break
			m_line = mfile.readline()
		# m_node_list[node_index] = m_sub_node
		arithmetic_node[node_index] = arithmetic_sub_node
		mov_node[node_index] = mov_sub_node
		compare_node[node_index] = compare_sub_node
	if m_nNode > 0:
		break
		#line = file.readline()
		#nEdge = line.find('edge')
	m_line = mfile.readline()	
# print(m_node_list)
# print(m_name_list)
print(arithmetic_node)
#print(mov_node)
#print(compare_node)

# store the relation among all nodes
while m_line:
	m_nNode = m_line.find('node')
	if m_nNode > 0:
		m_line = mfile.readline()
		m_nEdge = m_line.find('edge')
		if m_nEdge < 0:
			m_edge_list.append(None)
		if m_nEdge == 0:
			m_sub_edge = []
			while m_line:
				m_nEdge = m_line.find('edge')
				if m_nEdge == 0:
					m_nQuote_1 = m_line.find(': "', m_nEdge)
					m_nQuote_2 = m_line.find(': "', m_nQuote_1+3)
					m_nQuote_3 = m_line.find('"', m_nQuote_2+3)
					m_edge = m_line[m_nQuote_2+3:m_nQuote_3]
					m_sub_edge.append(m_edge)
					m_line = mfile.readline()
				if m_nEdge < 0:
					m_edge_list.append(m_sub_edge)
					#edge_list.append("end")
					break
	if m_nNode < 0:
		break
mfile.close()

# Through BFS to get all path
#for i in range(len(m_name_list)):
m_edge_list_size = len(m_edge_list)	
graph = {}
#print("sun", m_edge_list_size)
for i in range(m_edge_list_size-1, -1, -1):
	graph[str(i)] = m_edge_list[i]
	

#graph['0'] = ['0']
print("graph: ", graph)
print()
path_node_list = []


class MyQUEUE: # just an implementation of a queue
	
	def __init__(self):
		self.holder = []
		
	def enqueue(self,val):
		self.holder.append(val)
		
	def dequeue(self):
		val = None
		try:
			val = self.holder[0]
			if len(self.holder) == 1:
				self.holder = []
			else:
				self.holder = self.holder[1:]	
		except:
			pass
			
		return val	
		
	def IsEmpty(self):
		result = False
		if len(self.holder) == 0:
			result = True
		return result


path_queue = MyQUEUE() # now we make a queue

temp_validpath = []


def BFS(graph,start,end,q):
	
	temp_path = [start]	
	q.enqueue(temp_path)
	
	while q.IsEmpty() == False:
		tmp_path = q.dequeue()
		last_node = tmp_path[len(tmp_path)-1]
		#print("sun", last_node)
		#print(last_node, end)
		#print("ALL_PATH : ", tmp_path)
		if last_node == end:
			arithmetic_sum = 0
			mov_sum = 0
			compare_sum = 0
			for i in tmp_path:
				arithmetic_sum += len(arithmetic_node[i])
				#mov_sum += len(mov_node[i])
				#compare_sum += len(compare_node[i])

			#print("VALID_PATH : ",tmp_path, arithmetic_sum, mov_sum, compare_sum)
			temp_validpath.append(tmp_path)
			#print("VALID_PATH : ",tmp_path)

			#for i in tmp_path:
			#	print(i, ' (', len(arithmetic_node[i]), len(mov_node[i]), len(compare_node[i]), ') ->',)
			#print()
			# According to we have got all the path, then we can collect all related node
			for path_node in tmp_path:
				if path_node not in path_node_list:
					path_node_list.append(path_node)
		if last_node != end:
			try:
				#print(graph[last_node])
				for link_node in graph[last_node]:
					#print(link_node)
					if link_node not in tmp_path:
						new_path = []
						new_path = tmp_path + [link_node]
						q.enqueue(new_path)
			except:
				pass
	print()
	for x in temp_validpath:
		print("VALID_PATH : ",x)

for y in list(set(node_val)):
	BFS(graph,"0",y,path_queue)
