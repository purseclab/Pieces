#ifndef UTILDEF_H_
#define UTILDEF_H_

//#include <boost/cstdint.hpp>
#include <cstdint>
#include <stdint.h>
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
#include "Util/SVFUtil.h"      // Keep this order
#include "MemoryModel/PAG.h"
#include "MemoryModel/PointerAnalysis.h"

using namespace std;
using namespace llvm;
using namespace SVFUtil;

class PointerAnalysis;
class AndersenWaveDiff;
class SVFModule;

class UtilDef{
public:
    // Instruction related util
    inline const Function *getCallee(const Instruction *inst){
        if(const CallInst *ci = SVFUtil::dyn_cast<CallInst>(inst)){
            if(ci->getCalledFunction()) return ci->getCalledFunction();
            else if(ci->getNumOperands() > 0){
                const Value *v = ci->getOperand(0)->stripPointerCasts();
                if(const Function *callee = SVFUtil::dyn_cast<Function>(v)) return callee;
                else return NULL;
            }
            else return NULL;
        }
        //else if(const InvokeInst *ci = SVFUtil::dyn_cast<InvokeInst>(inst)) return ci->getCalledFunction();
        //else if(isa<CallBrInst>(inst)) return assert(0 && "[Error]: Does CallBrInst exist?"); // -> Deprecated
        else return NULL;
    }

    virtual bool isIndirectCall(const Instruction *inst){
        if(const CallInst *ci = SVFUtil::dyn_cast<CallInst>(inst))
            if(ci->getCalledFunction() == NULL) return true;
            else return false;
        else if(const InvokeInst *ci = SVFUtil::dyn_cast<InvokeInst>(inst)){
            if(ci->getCalledFunction() == NULL) return true;
            else return false;
        }
        //else if(isa<CallBrInst>(inst)) assert(0 && "[Error]: Does CallBrInst exist?");
        else return false;
    }

    virtual bool isCallInst(const Value *inst){//const Instruction *inst){  
        // Value is more generic and anyhow, it returns NULL if it is just Value
        if(const CallInst *ci = SVFUtil::dyn_cast<CallInst>(inst)) return true;
        else if(const InvokeInst *ci = SVFUtil::dyn_cast<InvokeInst>(inst)) return true;
        //else if(isa<CallBrInst>(inst)) assert(0 && "[Error]: Does CallBrInst exist?");
        else return false;
    }

    // Util
    inline const Function * v2func(const Value *v){
        const Function *func = NULL;
        if(v){
            if(const Instruction *vi = SVFUtil::dyn_cast<Instruction>(v))   func = vi->getParent()->getParent();
            else if(const Argument *vi = SVFUtil::dyn_cast<Argument>(v))    func = vi->getParent();
        }
        return func;
    }

    inline const Function * v2func(const Instruction *inst){
        return inst->getParent()->getParent();
    }

    inline const Function* inst2func(const Instruction *inst){
        return inst->getParent()->getParent();
    }

    unsigned func2sz(const Function *func){
        unsigned sz = 0;

        if(func2szMap.find(func) == func2szMap.end()){  //idxing
            for(auto it = inst_begin(func); it != inst_end(func); ++it) sz++;
        }
        else sz = func2szMap[func];

        return sz;
    }

    void instIdxer(const Function *func){
        unsigned idx = 0;
        for(auto it = inst_begin(func); it != inst_end(func); ++it){
            const Instruction * inst = &*it;
            func2inst2idxmap[func][inst] = idx;
            func2idx2instmap[func][idx] = inst;
            idx++;
        }

        return;
    }

    const Instruction* idx2inst(const Function *func, unsigned idx){
        if(func2idx2instmap.find(func) == func2idx2instmap.end()) instIdxer(func);

        if(func2idx2instmap[func].find(idx) == func2idx2instmap[func].end()) return NULL;
        else return SVFUtil::dyn_cast<Instruction>(func2idx2instmap[func][idx]);
    }

    const Value* idx2arg(const Function *func, unsigned idx){
        if(func2idx2argmap.find(func) == func2idx2argmap.end()){
            unsigned i = 0;
            for(auto it = func->arg_begin(); it != func->arg_end(); ++it){
                const Value *v = &*it;
                func2idx2argmap[func][i] = v;
                i++;
            }
        }
        return func2idx2argmap[func][idx];
    }

    unsigned inst2idx(const Instruction *inst){
        const Function *func = inst->getParent()->getParent();

        if(func->size() == 0) return 0;
        else if(func2inst2idxmap.find(func) == func2inst2idxmap.end()) instIdxer(func);

        return func2inst2idxmap[func][inst];
    }

    unsigned inst2idx(const Function *func, const Instruction *inst){
        if(func->size() == 0) return 0;
        else if(func2inst2idxmap.find(func) == func2inst2idxmap.end()) instIdxer(func);

        return func2inst2idxmap[func][inst];
    }

    unsigned inst2idx(const Value *v){  //only instruction acceptable
        const Function *func = v2func(v);
        if(func->size() == 0) return 0;
        else if(func2inst2idxmap.find(func) == func2inst2idxmap.end()) instIdxer(func);

        if(const Instruction *inst = SVFUtil::dyn_cast<Instruction>(v)) return func2inst2idxmap[func][inst];
        else assert(0);
    }

    unsigned inst2idx(const Function *func, const Value *v){        //only instruction acceptable
        if(func->size() == 0) return 0;
        else if(func2inst2idxmap.find(func) == func2inst2idxmap.end()) instIdxer(func);

        if(const Instruction *inst = SVFUtil::dyn_cast<Instruction>(v)) return func2inst2idxmap[func][inst];
        else assert(0);
    }

    unsigned insts2maxidx(const Function *func, set <const Value *> vset){
        unsigned maxidx = 0;
        for(auto it = vset.begin(); it != vset.end(); ++it){
            const Value *v = *it;
            unsigned idx = inst2idx(func, v);

            if(idx > maxidx) maxidx = idx;
        }
        return maxidx;
    }

    

    unsigned arg2idx(const Function *func, const Value *v){
        if(func2arg2idxmap.find(func) == func2arg2idxmap.end()){
            unsigned i = 0;
            for(auto it = func->arg_begin(); it != func->arg_end(); ++it){
                const Value *argv = &*it;
                func2arg2idxmap[func][argv] = i;
                i++;
            }
        }
        return func2arg2idxmap[func][v];
    }


    const Instruction* str2inst(const Function *func, string targetname){
        const Instruction *targetinst = NULL;
        for(auto it = inst_begin(func); it != inst_end(func); ++it){
            const Instruction *inst = &*it;
            if(inst->hasName()){
                string n = inst->getName().str();
                if(n == targetname){
                    if(targetinst){
                        cout << "[Error]: Both instructions have the same name, " << targetname << endl;
                        cout << "\t\t>> Previously Found: " << inst2str(targetinst) << endl;
                        cout << "\t\t>> Currently Found: " << inst2str(inst) << endl;
                        assert(0);
                    }
                    else{
                        targetinst = inst;
                    }
                }
            }
        }

        return targetinst;
    }

    string inline func2name(const Function *func){
        return func->getName().str();
    }

    vector<const Instruction*> func2insts(const Function *func){
        if(func2instvec.find(func) == func2instvec.end()){
            if(func->size() > 0){
                for(auto it = inst_begin(func); it != inst_end(func); ++it){
                    const Instruction *inst = &*it;
                    func2instvec[func].push_back(inst);
                }
            }
            else{
                vector<const Instruction*> instvec;
                instvec.clear();
                func2instvec[func] = instvec;
            }
        }
        return func2instvec[func];
    }

    // Argument util
    const Argument *getArgFromFunc(const Function *func, unsigned th){
        const Argument *arg;
        unsigned i = 0;
        for(auto it = func->arg_begin(); it != func->arg_end(); ++it){
            if(i == th){
                arg = &*it;
                return arg;
            }
            i++;
        }
        cout << "\n[Error]: func name: " << func->getName().str() << endl;
        return NULL;
    }

    // Basic block related
    unsigned numbbPred(const BasicBlock *bb){
        unsigned p = 0;

        if (!bb) return 0;

        for (auto PI = pred_begin((BasicBlock *)bb); PI != pred_end((BasicBlock *)bb); PI++){
            const BasicBlock *tp = (const BasicBlock *)*PI;
            p++;
        }
        return p;
    }

    unsigned numbbSucc(const BasicBlock *bb){
        unsigned s = 0;

        if (!bb) return 0;

        for (auto SI = succ_begin((BasicBlock *)bb); SI != succ_end((BasicBlock *)bb); SI++){
            const BasicBlock *tp = (const BasicBlock *)*SI;
            s++;
        }
        return s;
    }

    // debugging helper
    inline string inst2str(const Instruction *inst){
        if(inst){
            string inststr;
            raw_string_ostream rso(inststr);
            inst->print(rso);
            return inststr;
        }
        else{
            return "NULL";
        }
    }

    inline string inst2str(Instruction *inst){
        if(inst){
            string inststr;
            raw_string_ostream rso(inststr);
            inst->print(rso);
            return inststr;
        }
        else{
            return "NULL";
        }
    }

    inline string inst2str(const ConstantInt *ci){
        if(ci){
            string inststr;
            raw_string_ostream rso(inststr);
            ci->print(rso);
            return inststr;
        }
        else{
            return "NULL";
        }
    }

   inline string inst2str(const Value *val){
        if(val){
            string inststr;
            raw_string_ostream rso(inststr);
            val->print(rso);
            return inststr;
        }
        else{
            return "NULL";
        }
    }

    inline void constTypePrint(const Value *v){
        cout << "--------------------------------------" << endl;
        cout << "\tConstant: " << SVFUtil::dyn_cast<Constant>(v) << endl;
        cout << "\tConstantData: " << SVFUtil::dyn_cast<ConstantData>(v) << endl;
        cout << "\tConstantFP: " << SVFUtil::dyn_cast<ConstantFP>(v) << endl;
        cout << "\tConstantInt: " << SVFUtil::dyn_cast<ConstantInt>(v) << endl;
        cout << "\tConstantAggregate: " << SVFUtil::dyn_cast<ConstantAggregate>(v) << endl;
        cout << "\tConstantExpr: " << SVFUtil::dyn_cast<ConstantExpr>(v) << endl;
        cout << "\tGlobalValue: " << SVFUtil::dyn_cast<GlobalValue>(v) << endl;
        return;
    }


    // Gep mapping
    void offsetMapping(const Value *v){
        if (var2offvec.find(v) != var2offvec.end()) return;
        else if(const GetElementPtrInst *inst = SVFUtil::dyn_cast<GetElementPtrInst>(v)){
            unsigned sum = 0;
            const Function *func = inst2func(inst);
            for(auto iit = inst->idx_begin(); iit != inst->idx_end(); ++iit){
                const Value *iv = *iit;
                unsigned off = SVFUtil::dyn_cast<ConstantInt>(iv)->getZExtValue();
                var2offvec[v].push_back(off);
                func2var2offvec[func][v].push_back(off);
                sum += off;
            }
            var2offsum[v] = sum;
            off2varset[sum].insert(v);

            func2var2offsum[func][v] = sum;
            func2off2varset[func][sum].insert(v);
        }
        return;
    }

    void offsetMapping(map <const Value *, set <const Value *>> &var2varset){
        for(auto it = var2varset.begin(); it != var2varset.end(); ++it){
            const Value *v = it->first;
            offsetMapping(v);
        }
    }

    void offsetMapping(set <const Value *> &varset){
        for(auto it = varset.begin(); it != varset.end(); ++it){
            const Value *v = *it;
            offsetMapping(v);
        }
    }
    inline vector <unsigned> retOffVec(const Value *v){
        if(var2offvec.find(v) != var2offvec.end()){
            return var2offvec[v];
        }
        else assert(0);
    }

    inline unsigned retOffsum(const Value *v){
        if(var2offsum.find(v) != var2offsum.end()){
            return var2offsum[v];
        }
        else assert(0);
    }
    inline vector <unsigned> retIntraOffVec(const Value *v, const Function *func){
        if(func2var2offvec.find(func) != func2var2offvec.end()){
            map <const Value *, vector<unsigned>> v2ov = func2var2offvec[func];
            if(v2ov.find(v) != v2ov.end()){
                return v2ov[v];
            }
            else assert(0);
        }
        else assert(0);
    }

    inline unsigned retIntraOffsum(const Value *v, const Function *func){
        if(func2var2offsum.find(func) != func2var2offsum.end()){
            map <const Value *, unsigned> v2os = func2var2offsum[func];
            if(v2os.find(v) != v2os.end()){
                return v2os[v];
            }
            else assert(0);
        }
        else assert(0);
    }

    vector <string> strspliter(string s, string delimiter){
        vector <string> strsplit;
        size_t pos = 0;
        string token;
        while ((pos = s.find(delimiter)) != string::npos) {
            token = s.substr(0, pos);
            strsplit.push_back(token);
            s.erase(0, pos + delimiter.length());
        }
        strsplit.push_back(s);

        return strsplit;
    }


    // Gep mapping

//private:
    static map <const Function *, unsigned> func2szMap;
    static map <const Function *, map <const Instruction *, unsigned>> func2inst2idxmap;
    static map <const Function *, map <unsigned, const Value *>> func2idx2instmap;
    static map <const Function *, map <const Value *, unsigned>> func2arg2idxmap;
    static map <const Function *, map <unsigned, const Value *>> func2idx2argmap;
    static map <const Function *, vector<const Instruction*>> func2instvec;

    // Gep mapping
    static map <const Value *, vector <unsigned>> var2offvec;
    static map <const Value *, unsigned> var2offsum;
    static map <unsigned, set <const Value *>> off2varset;    //off = offsum
    static map <const Function *, map <const Value *, vector <unsigned>>> func2var2offvec;
    static map <const Function *, map <const Value *, unsigned>> func2var2offsum;
    static map <const Function *, map <unsigned, set <const Value *>>> func2off2varset;    //off = offsum
    static map <const Function *, set <unsigned>> func2offsums;    //off = offsum
    // Gep mapping
};

#endif
