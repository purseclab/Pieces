from .policy import *
from utils import read_line_vector_file
import inspect
import os
from .blob import blob

class component(Policy):
	def partition(self, firmware, clique):
		if firmware.os == "baremetal":
			b = blob()
			b.partition(firmware, clique)
			return


		#Hosted firmware, see component information
		self.group_leaves(firmware, clique)
		self.dominator_merge(firmware, clique)
		self.pair_merge(firmware, clique)
		dexpert = read_line_vector_file(os.path.dirname(inspect.getfile(component))+"/."+firmware.os+".comp")
		cCompartments = []
		for fcomp in dexpert:
			comp =  firmware.create_compartment()
			for func in firmware.clique_filter_pdg(clique):
				if fcomp in func:
					comp.add(func)
			if len(comp) > 0:
				cCompartments.append(comp)

		self.expandComponentsX(cCompartments, firmware, clique)
		self.assignLooseFunctions(firmware, clique)
		self.mergeComponentsExcept(cCompartments, firmware, clique)
