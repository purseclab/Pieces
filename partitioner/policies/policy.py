from abc import abstractmethod
from utils import clique_filter, debug

class Policy():
	def __init__(self):
		pass

	def group_leaves(self,firmware, clique):
		leaves = []
		leave_objs = 0
		for func in clique_filter(clique, firmware.pdg):
			if (len(firmware.pdg[func]) ==0):
				leaves.append(func)
				leave_objs = leave_objs+ 1
		for obj in clique_filter(clique, firmware.pddg):
			if (len(data[obj]) == 1):
				leaves.append(data[obj][0])
				leave_objs = leave_objs+ 1

		debug("Leaf Compartments:" + str(len(leaves)))
		debug("Loose Functions:" + str(firmware.total_objects - len(leaves)))

		for leaf in leaves:
			firmware.create_compartment().add(leaf)


	#Find the dominators and include them in the same compartment if they are loose and in same clique.
	def dominator_merge(self, firmware, clique):
		for func in firmware.clique_filter_pdg(clique):
			if len(firmware.pdg[func]) ==1 and firmware.pdg[func][0] in firmware.compartmentMap and firmware.pdg[func][0] in clique["objs"]:
				for compartment in firmware.compartments:
					if(firmware.pdg[func] in compartment):
						compartment.add(funcs[func])


	#This merges the SCCish type of nodes in the PDG if the are in clique and not loose.
	def pair_merge(self, firmware, clique):
		for func in firmware.clique_filter_pdg(clique):
			iter =0
			if func not in firmware.compartmentMap:
				for val in clique_filter(clique, firmware.pdg[func]):
					if val in firmware.compartmentMap:
						break;
					iter+=1
				if iter==len(firmware.pdg[func]):
					comp = firmware.create_compartment()
					#We checked earlier if func is in this clique
					comp.add(func)
					for val in clique_filter(clique, firmware.pdg[func]):
						comp.add(val)

	def assignLooseFunctions(self, firmware, clique):
		compartment = firmware.create_compartment()
		for func in firmware.clique_filter_pdg(clique):
			if func not in firmware.compartmentMap:
				compartment.add(func)
			for obj in clique_filter(clique, firmware.pdg[func]):
				if obj not in firmware.compartmentMap:
					compartment.add(obj)

	def expandComponentsX(self, tCompartments, firmware, clique):
		for comp in tCompartments:
			for obj in comp:
				if obj in clique_filter(clique, firmware.pdg):
					for ptsTo in clique_filter(clique, firmware.pdg[obj]):
						if ptsTo in firmware.compartmentMap:
							if firmware.compartmentMap[ptsTo] in tCompartments:
								continue
							else:
								comp.add(ptsTo)
						else:
							comp.add(ptsTo)
				elif obj in clique_filter(clique, firmware.pddg):
						for user in clique_filter(clique, firmware.pddg[obj]):
							if user in firmware.compartmentMap:
								if firmware.compartmentMap[user] in tCompartments:
									continue
								else:
									comp.add(user)
							else:
								comp.add(user)

	def mergeComponentsExcept(self, tCompartments, firmware, clique):
		comp = firmware.create_compartment()
		for compartment in list(firmware.compartments):
			if compartment not in tCompartments:
				for obj in clique_filter(clique, list(compartment)):
					comp.add(obj)
		
	@abstractmethod
	def partition(self, firmware, clique):
		pass

	def finalizer(self, firmware):
		return 
