from .policy import *
from utils import getSVDHandle, getDevice

#Old SVD database is available at: /home/arslan/.local/lib/python2.7/site-packages/cmsis_svd/data/STMicro/
class device(Policy):
	optionalDeviceDriverMerge = True
	def partition(self, firmware, clique):
		## The code above can go to firmware loader.
		for dev,base,size in firmware.svdfmap:
			#Give a compartment to every function that doesn't have a compartment
			comp = firmware.create_compartment() #New compartment for the device
			for fun in clique_filter(clique, firmware.svdfmap[(dev,base,size)]):
				if fun not in firmware.compartmentMap:
					comp.add(fun)
					for obj in clique_filter(clique, firmware.pdg[fun]):
						comp.add(obj)

		if self.optionalDeviceDriverMerge:
			for comp in list(firmware.compartments):
				for func in clique_filter(clique, list(comp)):
					#Get the file this function is in
					if func not in firmware.funcfilemap and func not in firmware.datafilemap:
						continue
					if func in firmware.funcfilemap:
						fil =  firmware.funcfilemap[func]
					else:
						fil = firmware.datafilemap[func]
					for drivFunc in clique_filter(clique, firmware.files[fil]):
						#if drivFunc not in compartmentMap:
						comp.add(drivFunc)

		firmware.dump()
		self.assignLooseFunctions(firmware, clique)
