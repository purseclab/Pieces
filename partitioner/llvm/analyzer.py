import os
from utils import run_cmd
from utils import debug
import subprocess
from abc import ABC, abstractmethod
import pathlib

class Analyzer(ABC):
	@abstractmethod
	def analyze(self, firmware):
			pass

class ThreadDiscoverer(Analyzer):
	def analyze(self, firmware):
		thread_discovery = pathlib.Path(__file__).parent.resolve() + "/backend/thread_discovery"
		run_cmd([thread_discovery, firmware.bitcode], out=subprocess.STDOUT)

