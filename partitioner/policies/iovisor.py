from .policy import *
from utils import getSVDHandle, getDevice

#Old SVD database is available at: /home/arslan/.local/lib/python2.7/site-packages/cmsis_svd/data/STMicro/
class iovisor(Policy):
	def partition(self, firmware, clique):
		## The code above can go to firmware loader.
		comp = firmware.create_compartment()
		for dev,base,size in firmware.svdfmap:
			print(list(clique_filter(clique, firmware.svdfmap[(dev,base,size)])))
			#Give a compartment to every function that doesn't have a compartment
			for fun in clique_filter(clique, firmware.svdfmap[(dev,base,size)]):
				if fun not in firmware.compartmentMap:
					comp.add(fun)
					for obj in clique_filter(clique, firmware.pdg[fun]):
						comp.add(obj)

		print(comp)
		firmware.dump()
		self.assignLooseFunctions(firmware, clique)
