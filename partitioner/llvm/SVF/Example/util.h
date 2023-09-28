#ifndef _UTIL_H_
#define _UTIL_H_
#include <llvm/IR/DebugLoc.h>
#include <llvm/IR/DebugInfoMetadata.h>
#include <llvm/Analysis/ValueTracking.h>
#include "llvm/ADT/APInt.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/InlineAsm.h"
#include "SVF-FE/LLVMUtil.h"
#include "Graphs/SVFG.h"
#include "WPA/Andersen.h"
#include "WPA/FlowSensitiveTBHC.h"
#include "SABER/LeakChecker.h"
#include "SVF-FE/PAGBuilder.h"
#include "MemoryModel/MemModel.h"
#include "SVF-FE/SymbolTableInfo.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/IR/PassManager.h"
#include "llvm/IR/Verifier.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/GlobalObject.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Module.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"
#include <fstream>
#include <iostream>
#include <ctime>
#include <unistd.h>
#include <queue>

#define FREERTOS
using namespace SVF;
using namespace llvm;
using namespace std;
void updateBC();
std::string gen_random(const int len);
Value * createGlobalPointer(Module * mod, Type * ty, string name);
Value * createGlobal(Module * mod, Type * ty, string name);
void dumper(Value * val);
typedef struct e{
        Value* val;
        Function * fun;
    } VFPair;

template<typename T> // declaration only for TD;
class TD;
//TD<decltype(x)> xType; where x is the thing you want to know 

extern vector<VFPair *> vfp;
extern vector<Value *> vec;
extern Type * qtype;
AliasResult aliasQuery(PointerAnalysis* pta, Value* v1, Value* v2);
std::string printPts(PointerAnalysis* pta, Value* val);
void traverseOnICFG(ICFG* icfg, const Instruction* inst);
void traverseOnVFG(const SVFG* vfg, Value* val);
void streamToVec(ifstream& infile, vector<string>& vec);
void printValsInFun(Function * fun);
void pushValsInFun(Function * fun, vector<Value *>& vec, PAG *pag, vector<VFPair*> *vfp);
bool isIntrinsic(string target);
void getFunctionfromUse(User * muse, vector<Function *>& users, int depth);
std::fstream& GotoLine(std::fstream& file, unsigned int num);


#define vContains(v, arg) (std::find(v.begin(), v.end(), arg) != v.end())
extern vector<Value *> uservalues;
extern std::vector<std::string> moduleNameVec;
extern vector<string> kernFuncs;
extern vector<string> userFuncs;
extern vector<string> safeFuncs;
extern map<string,int> creators;


void parseArguments(int argc, char ** argv);
void buildPTA();

extern SVFModule* svfModule;
extern PAG *pag;
extern Andersen *ander;
extern FlowSensitive *fspta;
extern llvm::Module * ll_mod;
void printDI(Instruction * instruction);


typedef struct tcb {
        string name;
        Function * func;
        vector<Value *> objects;
} TCB;
extern vector<tcb> tasks;
/* Value(Global Object)-New queue */
extern map<Value *, Value *> qmap;


void getTasks();
Value * createGlobalPointer(Module * mod, Type * ty, string name);
Value * createGlobal(Module * mod, Type * ty, string name);
int testPass();
void updateBC();
void printBanner(std::string s);


typedef struct {
        string dir;
        string file;
} di;
extern map<Function*,di> functionDI;
extern map<unsigned int, vector<Function*>, std::greater<unsigned int>> iomap;
//refcount[address][file] = access count;
extern map<unsigned int,map<string, int>> refcount;
/* Filtered version of svdmap */
typedef struct {
        std::string name;
        std::string driver;
		std::string base;
        unsigned int size;
} desc;
extern map<int, string> device;
extern llvm::cl::opt<std::string> InputFilename;
extern llvm::cl::opt<std::string> kleeFile;
extern llvm::cl::opt<std::string> partGuide;
#if 0
llvm::cl::opt<std::string> KernFuncs(cl::Positional, llvm::cl::desc("<kernel functions>"), cl::Required);
llvm::cl::opt<std::string> UserFuncs(cl::Positional, llvm::cl::desc("<user functions>"), cl::Required);
llvm::cl::opt<std::string> SafeFuncs(cl::Positional, llvm::cl::desc("<safelist functions>"), cl::Required);
llvm::cl::opt<std::string> TaskCreateFuncs(cl::Positional, llvm::cl::desc("<Task Create functions>"), cl::Required);
llvm::cl::opt<std::string> IOMAP(cl::Positional, llvm::cl::desc("<IO Map for the device compiled"), cl::Required);

llvm::cl::opt<bool> LEAKCHECKER("leak", llvm::cl::init(false),
                                       llvm::cl::desc("Memory Leak Detection"));
#endif 
extern llvm::cl::opt<bool> PRINT_PTS_TO;
extern llvm::cl::opt<bool> DUMP;
#endif 
int analyze();
