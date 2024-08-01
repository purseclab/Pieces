#ifndef INITPIDOBJ_H_
#define INITPIDOBJ_H_

#include "UtilDef.h"
#include "BinIRWrapper.h"
#include "InterProcAnal.h"
#include <boost/algorithm/string.hpp>

class InitPIDObj: public BinIRWrapper {

public:
    InitPIDObj(){ assert(0); };
    InitPIDObj(SVFModule &inmodule, unsigned inmaxdist, unsigned inmaxcallercxt, string inconfig){
        module = inmodule;
        maxdist = inmaxdist;
        maxcallercxt = inmaxcallercxt;
        config = inconfig;
        binInit(module);
        setHeuristicGep(true);
    };

    // Type
    class CtrlInfo{
        public:
            const Function *func;
            string cur;
            string ref;
            string err;
            string out;
            string p;   // param
            string i;
            string d;
            string ff;
            string pidty;
            string ctrl;
            string gt;
            unsigned controlidx;
    };

    class AxisInfo: public UtilDef{
        public:
            string axis;
            vector <string> ctrls;

            // input processing
            AxisInfo(){return;};
            AxisInfo(string axis_in, string pidsstr){
                axis = axis_in;
                string delimiter = ",";
                ctrls = strspliter(pidsstr, delimiter);
                return;
            };

            // Analysis
            string getAxis(){ return axis; }

            int getNumCtrls(){ return ctrls.size(); }

            string getCtrl(int idx){ 
                if(ctrls.size() <= idx) return "NULL";
                else return ctrls[idx];
            }

            string getPos(){ return getCtrl(0); }
            string getVel(){ return getCtrl(1); }
            string getAcc(){ return getCtrl(2); }

            vector <string> retCtrls(){
                return ctrls;
            }

            bool isCtrlP(int idx){ 
                if(getCtrl(idx) == "P") return true;
                else return false;
            }
            bool isCtrlPI(int idx){ 
                if(getCtrl(idx) == "PI") return true;
                else return false;
            }
            bool isCtrlPID(int idx){ 
                if(getCtrl(idx) == "PID") return true;
                else return false;
            }
            bool isCtrlPIDFF(int idx){ 
                if(getCtrl(idx) == "PIDFF") return true;
                else return false;
            }

            bool isPosP(){
                if(isCtrlP(0)) return true;
                else return false;
            }
            bool isPosPI(){
                if(isCtrlPI(0)) return true;
                else return false;
            }
            bool isPosPID(){
                if(isCtrlPID(0)) return true;
                else return false;
            }
            bool isPosPIDFF(){
                if(isCtrlPIDFF(0)) return true;
                else return false;
            }

            bool isVelP(){
                if(isCtrlP(1)) return true;
                else return false;
            }
            bool isVelPI(){
                if(isCtrlPI(1)) return true;
                else return false;
            }
            bool isVelPID(){
                if(isCtrlPID(1)) return true;
                else return false;
            }
            bool isVelPIDFF(){
                if(isCtrlPIDFF(1)) return true;
                else return false;
            }

            bool isAccP(){
                if(isCtrlP(2)) return true;
                else return false;
            }
            bool isAccPI(){
                if(isCtrlPI(2)) return true;
                else return false;
            }
            bool isAccPID(){
                if(isCtrlPID(2)) return true;
                else return false;
            }
            bool isAccPIDFF(){
                if(isCtrlPIDFF(2)) return true;
                else return false;
            }
    };


    // main
    void interpreter();
    void updateAllCtrlVars(InterProcAnal *inipa);

    // Submodule of interpreter
    void reflectInputConfig(const Function *func, CtrlInfo *ci, string cs, string rs, string es, string os, string gt, string pidty, unsigned controlidx);
    vector <const Value *> updateCtrlVars(string word, const Function *func, unsigned controlidx, string kind);
    int CtrlVar2Idx(const Function *func, string word);

    // Submodule for Src tracking
    const Value* initForwardSrcPoint(const Value *sv, int dist, const Function *func);
    const Value* recursiveForwardSrcPoint(const Value *sv, int dist, const Function *func);
    const Value* recursiveArgForward(const Argument *av, int dist, const Function *func);

    // Submodule for Dst tracking
    const Value* initBackwardDstPoint(const Value *sv, int dist, const Function *func);
    const Value* recursiveBackwardDstPoint(const Value *sv, int dist, const Function *func);
    const Value* recursiveRetBackward(int dist, const Function *func);
    const Value* recursiveBackwardArgCopyFinder(const Value *sv, const Function *func);
    
    // Util
    set <const Instruction *> getCalleeCSset(const Function *callee);
    inline int strconfig2num(string s){
        int num;
        stringstream ss(s);
        if(s == "-") num = -1;
        else ss >> num;

        return num;
    }

    // Helper function
    inline bool isCV(const Value *v, string key){
        if(varmapset.find(key) == varmapset.end()) assert(0);
        else{
            if(varmapset[key].find(v) != varmapset[key].end()) return true;
            else return false;
        }
    }
    inline bool isCV(const Value *v){
        if(varmapset["cur"].find(v) != varmapset["cur"].end()) return true;
        else if(varmapset["ref"].find(v) != varmapset["ref"].end()) return true;
        else if(varmapset["err"].find(v) != varmapset["err"].end()) return true;
        else return false;
    }

    inline set <const Value *> retCVset(string key){
        if(varmapset.find(key) == varmapset.end()){
            set <const Value *> tempset;
            tempset.clear();
            return tempset;
        }
        else{
            return varmapset[key];
        }
    }

    inline set <const Value *> retCtrlVset(const Value *v){
        if(ctrlvarset.find(v) == ctrlvarset.end()) assert(0);
        else{
            return ctrlvarset[v];
        }
    }

    // Binary info
    inline string retbinname(){ return binname; }
    inline string retarch(){ return arch; }
    inline uint64_t retbaseoff(){ return baseoff; }
    inline uint64_t retbaseaddr(){ return baseaddr; }

    // Semantic
    inline bool isSemanticFunc(string role, const Function *func){
        if(semanticFuncSetMap.find(role) != semanticFuncSetMap.end()){
            if(semanticFuncSetMap[role].find(func) != semanticFuncSetMap[role].end()) return true;
        }
        return false;
    }

    // Result Output
    map <string, set <const Value *>> varmapset;
    map <const Value *, set <const Value *>> ctrlvarset;
    map <unsigned, set <const Value *>> ctrlidx2varset;
    map <const Value *, unsigned> var2ctrlidx;
    vector <map<string, set <const Value *>>> ctrlmapvec;
    map <const Value *, string> v2GroundTruthController;
    map <const Value *, string> v2ctrlty;
    map <const Value *, CtrlInfo *> v2ctrlinfo;
    map <const Value *, uint64_t> initcv2cvaddr;
        // semantic
            // keys P, I, D, PID, FF, atanf
    map <string, set <const Function *>> semanticFuncSetMap;
    map <string, AxisInfo> pidInfoMap;

private:
    // Input
    SVFModule module;
    unsigned maxdist;
    unsigned maxcallercxt;
    string config, binname, arch;
    InterProcAnal *ipa;

    // layout 
    uint64_t baseoff, baseaddr;

    // Interpreted control var info for step 2
    vector <CtrlInfo> ctrlVec; 

    // Util data
    map <const Function *, set <const Instruction *>> callee2csset;
};

#endif

