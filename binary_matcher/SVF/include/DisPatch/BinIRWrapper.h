#ifndef BINIRWRAPPER_H_
#define BINIRWRAPPER_H_

#include <unistd.h>
#include <iostream>
#include <fstream>
#include <set>
#include <vector>
#include <stack>
#include <map>
#include <algorithm>
#include <stdlib.h>
#include <llvm/Pass.h>
#include <llvm/Analysis/ScalarEvolution.h>
#include <llvm/Analysis/LoopInfo.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/InstIterator.h>
#include <string>
#include <bits/stdc++.h>
#include <cstdio>
#include "Util/SVFUtil.h"
#include "MemoryModel/PAG.h"
#include "MemoryModel/PointerAnalysis.h"
#include "UtilDef.h"

#include <capstone/platform.h>
#include <capstone/capstone.h>

using namespace std;
using namespace llvm;
using namespace SVFUtil;

class FuncInfo{
    public:
        uint64_t startaddr;
        uint64_t endaddr;
        uint64_t sz;
        const Function *func;
        string exemode;
};


class BinIRWrapper: public UtilDef{
public:
    // Constant
    enum BinOpExtension {   
        VFMS = Instruction::InsertValue + 1, 
        VFMA = Instruction::InsertValue + 2
    };


    // Address Return
    uint64_t retFuncAddr(const Function *func);
    const Function* retFuncFromInstAddr(uint64_t addr);
    const Function* retFuncFromInstAddr(string straddr);
    uint64_t retFuncSz(uint64_t addr);
    string retFuncMode(uint64_t addr);
    void binFuncRangeUpdate(uint64_t startaddr, uint64_t endaddr, string exemode);
    void mapBinIndcs2ce(uint64_t csaddr, uint64_t ceaddr);
    void mapBinIndcs2ce(uint64_t csaddr, const Function *cefunc);
    uint64_t retBinIndcsaddr2ceaddr(uint64_t csaddr);
    uint64_t retSanitizedAddr(uint64_t rawaddr, uint64_t preaddr, string addrstr);
    uint64_t retBBAddr(const BasicBlock *bb);
    uint64_t inst2addr(const Instruction *inst){
        checkFunc(inst);
        if(inst2instaddr.find(inst) != inst2instaddr.end()) return inst2instaddr[inst];
        else return 0;
    }
    uint64_t inst2addr(const Value *v){
        if(const Instruction *inst = SVFUtil::dyn_cast<Instruction>(v)){
            checkFunc(v);
            return inst2addr(inst);
        }
        else return 0;
    }

    // inst ret
    vector <const Instruction *> retInstvecFromAddr(uint64_t addr){
        if(instaddr2instvec.find(addr) == instaddr2instvec.end()){
            vector <const Instruction *> nullvec;
            return nullvec;
        }
        else return instaddr2instvec[addr];
    }

    // Arg
    void retIRInputVars(const Function *func, set <const Value *> &inVars);

    // Gep Off
    set <unsigned> gep2off(set<const Value *> &geps);
    unsigned gep2off(const Value * v);

    // Function return;
    const Function* retIRFunc(uint64_t addr);
    const Function* retBinIndcsaddr2ce(uint64_t csaddr);
    const Function* retcsaddr2ce(uint64_t csaddr);
        //generic callee return;
    inline const Function* getCallee(const CallInst *ci){
        if(ci){
            const Function *ce = ci->getCalledFunction();
            if(ce->size()){
                //checkFunc(ce);
                return ce;
            }
            else if(inst2instaddr.find(ci) != inst2instaddr.end()){
                uint64_t csaddr = inst2instaddr[ci];
                ce = retBinIndcsaddr2ce(csaddr);
                if(ce){
                    //checkFunc(ce);
                    return ce;
                }
            }
        }
        return NULL;
    }
    inline const Function* getCallee(const Instruction *inst){
        if(inst){
            const CallInst *ci = SVFUtil::dyn_cast<CallInst>(inst);
            return getCallee(ci);
        }
        else return NULL;
    }
    inline const Function* getCallee(const Value *instv){
        const Instruction *inst = SVFUtil::dyn_cast<Instruction>(instv);
        return getCallee(inst);
    }

    // Setting
    void setHeuristicGep(bool enable){
        isHeuristicGep = enable;
        if(!isHeuristicGep) assert(0); // No such option at this time
        return;
    }
    void setHeuristicGep(string enable){
        if(enable == "true" || enable == "True" || enable == "TRUE") setHeuristicGep(true);
        else{
            setHeuristicGep(false);
            assert(0); // No such option at this time
        }
        return;
    }
    bool isHeursticGepMode(){
        if(isHeuristicGep) return true;
        else return false;
    }

    // Checker
    bool isCallInst(const Value *instv);
    bool isCastInst(const Value *instv);
    bool isBitCastInst(const Value *instv);
    bool isPtrCastInst(const Value *instv);
    bool isGepInst(const Value *instv);
    bool isLoadInst(const Value *instv);
    bool isStoreInst(const Value *instv);
    bool isVldrInst(const Value *instv);
    bool isVstrInst(const Value *instv);
    bool isBinOpInst(const Value *instv);
    bool isPhiInst(const Value *instv);
    bool isVmovInst(const Value *instv);
    bool isHeuriArg(const Value *instv);
    bool isGepFromMainThis(const Value *instv);
    bool isFromIntraSameThis(const Value *v1 , const Value *v2);
    bool isIntraAliasGep(const Value *v1 , const Value *v2, const Function *func);
    set <const Value *> retIntraAliasGep(const Value *v, const Function *func);
    const Value *recursiveGepOff(const Value *sink, const Function *func, set <const Value *> &argset, unsigned &off);
    const Value *retVLDRPtr(const Value *v);
    const Value *retVSTRPtr(const Value *v);
    const Value *retVMEMPtr(const Value *v);
    const Value *closestAliasInst(const Value *from, const Value *to, const Function *func);
    const Value *closestAliasInst(const Value *from, unsigned di, const Function *func);
    bool isRetAffectedByReadAlias(const Function *cefunc, unsigned ceAliasOff, const Value *v);
        // Checker's submodule
    bool isTargetHeapAccessRead(const Value *curv, const Value *initv, unsigned targetoff);

        //internal
    void inline checkFunc(const Function *func){
        binIRInit(func);
    }

    void inline checkFunc(const Instruction *inst){
        const Function *func = v2func(inst);
        if(func) checkFunc(func);
    }

    void inline checkFunc(const Value *instv){
        const Function *func = v2func(instv);
        if(func) checkFunc(func);
    }
    bool inline isHexStr(string str){
        bool falseflag = 0;
        for (int i = 0; i < str.length(); i++){
            if (!isxdigit(str[i])){
                falseflag = 1;
                break;
            }
        }

        if (falseflag) return true;
        else return true;
    }

    // Inst specific 
    const Value* retPtrVal(const Value *instv);    // For Load/Store
    const Value* retPtrPtr(const Value *instv);    // For Load/Store
    const Value* retPtrOffPtr(const Value *instv);    // For Load/Store
        // Asm instruction helper
    string retAsmStr(const Value *instv);
        // Binary OP specific
    bool isProcFP(const Value *instv);
    double retProcFP(const Value *instv);
    inline float int322fp( uint32_t floatingToIntValue){
        float *pf, f;
        pf = (float*)&(floatingToIntValue);
        f = *pf;
//        cout << "\t f: " << f << " from " << floatingToIntValue << endl;
        return f;
    }

    inline float int642fp(uint64_t floatingToIntValue){
        double *pf, f;
        pf = (double*)&(floatingToIntValue);
        f = *pf;
//        cout << "\t f: " << f << " from " << floatingToIntValue << endl;
        return f;
    }

            // Return Binary Op Type: Unsigned, Err = 0xffffffff
    unsigned retBinOpTy(const Value *instv);


    // Special type mapping
    void asmMapping(string inst_str, const Instruction *inst);
    void gepHeuristicMapping(vector <const Instruction*> &insts, const Function *func, unsigned idx);
    void mapInst2Regs(set <uint64_t> &addrset);
        // Submodule  of mapInst2Regs
    void indexRegs(vector <const Value *> &rvec, set <const Value *> &rset, vector <const Instruction *> &instvec);

        // Internal of gepMapping
    const Value *checkCastInst(const Value *instv, const Value *gi, const Function *func);
    const Value *checkOffsetInst(const Value *instv, const Value *gi, const Function *func);
    void offsetMappingHeuri(unsigned off, const Value *gi);            // w/ heuristic add is gep. You can change it if you want
    void readHeapFromCallee(const Function *func, set <unsigned> &offSet); //Currently, it is heuristic, VERY conservative function
    bool isWriteGivenHeap(const Function *func, set <unsigned> &offSet);

    // Init 
    void binInit(SVFModule &m);
    void binIRInit(const Function *func);
    uint64_t inferMissingEndAddr(const Function *func);
    string inferMissingExemode(const Function *func);

    // Callee 2 Caller
    set <const CallInst *> retCallSiteSet(const Function *cefunc, SVFModule &m);
    void initCallerSetMap(SVFModule &m);

    static bool init;
    static bool isHeuristicGep; // 1 = true / -1 = false / 0 = not determined
    static set <const Value *> ciset, giset, phiset, liset, siset, vldrset, vstrset, biset, castset, vmovset, heuriargset, bitcastset, ptrcastset;
    static set <const Function *> checkedfuncset;

    static map <const Value *, const Value *> pinst2val;
    static map <const Value *, const Value *> pinst2ptr;
    static map <const Value *, unsigned> binop2ty;

    // Addr 2 Read/Write Reg
    static map <uint64_t, vector <const Value *>> addr2IRrrvec;
    static map <uint64_t, vector <const Value *>> addr2IRwrvec;

    // gep related
    static set <const Value *> mainThisset;
    static map <const Value *, const Value *> gep2this;
    static map <const Value *, set<const Value *>> this2gepset;
    static map <uint64_t, FuncInfo *> addr2funcInfo;                        // Func addr
    static map <const Function *, FuncInfo *> func2funcInfo;
    static map <uint64_t, const Function *> indcsaddr2ce;
    static map <const Function *, uint64_t> indce2csaddr;
    static map <uint64_t, vector <const Instruction *>> instaddr2instvec;
    static map <const Instruction *, uint64_t> inst2instaddr;
    static map <const Value *, unsigned> gepCompute2offsum;
    static map <const Function *, map <unsigned, const Value *>> func2offsum2gepCompute;

    static map <const Function *, set <const CallInst*>> ce2cssetMap;
    static map <const Function *, set <const CallInst*>> func2cssetMap;
};

#endif
