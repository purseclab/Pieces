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
#include <fstream>
#include <limits>

vector<VFPair *> vfp;
vector<Value *> vec;
Type * qtype;
vector<Value *> uservalues;
std::vector<std::string> moduleNameVec;
vector<string> kernFuncs;
vector<string> userFuncs;
vector<string> safeFuncs;
map<string,int> creators;
map<unsigned int,desc> svdmap;
SVFModule* svfModule;
PAG *pag;
Andersen *ander;
FlowSensitive *fspta;
llvm::Module * ll_mod;
vector<tcb> tasks;
extern map<Function*,di> functionDI;
llvm::cl::opt<std::string> InputFilename(cl::Positional,
        llvm::cl::desc("<input bitcode>"), llvm::cl::init("-"), cl::Required);

llvm::cl::opt<std::string> KernFuncs(cl::Positional, llvm::cl::desc("<kernel functions>"), cl::Required);
llvm::cl::opt<std::string> UserFuncs(cl::Positional, llvm::cl::desc("<user functions>"), cl::Required);
llvm::cl::opt<std::string> SafeFuncs(cl::Positional, llvm::cl::desc("<safelist functions>"), cl::Required);
llvm::cl::opt<std::string> TaskCreateFuncs(cl::Positional, llvm::cl::desc("<Task Create functions>"), cl::Required);
llvm::cl::opt<std::string> IOMAP(cl::Positional, llvm::cl::desc("<IO Map for the device compiled"), cl::Required);
llvm::cl::opt<std::string> kleeFile("k", llvm::cl::desc("<KLEE Compatible bc for taint analysis/symex>"), cl::Optional);

llvm::cl::opt<std::string> partGuide("p", llvm::cl::desc("<KLEE Compatible bc for taint analysis/symex>"), cl::Optional);


llvm::cl::opt<bool> LEAKCHECKER("leak", llvm::cl::init(false),
                                       llvm::cl::desc("Memory Leak Detection"));
llvm::cl::opt<bool> PRINT_PTS_TO("pts_to", llvm::cl::init(false),
                                       llvm::cl::desc("Points to Kernel Values?"));

llvm::cl::opt<bool> DUMP("dump", llvm::cl::init(false),
                                       llvm::cl::desc("Dump offending values?"));



std::string gen_random(const int len) {

    std::string tmp_s;
    static const char alphanum[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";

    srand( (unsigned) time(NULL) * getpid());

    tmp_s.reserve(len);

    for (int i = 0; i < len; ++i)
        tmp_s += alphanum[rand() % (sizeof(alphanum) - 1)];


    return tmp_s;

}



std::fstream& GotoLine(std::fstream& file, unsigned int num){
    file.seekg(std::ios::beg);
    for(int i=0; i < num - 1; ++i){
        file.ignore(std::numeric_limits<std::streamsize>::max(),'\n');
    }
    return file;
}

void dumper(Value * val) {
		cerr << "***************************************************" <<endl;
		val->dump();
		cerr<< "****************************************************" <<endl;
}


/*!
 * An example to query alias results of two LLVM values
 */
AliasResult aliasQuery(PointerAnalysis* pta, Value* v1, Value* v2){
	return pta->alias(v1,v2);
}

/*!
 * An example to print points-to set of an LLVM value
 */
std::string printPts(PointerAnalysis* pta, Value* val){

    std::string str;
    raw_string_ostream rawstr(str);

    NodeID pNodeId = pta->getPAG()->getValueNode(val);
    const NodeBS& pts = pta->getPts(pNodeId);
    for (NodeBS::iterator ii = pts.begin(), ie = pts.end();
            ii != ie; ii++) {
        rawstr << " " << *ii << " ";
        PAGNode* targetObj = pta->getPAG()->getPAGNode(*ii);
        if(targetObj->hasValue()){
            rawstr << "(" <<*targetObj->getValue() << ")\t ";
        }
    }

    return rawstr.str();

}


/*!
 * An example to query/collect all successor nodes from a ICFGNode (iNode) along control-flow graph (ICFG)
 */
void traverseOnICFG(ICFG* icfg, const Instruction* inst){
	ICFGNode* iNode = icfg->getBlockICFGNode(inst);
	FIFOWorkList<const ICFGNode*> worklist;
	std::set<const ICFGNode*> visited;
	worklist.push(iNode);

	/// Traverse along VFG
	while (!worklist.empty()) {
		const ICFGNode* vNode = worklist.pop();
		for (ICFGNode::const_iterator it = iNode->OutEdgeBegin(), eit =
				iNode->OutEdgeEnd(); it != eit; ++it) {
			ICFGEdge* edge = *it;
			ICFGNode* succNode = edge->getDstNode();
			if (visited.find(succNode) == visited.end()) {
				visited.insert(succNode);
				worklist.push(succNode);
			}
		}
	}
}

/*!
 * An example to query/collect all the uses of a definition of a value along value-flow graph (VFG)
 */
void traverseOnVFG(const SVFG* vfg, Value* val){
	PAG* pag = PAG::getPAG();

    PAGNode* pNode = pag->getPAGNode(pag->getValueNode(val));
    const VFGNode* vNode = vfg->getDefSVFGNode(pNode);
    FIFOWorkList<const VFGNode*> worklist;
    std::set<const VFGNode*> visited;
    worklist.push(vNode);

	/// Traverse along VFG
	while (!worklist.empty()) {
		const VFGNode* vNode = worklist.pop();
		for (VFGNode::const_iterator it = vNode->OutEdgeBegin(), eit =
				vNode->OutEdgeEnd(); it != eit; ++it) {
			VFGEdge* edge = *it;
			VFGNode* succNode = edge->getDstNode();
			if (visited.find(succNode) == visited.end()) {
				visited.insert(succNode);
				worklist.push(succNode);
			}
		}
	}

    /// Collect all LLVM Values
    for(std::set<const VFGNode*>::const_iterator it = visited.begin(), eit = visited.end(); it!=eit; ++it){
    	const VFGNode* node = *it;
    //SVFUtil::outs() << *node << "\n";
        /// can only query VFGNode involving top-level pointers (starting with % or @ in LLVM IR)
        //if(!SVFUtil::isa<MRSVFGNode>(node)){
        //    const PAGNode* pNode = vfg->getLHSTopLevPtr(node);
        //    const Value* val = pNode->getValue();
        //}
    }
}

void streamToVec(ifstream& infile, vector<string>& vec) {
	std::string line;
	while (std::getline(infile, line))
	{
    		vec.push_back(line); 
	}
}

void printValsInFun(Function * fun) {
	for (inst_iterator I = inst_begin(*fun), J = inst_end(*fun); I != J; ++I) {
                                const Instruction *inst = &*I;
                                for (auto iter = inst->op_begin(); iter != inst->op_end(); ++iter) {
					if (!iter->get()->getName().empty())
                                        cerr << iter->get()->getName().data() << endl;
                                }
                        }
}
void pushValsInFun(Function * fun, vector<Value *>& vec, PAG *pag, vector<VFPair*> *vfp) {
        for (inst_iterator I = inst_begin(*fun), J = inst_end(*fun); I != J; ++I) {
                                const Instruction *inst = &*I;
                                for (auto iter = inst->op_begin(); iter != inst->op_end(); ++iter) {
					if (pag->hasValueNode(iter->get())) {
						//cerr<<"Pushing" << iter->get()->getName().data() <<endl;
                                        	vec.push_back((iter->get()));
											if(vfp){
												VFPair * pair = new VFPair;
												pair->val = iter->get(); pair->fun = fun;
												vfp->push_back(pair);
											}
					}	
                                }
                        }
}
static string intrinsics[] = {"llvm.trap", "llvm.dbg.declare"};
bool isIntrinsic(string target) {
		for (int i =0; i< (sizeof(intrinsics)/sizeof(intrinsics[0])); i++) {
				if(intrinsics[i].compare(target) == 0)
						return true;
		}
		return false;
}
void parseArguments(int argc, char ** argv) {
	int arg_num = 0;
    int error_value = 0;
    char **arg_value = new char*[argc];
    SVFUtil::processArguments(argc, argv, arg_num, arg_value, moduleNameVec);
    cl::ParseCommandLineOptions(arg_num, arg_value,
                                "Whole Program Points-to Analysis\n");

	ifstream kernFuncsFile(argv[KernFuncs.getPosition()]);
    streamToVec(kernFuncsFile, kernFuncs);
#if 0
    std::vector<string> kernFuncs((std::istreambuf_iterator<string>(kernFuncsFile)),
    std::istreambuf_iterator<string>());
#endif
    ifstream userFuncsFile(argv[UserFuncs.getPosition()]);
    streamToVec(userFuncsFile, userFuncs);


    ifstream safeFuncsFile(argv[SafeFuncs.getPosition()]);
    streamToVec(safeFuncsFile, safeFuncs);

    ifstream createFuncsFile(argv[TaskCreateFuncs.getPosition()]);
    vector<string> createFuncs;
    streamToVec(createFuncsFile, createFuncs);


    for (auto s: createFuncs) {
            std::string delimiter = ":";
            std::string name = s.substr(0, s.find(delimiter));
            string arg = s.substr(s.find(delimiter) + 1, s.size());
            int argi = stoi(arg, 0);
            creators.insert(pair<string, int>(name,argi));
    }

	ifstream iomapFile(argv[IOMAP.getPosition()]);
	desc temp;
	unsigned int addr;
	while(iomapFile >> temp.name)
	{
		iomapFile >> addr;
		iomapFile >> temp.size;
		iomapFile >> temp.driver;
		svdmap[addr] = temp;
	}

	cout << "*********************************" <<endl;
#if 0
	for (pair<int, desc> p : svdmap) {
			cout << std::hex << p.first << p.second.name << p.second.size <<endl;
	}
#endif 

	//REmove the functions that are supposed to be interfaces for the kernel. We could have local variables that
    //may alias to the local pointers in that function
    for (auto func: safeFuncs) {
        kernFuncs.erase(std::remove(kernFuncs.begin(), kernFuncs.end(), func), kernFuncs.end());
    }

}
void buildPTA() {
	svfModule = LLVMModuleSet::getLLVMModuleSet()->buildSVFModule(moduleNameVec);
    PAGBuilder builder;
    pag = builder.build(svfModule);
	// Create Andersen's pointer analysis
    //ander = AndersenWaveDiff::createAndersenWaveDiff(pag); TODO: This threw error will uncomment later
    //Create FlowSensitiveTBHC
    //FlowSensitiveTBHC fspta= FlowSensitiveTBHC(pag);
    static FlowSensitive fsptal = FlowSensitive(pag);
#if 01 // TODO: 
    fsptal.initialize();
    fsptal.analyze();
	fspta = &fsptal;
#endif 

#if 01
	PTACallGraph* callgraph = fspta->getPTACallGraph();
	    /// ICFG
    ICFG* icfg = pag->getICFG();
    //icfg->dump("icfg");

    /// Value-Flow Graph (VFG)
    VFG* vfg = new VFG(callgraph);
	vfg->dump("vfg");

    /// Sparse value-flow graph (SVFG)
    SVFGBuilder svfBuilder(true);
    SVFG* svfg = svfBuilder.buildFullSVFG(fspta);

	//svfg->dump("svfg");


#endif 

	SVFModule::llvm_iterator F = svfModule->llvmFunBegin();
	Function *fun = *F;
	ll_mod = fun->getParent();

}
void printDI(Instruction * instruction) {
		const llvm::DebugLoc &debugInfo = instruction->getDebugLoc();
						if (debugInfo) {
                        StringRef directory = debugInfo->getDirectory();
                        StringRef filePath = debugInfo->getFilename();
                        int line = debugInfo->getLine();
                        int column = debugInfo->getColumn();

                        cerr<<"Error: "<<directory.str()  << "/"<<filePath.str() << ":" << line << ":" << column<<endl;
						}
						else {
								instruction->dump();
						}
}
static auto init = false;
void getTasks() {
		if (!init) {
		init = true;
		std::ofstream threadD("threads.txt", std::ofstream::out);
		

		/* Get static threads for Zephyr */
        for (auto G = svfModule->global_begin(), E = svfModule->global_end(); G != E; ++G) {
                auto glob = &*G;
                if ((*glob)->getName().str().find("_k_thread_data_") != std::string::npos) {
//                        printBanner((*glob)->getName().str());
                        auto init = (*glob)->getInitializer();
                        if (auto cast = dyn_cast<llvm::User>(init->getOperand(3))) {
                                auto op = cast->getOperand(0);
                                if (auto task =  dyn_cast<llvm::Function>(op)) {
                                    string kernel_thread = "kernel";
                                    bool isKernelThread = false;
                                    if (task->getSection().str().find(kernel_thread) != std::string::npos) {
                                        isKernelThread = true;
                                    }
                                    threadD<<task->getName().str() <<(isKernelThread? ":Kernel": ":User")<<endl;
                                    TCB tcb;
                                    tcb.name = task->getName().str();
                                    tcb.func = task;

                                    if (!isKernelThread)
                                        pushValsInFun(task, tcb.objects,pag,NULL);
                                }
                        }
                }
        }
		for (SVFModule::llvm_iterator F = svfModule->llvmFunBegin(), E = svfModule->llvmFunEnd(); F != E; ++F) {
        //const SVFFunction* fun = LLVMModuleSet::getLLVMModuleSet()->getSVFFunction(*F);
        Function *fun = *F;
        Value * val = (Value *)fun;
        //cout << (*F)->getName().str()<<endl;
        auto name = (*F)->getName().data();

        /* Get all tasks */
        if(creators.count(name)) {
            /* Find all callers of this function */
            for(auto U : val->users()){
					int arg =0;
                    for(auto op: U->operand_values()) {
                            /* Here we can explicitly get the function
                               using the argument information, however instead
                               let's use a heuristic that only functions can
                               be passed taskcreation APIs as task, so probably
                               that would be the task function, in operand list
                               one could be creator itself */
							if (auto bitcast = dyn_cast<llvm::BitCastOperator>(op)) {
									op = bitcast->getOperand(0);
							}
                            if (auto task= dyn_cast<llvm::Function>(op)) {
                                if(task->getName().str() == name)
                                        continue;

								string kernel_thread = "kernel";
								bool isKernelThread = false;
				                if (task->getSection().str().find(kernel_thread) != std::string::npos) {
                			        isKernelThread = true;
                				}

								threadD<<task->getName().str() <<(isKernelThread? ":Kernel": ":User")<<endl;

                                TCB tcb;
                                tcb.name = task->getName().str();
                                tcb.func = task;
								
#if 0
                                for (auto bb=task->begin();bb!=task->end();bb++) {
                                        for (auto stmt =bb->begin();stmt!=bb->end(); stmt++) {
                                                cout<<stmt->getOpcodeName()<<endl;
                                        }
                                }
#endif
								if (!isKernelThread) 
								{
	                                pushValsInFun(task, tcb.objects,pag,NULL);

    	                            tasks.push_back(tcb);
								}
                            }
							else {
									U->dump();
#ifdef FREERTOS
									if (arg==0)
									cerr<<"ThreadDiscoveryError: Could not figure out the caller!"<<endl;
#endif
							}
							arg++;
					}
            }
        }
    	}
		threadD.close();
		}
}

Value * createGlobalPointer(Module * mod, Type * ty, string name) {
		auto init = ConstantPointerNull::get(PointerType::get(ty, 0));
		auto *GV = new llvm::GlobalVariable(
                          *mod, PointerType::get(ty,0), false, GlobalValue::InternalLinkage, init, name);
		return GV;
}
Value * createGlobal(Module * mod, Type * ty, string name) {
        //auto init = ConstantPointerNull::get(ty);
        auto *GV = new llvm::GlobalVariable(
                          *mod, ty, false, GlobalValue::InternalLinkage, nullptr, "");
		GV->setDSOLocal(true);
		//GV->setSection(".init"); //TODO:The section should be called something else, otherwise csu will call this symbol!
		GV->setAlignment(llvm::Align(4));
		// Constant Definitions
		ConstantAggregateZero* const_array_2 = ConstantAggregateZero::get(ty);

		// Global Variable Definitions
		GV->setInitializer(const_array_2);

        return GV;
}
int testPass() {

		for (SVFModule::llvm_iterator F = svfModule->llvmFunBegin(), E = svfModule->llvmFunEnd(); F != E; ++F)
		{
#if 0
				auto task = *F;
				auto name = (*F)->getName().str();
				if (name == "prvQueueReceiveTask") {
						cerr<<"Task found" <<endl;
						for (auto bb=task->begin();bb!=task->end();bb++) {
							 for (auto stmt =bb->begin();stmt!=bb->end(); stmt++) {
                                                cerr<<stmt->getOpcodeName()<<endl;
									 			stmt->dump();
												//continue;
												if (auto call= dyn_cast<CallInst>(stmt)) {
														cout << call->getCalledFunction()->getName().str()<<endl;
														if (call->getCalledFunction()->getName().str() == "_write") {
														//	llvm::ValueToValueMapTy vmap;
															auto *new_inst = call->clone();
															new_inst->insertAfter(stmt->getPrevNode());
														}
												}
                             }	
						}
				}
#endif 
				cerr << (*F)->getName().str() << endl;
		}
		return 1;
}
void updateBC() {
		verifyModule(*ll_mod);
		StringRef file = StringRef("temp.bc");
        std::error_code EC;
            //raw_fd_ostream output = raw_fd_ostream("temp.bc", EC); error: use of deleted function ‘llvm::raw_fd_ostream::raw_fd_ostream(const llvm::raw_fd_ostream&)’
		verifyModule(*ll_mod);
		//Holy Grail of debug
		ll_mod->dump();
        raw_fd_ostream output("temp.bc", EC);
        llvm::WriteBitcodeToFile(*ll_mod, output);
		cerr<<"temp.bc updated"<<endl;

#if 00
		for (SVFModule::llvm_iterator F = svfModule->llvmFunBegin(), E = svfModule->llvmFunEnd(); F != E; ++F) {
        	auto fun = *F;
			//fun->dump();
			if (fun->getName().str() == "test_etsan") {
					for (auto bb=fun->begin();bb!=fun->end();bb++) {
							for (auto stmt =bb->begin();stmt!=bb->end(); stmt++) {
									stmt->dump();
							}
					}
			}
		}
#endif 
}

static int i=0;
static vector<Value *> seen;
#define MAX_DEPTH 10000
void getFunctionfromUse(User * muse, vector<Function *>& users, int depth) {
		if (auto inst = dyn_cast<llvm::Instruction>(muse)) {
				auto func = inst->getFunction();
                        if(func)
                            users.push_back(func);
                        else {
                            cout<<"Function not null"<<endl;
                        }
		}
		for(auto user: muse->users()) {
				if (vContains(seen, user))
                        return;
				if (auto func = dyn_cast<llvm::Function>(user)) {
						if(!vContains(users, func))
								users.push_back(func);
				}
				else if (auto inst = dyn_cast<llvm::Instruction>(user))  {
						auto func = inst->getFunction();
                        if(func)
                        	users.push_back(func);
                        else {
                            cout<<"Function not null"<<endl;
                        }
				}
#if 0
				else if (auto type = dyn_cast<llvm::Type>(user)) {
						cout<<"encountered a type"<<endl;
				}
#endif 
				else {
						depth++;
						if (depth<MAX_DEPTH && user->hasNUsesOrMore(1))
							getFunctionfromUse(user,users, depth);
						else {
								if (vContains(seen, user) || user->hasNUses(0))
                                        return;
								seen.push_back(user);
								//Const to Const Data can be ignored, its probably LLVM used
								if (auto type = dyn_cast<llvm::Constant>(user)) {
										int i=0;
										for(auto user1: user->users()){
											if (auto type = dyn_cast<llvm::Constant>(user)) {
                                         		i++;
                                        	}
										}
										if (user->getNumUses () == i) {
												return;
										}
								}
								cout<<"********************"<<endl;
								cout<<"Depth Expired"<<endl;
								user->dump();
								cout<<"with "<<user->getNumUses ()<< " users:"<<endl;
								for(auto user1: user->users()){
										user1->dump();
								}
								cout<<"********************"<<endl;

						}
								
				}
		}
}



void printBanner(string s) {
	cout<<"#############################################################"<<endl;
	cout<<"#                    _                                      #"<<endl;
	cout<<"#                  -=\\`\\                                    #"<<endl;
	cout<<"#              |\\ ____\\_\\__                                 #"<<endl;
	cout<<"#            -=\\c`""""""" "`)                               #"<<endl;
	cout<<"#                 -==/ /                                    #"<<endl;
	cout<<"#                   '-'                                     #"<<endl;
	cout<<"#                  _  _                                     #"<<endl;
	cout<<s<<endl;
	cout<<"#                 ( `   )_                                  #"<<endl;
	cout<<"#                (    )    `)                               #"<<endl;
	cout<<"#              (_   (_ .  _) _)                             #"<<endl;
	cout<<"#                                             _             #"<<endl;
	cout<<"#                                            (  )           #"<<endl;
	cout<<"#             _ .                         ( `  ) . )        #"<<endl;
	cout<<"#           (  _ )_                      (_, _(  ,_)_)      #"<<endl;
	cout<<"#         (_  _(_ ,)                                        #"<<endl;
	cout<<"#############################################################"<<endl;
}
