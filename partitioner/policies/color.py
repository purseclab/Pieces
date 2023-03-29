from .policy import *
from utils import debug

class color(Policy):
	color_policy = "submerge"
	def __paint(self, firmware, clique):
		for func in firmware.clique_filter_pdg(clique):
			#List of different compartments, Each comp one color
			colors = []
			if func in firmware.compartmentMap:
				colors.append(firmware.compartmentMap[func])
			for obj in firmware.clique_filter_pdg(clique):
				if obj in firmware.compartmentMap:
					if firmware.compartmentMap[obj] not in colors:
						colors.append(firmware.compartmentMap[obj])
				else:
					#If an object in PDG has not been seen before we can safely put it in any of the color.
					if len(colors) > 0:
						compartment = colors[0]
						compartment.add(obj) 
	
	
			if len(colors)>1:
				compartment = firmware.create_compartment()
				if self.color_policy == "cherrypick":
					for color in colors:
						for obj in clique_filter(clique, color):
							compartment.add(obj)
				elif self.color_policy == "submerge":
					for color in colors:
						oldCompartment = color
						for obj in clique_filter(clique, color.objs):
							compartment.add(obj)
	
	def __spreadPaint(self, firmware, clique):
		compartment=firmware.create_compartment()
		for func in firmware.clique_filter_pdg(clique):
			if func not in firmware.compartmentMap:
				for val in clique_filter(clique, funcs[func]):
					if val in compartmentMap:
						#At this point all compartments must be same in the pointed thing
						compartment=compartmentMap[val]
						break;
				compartment.add(func)
				for val in clique_filter(clique, funcs[func]):
					if val not in firmware.compartmentMap:
						compartment.add(val)
	

	def partition(self, firmware, clique):
			self.group_leaves(firmware, clique)
			self.dominator_merge(firmware, clique)
			self.pair_merge(firmware, clique)
			self.__paint(firmware, clique)
			self.__spreadPaint(firmware, clique)
			# In coloring we usually generate a lot of compartments which usually don't fit in memory
			# because of alignment requirements. Therefore we need to constraint the number of compartments
			while len(firmware.compartments) > 20:
				firmware.compartments[0].mergeCompartments(firmware.compartments[1])
