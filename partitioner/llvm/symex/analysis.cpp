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
#include <llvm/Analysis/LoopInfo.h>
#include <llvm/IR/Dominators.h>
#include <llvm/Transforms/Utils/Cloning.h>


map<Value *, Function *> function_pointers;
map<string, vector<Value *>> pinned_resources;
vector<Value *> cloneFuncs;
int thread_discovery(char * argv[]) {
#define FREERTOS
#ifdef ZEPHYR
		ofstream threads;
		threads.open("./threads");
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

#endif
#ifdef FREERTOS
		ofstream threads;
		vector<llvm::Value *> thread_vec;
		threads.open("./threads");
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
#endif

}
