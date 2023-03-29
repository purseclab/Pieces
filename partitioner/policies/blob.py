from .policy import *

class blob(Policy):
	def partition(self, firmware, clique):
		comp = firmware.create_compartment()
		for obj in clique["objs"]:
			comp.add(obj)
