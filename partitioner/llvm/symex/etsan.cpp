#include "util.h"
#include "llvm/IR/Instructions.h"
vector<Value *> ptrs;
vector<Value *> locals; 		 /* Locals are local variables */
vector<Value *> local_ptrs; 	 /* Local pointers */
vector<Value *> local_arrays;    /* Local Arrays */ // NOTE: LLVM arrays should always carry a size. So we can delegate this to static checking.
vector<Value *> local_live_ptrs; /* Local pointers that are used */
vector<Value *> str;    /* Stores are where we dereference pointers, here we check: store  */
						/* TODO: We don't need to check for pointers to primitive types like int, double, etc.*/
						/* TODO: Check for pointer arithmetic; getelementptr */
						/* TODO: New pointer are made using load */
						/* TODO: Get malloc and free */
bool gating = true;
vector<string> gate_func = {"test_etsan", "test_etsan2"};
map<Value*, int> pointers;
ofstream myfile("./ownership.etsan");
ofstream dinfo("./debug.etsan");
int pointerId =0;
int findOrId(Value * val) {
		if (pointers.count(val)) {
				return pointers[val];
		} 
		int ret = pointerId++;
		pointers[val] = ret;
		if (val->hasName()) {
			dinfo<<std::hex<<ret<<":"<<val->getName().str()<<endl;
		} else {
			dinfo<<std::hex<<ret<<":"<<"unnamed"<<ret<<endl;
		}
		if(auto load = dyn_cast<llvm::LoadInst>(val)) {
				if (auto global = dyn_cast<llvm::GlobalVariable>(load->getPointerOperand())){
						if(global->hasInitializer()) {
								myfile<<std::hex<<ret<<":"<<global->getName().str()<<endl;
						}
				}
		} else if (auto gep = dyn_cast<llvm::GetElementPtrInst>(val)) {
				if (auto global = dyn_cast<llvm::GlobalVariable>(gep->getPointerOperand())) {
						if(global->hasInitializer()) {
								myfile<<std::hex<<ret<<":"<<global->getName().str()<<endl;
						}
				}
		}
		return ret;	
}
void findPointers() {
	for (SVFModule::llvm_iterator F = svfModule->llvmFunBegin(), E = svfModule->llvmFunEnd(); F != E; ++F)
    {
		auto fun = *F;
		for (auto bb=fun->begin();bb!=fun->end();bb++) {
                for (auto stmt =bb->begin();stmt!=bb->end(); stmt++) {
						if (stmt->getType()->isPointerTy()) {
								findOrId(&(*stmt));
						}
				}
		}
	}
	cout<<pointerId <<" pointers found"<<endl;
	for (SVFModule::llvm_iterator F = svfModule->llvmFunBegin(), E = svfModule->llvmFunEnd(); F != E; ++F)
    {
        auto fun = *F;
		if (gating && !vContains(gate_func, fun->getName().str())) {
				continue;
		}
        for (auto bb=fun->begin();bb!=fun->end();bb++) {
                for (auto stmt =bb->begin();stmt!=bb->end(); stmt++) {
						if (stmt->getType()->isPointerTy ()){
								ptrs.push_back(dyn_cast<Value>(stmt));
						}

						if(auto local = dyn_cast<llvm::AllocaInst>(stmt)) {

								if (local->getAllocatedType()->isPointerTy ()) {
										local_ptrs.push_back(local);
								}
								if (local->getAllocatedType()->isArrayTy ()) {
										local_arrays.push_back(local);
								}
								locals.push_back(local);
						}
				}
		}
	}
}
bool isPointerToPointer(const Value* V) {
    const Type* T = V->getType();
	cout<<"Insude pointer to pointer"<<endl;
	V->dump();
    return T->isPointerTy() && T->getContainedType(0)->isPointerTy();
}

void staticVerif() {
	/* Statically determine the arrary overflow */
	for (SVFModule::llvm_iterator F = svfModule->llvmFunBegin(), E = svfModule->llvmFunEnd(); F != E; ++F)
	{
			auto fun = *F;
	        if (gating && !vContains(gate_func, fun->getName().str())) {
    	            continue;
        	}
	        for (auto bb=fun->begin();bb!=fun->end();bb++) {
    	            for (auto stmt =bb->begin();stmt!=bb->end(); stmt++) {
						if (auto gep = dyn_cast<llvm::GetElementPtrInst>(stmt)) {
								if (auto arr = dyn_cast<llvm::ArrayType>(gep->getSourceElementType())) {
										auto max = arr->getNumElements();
										auto DL = fun->getParent()->getDataLayout();
										unsigned BitWidth = DL.getIndexTypeSizeInBits(gep->getType());
										auto byteoffset = BitWidth/8;
									    APInt offset(BitWidth, 0);	

										if (gep->accumulateConstantOffset(DL,offset)) {
											auto off = *(offset.getRawData());
//											auto off1 = DL.getIndexTypeSizeInBits(arr->getElementType());											
											if (off > (max* byteoffset)) {
													cout<<"Out of bound access"<<endl;
													stmt->dump();
											}
										}
								}
						}
    	            }
        	}
	}
}

void instrDynamic() {
	/* Find all updates */
	for (SVFModule::llvm_iterator F = svfModule->llvmFunBegin(), E = svfModule->llvmFunEnd(); F != E; ++F)
    {
			auto fun = *F;
            if (gating && !vContains(gate_func, fun->getName().str())) {
                    continue;
            }
			for (auto bb=fun->begin();bb!=fun->end();bb++) {
                    for (auto stmt =bb->begin();stmt!=bb->end(); stmt++) {
							if (auto str = dyn_cast<llvm::StoreInst>(stmt)) {
									if (isPointerToPointer(str->getPointerOperand())) {
									}
							}

							if (auto bc = dyn_cast<llvm::BitCastInst>(stmt)) {
									if(bc->getDestTy()->isPointerTy()) {
											//Pointer to Pointer
											auto funcName = "etsan_log_pcopy";
											IRBuilder<> Builder(bc);
											auto func = ll_mod->getFunction(funcName);
											auto func_type = func->getFunctionType();
											auto f = ll_mod->getOrInsertFunction (funcName, func_type);
											Builder.SetInsertPoint(bc->getNextNode());
											//auto addr = Builder.CreateConstGEP1_32 (NULL, bc, 0);
//											auto dest = Builder.CreatePointerCast(addr, Type::getInt8PtrTy(func->getContext()));
//											auto src = Builder.CreatePointerCast(Builder.CreateConstGEP1_32 (NULL, bc->getOperand(0), 0), Type::getInt8PtrTy(func->getContext()));
											auto dest = Builder.getInt32 (findOrId(bc));
											auto src  = Builder.getInt32 (findOrId(bc->getOperand(0)));

											auto call = Builder.CreateCall(f,{dest, src});
											while((Value *)(&(*stmt))!= (Value *)call) stmt++;
									}
							}
							vector<string> mcreators = {"malloc"};

							if (auto ci = dyn_cast<llvm::CallInst>(stmt)) {
									if(ci->getCalledFunction()->isIntrinsic())
											continue;
									cout<< ci->getCalledFunction()->getName().str()<<endl;
									if (vContains(mcreators, ci->getCalledFunction()->getName().str())) {
											auto funcName = "etsan_log_pcreate";
											IRBuilder<> Builder(ci);
											auto func = ll_mod->getFunction(funcName);
                                            auto func_type = func->getFunctionType();
                                            auto f = ll_mod->getOrInsertFunction (funcName, func_type);
											auto p = ci;
											auto size = ci->getArgOperand(0);
											Builder.SetInsertPoint(ci->getNextNode());
                                            //auto addr = Builder.CreateConstGEP1_32 (NULL, ci, 0);
											//auto ad_addr = Builder.CreatePointerCast(ci, ci->getType()->getPointerTo());
											//auto addr = Builder.CreateLoad (cast<PointerType>(ad_addr->getType())->getElementType(), ad_addr);
											//addr->dump();
											auto pid = Builder.getInt32 (findOrId(ci));
											auto addr = Builder.CreatePointerCast(ci, Type::getInt8PtrTy(func->getContext()));
											//auto addr_arg = Builder.CreatePointerCast(ci, Type::getInt8PtrTy(func->getContext()));
											auto call = Builder.CreateCall(f,{pid, addr, size});
											while((Value *)(&(*stmt))!= (Value *)call) stmt++;
									}
									else if(ci->getType()->isPointerTy()) {
                                            //Pointer to Pointer
                                            auto funcName = "etsan_log_pcopy_consume";
                                            IRBuilder<> Builder(ci);
                                            auto func = ll_mod->getFunction(funcName);
                                            auto func_type = func->getFunctionType();
                                            auto f = ll_mod->getOrInsertFunction (funcName, func_type);
                                            Builder.SetInsertPoint(ci->getNextNode());
                                            //auto addr = Builder.CreateConstGEP1_32 (NULL, ci, 0);
                                            //auto dest = Builder.CreatePointerCast(addr, Type::getInt8PtrTy(func->getContext()));
											auto dest = Builder.getInt32(findOrId(ci));
                                            auto call = Builder.CreateCall(f,{dest});
                                            while((Value *)(&(*stmt))!= (Value *)call) stmt++;
                                    }
                            }

							if (auto gep = dyn_cast<llvm::GetElementPtrInst>(stmt)) {
									auto funcName = "etsan_log_pcopy";
                                            IRBuilder<> Builder(gep);
                                            auto func = ll_mod->getFunction(funcName);
                                            auto func_type = func->getFunctionType();
                                            auto f = ll_mod->getOrInsertFunction (funcName, func_type);
                                            Builder.SetInsertPoint(gep->getNextNode());
                                            //auto addr = Builder.CreateConstGEP1_32 (NULL, gep, 0);
                                            //auto dest = Builder.CreatePointerCast(addr, Type::getInt8PtrTy(func->getContext()));
                                            //auto src = Builder.CreatePointerCast(Builder.CreateConstGEP1_32 (NULL, gep->getPointerOperand(), 0), Type::getInt8PtrTy(func->getContext()));
											auto dest = Builder.getInt32 (findOrId(gep));
                                            auto src  = Builder.getInt32 (findOrId(gep->getPointerOperand()));
                                            auto call = Builder.CreateCall(f,{dest, src});
                                            while((Value *)(&(*stmt))!= (Value *)call) stmt++;
							}

							if (auto phi = dyn_cast<llvm::PHINode>(stmt)) {
									cout<<"Phi Node Inst"<<endl;
									if(phi->getType()->isPointerTy()) {
											//TODO
											int i=0;
											auto funcName = "etsan_log_pcopy_generate";
                                            auto func = ll_mod->getFunction(funcName);
                                            auto func_type = func->getFunctionType();
                                            auto f = ll_mod->getOrInsertFunction (funcName, func_type);
											func->addAttribute(AttributeList::FunctionIndex, Attribute::NoInline);
											for (Value * val: phi->incoming_values()) {
													auto block = phi->getIncomingBlock (i);
													auto insn = block->	getTerminator();
													IRBuilder<> Builder(insn);
													Builder.SetInsertPoint(insn);
													auto dlinsn = insn->getPrevNode();
													auto dl = dlinsn->getDebugLoc();
													//auto addr = Builder.CreateConstGEP1_32 (NULL, val, 0);
													//auto src = Builder.CreatePointerCast(addr, Type::getInt8PtrTy(func->getContext()));
													auto src = Builder.getInt32(findOrId(val));
													auto call = Builder.CreateCall(f,{src});
													call->setDebugLoc(dl);
													i++;
											}
											funcName = "etsan_log_pcopy_consume";
                                            IRBuilder<> Builder(phi);
                                            func = ll_mod->getFunction(funcName);
                                            func_type = func->getFunctionType();
                                            f = ll_mod->getOrInsertFunction (funcName, func_type);
                                            Builder.SetInsertPoint(phi->getNextNode());
                                            //auto addr = Builder.CreateConstGEP1_32 (NULL, phi, 0);
                                            //auto src = Builder.CreatePointerCast(addr, Type::getInt8PtrTy(func->getContext()));
											auto dest = Builder.getInt32 (findOrId(phi));
                                            auto call = Builder.CreateCall(f,{dest});
                                            while((Value *)(&(*stmt))!= (Value *)call) stmt++;	
									}
							}

							if (auto ret = dyn_cast<llvm::ReturnInst>(stmt)) {
									cout << "Ret"<<endl;
									if (ret->getReturnValue()->getType()->isPointerTy()) {
											auto funcName = "etsan_log_pcopy_generate";
                                            IRBuilder<> Builder(ret);
                                            auto func = ll_mod->getFunction(funcName);
                                            auto func_type = func->getFunctionType();
                                            auto f = ll_mod->getOrInsertFunction (funcName, func_type);
                                            Builder.SetInsertPoint(ret);
                                            //auto addr = Builder.CreateConstGEP1_32 (NULL, ret->getReturnValue(), 0);
                                            //auto src = Builder.CreatePointerCast(addr, Type::getInt8PtrTy(func->getContext()));
											auto src = Builder.getInt32(findOrId(ret->getReturnValue()));
                                            auto call = Builder.CreateCall(f,{src});
//                                            while((Value *)(&(*stmt))!= (Value *)call) stmt++;
									}
							}
					}
			}
#if 0
			for (auto bb=fun->begin();bb!=fun->end();bb++) { 
					for (auto stmt =bb->begin();stmt!=bb->end(); stmt++) {
							stmt->dump();
					}
			}
#endif 
	}
}
void dumpStats() {
	cout<<"Pointers>>>>>>>>"<<endl;
	for (auto it: ptrs){
			it->dump();
	}

	cout<<"Locals>>>>>>>>>>>"<<endl;
	for (auto it: locals) {
			it->dump();
	}

	cout << "Local Pointers>>>>>>"<<endl;
	for (auto it: local_ptrs) {
			it->dump();
			if(it->hasNUsesOrMore (2)) {
					local_live_ptrs.push_back(it);
			}
	}

	cout<<"Local live Pointers" << endl;
	for (auto it: local_live_ptrs) {
			it->dump();
	}

	cout<<"Local arrays" <<endl;
	for(auto it: local_arrays) {
			it->dump();
	}
}
vector<string> etsan_func = {"etsan_log_pcopy", "etsan_log_pcopy_consume", "etsan_log_pcopy_generate"};
bool checkInst() {
	for (SVFModule::llvm_iterator F = svfModule->llvmFunBegin(), E = svfModule->llvmFunEnd(); F != E; ++F)
    {
		auto fun = *F;
		for (auto bb=fun->begin();bb!=fun->end();bb++) {
                    for (auto stmt =bb->begin();stmt!=bb->end(); stmt++) {
                            if (auto ci = dyn_cast<llvm::CallInst>(stmt))  {
									if (ci->getCalledFunction() && ci->getCalledFunction()->hasName() && vContains(etsan_func, ci->getCalledFunction()->getName().str())) 
											return true;
							}
					}
		}
	}
	return false;
}
void etsan() {
	if (checkInst()) {
			cout<<"Binary Instrumented, Skipping ETSAN Pass"<<endl;
	}
	else {
		findPointers();
		staticVerif();
		instrDynamic();
	}
}
