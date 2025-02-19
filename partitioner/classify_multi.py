import utils
import ec_loader
from utils import debug
from dotenv import load_dotenv
from llvm import Compiler
import os
import symex

from pebble import concurrent
from concurrent.futures import TimeoutError

load_dotenv()
try:
    input = utils.load_config(standalone_mode=False)
    if not input:
        exit()
except Exception as e:
    print(e)
    utils.print_help_msg(utils.load_config)
    exit()

os.environ["P_OUT_DIR"] = os.path.abspath(os.environ["P_OUT_DIR"]) + "/"
os.makedirs(os.environ["P_OUT_DIR"], exist_ok=True)
input["firmware"]["bc"] = os.path.abspath(input["firmware"]["bc"])
debug("Loading input firmware.")
compiler = Compiler()
compiler.analyze(input["firmware"])

firmware = ec_loader.Firmware(input["firmware"])
symex = symex.SymEx()
symex.threshold = 10000
symex.void_inline = False


functions_under_test = [
    # "_ZN6AC_PID10update_allEfffbf",  # verified, simplification failed
    # "_ZN13AverageFilterIiiLh10EE5applyEi",  # verified, simplification failed
    # "_ZN11NotchFilterIfE5applyERKf",  # verified, simplification worked
    # "_ZN19HarmonicNotchFilterIfE6updateEf", # takes too long to run
    # "_ZN15LowPassFilter2pIfE5applyERKf",  # verified, simplification worked
    # "_ZN10ModeFilterIfLh5EE5applyEf", # no relevant constraints found
    # "_ZN10ModeFilterIsLh3EEC1Eh",
    # "_ZN12NavEKF2_core12UpdateFilterEb", # we are gonna do this in parts
    # "_ZN12NavEKF2_core27UpdateStrapdownEquationsNEDEv",  # we will try NAVEKF2 first
    "_ZN12NavEKF2_core16calcOutputStatesEv"
    # "_ZN11SlewLimiter8modifierEff", # no constraint output
    # "_ZN16DerivativeFilterIfLh5EE5slopeEv",  # tested
    # "_ZN21AverageIntegralFilterIiiLh10EE5applyEi", # constraints found but none seeem relevant
    # "_ZN16FilterWithBufferIfLh3EE5applyEf", # constraints found but none seem relevant
    # "_ZN10DigitalLPFIfE5applyERKf", # verified, simplification worked
    # "_ZN19DigitalBiquadFilterIfE5applyERKfRKNS0_13biquad_paramsE", # verified, simpilification worked
]


def lift_to_sir(fun):
    summaries = symex.generate_summary(firmware, fun)

    math_q = symex.filter_queries_with_math(summaries)

    only_math_q = []
    for q in math_q:
        only_math_q = only_math_q + symex.filter_math(q)

    sir = []
    non_simplified = []
    for q in only_math_q:
        non_simplified.append(q)
        try:
            sir.append(symex.super_simple(q))
        except Exception as e:
            print(e)

    return sir, non_simplified


# @concurrent.process
def get_constraints(fun):
    sir, non_simplified = lift_to_sir(fun)
    for q in sir:
        print(f"Found {fun} simplified constraints: ({q})")
    for q in non_simplified:
        print(f"Found {fun} non-simplified constraints: ({q})")


for func in functions_under_test:
    get_constraints(func)
    # future = get_constraints(func)

# try:
#     results = future.result()
# except TimeoutError as error:
#     print("unstable_function took longer than %d seconds" % error.args[1])
# except Exception as error:
#     print("unstable_function raised %s" % error)
#     print(error.traceback)  # Python's traceback of remote process
