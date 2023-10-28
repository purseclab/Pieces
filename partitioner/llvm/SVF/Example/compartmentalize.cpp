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

bool analysisOnly = false;
void get_crt_functions(string file_name, vector<string> & funcs_vec)
{
		string line;
        std::ifstream infile(file_name);
        while (std::getline(infile, line))
        {
                vector <string> tokens;
                char *token = strtok((char *)line.c_str(), ",");
                while (token != NULL)
                {
                        string str(token);
                        char chars[] = "[]'' ";

                        for (unsigned int i = 0; i < strlen(chars); ++i)
                        {
                                // you need include <algorithm> to use general algorithms like std::remove()
                                str.erase (std::remove(str.begin(), str.end(), chars[i]), str.end());
                        }
                        tokens.push_back(str);
                        token = strtok(NULL, ",");
                }

                for (auto token: tokens) {
					if (token[0] != '\0') {
	                    if(!vContains(funcs_vec, token)) {
    	                    funcs_vec.push_back(token);
        	            }
					}
                }
        }
}


int crt_intertask(vector<string>& thread_funcs_vec, vector<string>& kernel_funcs_vec, map<Function *, SmallPtrSet<Function*, 16>> & thread_reach) {
		vector<Function *> threads;
		for (auto thread: thread_funcs_vec) {
				threads.push_back(ll_mod->getFunction(thread));
		}

		map<Function *, vector<Value *>>value_map;
		//get all values for this function
		for (auto thread: threads) {
				vector<Value *> vals;
				pushValsInFun(thread, vals, pag, NULL);
				for (auto fun: thread_reach[thread]) {
						pushValsInFun(fun, vals, pag, NULL);
				}
				value_map[thread] = vals;
		}


		for (auto task1 : threads) {
			for (auto task2: threads) {
					if (task1 == task2)
                                continue;
					for (auto val1: value_map[task1]) {
							if (auto cnst =dyn_cast<ConstantInt>(val1)) {
                                        continue;
                            }
							if (auto func = dyn_cast<Function>(val1)) {
                                        /* TODO: Should we allow functions to call anything??? */
                                        continue;
                             }
                                 //See if this is a null constant
                              if (auto nc =  dyn_cast<llvm::ConstantPointerNull>(val1)) {
                                                        //NULL Constants are shareable becaause they are literals
                                                        continue;
                              }
							  for (auto val2: value_map[task2]) {
                                        if (val1 == val2) {
                                                //See if we have explicit ownership
                                                if (auto go =  dyn_cast<llvm::GlobalVariable>(val1))
                                                    if ((go->getSection().str().find(task1->getName().str()) != std::string::npos) && (go->getSection().str().find(task2->getName().str()) != std::string::npos))
                                                            continue;


                                                cout<< "Tasks are sharing resources:"<<endl;
												cout<< task1->getName().str()<<endl;
												cout<< task2->getName().str()<<endl;
                                                val1->dump();
												exit(0);
										}
							  }
					}
			}
		}

		return 0;
}
int crt_instrument(vector<string>& thread_funcs_vec, vector<string>&  kernel_funcs_vec) {

		for (auto name: thread_funcs_vec) {
				cout<<name<<endl;
		}
		for (SVFModule::llvm_iterator F = svfModule->llvmFunBegin(), E = svfModule->llvmFunEnd(); F != E; ++F) {
				auto fun = *F;
				if (vContains(thread_funcs_vec, fun->getName().str())) {
						LLVMContext &Context = ll_mod->getContext();
			            FunctionType *ValidateFnType = FunctionType::get(Type::getVoidTy(Context), {Type::getInt8PtrTy(Context)}, false);
						auto ValidateFn = ll_mod->getOrInsertFunction("crt_validate", ValidateFnType);
						for (auto bb=fun->begin();bb!=fun->end(); bb++) {
								for (auto stmt =bb->begin();stmt!=bb->end(); stmt++) {
	                                if (auto ci = dyn_cast<llvm::CallInst> (stmt)) {
											if(!ci->getCalledFunction()) {
													cout<< fun->getName().str()<<" has indirect calls"<<endl;
													auto callee = ci->getCalledOperand();
													if (auto iasm = dyn_cast<InlineAsm>(callee)) {
															cerr<< iasm->getAsmString()<<endl;
															cerr<<"Bro, no inline asm in safe code, you sly fox!!"<<endl;
															exit(1);
													}
													// This is an indirect call
					                                IRBuilder<> Builder(ci);
													auto ccallee = Builder.CreatePointerCast(ci->getCalledOperand(), Builder.getInt8PtrTy());
					                                // Call the validate function with the argument
                    					            Builder.CreateCall(ValidateFn, {ccallee});
											}
									}
								}
						}
				}
		}

		return 0;
}
string  argToBridge(CallInst * ci, int argnum, Value ** v, Value ** sizeInt) {
		auto arg = ci->getArgOperand(argnum);
		string args;
		auto md = ci->hasFnAttr ("rtmkxcmd");
		auto fun = ci->getCalledFunction();
		/* If its a normal int we don't need to pass in anything */
		if (arg->getType()->isIntegerTy()) {
				auto intt  = dyn_cast<llvm::IntegerType> (arg->getType());
				if (intt->getBitWidth() <= 32) {
					args = "i";
					IRBuilder<> Builder(ci);
					auto cast = Builder.CreateIntCast (arg, Type::getInt32Ty(arg->getContext()), false);
					*v = cast;
					*sizeInt = *v;
				} else if (intt->getBitWidth() <= 64) {
					args = "d";
					IRBuilder<> Builder(ci);
                    auto cast = Builder.CreateIntCast (arg, Type::getInt64Ty(arg->getContext()), false);
                    *v = cast;
                    *sizeInt = *v;
				} else {
					cerr<<"Really Huge Variable is being used!!" <<endl;
					while(1);
				}
		} else if (arg->getType()->isPointerTy()) {
				args = "p";
				IRBuilder<> Builder(ci);
				if (fun && fun->getAttributes().hasParamAttr (argnum, "rtmkxcmd") ) {
						auto attr = fun->getAttributes().getParamAttr (argnum, "rtmkxcmd");
						auto kw = attr.getValueAsString().str();
						*v = Builder.CreatePointerCast(arg, Type::getInt8PtrTy(arg->getContext()));
						string argkw = "arg";

						if (kw == "opaque") {
								*sizeInt = ConstantInt::get(arg->getContext(),llvm::APInt(32, -1, true));
						} else if (kw == "string") {
								//Get strlen function
								auto func = ll_mod->getFunction("mystrlen");
                                if (func==NULL) {
                                        cerr<< "strlen not implemented" <<endl;
                                        return 0;
                                }
                                auto func_type = func->getFunctionType();
                                auto f = ll_mod->getOrInsertFunction ("strlen", func_type); //FuncCallee
								//STRLEN Gives you length of string, but we want to get the null terminator too.
								*sizeInt = Builder.CreateAdd(Builder.CreateCall(f, {arg}), ConstantInt::get(arg->getContext(),
                                                        llvm::APInt(32, 1, false))); 
						} else if (kw.find(argkw) != std::string::npos) { 
								kw.erase(kw.find(argkw), argkw.length());
								int argNum = stoi(kw);
								*sizeInt = ci->getArgOperand(argNum);
						} else {
								*sizeInt = ConstantInt::get(arg->getContext(),llvm::APInt(32, 0, true));
						}

				}
				else if (arg->getType()->getPointerElementType()->isFunctionTy()) {
						*v = Builder.CreatePointerCast(arg, Type::getInt8PtrTy(arg->getContext()));
						auto sizeP = Builder.CreateIntToPtr (ConstantInt::get(arg->getContext(),
												llvm::APInt(32, 0, false)), arg->getType());
						auto size = Builder.CreateConstGEP1_32 (NULL, sizeP, 1);
						*sizeInt = ConstantInt::get(arg->getContext(),
										llvm::APInt(32, -1, true));

				} else 
						if (arg->getType()->isSized()) {
								*v = Builder.CreatePointerCast(arg, Type::getInt8PtrTy(arg->getContext()));
								auto sizeP = Builder.CreateIntToPtr (ConstantInt::get(arg->getContext(),
														llvm::APInt(32, 0, false)), arg->getType());
								auto size = Builder.CreateConstGEP1_32 (NULL, sizeP, 1);
								*sizeInt = Builder.CreatePtrToInt(size, Type::getInt32Ty(arg->getContext()));
						} else {
								*v = Builder.CreatePointerCast(arg, Type::getInt8PtrTy(arg->getContext()));
								cerr <<"Unsized pointer:";
								ci->dump();
								*sizeInt  = ConstantInt::get(arg->getContext(),
												llvm::APInt(32, 1, false));
						}
		} else {
				cerr<<"Pass incomplete" <<endl;
				ci->dump();
				*v = NULL; *sizeInt = NULL;
				args = "";
		}
		return args;
}

string getRetType(CallInst * ci) {
		string ret;
		if (ci->getType()->isVoidTy()) {
				ret = "x";
		} else if (ci->getType()->isIntegerTy()) {
				ret = "i";
		} else if (ci->getType()->isPointerTy()) {
				ret = "p";
		} else {
				cerr<<"Pass incomplete" <<endl;
				ci->dump();
				ret = "";
		}
		return ret;
}

Type* getRetTy(CallInst * ci, IRBuilder<> &Builder) {
		llvm::Type * ret;
        if (ci->getType()->isVoidTy()) {
                ret = Builder.getVoidTy();
        } else if (ci->getType()->isIntegerTy()) {
                ret = Builder.getInt32Ty();
        } else if (ci->getType()->isPointerTy()) {
				ret = Builder.getInt8PtrTy();
        } else {
                cerr<<"Pass incomplete" <<endl;
                ci->dump();
                ret = NULL;
        }
        return ret;
}
static map<int,vector<string>> compartments;
static map<string, int>compartmentMap;

int promoteXCallNoCalee(CallInst * ci, BasicBlock::iterator& stmt, int compID);

int promoteXCall(CallInst * ci, Function * callee, BasicBlock::iterator& stmt) {
		//Builder.SetInsertPoint(stmt->getNextNode()->getPrevNode());
		IRBuilder<> Builder(ci);
		auto num = ci->arg_size();
		BasicBlock::iterator it(stmt);it--;
		auto fun = ci->getCalledFunction();
		if (fun && fun->getAttributes().getFnAttributes().hasAttribute("rtmkxcmd")) {
				cout<<"Found function with metadata" <<endl;
				auto attr = fun->getAttributes().getFnAttributes().getAttribute("rtmkxcmd");
				auto kw = attr.getValueAsString().str();
				if (kw == "custom_bridge") {
						num = -1;
						cout<<"CUstom Bridge found";
				}
		}

		int compID = compartmentMap[callee->getName().str()];
		promoteXCallNoCalee(ci, stmt, compID);
		return 0;
}
int promoteXCallNoCalee(CallInst * ci, BasicBlock::iterator& stmt, int compID) {
		IRBuilder<> Builder(ci);
		BasicBlock::iterator it(stmt);it--;
		//Builder.SetInsertPoint(stmt->getNextNode()->getPrevNode());


		auto params = ci->getFunctionType()->params().vec();
		vector<Type *> args;
		vector<Value *> args_val;
		args.push_back(Builder.getInt32Ty());
		args.push_back(Builder.getInt8PtrTy());
		auto p = llvm::ConstantInt::get(Builder.getInt32Ty(), llvm::APInt(32, compID, false));
		args_val.push_back(p);
		auto ccallee = Builder.CreatePointerCast(ci->getCalledOperand(), Builder.getInt8PtrTy());
		args_val.push_back(ccallee);


		int i =0;
		string func_name = getRetType(ci);
		func_name = func_name + "call_arg";
		string suffix = "";
		for (auto arg: params) {
				Value * v;
				Value * size;
				suffix = suffix + argToBridge(ci, i++, &v, &size);
				args.push_back(v->getType());
				args.push_back(size->getType());
				args_val.push_back(v);
				args_val.push_back(size);
		}
		func_name = func_name + std::to_string(i) + suffix;

		auto func_type = FunctionType::get(getRetTy(ci, Builder), args, false);
		auto f = ll_mod->getOrInsertFunction(func_name, func_type);
//		Function* f = Function::Create(func_type, Function::ExternalLinkage, func_name, ll_mod);

		auto new_inst = Builder.CreateCall(f,args_val);
		Instruction * ins;
		if(ci->getType() == new_inst->getType()) {
				ins = new_inst;
		}
		else if (ci->getType()->isPointerTy()) {
			ins = dyn_cast<llvm::Instruction>(Builder.CreatePointerCast(new_inst, ci->getType()));
		}else {
			ins = dyn_cast<llvm::Instruction>(Builder.CreateIntCast(new_inst, ci->getType(),false));
		}

		stmt++;
		ins->removeFromParent();
		ins->dump();
        ReplaceInstWithInst(ci, ins);

		return 0;
}
int promoteXCallNoCaleeNoId(CallInst * ci, BasicBlock::iterator& stmt) {
		IRBuilder<> Builder(ci);
		BasicBlock::iterator it(stmt);it--;
		//Builder.SetInsertPoint(stmt->getNextNode()->getPrevNode());

		auto params = ci->getFunctionType()->params().vec();
        vector<Type *> args;
        vector<Value *> args_val;
        args.push_back(Builder.getInt8PtrTy());
		auto ccallee = Builder.CreatePointerCast(ci->getCalledOperand(), Builder.getInt8PtrTy());
        args_val.push_back(ccallee);


        int i =0;
        string func_name = getRetType(ci);
        func_name = func_name + "call_arg";
        string suffix = "_noid";
        for (auto arg: params) {
                Value * v;
                Value * size;
                suffix = suffix + argToBridge(ci, i++, &v, &size);
                args.push_back(v->getType());
                args.push_back(size->getType());
                args_val.push_back(v);
                args_val.push_back(size);
        }
        func_name = func_name + std::to_string(i) + suffix;

        auto func_type = FunctionType::get(getRetTy(ci, Builder), args, false);
		auto f = ll_mod->getOrInsertFunction(func_name, func_type);
//        Function* f = Function::Create(func_type, Function::ExternalLinkage, func_name, ll_mod);

        auto new_inst = Builder.CreateCall(f,args_val);

		Instruction * ins;
        if(ci->getType() == new_inst->getType()) {
                ins = new_inst;
        }
        else if (ci->getType()->isPointerTy()) {
            ins = dyn_cast<llvm::Instruction>(Builder.CreatePointerCast(new_inst, ci->getType()));
        }else {
            ins = dyn_cast<llvm::Instruction>(Builder.CreateIntCast(new_inst, ci->getType(),false));
        }

        stmt++;
        ins->removeFromParent();
		ins->dump();
        ReplaceInstWithInst(ci, ins);

		return 0;
}
map<Value *, Function *> function_pointers;
typedef struct {
	vector <Value *> data;
	vector <Value *> code;
	int compartmentID;
} COMP;
map<string, COMP> pinned_resources;
vector<Value *> cloneFuncs;
vector<Value *> secrets;

void forward_slice_crt(Function *F, SmallPtrSet<Function*, 16> &visitedFunctions, vector<Function *> &compat) {
            if (visitedFunctions.count(F) > 0 || vContains(compat, F) || F->isIntrinsic())
                return;
            visitedFunctions.insert(F);

            for (BasicBlock &BB : *F) {
                for (Instruction &I : BB) {
                    if (CallInst *CI = dyn_cast<CallInst>(&I)) {
                        Function *calledFunction = CI->getCalledFunction();
                        if (calledFunction)
                            forward_slice_crt(calledFunction, visitedFunctions, compat);
                    }
                }
            }
}
int compartmentalize(char * argv[]) {
		ofstream debug;
		ofstream ignoreList;
		ignoreList.open("./rtmk.ignore");
		debug.open("./rtmk.log");
		string rtmksec= "rtmk";
        string shared = "shared";
        string pinned = "pinned";
        string clone = "clone";
		string secret = "secret";


		map<Value *, vector<Value *>> PDG; // Function-> Global/Functions
		ofstream dfg;
		dfg.open("./dg");

		
		for (auto G = svfModule->global_begin(), E = svfModule->global_end(); G != E; ++G) {
				auto glob = &*G;
				if ((*glob)->getName().str() == "llvm.used" || (*glob)->getName().str() == "_shared_region"
								|| (*glob)->getSection().str().find(rtmksec) != std::string::npos
								|| (*glob)->getSection().str().find(shared) != std::string::npos) {
						ignoreList<<(*glob)->getName().str()<<endl;
						continue;
				}

				if ((*glob)->getSection().str().find(pinned) != std::string::npos) {
						pinned_resources[(*glob)->getSection().str()].data.push_back(*glob);
						ignoreList<<(*glob)->getName().str()<<endl;
						continue;
				}

				if ((*glob)->getSection().str().find(clone) != std::string::npos) {
						cloneFuncs.push_back(*glob);
						ignoreList<<(*glob)->getName().str()<<endl;
						//TODO: Check if original function is never address taken of, if 
						// thats the case delete the original function to save memory.
						continue;
				}

				dfg<<(*glob)->getName().str() <<endl;

				if ((*glob)->getSection().str().find(secret) != std::string::npos) {
						secrets.push_back(*glob);
				}
#if 0
				if ((*glob)->getAttributes().hasAttribute("rtmkxcmd")) {
					auto attr = (*glob)->getAttributes().getAttribute("rtmkxcmd");
					auto kw = attr.getValueAsString().str();
					if (kw == "secret") { 
						secrets.push_back(*glob);
					}
				}
#endif

				vector<Function *> funcs;
				for (auto user: (*glob)->users()) {
						getFunctionfromUse(user, funcs, 0);
				}
				dfg<<"Used By:"<<endl;
				set<Function *> s( funcs.begin(), funcs.end() );
				funcs.assign( s.begin(), s.end() );
				for (auto func: funcs) {
						dfg<<func->getName().str()<<endl;
				}
				dfg<<"****************"<<endl;
		}
		dfg.close();


		ofstream secret_file;
		secret_file.open("./rtmk.secrets");
		for (auto sec: secrets) {
				llvm::SmallVector<DIGlobalVariableExpression *, 1> GVs;
                auto glob = sec;
				if (auto gv = dyn_cast<llvm::GlobalVariable>(sec)) {
    	            gv->getDebugInfo(GVs);
            	    for (auto *g: GVs) {
						secret_file<<sec->getName().str()<<"##"<<g->getVariable()->getFilename().str()<<endl;
					}
				}
		}

		ofstream pin;
		pin.open("./rtmk.pinned");
		for (auto res: pinned_resources) {
				auto pinTag = res.first;
				auto resources = res.second;
				pin<< pinTag <<endl;

				pin<<"Code"<<endl;
				for (auto val: resources.code) {
						pin<<"	"<<val->getName().str()<<endl;
				}
				pin<<"Data"<<endl;
				for (auto val: resources.data) {
                        pin<<"	"<<val->getName().str()<<endl;
                }
		}


		// Fmap basically captures the CFG
		ofstream ffmap;
		ofstream fdirmap;
		ffmap.open("./ffmap");
		fdirmap.open("./fdirmap");

		for(auto F = ll_mod->ifunc_begin(), E=ll_mod->ifunc_end(); F != E; ++F) {
				auto fun = F;
				cout<< fun->getName().str() <<endl;

		}
		//for (SVFModule::llvm_iterator F = svfModule->llvmFunBegin(), E = svfModule->llvmFunEnd(); F != E; ++F)
		for (auto F = ll_mod->begin(), E=ll_mod->end(); F!= E; ++F) 
		{
				//auto fun = *F;
				auto fun = F;
				volatile unsigned int i =0;
		//		string tt = "heval_I2c1";
			//	if (fun->getName().str() == tt)
		//				while(i ==0);
				//cout<< fun->getName().str() <<endl;

                if (fun->getSection().str().find(rtmksec) != std::string::npos) {
					ignoreList<<fun->getName().str()<<endl;
                    continue;
                }
                if (fun->getSection().str().find(shared) != std::string::npos) {
						ignoreList<<fun->getName().str()<<endl;
                        continue;
                }
				if (fun->isIntrinsic ()) {
						//ffmap<<fun->getName().str()<<"##" << "intrinsic"<<endl;
						ignoreList<<fun->getName().str()<<endl;
						continue;
				}

				if (fun->getSection().str().find(clone) != std::string::npos) {
                        cloneFuncs.push_back(&(*fun));
						ignoreList<<fun->getName().str()<<endl;
                        //TODO: Check if original function is never address taken of, if
                        // thats the case delete the original function to save memory.
                        continue;
                }
				int found = 0;
				for (auto bb=fun->begin();bb!=fun->end(); bb++) {
						if (found==1)
								break;
						for (auto stmt =bb->begin();stmt!=bb->end(); stmt++) {
								auto &debugInfo = stmt->getDebugLoc();
								if (debugInfo) {
										ffmap<<fun->getName().str()<<"##" <<debugInfo->getFilename().str() <<endl;
										fdirmap<<fun->getName().str()<<"##"<<debugInfo->getDirectory().str() <<endl; 
										found =1;
										break;
								}
						}
				}
				if (found ==0) {
						//ffmap<<fun->getName().str()<<"##" << "external"<<endl;
						cout<<fun->getName().str()<< " is defined externally" <<endl;
				}
		}
		//DFMAP basically captures the file location of all data variables
		ofstream dfmap;
        dfmap.open("./dfmap");
		for (auto G = svfModule->global_begin(), E = svfModule->global_end(); G != E; ++G) {
				llvm::SmallVector<DIGlobalVariableExpression *, 1> GVs;
				auto glob = &*G;
				(*glob)->getDebugInfo(GVs);
				for (auto *g: GVs) {
						dfmap<<(*glob)->getName().str()<<"##";
						dfmap<<g->getVariable()->getFilename().str() << endl;
				}
		}
		dfmap.close();
		//FDMAP basically captures all the data accesses by a function.
		ofstream fdmap;
		fdmap.open("./fdmap");
		for (SVFModule::llvm_iterator F = svfModule->llvmFunBegin(), E = svfModule->llvmFunEnd(); F != E; ++F)
		{
				auto fun = *F;
				string rtmksec= "rtmk";
                if (fun->getSection().str().find(rtmksec) != std::string::npos) {
					ignoreList<<fun->getName().str()<<endl;
                    continue;
                }
				string shared = "shared_func";
                if (fun->getSection().str().find(shared) != std::string::npos) {
						ignoreList<<fun->getName().str()<<endl;
                        continue;
                }
				for (auto bb=fun->begin();bb!=fun->end();bb++) {
						for (auto stmt =bb->begin();stmt!=bb->end(); stmt++) {
								/* Iterate all operands and see if there is inttoptr */
								for (auto op : stmt->operand_values()) {
										if (auto cast= dyn_cast<llvm::Instruction>(op)) {
												//                                      cout<< "********************************************"<<endl;
										}
										/* Harcoded pointers might appear as Constant Expressions */
										if (auto cast= dyn_cast<llvm::ConstantExpr>(op)) {
												/* Get the thing as an instruction */
												if (auto inttoptr = dyn_cast<llvm::IntToPtrInst>(cast->getAsInstruction())) {
														if (auto ptsTo = dyn_cast<llvm::ConstantInt>(inttoptr->getOperand(0))) {
																auto addr = *ptsTo->getValue().getRawData();
																if (addr == 0 || addr ==0xFFFFFFFF) {
																		/* These values could be error codes or a weird
																		   way to make a nullptr */
																		continue;
																}
																fdmap << fun->getName().str() << "##";
																fdmap << std::hex <<"0x"<<*ptsTo->getValue().getRawData() <<endl;
														}
												}
										}
								}
						}
				}
		}
		fdmap.close();

		//TODO: Use SVD parser like in other pass.
		ofstream fdevmap;
		fdevmap.open("./fdevmap");
		for (SVFModule::llvm_iterator F = svfModule->llvmFunBegin(), E = svfModule->llvmFunEnd(); F != E; ++F)
		{
				auto fun = *F;
				string rtmksec= "rtmk";
                if (fun->getSection().str().find(rtmksec) != std::string::npos) {
					ignoreList<<fun->getName().str()<<endl;
                    continue;
                }
				string shared = "shared_func";
                if (fun->getSection().str().find(shared) != std::string::npos) {
						ignoreList<<fun->getName().str()<<endl;
                        continue;
                }
				for (auto bb=fun->begin();bb!=fun->end();bb++) {
						for (auto stmt =bb->begin();stmt!=bb->end(); stmt++) {
								/* Iterate all operands and see if there is inttoptr */
								for (auto op : stmt->operand_values()) {
										/* Harcoded pointers might appear as Constant Expressions */
										if (auto cast= dyn_cast<llvm::ConstantExpr>(op)) {
												/* Get the thing as an instruction */
												if (auto inttoptr = dyn_cast<llvm::IntToPtrInst>(cast->getAsInstruction())) {
														cout << fun->getName().str() << " accesses "; inttoptr->dump();
														if (auto ptsTo = dyn_cast<llvm::ConstantInt>(inttoptr->getOperand(0))) {
																auto addr = *ptsTo->getValue().getRawData();
																if (addr == 0 || addr ==0xFFFFFFFF) {
																		/* These values could be error codes or a weird
																		   way to make a nullptr */
																		continue;
																}

																/* OK, this is a pointer, let's sanitize the shit out of its usage */
																if (isa<llvm::StoreInst>(stmt) || isa<llvm::LoadInst>(stmt)) {
																		if (stmt->hasNUsesOrMore(2)) {
																				cout<<"*****************************"<<endl;
																				stmt->dump();
																				printDI(dyn_cast<llvm::Instruction>(stmt));
																				cout<<"used by:  "<<endl;
																				int i =0;
																				for (auto user: stmt->users()) {
																						user->dump();
																						printDI(dyn_cast<llvm::Instruction>(user));
																				}
																		}
																}
																if (auto gep = dyn_cast<llvm::GetElementPtrInst>(stmt)) {
																		/* Trying to escape with pointer arithmetic not allowed */
																		gep->dump();
																		if (!gep->hasAllConstantIndices()) {
																				if (gep->getNumIndices ()  == 2) {
																						gep->getOperand(1)->dump();
																						gep->getOperand(2)->dump();
																						auto cr = computeConstantRange(gep->getOperand(2));

																				}
																		}
																}
																cout << std::hex <<"0x"<<*ptsTo->getValue().getRawData() <<endl;
																if (!vContains(iomap[addr], fun)) {
																		iomap[(unsigned int)addr].push_back(fun);
																}

																fdevmap << fun->getName().str() << "##";
																fdevmap << std::hex <<"0x"<<addr <<endl;
														}
												}
										}
								}
						}
				}
		}

		fdevmap.close();

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
				if (val->getName().str().compare("xTaskCreate")==0 || val->getName().str().compare("SafeTaskCreate")==0) {
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

		ofstream compat;
		vector<llvm::Function *> compat_vec;
        compat.open("./compat");
        for (SVFModule::llvm_iterator F = svfModule->llvmFunBegin(), E = svfModule->llvmFunEnd(); F != E; ++F) {
                Function *fun = *F;
				string rtmksec= "compat";
                if (fun->getSection().str().find(rtmksec) != std::string::npos) {
						compat<<fun->getName().str()<<endl;
						compat_vec.push_back(fun);
				}
        }
		ofstream reach;
		map<Function *, SmallPtrSet<Function*, 16>> thread_reach;
		reach.open("./rtmk.threadsreach");
		for (auto thread :thread_vec) {
				if (auto thread_fn =  dyn_cast<llvm::Function>(thread)) {
					SmallPtrSet<Function*, 16> visitedFunctions;
					forward_slice_crt(thread_fn, visitedFunctions, compat_vec);
					reach<<thread->getName().str()<<"\n";
					for (Function *F : visitedFunctions) {
	    	            reach << "	"<<F->getName().str() << "\n";
    	    	    }
					thread_reach[thread_fn] = visitedFunctions;
				}
		}

		ofstream loopInfoFile;
        loopInfoFile.open("./loop");
		//Add loop analysis.
		for (SVFModule::llvm_iterator F = svfModule->llvmFunBegin(), E = svfModule->llvmFunEnd(); F != E; ++F) {
			Function *func = *F;
			if (func->isIntrinsic()) {
					continue;
			}
		    if (!vContains(thread_vec, func)) {
					continue;
			}
			cerr<<"Ran once"<<endl;
			cerr<<func->getName().str()<<endl;
 	    	//get the dominatortree of the current function
    		llvm::DominatorTree* DT = new llvm::DominatorTree();
        	DT->recalculate(*func);
        	//generate the LoopInfoBase for the current function
        	llvm::LoopInfoBase<llvm::BasicBlock, llvm::Loop>* loopInfo = new llvm::LoopInfoBase<llvm::BasicBlock, llvm::Loop>();
        	loopInfo->releaseMemory();
        	loopInfo->analyze(*DT);

			cerr<<loopInfo->getTopLevelLoops().size()<<endl;
			for (auto loop : (*loopInfo)) {
					// Blocks may have no name, but SO says you can use this as the ID. In my xp, these names are repeated across functions
					// so complement it with function name.
					string temp;
					llvm::raw_string_ostream sstream(temp);
					loop->getHeader()->printAsOperand(sstream, false);
					loopInfoFile<<loop->getHeader()->getParent()->getName().str() << "##" << sstream.str() << endl;;
			}

		}


		//Run klee tainter
		if (!kleeFile.getValue().empty()) {
		string space = " ";
		for (auto thread: thread_vec) {
			string cmd = "klee --solver-backend=z3 --search=lloop --entry-point=";
			cmd += thread->getName().str();
			cmd += space;
    	    cmd += kleeFile.getValue();
			cout<<cmd<<endl;
	        system(cmd.c_str());
		}
		}



		ofstream cloned;
        cloned.open("./rtmk.cloned");
        for (auto res: cloneFuncs) {
                cloned<<res->getName().str()<<endl;
        }
		//Kick the compartmentalization phase
		if (!kleeFile.getValue().empty() || analysisOnly || partGuide.getValue().empty())
				return 0;


		auto pgFile = partGuide.getValue();
		auto safe_funcs_file = "";
		auto unsafe_funcs_file = "";
		//Create compartments
		if (partGuide.getValue().empty()) {
			string cmd = "./partition.py -c ";
			cmd += argv[InputFilename.getPosition()];
			cmd += " -d ./dg";
			system(cmd.c_str());
			pgFile = "./policy";
		}

		safe_funcs_file = "./.crtsafe";
		unsafe_funcs_file = "./.crtunsafe";

		std::ifstream infile(pgFile);
		std::string line;
		int compartmentID = 0;
		while (std::getline(infile, line))
		{
				vector <string> tokens;
				char *token = strtok((char *)line.c_str(), ",");
				while (token != NULL)
				{
						string str(token);
						char chars[] = "[]'' ";

						for (unsigned int i = 0; i < strlen(chars); ++i)
						{
								// you need include <algorithm> to use general algorithms like std::remove()
								str.erase (std::remove(str.begin(), str.end(), chars[i]), str.end());
						}
						tokens.push_back(str);
						compartmentMap[str] = compartmentID;
						token = strtok(NULL, ",");
				}
				compartments[compartmentID] = tokens;
				compartmentID++;
		}

		vector<string> safe_funcs_vec;
		get_crt_functions(safe_funcs_file, safe_funcs_vec);

		//Remove compatibility layer from safe_funcs
		for (auto compat: compat_vec) {
			if (vContains(safe_funcs_vec, compat->getName().str())) {
					safe_funcs_vec.erase(std::remove(safe_funcs_vec.begin(), safe_funcs_vec.end(), compat->getName().str()), safe_funcs_vec.end());
			}
		}

		vector<string> unsafe_funcs_vec;
		get_crt_functions(unsafe_funcs_file, unsafe_funcs_vec);

		for (auto elem: safe_funcs_vec) {
				cout<<elem<<endl;
		}
		crt_instrument(safe_funcs_vec, unsafe_funcs_vec);

		vector<string> crt_threads;
		for (auto thread: thread_vec){
			if (vContains(safe_funcs_vec, thread->getName().str())) {
				crt_threads.push_back(thread->getName().str());
			}
		}
		crt_intertask(crt_threads, unsafe_funcs_vec, thread_reach);

#ifdef DEBUG
		for(const auto& elem : compartments) {
				std::cout << elem.first << ":";
				for (const auto &mem: elem.second) {
						cout<<mem<<endl;
				}
		}
#endif




		//Partition global 
		for (auto G = svfModule->global_begin(), E = svfModule->global_end(); G != E; ++G) {
				auto glob = &*G;
				// Code to change sections
				//if (auto go= dyn_cast<llvm::GlobalObject>(*glob)) {
				if (auto go= dyn_cast<llvm::GlobalVariable> (*glob)) {
						string rtmksec= "rtmk";
						if (go->getSection().str().find(rtmksec) != std::string::npos) {
								continue;
						}
						string shared = "shared_data";
                		if (go->getSection().str().find(shared) != std::string::npos) {
                        	continue;
                		}
						string init= "init";
						if (go->getSection().str().find(init) != std::string::npos) {
								continue;
						}
						//Linker symbol used for rtmk
						if (go->getName().str() == "_shared_region") {
								continue;
						}
						string osec= "osection";
						if (go->getSection().str().find(osec) != std::string::npos) {
								continue;
						}
						string csec= "csection";
						if (go->getSection().str().find(csec) != std::string::npos) {
								continue;
						}
						debug<<go->getName().str()<<":"<<endl;
						debug<<"moved from "<<go->getSection().str()<< " to ";
						auto compartmentID = compartmentMap[go->getName().str()]; 
						//StringRef s = ".object_section" + std::to_string(compartmentID);
						StringRef s = ".osection" + std::to_string(compartmentID);
						//string st = ".object_section" + std::to_string(compartmentID);
						//cout<<st<<endl;
						go->setSection(s);
						debug<<go->getSection().str()<<endl;
				}
		}

		for (SVFModule::llvm_iterator F = svfModule->llvmFunBegin(), E = svfModule->llvmFunEnd(); F != E; ++F) {
				auto fun = *F;
				string rtmksec= "rtmk";
				if (fun->getSection().str().find(rtmksec) != std::string::npos) {
						continue;
				}
				string init= "init";
				if (fun->getSection().str().find(init) != std::string::npos) {
						continue;
				}
				string shared = "shared_func";
                if (fun->getSection().str().find(shared) != std::string::npos) {
                        continue;
                }

				debug<<fun->getName().str()<<":" <<endl;
				debug<<"moved from "<<fun->getSection().str()<< " to ";
				auto compartmentID = compartmentMap[fun->getName().str()];
				StringRef s = ".csection" + std::to_string(compartmentID);
				fun->setSection(s);
				debug<<fun->getSection().str()<<endl;
		}

		/* Find all the address taken function pointers */
		for (SVFModule::llvm_iterator F = svfModule->llvmFunBegin(), E = svfModule->llvmFunEnd(); F != E; ++F) { 
				auto fun = *F;
				for (auto bb=fun->begin();bb!=fun->end();bb++) {
						for (auto stmt =bb->begin();stmt!=bb->end(); stmt++) { 
								if (auto si = dyn_cast<llvm::StoreInst> (stmt)) {
										if (auto fun_ptee = dyn_cast <llvm::Function> (si->getValueOperand ())){
												cerr<<"Function Addr" <<endl;
												cerr<< fun_ptee->getName().str() <<endl;
												auto ptr = si->getPointerOperand ();
												//function_pointers[ptr] = fun_ptee;
												//function_pointers[si] = fun_ptee;
												function_pointers[fun_ptee] = fun_ptee;
										}
								}
						}
				}
		}

		map<int, int>heapMap;
		/* Find heap accesses */
		for (auto cset : compartments) {
				int instructions =0;
				int objects = 0;
				string fnName;
				string obName;
				for (auto obj : cset.second) {
						auto fun = ll_mod->getFunction(obj);
						if (fun) {
								auto callerID  = compartmentMap[fun->getName().str()];
								fnName = fun->getSection().str();
								for (auto bb=fun->begin();bb!=fun->end();bb++) {
										for (auto stmt =bb->begin();stmt!=bb->end(); stmt++) {
												if (auto ci= dyn_cast<llvm::CallInst> (stmt)) {
														if (ci->isInlineAsm ()) continue; /* TODO: Currently we don't cater to inline asm */
														auto callee = ci->getCalledFunction ();
														if (callee) {
																if (callee->getName().str() == "pvPortMalloc") {
																		heapMap[callerID] = 1; // 1 means for sure
																}
														} else {
																if (heapMap.count(callerID)) {
																		if (heapMap[callerID] != 1) {
																				heapMap[callerID] = 2; // 2 means maybe
																		}
																}
																else {
																		heapMap[callerID] = 2;
																}
														}
												}
										}
								}
						}
				}
		}
		getTasks();
		for (auto task:tasks) {
				heapMap[compartmentMap[task.name]] =1;
		}

		ofstream hstats;
		hstats.open("./rtmk.ha");

		for (auto pair: heapMap) {
				hstats<<pair.first<<":"<<pair.second<<endl;
		}

		ofstream stats;
		stats.open("./rtmk.stat");
		ofstream heapS;
		heapS.open("./rtmk.heaps");
		ofstream dataS;
		dataS.open("./rtmk.datas");
		ofstream instS;
		instS.open("./rtmk.insts");

		map<string, int> gstat;
		for (auto G = svfModule->global_begin(), E = svfModule->global_end(); G != E; ++G) {
				auto glob = &*G;
				gstat[(*glob)->getSection().str()]=0;
		}
		for (auto G = svfModule->global_begin(), E = svfModule->global_end(); G != E; ++G) {
				auto glob = &*G;
				gstat[(*glob)->getSection().str()]++;
		}
		for (auto cset : compartments) {
				int instructions =0;
				int objects = 0;
				string fnName;
				string obName;
				for (auto obj : cset.second) {
						auto fun = ll_mod->getFunction(obj);
						if (fun) {
								fnName = fun->getSection().str();
								for (auto bb=fun->begin();bb!=fun->end();bb++) {
										for (auto stmt =bb->begin();stmt!=bb->end(); stmt++) {
												instructions++;
										}
								}
						}
						else {
								auto glob=	ll_mod->getGlobalVariable(obj);
								if (glob) {
										obName = glob->getSection().str();
										objects++;
								}
								else cerr<<obj<<endl;
						}
				}
				stats<<"Compartment"<<cset.first<<":"<<endl;
				stats<<"	Code Section Name: "	<<fnName<<endl;
				stats<<"	Object Section Name: "	<<gstat[".osection"+ std::to_string(cset.first)]<<endl; 
				stats<<"	instructions: "<< instructions<<endl;
				instS<< instructions<<endl;
				stats<<"	objects: "<<objects<<endl;
				dataS<<objects<<endl;
				stats<<"	Heap Access:";
				if (heapMap[cset.first] == 0) {
						stats<< "No" <<endl;
						heapS<< "No" <<endl;
				} else if (heapMap[cset.first] == 1) {
						stats<< "Yes" <<endl;
						heapS<< "Yes" <<endl;
				} else {
						stats<< "Maybe" <<endl;
						heapS<< "Maybe" <<endl;
				}
		}

		for (auto temp: gstat) {
				stats<<temp.first<<":"<<temp.second<<endl;
		}

		//Lets allocate compartments for pinned resources here
		int lastAutoCompart = compartmentID;

		if (partGuide.getValue().empty()) {
		for (auto res: pinned_resources) {
                auto pinTag = res.first;
                auto resources = res.second;
				compartmentID++;
                for (auto val: resources.data) {
						if (auto go = dyn_cast<llvm::GlobalObject> (val)) {
	                        StringRef s = ".osection" + std::to_string(compartmentID);
    	                    go->setSection(s);
						}
                }
                for (auto val: resources.code) {
						if (auto go = dyn_cast<llvm::GlobalObject> (val)) {
    	                    StringRef s = ".csection" + std::to_string(compartmentID);
	                        go->setSection(s);
						}
                }
        }
		}

		/* Stats to hold number of xcalls */
		int directCall[compartmentID];
		int indirectCall[compartmentID];

		memset(directCall, 0, sizeof(directCall));
		memset(indirectCall, 0, sizeof(indirectCall));


		for (auto res: cloneFuncs) {
				if (auto fun = dyn_cast<llvm::Function> (res)) {
                for (auto user : fun->users ()) {
						cout<<"Dump old user"<<endl;
						user->dump();
				}
			}
		}
		vector<Value *> clones; //We need to track our clones so we don't instrument them.
        for (auto res: cloneFuncs) {
				if (auto fun = dyn_cast<llvm::Function> (res)) {
				vector<Value *> temp;
				//Deep copy for iteration
				for (auto user : fun->users ()) {
						temp.push_back(user);
				}
                for (auto user : temp) {
						user->dump();
						ValueToValueMapTy VMap;
					    auto cfun = CloneFunction(fun, VMap);
						clones.push_back(cfun);
						if (auto inst = dyn_cast<llvm::Instruction> (user) ){
						cfun->setSection(inst->getParent()->getParent()->getSection());

						if (auto ci= dyn_cast<llvm::CallInst> (user)) {
								ci->setCalledFunction (cfun);
								cout<<"Cloning to new function"<<endl;
								ci->dump();
						}
						}
				}
				}
        }

		/* TODO: Assign pinned resources their own compartments based on data or code */


		/* Instrument code for interprocess calls */
		for (SVFModule::llvm_iterator F = svfModule->llvmFunBegin(), E = svfModule->llvmFunEnd(); F != E; ++F) {
				auto fun = *F;
				string rtmksec= "rtmk";
				if (fun->getSection().str().find(rtmksec) != std::string::npos) {
						continue;
				}
				string init= "init";
				if (fun->getSection().str().find(init) != std::string::npos) {
						continue;
				}
				string shared = "shared_func";
				if (fun->getSection().str().find(shared) != std::string::npos) {
                        continue;
                }

				for (auto bb=fun->begin();bb!=fun->end();bb++) {
						for (auto stmt =bb->begin();stmt!=bb->end(); stmt++) {
								auto callerID  = compartmentMap[fun->getName().str()];
								if (auto ci= dyn_cast<llvm::CallInst> (stmt)) {
										if (ci->isInlineAsm ()) continue; /* TODO: Currently we don't cater to inline asm */
										auto callee = ci->getCalledFunction ();
										if (callee) {
												string rtmksec= "rtmk";
												if (callee->getSection().str().find(rtmksec) != std::string::npos) {
														cout<<"Skipping call because its to rtmk"<<endl;
														continue;
												}
												auto calleeID = compartmentMap[callee->getName().str()];
												/* See if this is a cross call */
												if (callerID == calleeID)
														continue;
												//TODO: Inconsistency between compartmentMap and actual compartment due to cloning
												if (vContains(clones, callee) || vContains(cloneFuncs, callee)) {
																continue;
												}
												/* See if this is a debug call/intrinsic */
												string llvm = "llvm";
												if (callee->getName().str().find(llvm) != std::string::npos) continue;
												IRBuilder<> Builder(stmt->getParent());
												BasicBlock::iterator it(stmt);it--;
												//Builder.SetInsertPoint(stmt->getNextNode()->getPrevNode());
												directCall[callerID]++;
												promoteXCall(ci, callee, stmt);
										}
										else {
												/* Indirect calls */
												cerr<<"Indirect Call"<<endl;
												vector<Function *> targets;
												auto called = ci->getCalledOperand();
												called->dump();
												auto ptr = called;
												if (auto li= dyn_cast<llvm::LoadInst>(called)) {
														ptr= li->getPointerOperand();
												}
												{
														cout<<"An alias pointer used"<<endl;
														ptr = called;
														ptr->dump();
														for(auto &pts: function_pointers) {
																//cerr<<"Comparing with:"; pts.first->dump();
																if (aliasQuery(fspta, ptr, pts.first)) {
																		cerr<<"Target Found:";
																		cerr<<pts.second->getName().str()<<endl;
																		targets.push_back(pts.second);
																}
														}
														set<Function *> s( targets.begin(), targets.end() );
														targets.assign( s.begin(), s.end() );
														/* See if all targets are within the same compartment?? */
														callerID = compartmentMap[fun->getName().str()];
														map<int, int> calledComp;
														int onlyTargetCache = 0;
														for (auto &t: targets) {
																calledComp[compartmentMap[t->getName().str()]] = 1;
																onlyTargetCache = compartmentMap[t->getName().str()];
														}

														if (calledComp.size() == 0) {
																/* Could not determine anything */
																cerr<<"Zero Target"<<endl;
																indirectCall[callerID]++;
																promoteXCallNoCaleeNoId(ci, stmt);
														}
														else if (calledComp.size() == 1) {
																/* Instrument function for direct call */
																cerr<<"Only1 targets"<<endl;
																if (onlyTargetCache != callerID) {
																		indirectCall[callerID] ++;
																		promoteXCallNoCalee(ci, stmt, onlyTargetCache);
																}

														} else {
																/* Instrument call so that runtime figures the required compartment */
																cerr<<"Multiple target"<<endl; //Specialize
																indirectCall[callerID]++;
																promoteXCallNoCaleeNoId(ci,stmt);
														}
												}

										}
								}
						}
				}

		}

		ofstream xcalls;
		xcalls.open("./rtmk.xcall");
		for (int i =0; i < compartmentID; i++) {
				xcalls<<"Compartment #:"<<i<<endl;
				xcalls<<"Direct xcalls:			"<<directCall[i]<<endl;
				xcalls<<"Indirect xcalls:		"<<indirectCall[i]<<endl;
		}

		ofstream serializedD;
		ofstream serializedI;
		serializedD.open("./rtmk.xcalld");
		serializedI.open("./rtmk.xcalli");
		for(int i =0; i< compartmentID; i++) {
				serializedD<<directCall[i]<<endl;
				serializedI<<indirectCall[i]<<endl;
		}


		updateBC();
		return 0;
		}

