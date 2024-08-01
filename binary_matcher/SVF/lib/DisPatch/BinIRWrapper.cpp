#include "DisPatch/BinIRWrapper.h"

// Static variable declaration (in class) (i.e., Global )
bool BinIRWrapper::isHeuristicGep = 0; // 1 = true / -1 = false / 0 = not determined
bool BinIRWrapper::init = false;
//SVFModule BinIRWrapper::bm;
set <const Value *> BinIRWrapper::ciset;
set <const Value *> BinIRWrapper::giset;
set <const Value *> BinIRWrapper::phiset;
set <const Value *> BinIRWrapper::liset;
set <const Value *> BinIRWrapper::siset;
set <const Value *> BinIRWrapper::vldrset;
set <const Value *> BinIRWrapper::vstrset;
set <const Value *> BinIRWrapper::biset;
set <const Value *> BinIRWrapper::castset;
set <const Value *> BinIRWrapper::bitcastset;
set <const Value *> BinIRWrapper::vmovset;
set <const Value *> BinIRWrapper::heuriargset;
set <const Value *> BinIRWrapper::ptrcastset;
set <const Value *> BinIRWrapper::mainThisset;
set <const Function *> BinIRWrapper::checkedfuncset;

map <const Value *, const Value *> BinIRWrapper::pinst2val;
map <const Value *, const Value *> BinIRWrapper::pinst2ptr;
map <const Value *, unsigned> BinIRWrapper::binop2ty;
map <const Value *, const Value *> BinIRWrapper::gep2this;
map <const Value *, set<const Value *>> BinIRWrapper::this2gepset;
map <uint64_t, FuncInfo *> BinIRWrapper::addr2funcInfo;
map <const Function *, FuncInfo *> BinIRWrapper::func2funcInfo;
map <uint64_t, const Function *> BinIRWrapper::indcsaddr2ce;
map <const Function *, uint64_t> BinIRWrapper::indce2csaddr;
map <uint64_t, vector<const Instruction *>> BinIRWrapper::instaddr2instvec;
map <const Instruction *, uint64_t> BinIRWrapper::inst2instaddr;

map <uint64_t, vector <const Value *>> BinIRWrapper::addr2IRrrvec;
map <uint64_t, vector <const Value *>> BinIRWrapper::addr2IRwrvec;

map <const Value *, unsigned> BinIRWrapper::gepCompute2offsum;
map <const Function *, map <unsigned, const Value *>> BinIRWrapper::func2offsum2gepCompute;

map <const Function *, set <const CallInst*>> BinIRWrapper::ce2cssetMap;
map <const Function *, set <const CallInst*>> BinIRWrapper::func2cssetMap;

// Function description
bool BinIRWrapper::isCallInst(const Value *instv){
    checkFunc(instv);
    if(ciset.find(instv) != ciset.end()) return true;
    else return false;
}

bool BinIRWrapper::isCastInst(const Value *instv){
    checkFunc(instv);
    if(castset.find(instv) != castset.end()) return true;
    else return false;
}

bool BinIRWrapper::isBitCastInst(const Value *instv){
    checkFunc(instv);
    if(bitcastset.find(instv) != bitcastset.end()) return true;
    else return false;
}

bool BinIRWrapper::isPtrCastInst(const Value *instv){
    checkFunc(instv);
    if(ptrcastset.find(instv) != ptrcastset.end()) return true;
    else return false;
}

bool BinIRWrapper::isGepInst(const Value *instv){
    checkFunc(instv);
    if(giset.find(instv) != giset.end()) return true;
    else return false;
}

bool BinIRWrapper::isLoadInst(const Value *instv){
    checkFunc(instv);
    if(liset.find(instv) != liset.end()) return true;
    else return false;
}

bool BinIRWrapper::isStoreInst(const Value *instv){
    checkFunc(instv);
    if(siset.find(instv) != siset.end()) return true;
    else return false;
}

bool BinIRWrapper::isVldrInst(const Value *instv){
    checkFunc(instv);
    if(vldrset.find(instv) != vldrset.end()) return true;
    else return false;
}

bool BinIRWrapper::isVstrInst(const Value *instv){
    checkFunc(instv);
    if(vstrset.find(instv) != vstrset.end()) return true;
    else return false;
}

bool BinIRWrapper::isVmovInst(const Value *instv){
    checkFunc(instv);
    if(vmovset.find(instv) != vmovset.end()) return true;
    else return false;
}

bool BinIRWrapper::isHeuriArg(const Value *instv){
    checkFunc(instv);
    if(heuriargset.find(instv) != heuriargset.end()) return true;
    else return false;
}

bool BinIRWrapper::isGepFromMainThis(const Value *instv){
    checkFunc(instv);
    if(mainThisset.find(instv) != mainThisset.end()) return true;
    else return false;
}

const Value* BinIRWrapper::retPtrVal(const Value *instv){
    checkFunc(instv);
    if(pinst2val.find(instv) != pinst2val.end()) return pinst2val[instv];
    else return NULL;
}

const Value* BinIRWrapper::retPtrPtr(const Value *instv){
    checkFunc(instv);
    if(pinst2ptr.find(instv) != pinst2ptr.end()) return pinst2ptr[instv];
    else return NULL;
}

const Value* BinIRWrapper::retPtrOffPtr(const Value *instv){
    checkFunc(instv);
    const Value *v = retPtrPtr(instv);
    if(v != NULL && gep2off(v) == 0 && SVFUtil::isa<Instruction>(v)){
        const Instruction *inst = SVFUtil::dyn_cast<Instruction>(v);
        return inst->getOperand(0);
    }
    return v;
}


set <unsigned> BinIRWrapper::gep2off(set<const Value *> &geps){
    set <unsigned> offset;
    for(auto it = geps.begin(); it != geps.end(); ++it){
        const Value *v = *it;
        if(!v) continue;

        unsigned off = gep2off(v);
        offset.insert(off);
    }
    return offset;
}

unsigned BinIRWrapper::gep2off(const Value * v){
    assert(v);
    if(isGepInst(v)){
        if(var2offsum.find(v) != var2offsum.end()) return var2offsum[v];
        else if(gepCompute2offsum.find(v) != gepCompute2offsum.end()) return gepCompute2offsum[v];
    }
    else if(isBinOpInst(v)){
        if(retBinOpTy(v) == Instruction::Add){
            const Instruction *bi = SVFUtil::dyn_cast<Instruction>(v);
            const Value *constv = bi->getOperand(1);
            if(const ConstantInt *ci = SVFUtil::dyn_cast<ConstantInt>(constv)){
                return ci->getZExtValue();   // valid ptr
            }
        }
    }
    return 0;
}

// Bin OP handlers
bool BinIRWrapper::isBinOpInst(const Value *instv){
    if(const BinaryOperator *bi = SVFUtil::dyn_cast<BinaryOperator>(instv)) return true;
    else if(SVFUtil::dyn_cast<CallInst>(instv)){
        string asmstr = retAsmStr(instv);
        if(asmstr.find("vabs") == 0) return true;
        else if(asmstr.find("vdiv") == 0) return true;
        else if(asmstr.find("vadd") == 0) return true;
        else if(asmstr.find("vneg") == 0) return true;
        else if(asmstr.find("vmul") == 0) return true;
        else if(asmstr.find("vnmul") == 0) return true;
        else if(asmstr.find("vsub") == 0) return true;
        else if(asmstr.find("vsqrt") == 0) return true;
        else if(asmstr.find("vfms") == 0) return true;
        else if(asmstr.find("vfma") == 0) return true;
    }
    return false;
}

unsigned BinIRWrapper::retBinOpTy(const Value *instv){
    if(const BinaryOperator *bi = SVFUtil::dyn_cast<BinaryOperator>(instv)){
        return bi->getOpcode();
    }
    else if(SVFUtil::dyn_cast<CallInst>(instv)){
        string asmstr = retAsmStr(instv);
        if(asmstr.find("vdiv") == 0) return Instruction::FDiv;
        else if(asmstr.find("vadd") == 0) return Instruction::FAdd;
        else if(asmstr.find("vmul") == 0) return Instruction::FMul;
        else if(asmstr.find("vnmul") == 0) return Instruction::FMul;
        else if(asmstr.find("vsub") == 0) return Instruction::FSub;
        else if(asmstr.find("vneg") == 0) return Instruction::FNeg;
        else if(asmstr.find("vfms") == 0) return VFMS;
        else if(asmstr.find("vfma") == 0) return VFMA;        
    }
    return 0xFFFFFFFF;
}

bool BinIRWrapper::isPhiInst(const Value *instv){
    if(phiset.find(instv) != phiset.end()) return true;
    else return false;
}

const Value *BinIRWrapper::retVLDRPtr(const Value *v){ 
    if(isVldrInst(v)) return retVLDRPtr(retPtrPtr(v));
    else if(isLoadInst(v)) return retVLDRPtr(retPtrPtr(v));
    else if(isGepInst(v)) return v;

    return NULL;
}

const Value *BinIRWrapper::retVSTRPtr(const Value *v){
    if(isVstrInst(v)) return retVSTRPtr(retPtrPtr(v));
    else if(isStoreInst(v)) return retVSTRPtr(retPtrPtr(v));
    else if(isGepInst(v)) return v;

    return NULL;
}

const Value *BinIRWrapper::retVMEMPtr(const Value *v){
    if(retVLDRPtr(v)) return retVLDRPtr(v);
    else if(retVSTRPtr(v)) return retVSTRPtr(v); 
    return NULL;
}


bool BinIRWrapper::isProcFP(const Value *instv){
    if(SVFUtil::dyn_cast<ConstantFP>(instv)) return true;
    else{
        string asmstr = retAsmStr(instv);
        if(asmstr.find("vldr") == 0) return true;
    }
    return false;
}

double BinIRWrapper::retProcFP(const Value *instv){
    double fdvalue = 0.0;
    if(const ConstantFP *cf = SVFUtil::dyn_cast<ConstantFP>(instv)){
        unsigned bits = cf->getType()->getPrimitiveSizeInBits();
        if(bits == 32){
            fdvalue = cf->getValueAPF().convertToFloat();
            return fdvalue;
        }
        else if(bits == 64){
            fdvalue = cf->getValueAPF().convertToDouble();
            return fdvalue;
        }
        else assert(0);
    }
    else if(const CallInst *cinst = SVFUtil::dyn_cast<CallInst>(instv)){
        string asmstr = retAsmStr(cinst);
        if(asmstr.find("vldr") == 0){
            if(const ConstantInt *ci = SVFUtil::dyn_cast<ConstantInt>(cinst->getOperand(0))){
                uint64_t iv = ci->getZExtValue();
                unsigned bits = ci->getType()->getPrimitiveSizeInBits();
                if(bits == 32){
                    fdvalue = (double)int322fp(iv);
                    return fdvalue;
                }
                else if(bits == 64){
                    fdvalue = int642fp(iv);
                    return fdvalue;
                }
            }
        }
    }
    return 0.0;
}


string BinIRWrapper::retAsmStr(const Value *instv){
    if(const CallInst *ci = SVFUtil::dyn_cast<CallInst>(instv)){    // BinOp, Load, Store possible
        const Function *callee = getCallee(ci);

        if(!callee){
            string asmkey = "__asm_";
            string inst_str;
            raw_string_ostream rso(inst_str);
            ci->print(rso);

            if(inst_str.find(asmkey) != string::npos){
                inst_str = inst_str.substr(inst_str.find(asmkey) + asmkey.length());
                unsigned pos_e1 = inst_str.find(".");
                unsigned pos_e2 = inst_str.find("(");
                unsigned pos_e;
                if(pos_e1 == string::npos && pos_e1 == string::npos) assert(0);
                else if(pos_e1 < pos_e2) pos_e = pos_e1;
                else pos_e = pos_e2;

                inst_str = inst_str.substr(0, pos_e);
                return inst_str;
            }
            else if(inst_str.find("__decompiler_undefined_function") != string::npos){ // heuri arg
                return inst_str;
            }
        }
    }
    return "";
}

void BinIRWrapper::retIRInputVars(const Function *func, set <const Value *> &inVars){
    for(auto it = func->arg_begin(); it != func->arg_end(); ++it){
        const Value *v = &*it;//SVFUtil::dyn_cast<Value>(*it);
        assert(v);
        inVars.insert(v);
    }

    vector <const Instruction*> insts = func2insts(func);
    for(int i = 0; i < insts.size(); i++){
        const Instruction *inst = insts[i];
        if(const CallInst *ci = SVFUtil::dyn_cast<CallInst>(inst)){
            string funcname = func2name(ci->getCalledFunction());
            if(funcname.find("__decompiler_undefined_") == 0){  //Either argument or stack offset const
                inVars.insert(inst);                            
            }
            else break;         // It is continuous in RetDec IR. So, it is valid condition
        }
        else break;             // It is continuous in RetDec IR. So, it is valid condition
    }

    return;
}

// Heuristic checker
bool BinIRWrapper::isIntraAliasGep(const Value *v1 , const Value *v2, const Function *func){
    checkFunc(func);
    
    if(func2var2offsum.find(func) == func2var2offsum.end()) return false;
    else if(var2offsum.find(v1) == var2offsum.end()) return false;
    else if(var2offsum.find(v2) == var2offsum.end()) return false;   
    else if(isFromIntraSameThis(v1, v2) && var2offsum[v1] == var2offsum[v2]) return true;
    else return false;
}

set <const Value *> BinIRWrapper::retIntraAliasGep(const Value *v, const Function *func){
    checkFunc(func);
    if(var2offsum.find(v) == var2offsum.end()){
        set <const Value *> singleset;
        singleset.insert(v);
        return singleset;
    }
    else{
        // legit offset
        if(func2var2offsum.find(func) == func2var2offsum.end()){
            set <const Value *> singleset;
            singleset.insert(v);
            return singleset;
        }
        else if(func2var2offsum[func].find(v) == func2var2offsum[func].end()){
            set <const Value *> singleset;
            singleset.insert(v);
            return singleset;
        }
        unsigned off = func2var2offsum[func][v];

        // return the legit varset
        if(func2off2varset[func].find(off) == func2off2varset[func].end()){
            set <const Value *> singleset;
            singleset.insert(v);
            return singleset;
        }
        else return func2off2varset[func][off];
    }
}

const Value *BinIRWrapper::recursiveGepOff(const Value *sink, const Function *func, set <const Value *> &argset, unsigned &off){
    if(sink){
        if(argset.find(sink) != argset.end()) return sink;  // indicates it reached arg
        else if(isCastInst(sink)){
            const Instruction *inst = SVFUtil::dyn_cast<Instruction>(sink);
            if(inst){
                for(int i = 0; i < inst->getNumOperands(); i++){
                    const Value *v = inst->getOperand(i);
                    if(SVFUtil::dyn_cast<Instruction>(v) != NULL || SVFUtil::dyn_cast<Argument>(v) != NULL){
                        return recursiveGepOff(v, func, argset, off);       // indicates it reached casting. So, recursive again
                    }
                }
            }
        }
        else if(isBinOpInst(sink)){
            if(retBinOpTy(sink) == Instruction::Add){
                const Instruction *bi = SVFUtil::dyn_cast<Instruction>(sink);
                const Value *constv = bi->getOperand(1);
                if(const ConstantInt *ci = SVFUtil::dyn_cast<ConstantInt>(constv)){
                    off = ci->getZExtValue();   // valid ptr
                    return NULL;
                }
            }
        }
    }

    // no reached ptr
    off = 0;
    return NULL;
}


bool BinIRWrapper::isFromIntraSameThis(const Value *v1 , const Value *v2){
    checkFunc(v1);
    if(gep2this.find(v1) == gep2this.end()) return false;
    else if(gep2this.find(v2) == gep2this.end()) return false;
    else if(gep2this[v1] == gep2this[v2]) return true;
    else return false;
}


bool BinIRWrapper::isRetAffectedByReadAlias(const Function *cefunc, unsigned ceAliasOff, const Value *v){
    if(func2offsum2gepCompute.find(cefunc) != func2offsum2gepCompute.end()){
        unsigned idx = 0;

        if(!v) return false;
        else if(SVFUtil::dyn_cast<Instruction>(v)){
            idx = inst2idx(cefunc, v);
        }
        else if(SVFUtil::dyn_cast<Argument>(v)){
            idx = 0;
        }

        vector <const Instruction*> instvec = func2insts(cefunc);
        for(unsigned i = idx; i < instvec.size(); i++){
            const Instruction *inst = instvec[i];
            if(vldrset.find(inst) != vldrset.end()){
                if(isTargetHeapAccessRead(inst, v, ceAliasOff)) return true;
            }
        }
    }

    return false;
}

bool BinIRWrapper::isTargetHeapAccessRead(const Value *curv, const Value *initv, unsigned targetoff){
    if(vldrset.find(curv) != vldrset.end()){
        if(const Instruction *inst = SVFUtil::dyn_cast<Instruction>(curv)){
            const Value *v = inst->getOperand(0);
            if(v == initv && targetoff == 0) return true;
            else return isTargetHeapAccessRead(v, initv, targetoff);
        }
        else return false;
    }
    else if(const LoadInst *linst = SVFUtil::dyn_cast<LoadInst>(curv)){
        const Value *v = linst->getPointerOperand();
        return isTargetHeapAccessRead(v, initv, targetoff);
    }
    else if(const IntToPtrInst *pinst = SVFUtil::dyn_cast<IntToPtrInst>(curv)){
         const Value *v = pinst->getOperand(0);
        return isTargetHeapAccessRead(v, initv, targetoff);
    }
    else if(const PHINode *pi = SVFUtil::dyn_cast<PHINode>(curv)){
        for(unsigned i = 0; i < pi->getNumIncomingValues(); i++){
            const Value *v = pi->getIncomingValue(i);
            return isTargetHeapAccessRead(v, initv, targetoff);
        }
    }
    else if(const BinaryOperator *bi = SVFUtil::dyn_cast<BinaryOperator>(curv)){
        const Value *v = bi->getOperand(0);
        const Value *constv = bi->getOperand(1);
        if(v == initv && bi->getOpcode() == Instruction::Add){
            if(const ConstantInt *ci = SVFUtil::dyn_cast<ConstantInt>(constv)){
                unsigned off = ci->getZExtValue();
                if(off == targetoff){
                    return true;
                }
            }
        }
    }
    return false;
}

// Close Gep = alias detection
const Value *BinIRWrapper::closestAliasInst(const Value *from, const Value *to, const Function *func){
    checkFunc(func);
    unsigned di = inst2idx(func, to);
    return closestAliasInst(from, di, func);
}

const Value *BinIRWrapper::closestAliasInst(const Value *from, unsigned di, const Function *func){
    checkFunc(func);
    unsigned si = inst2idx(from);
    unsigned ni = si;
    const Value *ret = from;
    set <const Value *> aset = retIntraAliasGep(from, func);

    for(auto it = aset.begin(); it != aset.end(); ++it){
        const Value *v = *it;
        unsigned i = inst2idx(v);
        if(ni < i && i < di){
            ni = i;
            ret = v;
        }
    }

    assert(ret);
    if(si != ni)
        cout << "\t\t>> Closest Alias: " << inst2str(ret) << "(" << si << ") / from " 
             << inst2str(from) << " (" << ni << ")" << endl;

    return ret;
}

///////////////////////////////////////////////////////////
// Functinfo update
//////////////////////////////////////////////////////////
void BinIRWrapper::binFuncRangeUpdate(uint64_t startaddr, uint64_t endaddr, string exemode){
    if(((endaddr - startaddr) & 0xFFFFFFFFFFFFFFFE) == 0) return;
    else if(isHeursticGepMode() && addr2funcInfo.find(startaddr) != addr2funcInfo.end()){
        //addr2funcInfo update
        addr2funcInfo[startaddr]->startaddr = startaddr;
        addr2funcInfo[startaddr]->endaddr = endaddr;
        addr2funcInfo[startaddr]->sz = (endaddr - startaddr) & 0xFFFFFFFFFFFFFFFE;
        addr2funcInfo[startaddr]->exemode = exemode;

        // bug fix
        if(startaddr % 4 == 2){
            if(addr2funcInfo.find(startaddr + 2) == addr2funcInfo.end()){
                FuncInfo *fi = new FuncInfo();
                addr2funcInfo[startaddr + 2] = fi;
            }
            addr2funcInfo[startaddr + 2]->startaddr = startaddr;
            addr2funcInfo[startaddr + 2]->endaddr = endaddr;
            addr2funcInfo[startaddr + 2]->sz = (endaddr - startaddr) & 0xFFFFFFFFFFFFFFFE;
            addr2funcInfo[startaddr + 2]->exemode = exemode;
        }
    }
    else return;
}

uint64_t BinIRWrapper::retFuncAddr(const Function *func){
    if(!func) return 0;
    else if(!isHeursticGepMode()) return 0;
    else if(func2funcInfo.find(func) == func2funcInfo.end()) return 0;
    else return func2funcInfo[func]->startaddr;
}

const Function* BinIRWrapper::retFuncFromInstAddr(uint64_t addr){
    for(auto it = func2funcInfo.begin(); it != func2funcInfo.end(); ++it){
        FuncInfo *fi = it->second; 
        if(fi->startaddr <= addr && addr < fi->endaddr){
            return fi->func;
        }
    }
    return 0;
}

const Function* BinIRWrapper::retFuncFromInstAddr(string straddr){
    uint64_t addr = stoull(straddr, nullptr, 16);
    return retFuncFromInstAddr(addr);
}

uint64_t BinIRWrapper::retFuncSz(uint64_t addr){
    if(addr2funcInfo.find(addr) == addr2funcInfo.end()) return 0;
    return addr2funcInfo[addr]->sz;
}

string BinIRWrapper::retFuncMode(uint64_t addr){
    if(addr2funcInfo.find(addr) == addr2funcInfo.end()) return "";
    return addr2funcInfo[addr]->exemode;
}

const Function* BinIRWrapper::retIRFunc(uint64_t addr){
    if(addr2funcInfo.find(addr) == addr2funcInfo.end()) return NULL;
    return addr2funcInfo[addr]->func;
}

void BinIRWrapper::mapBinIndcs2ce(uint64_t csaddr, uint64_t ceaddr){
    if(const Function *func = retIRFunc(ceaddr)){
        mapBinIndcs2ce(csaddr, func);
        return;
    }

    for(int i = 0; i < 2; i++){   
        uint64_t newceaddr = ceaddr - ((uint64_t)i * 2);
        if(const Function *func = retIRFunc(newceaddr)){
            mapBinIndcs2ce(csaddr, func);
            return;
        }
    }
    for(int i = 0; i < 2; i++){   
        uint64_t newceaddr = ceaddr + ((uint64_t)i * 2);
        if(const Function *func = retIRFunc(newceaddr)){
            mapBinIndcs2ce(csaddr, func);
            return;
        }
    }

    return ;
}

void BinIRWrapper::mapBinIndcs2ce(uint64_t csaddr, const Function *cefunc){
    indcsaddr2ce[csaddr] = cefunc;
    indce2csaddr[cefunc] = csaddr;
    return ;
}

const Function* BinIRWrapper::retcsaddr2ce(uint64_t csaddr){
    const Function *func;
    if(instaddr2instvec.find(csaddr) != instaddr2instvec.end()){
        vector <const Instruction *> instvec = instaddr2instvec[csaddr];
        for(int i = 0; i < instvec.size(); i++){
            if(func = getCallee(instvec[i])) return func;
        }
    }
    return NULL;
}

const Function* BinIRWrapper::retBinIndcsaddr2ce(uint64_t csaddr){
    if(indcsaddr2ce.find(csaddr) != indcsaddr2ce.end()) return indcsaddr2ce[csaddr];
    else return NULL;
}

uint64_t BinIRWrapper::retBinIndcsaddr2ceaddr(uint64_t csaddr){
    const Function *ce = retBinIndcsaddr2ce(csaddr);
    if(!ce) assert(0);
    else if(func2funcInfo.find(ce) != func2funcInfo.end()){
        return func2funcInfo[ce]->startaddr;
    }
    else return 0;
}

uint64_t BinIRWrapper::retSanitizedAddr(uint64_t rawaddr, uint64_t preaddr, string addrstr){
    // heuristic sanitizer due to LLVM IR lifter's bug
    if(rawaddr > preaddr && (rawaddr - preaddr < 800)) return rawaddr;
    else if(rawaddr <= preaddr && (preaddr - rawaddr < 800)) return preaddr + 2; //rawaddr;
    else{
        // Assume rawaddr > retaddr). This is the only bug from BINIR Lifter
        assert(rawaddr);
        assert(preaddr);
        uint64_t retaddr;
        for(int i = 0; i < 64; i+=4){
            retaddr = rawaddr >> i;
            if(retaddr - preaddr < 800){
                if(retaddr < preaddr) return preaddr + 2;
                else return retaddr;
            }
        }
        return rawaddr;
    }
}

uint64_t BinIRWrapper::retBBAddr(const BasicBlock *bb){
    uint64_t addr;
    string bbstr = bb->getName().str();
    size_t pos = bbstr.find("dec_label_pc_");
    if(pos != string::npos){
        bbstr = bbstr.substr(pos);
        addr = stoull(bbstr, nullptr, 16);
    }
    else{
        for(auto it = bb->begin(); it != bb->end(); ++it){
            const Instruction *inst = &*it;
            cout << "inst: " << inst2str(inst) << endl;
            addr = inst2addr(inst);
            if(addr == 0 && inst->hasMetadata() && (inst->getMetadata("addr") != NULL)){
                string addrstr = cast<MDString>(inst->getMetadata("addr")->getOperand(0))->getString();
                addr = stoull(addrstr, nullptr, 16);
            } 
            if(addr) break;
        }
        if(!addr){
            cout << "[DEBUG] bb: " << bbstr << endl;
            cout << "\t\t func: " << bb->getParent()->getName().str() << endl;
            assert(addr);
        }
    }
    return addr;
}
///////////////////////////////////////////////////////////
// Main init
//////////////////////////////////////////////////////////
void BinIRWrapper::binInit(SVFModule &m){
    if(init) return;
    init = true;

    setHeuristicGep(true); 
    if(isHeursticGepMode()){
        string key = "function_";
        for(auto it = m.begin(); it != m.end(); ++it){
            const Function *func = *it;
            string funcname = func2name(func);
            if(funcname.find(key) == 0){
                string addrstr = funcname.substr(key.length());
                uint64_t addr = stoull(addrstr, nullptr, 16);
                FuncInfo *fi = new FuncInfo();
                fi->startaddr = addr;
                fi->func = func;
                addr2funcInfo[addr] = fi;
                func2funcInfo[func] = fi;
            }
        }

        if(!func2funcInfo.size()) assert(0);
    }
    else assert(0);
    return;
}

void BinIRWrapper::binIRInit(const Function *func){
    // Retrieve instructions
    if(checkedfuncset.find(func) != checkedfuncset.end()) return;
    else if(!func) assert(0);
    else if(!isHeursticGepMode()) assert(0); //This direction's implementation is not ready
    else if(!func->size()) return;

    cout << "[INFO] binIRInit: " << func2name(func) << endl;
    checkedfuncset.insert(func);    // Processed func
    vector <const Instruction*> insts = func2insts(func);
    set <const Value *> loadsetOfvstr;
    set <uint64_t> addrset;

    // address mapping
    unsigned addr = retFuncAddr(func);
    vector <const Instruction*> preinsts;
    for(int i = 0; i < insts.size(); i++){
        const Instruction *inst = insts[i];

        // address init
        if(inst->hasMetadata() && (inst->getMetadata("addr") != NULL)){
            string addrstr = cast<MDString>(inst->getMetadata("addr")->getOperand(0))->getString();
            addr = stoull(addrstr, nullptr, 16);

            for(int j = 0; j < preinsts.size(); j++){
                const Instruction *pinst = preinsts[j];
                instaddr2instvec[addr].push_back(pinst);
                inst2instaddr[pinst] = addr;               
            }

            instaddr2instvec[addr].push_back(inst);
            inst2instaddr[inst] = addr;
            addrset.insert(addr);
            assert(addr);

            if(preinsts.size()) preinsts.clear();
        }
        else if(inst->hasName()){
            string inststr = inst->getName().str();
            if(inststr.find("tmp") == 0 || inststr.find("stack") == 0) continue;
            else if((SVFUtil::dyn_cast<PHINode>(inst) != NULL || SVFUtil::dyn_cast<SelectInst>(inst) != NULL ||
                    SVFUtil::dyn_cast<IndirectBrInst>(inst) != NULL || SVFUtil::dyn_cast<BranchInst>(inst) != NULL) && addr > 0){
                if(SVFUtil::dyn_cast<PHINode>(inst) != NULL){
                    size_t pos1 = inststr.find("_");
                    size_t pos2 = inststr.find(".pre-phi");
                    if(pos1 != string::npos && pos2 != string::npos){
                        string addrstr = inststr.substr(inststr.find("_") + 1, inststr.length() - (inststr.find("_") + 1));
                        pos2 = addrstr.find(".pre-phi");
                        if(pos2 > 16) pos2 = 16;    //limit size  >16 (=hex character * 16) cause err for stoull

                        addrstr = addrstr.substr(0, pos2);
                        cout << "PHI inst " <<inst2str(inst) << " at addrstr: " << addrstr << endl;

                        uint64_t rawaddr = stoull(addrstr, nullptr, 16);
                        addr = retSanitizedAddr(rawaddr, addr, addrstr);


                        cout << "PHI inst " <<inst2str(inst) << std::hex << " at new addr: " << addr << std::dec << endl;

                        if(!addr){
                            addr = retBBAddr(inst->getParent());
                            assert(addr);
                        }
                        addrset.insert(addr);

                        for(int j = 0; j < preinsts.size(); j++){
                            const Instruction *pinst = preinsts[j];
                            instaddr2instvec[addr].push_back(pinst);
                            inst2instaddr[pinst] = addr;               
                        }

                        instaddr2instvec[addr].push_back(inst);
                        inst2instaddr[inst] = addr;
                        assert(addr);

                        if(preinsts.size()) preinsts.clear();
                        continue;
                    }
                }
                //continue;   // We do not instrument this type of inst. Hence, we skip it 
               
                // Use the previous addr
                // e.g., %r0.01 = phi i32 [ %v6_8031230, %bb ], [ %tmp7, %dec_label_pc_8031228 ] at addrstr: r0.01 
                for(int j = 0; j < preinsts.size(); j++){
                    const Instruction *pinst = preinsts[j];
                    instaddr2instvec[addr].push_back(pinst);
                    inst2instaddr[pinst] = addr;               
                }

                instaddr2instvec[addr].push_back(inst);
                inst2instaddr[inst] = addr;
                assert(addr);

                if(preinsts.size()) preinsts.clear();
            }
            else if(inststr.find("_") == 2 && inststr.find("v") == 0 && 
                    SVFUtil::dyn_cast<PHINode>(inst) == NULL && SVFUtil::dyn_cast<SelectInst>(inst) == NULL &&
                    SVFUtil::dyn_cast<IndirectBrInst>(inst) == NULL && SVFUtil::dyn_cast<BranchInst>(inst) == NULL){
                string addrstr = inststr.substr(3);

                uint64_t rawaddr = stoull(addrstr, nullptr, 16);
                addr = retSanitizedAddr(rawaddr, addr, addrstr);

                addrset.insert(addr);
                instaddr2instvec[addr].push_back(inst);
                inst2instaddr[inst] = addr;
            }

            else{
                size_t pos1 = inststr.find("_");
                size_t pos2 = inststr.find(".");
                if(pos1 == string::npos || pos2 != string::npos){
                    preinsts.push_back(inst);
                }
                else{
                    string addrstr = inststr.substr(inststr.find("_") + 1, inststr.length() - (inststr.find("_") + 1));
                    //cout << "inst " <<inst2str(inst) << " at addrstr: " << addrstr << endl;
                    uint64_t rawaddr = stoull(addrstr, nullptr, 16);
                    addr = retSanitizedAddr(rawaddr, addr, addrstr);
                    if(!addr){
                        addr = retBBAddr(inst->getParent());
                        assert(addr);
                    }
                    addrset.insert(addr);

                    for(int j = 0; j < preinsts.size(); j++){
                        const Instruction *pinst = preinsts[j];
                        instaddr2instvec[addr].push_back(pinst);
                        inst2instaddr[pinst] = addr;               
                    }

                    instaddr2instvec[addr].push_back(inst);
                    inst2instaddr[inst] = addr;
                    assert(addr);

                    if(preinsts.size()) preinsts.clear();
                }
            }
        }
    }

    // NOTE: Don't change this reverse order. 
    //      This is to distinguish from load instructions of vstr and vldr
    // Mapping the others functions
    for(int i = insts.size() - 1; i >= 0; i--){
        const Instruction *inst = insts[i];

        // inst specific init
        if(const BitCastInst *pi = SVFUtil::dyn_cast<BitCastInst>(inst)){
            castset.insert(pi);
            bitcastset.insert(pi);
        }
        else if(const IntToPtrInst *pi = SVFUtil::dyn_cast<IntToPtrInst>(inst)){
            castset.insert(pi);
            ptrcastset.insert(pi);
        }
        else if(const PHINode *pi = SVFUtil::dyn_cast<PHINode>(inst)){
            castset.insert(pi);
            phiset.insert(pi);
        }
        else if(const GetElementPtrInst *gi = SVFUtil::dyn_cast<GetElementPtrInst>(inst)){
            giset.insert(gi);
        }
        else if(const StoreInst *si = SVFUtil::dyn_cast<StoreInst>(inst)){
            siset.insert(si);
            pinst2val[si] = si->getValueOperand();
            pinst2ptr[si] = si->getPointerOperand();
        }
        else if(const LoadInst *li = SVFUtil::dyn_cast<LoadInst>(inst)){
            if(loadsetOfvstr.find(li) == loadsetOfvstr.end()){  // Exception for vstr
                liset.insert(li);
            }
            else{
                siset.insert(li);
            }
            pinst2val[li] = li;
            pinst2ptr[li] = li->getPointerOperand();
        }
        else if(const CallInst *ci = SVFUtil::dyn_cast<CallInst>(inst)){    // BinOp, Load, Store possible
            const Function *callee = getCallee(ci);
            string inst_str = retAsmStr(ci);

            if(inst_str != ""){
                asmMapping(inst_str, inst);

                // Exception for vstr
                if(inst_str == "vstr"){
                    loadsetOfvstr.insert(inst->getOperand(1));
                }
            }
            else{
                ciset.insert(ci); //Normal call Inst
                func2cssetMap[func].insert(ci);
            }
        }
    }

    // Gep (if heursitic) check first
    gepHeuristicMapping(insts, func, 0);

    // Register handling
    mapInst2Regs(addrset);

    return;
}

void BinIRWrapper::indexRegs(vector <const Value *> &rvec, set <const Value *> &rset, vector <const Instruction *> &instvec){
    // Last inst first, first from left operand to right operand
    set <const Value *> hset;
    for(int i = instvec.size() - 1; i >=0; i--){
        const Instruction *inst = instvec[i];
        if(rset.find(inst) != rset.end() && hset.find(inst) == hset.end()){
            rvec.push_back(inst);
            hset.insert(inst);
        }

        for(int j = 0; j < inst->getNumOperands(); j++){
            const Value *v = inst->getOperand(j);
            if(rset.find(v) != rset.end() && hset.find(v) == hset.end()){
                rvec.push_back(v);
                hset.insert(v);
            }
        }
    }

    assert(rset.size() == rvec.size());
    return;
}

void BinIRWrapper::mapInst2Regs(set <uint64_t> &addrset){
    for(auto it = addrset.begin(); it != addrset.end(); ++it){
        uint64_t addr = *it;
        vector <const Instruction *> instvec = instaddr2instvec[addr];
        set <const Value *> rset, wset, pset;   //pset = propaset

        for(int i = 0; i < instvec.size(); i++){
            const Instruction *inst = instvec[i];

            if(isStoreInst(inst) || isVstrInst(inst)){  // Mem write. So, consider both "regsters" are read
                const Value *vv = retPtrVal(inst);
                const Value *pv = retPtrPtr(inst);
                if(pset.find(vv) == pset.end() && (SVFUtil::dyn_cast<Constant>(vv) == NULL)){
                    rset.insert(vv);
                }
                if(pset.find(pv) == pset.end() && (SVFUtil::dyn_cast<Constant>(pv) == NULL)){
                    rset.insert(pv);
                }
            }
            else{
                // generic skip check
                if((instvec.size() > 1) && (SVFUtil::dyn_cast<PHINode>(inst) != NULL || SVFUtil::dyn_cast<ReturnInst>(inst) != NULL 
                  || SVFUtil::dyn_cast<UnreachableInst>(inst) != NULL || SVFUtil::dyn_cast<SwitchInst>(inst) != NULL)){
                    continue;
                }

                // return check
                if(!inst->getType()->isVoidTy()){
                    if(pset.find(inst) == pset.end()){
                        wset.insert(inst);
                        pset.insert(inst);
                    }
                    if(rset.find(inst) != rset.end()){
                        rset.erase(inst);
                    }
                }

                // generic handler + branch handler
                if(const BranchInst *bi = SVFUtil::dyn_cast<BranchInst>(inst)){
                    if(bi->isConditional() && (bi->getCondition() != NULL)) rset.insert(bi->getCondition());
                } 
                else if(const IndirectBrInst *bi = SVFUtil::dyn_cast<IndirectBrInst>(inst)){
                    if(bi->getAddress()) rset.insert(bi->getAddress());
                }
                else if(const SwitchInst *si = SVFUtil::dyn_cast<SwitchInst>(inst)){
                    const Value *cv = si->getCondition();
                    if(SVFUtil::dyn_cast<Constant>(cv)) rset.insert(cv);
                }
                else{
                    for(int j = 0; j < inst->getNumOperands(); j++){
                        const Value *ov = inst->getOperand(j);
                        if(pset.find(ov) == pset.end() && (SVFUtil::dyn_cast<Constant>(ov) == NULL)){   // not constant
                            rset.insert(ov);
                        }
                        else if(SVFUtil::dyn_cast<Constant>(ov) == NULL){   //not constant
                            wset.erase(ov);
                        }
                    }
                }
            }
        }

        if(rset.size() > 0){
            vector <const Value *> rvec;
            indexRegs(rvec, rset, instvec);
            addr2IRrrvec[addr] = rvec;
            rvec.clear();
        }
        if(wset.size() > 0){
            vector <const Value *> wvec;
            indexRegs(wvec, wset, instvec);
            addr2IRwrvec[addr] = wvec;
            wvec.clear();
        }
        rset.clear();
        wset.clear();
        pset.clear();
    }

    return ;
}


void BinIRWrapper::asmMapping(string inst_str, const Instruction *inst){
    // Load
        //  This one should be considered as "mov" instruction.
        //      ex1: 
        //          >> %v3_805983e = load i32, i32* %v2_805983e, align 4
        //          >> %v4_805983e = call float @__asm_vldr(i32 %v3_805983e)   // This one is addr
        //      ex2: Direct constant copy (just like mov)
        //          >> %v1_805983a = call float @__asm_vldr(i32 -289950448)    // This one is constant value
    checkFunc(inst);
    if(inst_str.substr(0, 4) == "vldr"){
        vldrset.insert(inst);
        pinst2val[inst] = inst;
        pinst2ptr[inst] = inst->getOperand(0);
    }
    // Store
    else if(inst_str.substr(0, 4) == "vstr"){
        vstrset.insert(inst);
        pinst2val[inst] = inst->getOperand(0);
        pinst2ptr[inst] = inst->getOperand(1);
    }
    // Casting or equal
    else if(inst_str.substr(0, 4) == "vcvt" || inst_str.substr(0, 4) == "vmov"){
        castset.insert(inst);
        if(inst_str.substr(0, 4) == "vmov"){
            vmovset.insert(inst);
        }
    }
    // heuri arg (specific to retdec 3.3)
    else if(inst_str.find("__decompiler_undefined_function") != string::npos){
        heuriargset.insert(inst);
    }
    return;
}

// Stands for this pointer
void BinIRWrapper::gepHeuristicMapping(vector <const Instruction*> &insts, const Function *func, unsigned idx){
    if(!isHeursticGepMode()){
        for(int i = 0; i < insts.size(); i++){
            const Instruction *inst = insts[i];
            // Change it into gep
            if(const GetElementPtrInst *gi = SVFUtil::dyn_cast<GetElementPtrInst>(inst)){
                giset.insert(gi);
                offsetMapping(gi);
            }
        }
    }
    else{
        checkFunc(func);

        // This pointer detect 
        set <const Value *> thisset;
        thisset.clear();

        for(int i = 0; i < insts.size(); i++){
            const Instruction *inst = insts[i];

            if(const StoreInst *si = SVFUtil::dyn_cast<StoreInst>(inst)){
                if(const Value *thisVar = checkCastInst(si->getPointerOperand(), si->getPointerOperand(), func)){
                    giset.insert(si->getPointerOperand());
                    thisset.insert(thisVar);
                    gep2this[si->getPointerOperand()] = thisVar;        // gep = Typically inttoptr 
                    this2gepset[thisVar].insert(si->getPointerOperand());
                }
            }
            else if(const LoadInst *li = SVFUtil::dyn_cast<LoadInst>(inst)){
                if(const Value *thisVar = checkCastInst(li->getPointerOperand(), li->getPointerOperand(), func)){
                    giset.insert(li->getPointerOperand());       
                    thisset.insert(thisVar);
                    gep2this[li->getPointerOperand()] = thisVar;        // gep = Typically inttoptr
                    this2gepset[thisVar].insert(li->getPointerOperand());
                }
            }
        }

        // Phi Gep Mapping
        for(int i = 0; i < insts.size(); i++){
            const Instruction *inst = insts[i];
            // NOTE: design limitation, Theoretically, PHI can have multiple different GEP, but, I choose to consider only one
            if(const PHINode *pi = SVFUtil::dyn_cast<PHINode>(inst)){
                for(unsigned j = 0; j < pi->getNumIncomingValues(); j++){
                    const Value *phiv = pi->getIncomingValue(j);
                    if(giset.find(phiv) != giset.end()){
                        if(var2offsum.find(phiv) != var2offsum.end()){
                            unsigned off = var2offsum[phiv];

                            var2offvec[pi].push_back(off);
                            var2offsum[pi] = off;
                            off2varset[off].insert(pi);

                            func2var2offvec[func][pi].push_back(off);
                            func2var2offsum[func][pi] = off;
                            func2off2varset[func][off].insert(pi);

                            break;
                        }
                    }
                }
            }
        }

        // Check this main pointer is valid
        if(thisset.size() > 0){
            unsigned lowestIdx = func2sz(func) + func->arg_size();       // offset with arg for easy comparison
            for(auto it = thisset.begin(); it != thisset.end(); ++it){
                const Value *v = *it;
                if(SVFUtil::dyn_cast<Instruction>(v)){
                    unsigned idx = inst2idx(v);
                    if(idx < lowestIdx) lowestIdx = idx + func->arg_size();
                }
                else if(SVFUtil::dyn_cast<Argument>(v)){
                    unsigned idx = arg2idx(func, v);
                    if(idx < lowestIdx) lowestIdx = idx;
                }
            }

            if(lowestIdx < func2sz(func) + func->arg_size()){
                if(lowestIdx < func->arg_size())
                    mainThisset.insert(idx2arg(func, lowestIdx));
                else
                    mainThisset.insert(idx2inst(func, lowestIdx - func->arg_size()));
            }
        }
    }
    return;
}

const Value *BinIRWrapper::checkCastInst(const Value *instv, const Value *gi, const Function *func){
    checkFunc(func);
    if(SVFUtil::dyn_cast<GlobalValue>(instv)) return instv;    
    else if(const BitCastInst *pi = SVFUtil::dyn_cast<BitCastInst>(instv)){ // NOTE: I don't think PHINode is used
        return checkCastInst(pi->getOperand(0), gi, func);                    //      since this pointer does not depends on branch
                                                                        // not checkOffsetInst
    }
    else if(const IntToPtrInst *pi = SVFUtil::dyn_cast<IntToPtrInst>(instv)){ 
        return checkOffsetInst(pi->getOperand(0), gi, func);      
    }
    else return NULL;
}

const Value *BinIRWrapper::checkOffsetInst(const Value *instv, const Value *gi, const Function *func){
    checkFunc(func);
    if(const BinaryOperator *bi = SVFUtil::dyn_cast<BinaryOperator>(instv)){    // NOTE: I don't think PHINode is used
        if(retBinOpTy(bi) != Instruction::Add) return NULL;
        unsigned constcount = 0;
        unsigned varcount = 0;
        const Value *thisPtr = NULL;

        for(unsigned i = 0; i < bi->getNumOperands(); i++){                     //   since this pointer does not depends on branch
            if(!SVFUtil::dyn_cast<Constant>(bi->getOperand(i))){
                thisPtr = bi->getOperand(i);
                varcount++;
            }
            else{  
                if(const ConstantInt *ci = SVFUtil::dyn_cast<ConstantInt>(bi->getOperand(i))){
                    offsetMappingHeuri(ci->getZExtValue(), gi);
                    constcount++;
                }
            }
        }

        if(constcount == 1 && varcount == 1) return thisPtr;
        else return NULL;
    }
    else return NULL;
}

void BinIRWrapper::offsetMappingHeuri(unsigned off, const Value *gi){
    if(var2offvec.find(gi) != var2offvec.end()) return; //TAG OFFVEC
    else if(const Instruction *ginst = SVFUtil::dyn_cast<Instruction>(gi)){
        const Function *func = v2func(ginst);
        unsigned offsum = 0;
        checkFunc(func);

        // Gep handler
            // NOTE: var2off"vec" is not actually used. (No such pattern, just add offset at "this" pointer)
            //       If you think it is not necessary, please update "TAG OFFVEC" correctly
        var2offvec[gi].push_back(off);
        func2var2offvec[func][gi].push_back(off);

        for(int i = 0; i < var2offvec[gi].size(); i++){
            offsum += var2offvec[gi][i];
        }

        var2offsum[gi] = offsum;
        func2var2offsum[func][gi] = offsum;

        // reupdate
        if(func2off2varset[func].find(off) != func2off2varset[func].end()){
            if(func2off2varset[func][off].find(gi) != func2off2varset[func][off].end()){
                off2varset[off].erase(gi);
                func2off2varset[func][off].erase(gi);
            }
        }
        off2varset[offsum].insert(gi);
        func2off2varset[func][offsum].insert(gi);

        func2offsums[func].insert(offsum);

        // Gep compute handler
            // So far, only this pattern has been observed
        if(const IntToPtrInst *pinst = SVFUtil::dyn_cast<IntToPtrInst>(gi)){
            if(const Instruction *preinst = SVFUtil::dyn_cast<Instruction>(pinst->getOperand(0))){
                gepCompute2offsum[preinst] = offsum;
                // NOTE: func2offsum2gepCompute can be necessary to be set. But, it is computed only once for binary's efficiency 
                if(func2offsum2gepCompute[func].find(offsum) != func2offsum2gepCompute[func].end()){
                    func2offsum2gepCompute[func].erase(offsum);
                }
                func2offsum2gepCompute[func][offsum] = preinst;
            }
        }

        return;
    }
    else assert(0);
}

//Currently, it is heuristic, VERY conservative function
void BinIRWrapper::readHeapFromCallee(const Function *func, set <unsigned> &offSet){
    if(func2offsums.find(func) != func2offsums.end()){
        set <unsigned> allset = func2offsums[func];
        for(auto it = allset.begin(); it != allset.end(); ++it){
            unsigned off = *it;
            offSet.insert(off);
        }    
    }
    return;
}

bool BinIRWrapper::isWriteGivenHeap(const Function *func, set <unsigned> &offSet){
    if(func2offsums.find(func) != func2offsums.end()){
        set <unsigned> allset = func2offsums[func];
        for(auto it = offSet.begin(); it != offSet.end(); ++it){
            unsigned off = *it;
            if(allset.find(off) != allset.end()) return true;
        }
    }
    return false;
}


///// Callee -> Caller
set <const CallInst *> BinIRWrapper::retCallSiteSet(const Function *cefunc, SVFModule &m){
    initCallerSetMap(m);
    if(ce2cssetMap.find(cefunc) != ce2cssetMap.end()){
        return ce2cssetMap[cefunc];
    }
    else{
        set <const CallInst *> dummy;
        return dummy;
    }
}

uint64_t BinIRWrapper::inferMissingEndAddr(const Function *func){
    vector <const Instruction*> instvec = func2insts(func);
    for(int i = instvec.size() - 1; i >=0; i--){
        uint64_t addr = inst2addr(instvec[i]);
        if(addr){
            uint64_t startaddr = retFuncAddr(func);
            if(startaddr > 0 && addr2funcInfo.find(startaddr) == addr2funcInfo.end()){
                FuncInfo *fi = new FuncInfo();
            }
            binFuncRangeUpdate(startaddr, addr, inferMissingExemode(func));
            return addr;
        }
    }

    return retFuncAddr(func); //if it is not found
}

string BinIRWrapper::inferMissingExemode(const Function *func){
    // return the most closest prceding function's exemode
    uint64_t maxaddr = 0;
    uint64_t curaddr = retFuncAddr(func);
    for(auto it = addr2funcInfo.begin(); it != addr2funcInfo.end(); ++it){
        uint64_t addr = it->first;
        if(maxaddr < addr && addr < curaddr) maxaddr = addr;
    }
    // default = THUMB
    if(!maxaddr) return "THUMB";
    else return addr2funcInfo[maxaddr]->exemode;
}



void BinIRWrapper::initCallerSetMap(SVFModule &m){
    if(ce2cssetMap.size()) return;

    cout << "begin - - - -" << endl;
    for(auto it = m.begin(); it != m.end(); ++it){
        const Function *func = *it;

        // Direct call handler
        for(auto iit = inst_begin(func); iit != inst_end(func); ++iit){
            const Instruction *inst = &*iit;
            if(const CallInst *ci = SVFUtil::dyn_cast<CallInst>(inst)){
                // Direct handler
                const Function *ce = ci->getCalledFunction();
                if(ce != NULL && ce->size() > 0){
                    ce2cssetMap[ce].insert(ci);
                }
            }
        }
        if(!indcsaddr2ce.size()) continue;


        // ind handler
        for(auto cit = indcsaddr2ce.begin(); cit != indcsaddr2ce.end(); ++cit){
            uint64_t csaddr = cit->first;
            const Function *ce = cit->second;
            if(!ce) continue;

            uint64_t minaddr = retFuncAddr(func);
            if(!minaddr) continue;

            uint64_t maxaddr;
            if(retFuncSz(minaddr)) maxaddr = minaddr + retFuncSz(minaddr);
            else if(!(minaddr <= csaddr && csaddr <= (minaddr + 0x300))) continue;// Avoid unnecessary init step, 0x300 is size thres
            else maxaddr = inferMissingEndAddr(func);

            if(minaddr <= csaddr && csaddr <= maxaddr){
                binIRInit(func);    //IR init
                binIRInit(ce);    //IR init
                if(instaddr2instvec.find(csaddr) != instaddr2instvec.end()){                    //"Try" to find inst
                    vector<const Instruction *> instvec = instaddr2instvec[csaddr];
                    for(int i = 0; i < instvec.size(); i++){
                        const Instruction *inst = instvec[i];
                        if(const CallInst *ci = SVFUtil::dyn_cast<CallInst>(inst)){
                            if(ci->getCalledFunction() != NULL && func2name(ci->getCalledFunction()) == "__indcall"){
                                ce2cssetMap[ce].insert(ci);
                                break;
                            }
                        }
                    }
                }
            }
        }
    }

    return;
}
