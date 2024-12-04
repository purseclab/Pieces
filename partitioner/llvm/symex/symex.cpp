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
#include <llvm/Transforms/Utils/Cloning.h>
#include "llvm/ADT/SmallVector.h"
#include "llvm/IR/DebugInfoMetadata.h"
#include "util.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Pass.h"

/* Entry point for the function */
llvm::cl::opt<std::string> entry_point(cl::Positional, llvm::cl::desc("Entry Point"), llvm::cl::init("-"), cl::Required);
llvm::cl::opt<std::string> symbolics_file("symbolics", llvm::cl::desc("Global Variables you want to be symbolic, symex will recusrively make everything symbolic. For polymorphic pointers, you need annotations."), llvm::cl::init(""), cl::Optional);
llvm::cl::opt<unsigned int> THRESHOLD("threshold", llvm::cl::desc("Number to control accuracy of semantics, higher number means more modeled but will be slower"), llvm::cl::init(10), cl::Required);
llvm::cl::opt<bool> VOID_INLINE("void_inline", llvm::cl::desc("Consider void functions for semantics. Void function might not be pure, so can result in better semantics"), llvm::cl::init(false));

Type* getInnermostPointedToType(Type* type);
vector <string> global_symbolics;
Function * mk_sym;
ConstantPointerNull* null_ptr;
FunctionType *mk_type;
Function* entry_fn;
int symex_create_taint() {
		/* */
		int instrumented =0;

		/* Create function */
		Module *M = ll_mod;
		llvm::LLVMContext&  ctx = M->getContext();
		// Create the function type for the external function.
		/* void klee_make_symbolic(void *array, size_t nbytes, const char *name) */
		Type *retTy = Type::getVoidTy(ctx);
		Type *int64Ty = Type::getInt64Ty(ctx); //We symex on 32 bit
		Type* char_ptr_type = PointerType::getUnqual(Type::getInt8Ty(ctx));
		Type *args[] = {char_ptr_type, int64Ty, char_ptr_type};
		ConstantPointerNull* null_ptr = ConstantPointerNull::get(
						PointerType::getUnqual(Type::getInt8Ty(ctx)));
		FunctionType *mk_type = FunctionType::get(retTy, args, false);

		// Get the external function declaration.
		Function *mk_sym = M->getFunction("klee_make_symbolic");
		if (!mk_sym) {
				mk_sym = Function::Create(mk_type, GlobalValue::ExternalLinkage,
								"klee_make_symbolic", M);
		}

		mk_sym->dump();


		std::ifstream infile(symbolics_file);
		std::string line;
		vector <string> secrets;
		while (std::getline(infile, line)) {
				secrets.push_back(line);
				cout <<line << endl;
		}

		for (SVFModule::llvm_iterator F = svfModule->llvmFunBegin(), E = svfModule->llvmFunEnd(); F != E; ++F) {
				Function *fun = *F;
				Value * val = (Value *)fun;
				if (val->getName().str().compare(entry_point)==0) {

						for (auto secret_name: secrets) {
								auto secret = M->getNamedValue(secret_name);
								/* Instrument harness for symbolic variables */
								cout<<"Instrumenting: "<<entry_point<<endl;
								Function::iterator b = fun->begin();
								BasicBlock* BB = &(*b);
								auto first = &(*BB->begin());
								IRBuilder<> Builder(first);
								auto arg = secret;
								Value * v;
								Value * sizeInt;
								if (arg->getType()->isSized()) {
										v = Builder.CreatePointerCast(arg, Type::getInt8PtrTy(arg->getContext()));
										auto sizeP = Builder.CreateIntToPtr (ConstantInt::get(arg->getContext(),
																llvm::APInt(32, 0, false)), arg->getType());
										auto size = Builder.CreateConstGEP1_32 (NULL, sizeP, 1);
										sizeInt = Builder.CreatePtrToInt(size, Type::getInt64Ty(arg->getContext()));
								}
								auto call = Builder.CreateCall(mk_sym,{v, sizeInt, null_ptr});

								call->dump();
						}
						instrumented =1;
						return 0;
				}
		}
		if (instrumented ==0) {
				cout << "Not Instrumented"<<endl;
		}
		return 0;

}

#include <stack>
std::stack<std::pair<Value *, int>> wrk_list;
int make_symbolic(IRBuilder<> &Builder, Value * symbol, string name) {
		Value * v = NULL;
		Value * sizeInt;
		auto local_copy = false;
		PointerType *Int8PtrTy = Type::getInt8PtrTy(Builder.getContext());
		//Crude way to control recursion, no one making this long of a name
		if (name.length() > 80) {
				cerr<<"Ending nesting:"<<endl;
				cerr<<name<<" will not be symbolic"<<endl;
				return 0;
		} 
		if (symbol->getType()->isSized()) {
				auto type = symbol->getType();
				auto ty = type;
				if (auto * st = dyn_cast<StructType>(type)) {
						if (isa<Argument>(symbol)) {
					        errs() << "Argument: " << symbol->getName() << "\n";
				        }

						for (unsigned i = 0; i < st->getNumElements(); ++i) {
			                Type *FieldType = st->getElementType(i);
            			}
				}
				else if (auto ptr = dyn_cast<PointerType>(symbol->getType())) {
//						if (isa<Argument>(symbol)) {
						if (false) {
							//TODO;
#if 0
							AllocaInst * LocalVar = Builder.CreateAlloca(ptr->getElementType(), nullptr,"sym_local");
							symbol->replaceAllUsesWith(LocalVar);
							v = LocalVar;
#endif
						} else {
							if (getInnermostPointedToType(ptr)->isFunctionTy()) {
									cerr<< "Function Pointer requested, I will not make it symbolic"<<endl;
									ptr->dump();
									return 0;
							}
							if (isa<PointerType>(ptr->getElementType())) {
									cerr<<"Experimental Nested pointers support"<<endl;
									//If Function 
									if (getInnermostPointedToType(ptr->getElementType())->isFunctionTy()) {
											cerr<< "Function Pointer requested while nesting, I will not make it symbolic"<<endl;
											
		                                    ptr->dump();
											return 0;
									}
									//Allocate memory for the pointee as well.
									auto pointee_alloca = 
											Builder.CreateAlloca((cast<PointerType>(ptr->getElementType()))->getElementType(), nullptr, "pointee_mem");
//									auto local_ptr = Builder.CreateLoad(ptr->getElementType(), symbol, "local_load");
									auto local_str = Builder.CreateStore(pointee_alloca, symbol);
									make_symbolic(Builder, pointee_alloca, name);
									return 0;
							}
							else if (auto * st = dyn_cast<StructType>(ptr->getElementType())) {
									//Nested structs are in reverse order
									//https://www.mail-archive.com/klee-dev@imperial.ac.uk/msg02088.html
									Value * name_val = null_ptr;
									v = Builder.CreateBitCast(symbol, Int8PtrTy);

		                            auto sizeP = Builder.CreateIntToPtr (ConstantInt::get(symbol->getContext(),
                                        llvm::APInt(32, 0, false)), symbol->getType());
                    		        auto size = Builder.CreateConstGEP1_32 (NULL, sizeP, 1);
           			                sizeInt = Builder.CreatePtrToInt(size, Type::getInt64Ty(symbol->getContext()));
									if(!name.empty()) {
                Constant *StrConstant = ConstantDataArray::getString(Builder.getContext(), name);
                AllocaInst *StrAlloca = Builder.CreateAlloca(StrConstant->getType(), nullptr, "temp_name");
                Builder.CreateStore(StrConstant, StrAlloca);
                Value *StrPtr = Builder.CreateLoad(StrAlloca);
                //auto cast  = Builder.CreateBitCast(StrPtr, Int8PtrTy);
                //name_val = cast;
                //name_val = Builder.CreateConstGEP1_32(Type::getInt8Ty(Builder.getContext()), StrPtr, 0);
                Value *StrStartPtr = Builder.CreateConstGEP1_32(StrAlloca->getAllocatedType(), StrAlloca, 0, "str_start_ptr");
                Value * real_ptr = Builder.CreateConstGEP2_64 (StrAlloca->getAllocatedType(), StrAlloca, 0, 0, "real_ptr");
                name_val = real_ptr;


            }
            auto call = Builder.CreateCall(mk_sym,{v, sizeInt, name_val});


									std::stack<std::pair<Value *, int>> wrk_list;
									//Go through all fields while nesting in structures
									wrk_list.push(std::make_pair(symbol, 0));
									//Now the parent is symbolic, make pointers concrete
									//Structure types get special love
									while (!wrk_list.empty()) {
										std::pair<Value *, int> top = wrk_list.top();
										auto i = top.second;
										wrk_list.top().second++;
										auto symbol = top.first;
										auto ptr = cast<PointerType>(symbol->getType());
										auto st = cast<StructType>(ptr->getElementType());
										if (i < st->getNumElements()) {
				                            Type *FieldType = st->getElementType(i);
											Value *Idx[] = {
	                                                    Builder.getInt32(0), // first index (struct base address)
	                                                    Builder.getInt32(i)  // field index
	                                        };
											if (auto field_ptr = dyn_cast<PointerType>(FieldType)) {
												if ((getInnermostPointedToType(field_ptr))->isFunctionTy()) {
														cerr<<"Skipping Functino Pointer Field"<<endl;
														symbol->dump();
														return 0;
												}
												auto pointee_alloca =
	                 	                        Builder.CreateAlloca(field_ptr->getElementType(), nullptr, "pointee_mem");
												make_symbolic(Builder, pointee_alloca, name + "_field_" + std::to_string(i));
												Value *GEP = Builder.CreateGEP(st, symbol, Idx, "fieldPtr");
												Builder.CreateStore(pointee_alloca, GEP);
											}
											else if (auto field_struct_ty = dyn_cast<StructType>(FieldType)) {
												Value *GEP = Builder.CreateGEP(st, symbol, Idx, "fieldPtr");
												wrk_list.push(std::make_pair(GEP, 0));
												continue;
											}
				                        } else {
											if (wrk_list.empty())
													break;
											wrk_list.pop();
										}
									}
									return 0;
							}
	
							v = Builder.CreateBitCast(symbol, Int8PtrTy);

							auto sizeP = Builder.CreateIntToPtr (ConstantInt::get(symbol->getContext(),
                                        llvm::APInt(32, 0, false)), symbol->getType());
	                        auto size = Builder.CreateConstGEP1_32 (NULL, sizeP, 1);
    	                    sizeInt = Builder.CreatePtrToInt(size, Type::getInt64Ty(symbol->getContext()));
						}
				}
				else if (auto ptr = dyn_cast<llvm::VectorType>(ty)) {
						//TODO:
				}
				else if (auto ptr = dyn_cast<llvm::ArrayType>(ty)) {
						//TODO:
				}
				else if (ty->isFloatingPointTy()) {
						//TODO:
						if (isa<Argument>(symbol)) {
                                //The order is very critical
                            AllocaInst * LocalVar = Builder.CreateAlloca(ty, nullptr,"sym_local");
							v = Builder.CreateBitCast (LocalVar, Int8PtrTy);

							auto size = Builder.getInt64((ty->getPrimitiveSizeInBits()/8));
							Value * name_val = null_ptr;
            if(!name.empty()) {
                Constant *StrConstant = ConstantDataArray::getString(Builder.getContext(), name);
                AllocaInst *StrAlloca = Builder.CreateAlloca(StrConstant->getType(), nullptr, "temp_name");
                Builder.CreateStore(StrConstant, StrAlloca);
                Value *StrPtr = Builder.CreateLoad(StrAlloca);
                //auto cast  = Builder.CreateBitCast(StrPtr, Int8PtrTy);
                //name_val = cast;
                //name_val = Builder.CreateConstGEP1_32(Type::getInt8Ty(Builder.getContext()), StrPtr, 0);
                Value *StrStartPtr = Builder.CreateConstGEP1_32(StrAlloca->getAllocatedType(), StrAlloca, 0, "str_start_ptr");
                Value * real_ptr = Builder.CreateConstGEP2_64 (StrAlloca->getAllocatedType(), StrAlloca, 0, 0, "real_ptr");
                name_val = real_ptr;


            }
							auto call = Builder.CreateCall(mk_sym,{v, size, name_val});
							auto local_load = Builder.CreateLoad (ty, LocalVar);
							for (User *U : symbol->users()) {
     						   if (auto Inst = dyn_cast<StoreInst>(U)) {
									Builder.SetInsertPoint(Inst);
									v = Builder.CreateBitCast (Inst->getPointerOperand (), Int8PtrTy);
								
									local_copy =true;
									//Get rid of overwriting store
									Inst->removeFromParent();
									if (Instruction *instr = dyn_cast<Instruction>(v)) {
										Builder.SetInsertPoint(instr->getNextNonDebugInstruction());
									}
						       }
						    }
							if (!local_copy) {
									symbol->replaceAllUsesWith(local_load);
	    	                        auto store = Builder.CreateStore (symbol, LocalVar);
    	    	                    v = Builder.CreateBitCast (LocalVar, Int8PtrTy);
							}
                        }

						return 0;
				}
				else if (auto int_ty = dyn_cast<llvm::IntegerType>(ty)) {
						if (isa<Argument>(symbol)) {
								//The order is very critical
							auto local_copy = false;
                            AllocaInst * LocalVar = Builder.CreateAlloca(ty, nullptr,"sym_local");
							auto local_load = Builder.CreateLoad (ty, LocalVar);
							for (User *U : symbol->users()) {
                               if (auto Inst = dyn_cast<StoreInst>(U)) {
									Builder.SetInsertPoint(Inst);
                                    v = Builder.CreateBitCast (Inst->getPointerOperand (), Int8PtrTy);
                                    local_copy =true;
                               }
                            }
							if (!local_copy) {
							symbol->replaceAllUsesWith(local_load);
							auto store = Builder.CreateStore (symbol, LocalVar);
                            v = Builder.CreateBitCast (LocalVar, Int8PtrTy);
							}
                        }
						auto byte_width =1;
						if (int_ty->getBitWidth() > 8) {	
							byte_width = int_ty->getBitWidth()/8;
						}
						auto size = Builder.getInt64(byte_width);
						sizeInt = size;

				} else if (ty->isFloatingPointTy()){
					//v = Builder.CreatePointerCast(arg, Type::getInt8PtrTy(arg->getContext()));
					v = symbol;
				}
		}


		if (v) {
			//TODO: this is because pass isn't complete yet.
			Value * name_val = null_ptr;
			if(!name.empty()) {
				Constant *StrConstant = ConstantDataArray::getString(Builder.getContext(), name);
				AllocaInst *StrAlloca = Builder.CreateAlloca(StrConstant->getType(), nullptr, "temp_name");
				Builder.CreateStore(StrConstant, StrAlloca);
				Value *StrPtr = Builder.CreateLoad(StrAlloca);
				//auto cast  = Builder.CreateBitCast(StrPtr, Int8PtrTy);
				//name_val = cast;
				//name_val = Builder.CreateConstGEP1_32(Type::getInt8Ty(Builder.getContext()), StrPtr, 0);
				Value *StrStartPtr = Builder.CreateConstGEP1_32(StrAlloca->getAllocatedType(), StrAlloca, 0, "str_start_ptr");
				Value * real_ptr = Builder.CreateConstGEP2_64 (StrAlloca->getAllocatedType(), StrAlloca, 0, 0, "real_ptr");
				name_val = real_ptr;


			}
			auto call = Builder.CreateCall(mk_sym,{v, sizeInt, name_val});
			call->dump();
		}

		return 0;
}
int symex_create_summary(IRBuilder<> &builder) {
		/* */
		int instrumented =0;

		cerr<<"Generating Function summary for:" << entry_point<< endl;

		/* Create function */
		Module *M = ll_mod;
		llvm::LLVMContext&  ctx = M->getContext();
		// Create the function type for the external function.
		/* void klee_make_symbolic(void *array, size_t nbytes, const char *name) */
		Type *retTy = Type::getVoidTy(ctx);
		Type *int64Ty = Type::getInt64Ty(ctx); //We symex on 32 bit
		Type* char_ptr_type = PointerType::getUnqual(Type::getInt8Ty(ctx));
		Type *args[] = {char_ptr_type, int64Ty, char_ptr_type};
		ConstantPointerNull* null_ptr = ConstantPointerNull::get(
						PointerType::getUnqual(Type::getInt8Ty(ctx)));
		FunctionType *mk_type = FunctionType::get(retTy, args, false);

		// Get the external function declaration.
		Function *mk_sym = M->getFunction("klee_make_symbolic");
		if (!mk_sym) {
				mk_sym = Function::Create(mk_type, GlobalValue::ExternalLinkage,
								"klee_make_symbolic", M);
		}

		mk_sym->dump();

		std::string line;
		std::string fun_name = entry_point;
		Function *target = ll_mod->getFunction(fun_name);
		if (!target) {
				cerr<<"Undefined Function"<<endl;
				return -1;
		}
		if (target->empty()) {
				cerr<<"Function declared, but not defined"<<endl;
				return -1;
		}


		cout<<"Preparing SymEx workload"<<endl;
		for (auto & arg : target->args()) {
				arg.dump();
//				make_symbolic(builder, &arg);
		}

		return 0;

}
void generate_klee_fun() {
		/* Create function */
        Module *M = ll_mod;
        llvm::LLVMContext&  ctx = M->getContext();

		Type *retTy = Type::getVoidTy(ctx);
        Type *int64Ty = Type::getInt64Ty(ctx); //We symex on 32 bit
        Type* char_ptr_type = PointerType::getUnqual(Type::getInt8Ty(ctx));
        Type *args[] = {char_ptr_type, int64Ty, char_ptr_type};
        null_ptr = ConstantPointerNull::get(
                        PointerType::getUnqual(Type::getInt8Ty(ctx)));
        mk_type = FunctionType::get(retTy, args, false);


		// Get the external function declaration.
		mk_sym = M->getFunction("klee_make_symbolic");
		if (!mk_sym) {
				mk_sym = Function::Create(mk_type, GlobalValue::ExternalLinkage,
								"klee_make_symbolic", M);
		}
		mk_sym->dump();

		null_ptr = ConstantPointerNull::get(
                        PointerType::getUnqual(Type::getInt8Ty(ctx)));
}
void float_to_int_pass(Function * fun) {

}
int get_entry_fun() {
		std::string fun_name = entry_point;
		entry_fn = ll_mod->getFunction(fun_name);
        if (!entry_fn) {
                cerr<<"Undefined Function"<<endl;
                return -1;
        }
        if (entry_fn->empty()) {
                cerr<<"Function declared, but not defined"<<endl;
                return -1;
        }

		ClonedCodeInfo CodeInfo;
		ValueToValueMapTy VMap;
		Function * F = entry_fn;

		std::vector<Type*> ArgTypes;

  // The user might be deleting arguments to the function by specifying them in
  // the VMap.  If so, we need to not add the arguments to the arg ty vector
  //
  for (const Argument &I : F->args()) {
	VMap[&I] = UndefValue::get(I.getType());
    if (VMap.count(&I) == 0) // Haven't mapped the argument to anything yet?
      ArgTypes.push_back(I.getType());
  }

  // Create a new function type...
  FunctionType *FTy = FunctionType::get(entry_fn->getFunctionType()->getReturnType(),
                                    ArgTypes, entry_fn->getFunctionType()->isVarArg());

  // Create the new function...
  Function *NewF = Function::Create(FTy, F->getLinkage(), F->getAddressSpace(),
                                    F->getName(), F->getParent());

  // Loop over the arguments, copying the names of the mapped arguments over...
  Function::arg_iterator DestI = NewF->arg_begin();
  for (const Argument & I : F->args())
    if (VMap.count(&I) == 0) {     // Is this argument preserved?
      DestI->setName(I.getName()); // Copy the name over...
      VMap[&I] = &*DestI++;        // Add mapping to VMap
    }

		
  llvm::SmallVector<ReturnInst*, 8> Returns;  // Ignore returns cloned.
  CloneFunctionInto(NewF, F, VMap, F->getSubprogram() != nullptr, Returns, "",
                    &CodeInfo);


  //Maybe later i will write this pass
//		entry_fn = NewF;
		//float->int transform

		return 0;
}
void get_symbolics() {
		std::ifstream infile(symbolics_file);
        std::string line;
        vector <string> secrets;
        while (std::getline(infile, line)) {
                global_symbolics.push_back(line);
                cout <<line << endl;
        }
}
void symex_create_global_symbolics (IRBuilder<> &builder) {
		for (auto G = svfModule->global_begin(), E = svfModule->global_end(); G != E; ++G) {
				auto glob = *G;
				if (vContains(global_symbolics,  glob->getName().str())) {
								if (glob->isConstant() && glob->hasInitializer()) {
										//probably a hardcoded string
										cerr<<"Skipping Global Var"<<endl;
										glob->dump();
										continue;
								}
								make_symbolic(builder, glob, "global_var" + glob->getName().str());	
				}
		}
}

void symex_create_calls_symbolics(IRBuilder<> &builder) {
		vector<CallInst *> calls;
		for (auto bb=entry_fn->begin();bb!=entry_fn->end(); bb++) {
                                for (auto stmt =bb->begin();stmt!=bb->end(); stmt++) {
                                        if (auto ci = dyn_cast<llvm::CallInst> (stmt)) {
												auto ty = ci->getType();
												auto intrinsic = false;
												if (ci->getCalledFunction()) {
								                    intrinsic  = ci->getCalledFunction()->isIntrinsic();
								                }
												if (!ty->isVoidTy() && !intrinsic) {
														calls.push_back(ci);
												}
										}
								}
		}

		for (auto ci: calls) {
				builder.SetInsertPoint(ci);
				auto ty = ci->getType();
				auto local_var = builder.CreateAlloca(ty, nullptr, "sym_local_call");
				auto local_load = builder.CreateLoad (ty, local_var);
				string fun_name = "";
				if (ci->getCalledFunction()) {
					fun_name  = ci->getCalledFunction()->getName().str();
				}
				make_symbolic(builder, local_var, "external_call_"+ fun_name);
				auto local_load_fresh = builder.CreateLoad (ty, local_var);
				ci->replaceAllUsesWith(local_load_fresh);
				ci->removeFromParent();
		}
		
}

#include "llvm/IR/DebugInfoMetadata.h"
void parseDICompositeType(DICompositeType *DITy) {
        errs() << "Debug information for composite type: " << DITy->getName() << "\n";
        for (unsigned i = 0; i < DITy->getElements().size(); ++i) {
            if (auto *Member = dyn_cast<DIDerivedType>(DITy->getElements()[i])) {
                if (!Member->getName().empty()) {
                    errs() << "Field " << i << ": " << Member->getName() << "\n";
                }
            }
        }
        errs() << "\n";
}


void print_structs(llvm::Module & M) {
		for (auto &NamedMD : M.named_metadata()) {
            if (NamedMD.getName() == "llvm.dbg.cu") {
                for (auto *Op : NamedMD.operands()) {
						for (unsigned i = 0; i < Op->getNumOperands(); ++i) {
						if (!Op->getOperand(i))
								continue;
						Op->dump();
                        if (auto *MD = dyn_cast<MDNode>(Op->getOperand(i))) {
                            for (unsigned j = 0; j < MD->getNumOperands(); ++j) {
								if (MD->getOperand(j)) {
									MD->dump();
                                	if (auto *DITy = dyn_cast<DICompositeType>(MD->getOperand(j))) {
                                    	parseDICompositeType(DITy);
	                                }
								}
                            }
                        }
                    }
                }
            }
        }
}
#include "llvm/IR/DebugInfo.h"
void print_classes(llvm::Module & M) {
for (auto &NamedMD : M.named_metadata()) {
      for (auto Op : NamedMD.operands()) {
        if (auto *CompositeType = dyn_cast<DICompositeType>(Op)) {
          if (CompositeType->getTag() == dwarf::DW_TAG_class_type) {
            errs() << "Found class type: " << CompositeType->getName() << "\n";
          }
        }
      }
    }
//CLass types with instruction

	 for (auto &F : M) {
    for (const auto &BB : F) {
      for (const auto &I : BB) {
        if (auto *DbgDeclare = dyn_cast<DbgDeclareInst>(&I)) {
          if (auto *Var = dyn_cast<DILocalVariable>(DbgDeclare->getVariable())) {
            if (auto *DIType = Var->getType()) {
              if (auto *CompositeType = dyn_cast<DICompositeType>(DIType)) {
                if (CompositeType->getTag() == dwarf::DW_TAG_class_type) {
                  errs() << "Found class type: " << CompositeType->getName() << "\n";
                }
              }
            }
          }
        }
      }
    }
	 }

	DebugInfoFinder Finder;
    Finder.processModule(M);
    for (auto type : Finder.types()){
			if (auto *CompositeType = dyn_cast<DICompositeType>(type)) {
                if (CompositeType->getTag() == dwarf::DW_TAG_class_type) {
                  errs() << "Found class type: " << CompositeType->getName() << "\n";
                }
            }
	}
}

llvm::DICompositeType * get_class_di(string name) {
		DebugInfoFinder Finder;
    Finder.processModule(*ll_mod);
    for (auto type : Finder.types()){
            if (auto *CompositeType = dyn_cast<DICompositeType>(type)) {
                if (CompositeType->getTag() == dwarf::DW_TAG_class_type) {
						if (CompositeType->getName() == name)
								return CompositeType;
                }
            }
    }
	return NULL;
}
#include "llvm/IR/DebugInfoMetadata.h"
ofstream out_file("symex_debug_info.csv");
Type* getInnermostPointedToType(Type* type);

DIType * getBaseType(DIType * di_type) {
		if (auto di_derived = dyn_cast<llvm::DIDerivedType>(di_type)) {
				return getBaseType(di_derived->getBaseType());
		}
		return di_type;
}

void create_ptr_map(llvm::Value *arg, string name) {
		const DataLayout &DL = ll_mod->getDataLayout();
		auto ty = arg->getType();
		if (auto sty = dyn_cast<StructType>(getInnermostPointedToType(ty))) {
			auto di = get_class_di("PID_t");
			cerr<< sty->getName().str()<<" has size: "<<DL.getTypeSizeInBits(sty)/ 8;
			out_file <<name<<endl;//TODO: Parameterize this
			if (di) {
				di->dump();
				int i =0;
				for (auto elem: di->getElements()) {
						if (elem->getTag() == dwarf::DW_TAG_member) {
						 	if (auto MemberVar = dyn_cast<DIType>(elem)) {
							if 	(!(MemberVar->getFlags() & DINode::FlagStaticMember)) {
								string typeName = "primitive";
								if (auto field = dyn_cast<StructType>(getInnermostPointedToType(sty->getElementType(i)))) {
			                        typeName= field->getStructName().str();
            			        }
								out_file<< "Name:" <<MemberVar->getName().str();
								out_file<< ", Offset:" <<(MemberVar->getOffsetInBits()/8);
								out_file<< ", Type:" << typeName<<endl;
								i++;
							}
							}
						}
				}
	
			}
			else {

				Instruction * I;
				for (User *U : arg->users()) {
                	if (I = dyn_cast<Instruction>(U)) {
							break;
					}
				}
				if (const DILocation *Loc = I->getDebugLoc()) {
                            if (DISubprogram *Subprogram = Loc->getScope()->getSubprogram()) {
								for (auto node : Subprogram->getRetainedNodes()) {
									if (auto var_di = dyn_cast<llvm::DIVariable>(Subprogram->getRetainedNodes()[0])) {
											auto di_type = getBaseType(var_di->getType());
											if (auto composite = dyn_cast<llvm::DICompositeType>(di_type)) {
												int i =0;
												for (auto elem: composite->getElements()) {
							                        if (elem->getTag() == dwarf::DW_TAG_member) {
							                            if (auto MemberVar = dyn_cast<DIType>(elem)) {
								                            if  (!(MemberVar->getFlags() & DINode::FlagStaticMember)) {
								                                string typeName = "primitive";
								                                if (auto field = dyn_cast<StructType>(getInnermostPointedToType(sty->getElementType(i)))) {
								                                    typeName= field->getStructName().str();
								                                }
								                                out_file<< "Name:" <<MemberVar->getName().str();
								                                out_file<< ", Offset:" <<(MemberVar->getOffsetInBits()/8);
                                								out_file<< ", Type:" << typeName<<endl;
								                                i++;
								                            }
                            							}
							                        }
								                }
											}
            	                	}
								}
                            }
            	}
			}

#if 0
			//TODO: This way didn't get the padding right, investigate.
			int offset = 0;
			for (unsigned i = 0; i < sty->getNumElements(); ++i) {
					string typeName;
					if (auto field = dyn_cast<StructType>(getInnermostPointedToType(sty->getElementType(i)))) {
						typeName= field->getStructName().str();
					}

                            errs() << "Field " << i << ": "<< typeName <<
							":"<<DL.getTypeSizeInBits(sty->getElementType(i))/8
								   <<": offset:"<< offset <<"\n";
							offset += (DL.getTypeSizeInBits(sty->getElementType(i))/8);
          	}
#endif
		}

#if 0
		auto type = arg->getType(); 
		if (auto *STy = dyn_cast<StructType>(type)) {
            if (auto *DIType = arg->getMetadata("dbg")) {
                if (auto *DCompositeType = dyn_cast<DICompositeType>(DIType)) {
                    if (DCompositeType->getTag() == dwarf::DW_TAG_structure_type) {
                        for (unsigned i = 0; i < STy->getNumElements(); ++i) {
                            StringRef FieldName = DCompositeType->getName(i);
                            errs() << "Field " << i << ": " << FieldName << "\n";
                        }
                    }
                }
            }
        }
#endif 
}

void symex_create_args_symbolics(IRBuilder<> &builder) {
		int i =0;

		auto *subProgram = entry_fn->getSubprogram();
		subProgram->dump();
		auto ret_type = subProgram->getType()->getTypeArray()[0];

		for (Argument &arg : entry_fn->args()) {
			string name = "";
			Value *ArgFirstUser = arg.user_back();

			//This loop will almost always yield nothing cuz arguments are usually not used, instead a local var(alloca) is used.
			for (auto user : arg.users())  {
	    	    // Check if the user is a dbg.declare instruction
    	    	if (auto *DbgDeclare = dyn_cast<DbgDeclareInst>(user)) {
        	    	// Get the variable associated with the dbg.declare
	            	DIVariable *Variable = DbgDeclare->getVariable();

		            // Get the name of the argument from the variable
    		        llvm::StringRef ArgName = Variable->getName();

        		    // Print the name of the argument
					name = ArgName.str();
		        }
			}
			if (name.empty()) {
				//Check retained nodes
				for (auto node : subProgram->getRetainedNodes()) {
                    if (auto var_di = dyn_cast<llvm::DILocalVariable>(node)) {
											if (var_di->getArg () == i+1) {
													name = var_di->getName().str();
											}
                	}
                }
				//Just use the fkin IR name
				if (name.empty()) {
					name = arg.getName().str();
				}
			}
			name = "argument" + std::to_string(i++) + name;
			create_ptr_map(&arg, name);
			make_symbolic(builder, &arg, name);
		}
}

void symex_inline_funcs(IRBuilder<> &builder) {
		vector<CallInst *> calls;
        for (auto bb=entry_fn->begin();bb!=entry_fn->end(); bb++) {
                                for (auto stmt =bb->begin();stmt!=bb->end(); stmt++) {
                                        if (auto ci = dyn_cast<llvm::CallInst> (stmt)) {
                                                auto ty = ci->getType();
                                                if (!ty->isVoidTy() || VOID_INLINE) {
                                                        calls.push_back(ci);
                                                }
                                        }
                                }
        }

		for (auto ci: calls) {
			auto callee = ci->getCalledFunction();
			if (callee && !callee->isDeclaration()) {
					auto inlineFunc = true;
					//TODO: This fucks up for some reason, look into it
//					if (callee->getName().str() == "_ZN11NotchFilterIfE5applyERKf")
//							continue;
					cerr<<"Checking for "<< callee->getName().str() <<endl;
					int calls = 0;
					for (auto bb=callee->begin();bb!=callee->end(); bb++) {
							for (auto stmt =bb->begin();stmt!=bb->end(); stmt++) {
                                        if (auto ci = dyn_cast<llvm::CallInst> (stmt)) {
                                                auto ty = ci->getType();
                                                if (ty->isVoidTy() || callee->isIntrinsic()) {
														//We don't care about function that don't return or intrinsics
														continue;
                                                } else {
														calls++;
														if (calls > THRESHOLD) {
															inlineFunc = false;
														}
												}
                                        }
                                }
					}
					if (inlineFunc) {
							cerr<<"Inlining "<< callee->getName().str() <<endl; 
							InlineFunctionInfo IFI;
				            InlineFunction(*ci, IFI);
					}
			}
		}

}
int main(int argc, char ** argv) {
		int error_value = 0;
		string entry = entry_point;
		parseArguments(argc,argv);
		generate_klee_fun();
		get_symbolics();
		error_value = get_entry_fun();
		        auto b = entry_fn->begin();
        BasicBlock* BB = &(*b);
        auto first = &(*BB->begin());
        IRBuilder<> builder(first);
//		print_structs(*ll_mod);
//		print_classes(*ll_mod);
		if (!error_value) {
			symex_inline_funcs(builder);
			symex_create_global_symbolics(builder);
			symex_create_args_symbolics(builder);
			symex_create_calls_symbolics(builder);

		updateBC("symex_temp.bc");
		string cmd = "klee -disable-verify=true --write-smt2s -solver-backend=z3 --check-div-zero=false --check-overshift=false --optimize=false --search=lloop --libc=baremetal --entry-point=" + entry_point +" --allocate-determ --allocate-determ-start-address=0x10000 --debug-dump-stp-queries symex_temp.bc";

		cout<<"Running Command:";
		cout<<cmd<<endl;

		error_value = system(cmd.c_str());

    if (error_value == 0) {
        printf("KLEE execution completed successfully.\n");
    } else {
        printf("KLEE execution failed.\n");
    }
		}
		//Temp for debugging and hacking
		return error_value;
}
