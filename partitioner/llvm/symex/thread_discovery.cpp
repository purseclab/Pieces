//===- svf-ex.cpp -- A driver example of SVF-------------------------------------//
//
//                     SVF: Static Value-Flow Analysis
//
// Copyright (C) <2013->  <Yulei Sui>
//

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
//===-----------------------------------------------------------------------===//

/*
 // A driver program of SVF including usages of SVF APIs
 //
 // Author: Yulei Sui,
 */
#include "util.h"
#include "ec.h"
#include "crt.h"
#include "etsan.h"
#include <iostream>
#include <algorithm>

bool EC = true;
bool TestPass = false;
bool CRT = false; bool emitAll = true;
bool ETSAN = false;
bool analyzecode = true;


enum RTOS_type {
  Guess, /* Special RTOS type to find RTOS */
  FreeRTOS,
  Zephyr,
  MAX_OS
};

std::string RTOSName[] = {
		"",
		"FreeRTOS",
		"Zephyr"
}; 

/* RTOS type */
cl::opt<RTOS_type> RTOS(cl::desc("Choose firmware RTOS"),
  cl::values(
    clEnumVal(FreeRTOS , "FreeRTOS Threads"),
    clEnumVal(Zephyr, "Zephyr Threads")));


/// Try to find in the Haystack the Needle - ignore case
bool findStringIC(const std::string & strHaystack, const std::string & strNeedle)
{
  auto it = std::search(
    strHaystack.begin(), strHaystack.end(),
    strNeedle.begin(),   strNeedle.end(),
    [](unsigned char ch1, unsigned char ch2) { return std::toupper(ch1) == std::toupper(ch2); }
  );
  return (it != strHaystack.end() );
}
RTOS_type determine_rtos() {
		if (ll_mod->getNamedValue("pxCurrentTCB") && ll_mod->getNamedValue("xTickCount")) {
				cout<<"Assuming FreeRTOS"<<endl;
				return FreeRTOS;
		}
		else {
				cout<<"Assuming Zephyr RTOS"<<endl;
				return Zephyr;
		}
}
void thread_discovery(char * argv[]) { 
        ofstream threads;
		vector<Value *> thread_vec;
        threads.open("./threads");

		if (RTOS==Guess) {
				RTOS=determine_rtos();
		}
		if (RTOS==Zephyr) {
        /* Get static threads for Zephyr */
        for (auto G = svfModule->global_begin(), E = svfModule->global_end(); G != E; ++G) {
                auto glob = &*G;
                if ((*glob)->getName().str().find("_k_thread_data_") != std::string::npos) {
                        cout << (*glob)->getName().str() <<endl;
                        auto init = (*glob)->getInitializer();
                        if (auto cast = dyn_cast<llvm::User>(init->getOperand(3))) {
                                threads<< cast->getOperand(0)->getName().str() << endl;
                        }
                }
        }
		} else if (RTOS==FreeRTOS) {
				for (SVFModule::llvm_iterator F = svfModule->llvmFunBegin(), E = svfModule->llvmFunEnd(); F != E; ++F) {
                Function *fun = *F;
                Value * val = (Value *)fun;
                if (val->getName().str().compare("xTaskCreate")==0) {
                        for (auto user : val->users()) {
                                if (auto ci =  dyn_cast<llvm::CallInst>(user)) {
                                        auto  thread = ci->getArgOperand(0);
                                        threads<<thread->getName().str()<<endl;
                                        thread_vec.push_back(thread);
                                }
                        }
                }
				}
		} else {
				cerr<<"Unkown RTOS"<<endl;
		}
}


int main(int argc, char ** argv) {
    int error_value = 0;
    parseArguments(argc,argv);

	thread_discovery(argv);

    //Temp for debugging and hacking
    return error_value;
}
