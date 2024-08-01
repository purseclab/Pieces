/*
 * DisPatch.cpp
 */

#include "DisPatch/DisPatch.h"

using namespace SVFUtil;

static cl::opt<string> config("config", cl::init(""), cl::desc("PID controller input file"));
static cl::opt<unsigned> maxdist("maxdist", cl::init(300), cl::desc("Maximally allowed distance between dependency instructions"));
static cl::opt<unsigned> maxcallercxt("maxcallercxt", cl::init(2), cl::desc("Maximally allowed caller which we explores to find data flows"));
static cl::opt<string> idxprint("idxprint", cl::init(""), cl::desc("Debug Index Print"));


char DisPatch::ID = 0;
ModulePass* DisPatch::modulePass = NULL;

DisPatch::DisPatch() :
    ModulePass(ID) {
    return;
}

DisPatch::~DisPatch() {
    return;
}

bool DisPatch::runOnModule(Module& inmodule) {
	SVFModule module = SVFModule(inmodule);
    modulePass = this;

    if(idxprint != ""){
        debug_instprint(idxprint);
    }

    // Seed control variable initialization
    InitPIDObj *initpid = new InitPIDObj(module, maxdist, maxcallercxt, config);
    initpid->interpreter();

    InterProcAnal *ipa = new InterProcAnal(module, initpid->retbinname(), initpid->retarch(), initpid->retbaseoff(), initpid->retbaseaddr());
    initpid->updateAllCtrlVars(ipa);

    // Semantic Controller Identifier 
        // Note: All varmapset's value must be instructions. Refer to CtrlIdentifier::innerVarDependencySorting
    CtrlIdentifier *ci = new CtrlIdentifier(module, initpid, ipa, maxdist, maxcallercxt);
    ci->cimain();

    cout << "[NOTE]: Worked to the end (DisPatch)" << endl;
    return true;
}


void DisPatch::debug_instprint(string fn) {
    const Function *func = module.getFunction(fn);
    cout << "Func: " << fn << endl;

    unsigned idx = 0;
    for(auto it = func->begin(); it != func->end(); ++it){
        const BasicBlock *bb = &*it;
        const Instruction *inst = &(bb->front());

        cout << "At " << idx << " -> inst: " << inst2str(inst) << endl;
        idx += bb->size();
    }
    assert(0);

    return;
}


void DisPatch::debug_asmprint(){
    set <string> strset;
    for(auto it = module.begin(); it != module.end(); ++it){
        const Function *func = *it;
        for(auto iit = inst_begin(func); iit != inst_end(func); ++iit){
            const Instruction *inst = &*iit;
            if(const CallInst *ci = SVFUtil::dyn_cast<CallInst>(inst)){    // BinOp, Load, Store possible
                const Function *callee = ci->getCalledFunction();
                if(callee != NULL && callee->size() == 0){  //debug
                    string inst_str;
                    raw_string_ostream rso(inst_str);
                    inst->print(rso);
                    if(inst_str.find("__asm_") != string::npos){
                        unsigned pos1 = inst_str.find("__asm_");
                        unsigned pos2 = inst_str.find("(");
                        string ss = inst_str.substr(pos1, pos2 - pos1);
                        strset.insert(ss);
                    }
                }
            }
        }
    }
    for(auto it = strset.begin(); it != strset.end(); ++it){
        string s = *it;
        cout << s << endl;
    }
    assert(0);
    return;
}
