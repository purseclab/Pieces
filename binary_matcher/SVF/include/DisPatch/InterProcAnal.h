#ifndef INTERPROCANAL_H_
#define INTERPROCANAL_H_

#include "UtilDef.h"
#include "BinIRWrapper.h"
#include <capstone/platform.h>
#include <capstone/capstone.h>

class InterProcAnal: public BinIRWrapper {

public:
    InterProcAnal(){ assert(0); };
    InterProcAnal(SVFModule &inmodule){ assert(0); };
    InterProcAnal(SVFModule &inmodule, string infilename, string inarch, uint64_t inbaseoff, uint64_t inbaseaddr){
        module = inmodule;
        filename = infilename;
        arch = inarch;
        baseoff = inbaseoff;
        baseaddr = inbaseaddr;
        binInit(module);
        ipainit();
    };
    ~InterProcAnal(){
        cs_close(&handle);
    }
    void ipainit();

    // Type
    struct platform {
        cs_arch arch;
        cs_mode mode;
        unsigned char *code;
        size_t size;
    };


    // main
    void interAnalysis(const Instruction *cs);
    void disFunc(uint64_t addr, uint64_t sz, string mode);
    void calleeAnalysis(uint64_t addr, uint64_t sz, string mode);
    void callsiteAnalysis(uint64_t csaddr, uint64_t ceaddr, uint64_t craddr, uint64_t crsz, string mode);
    void mapInterPass(uint64_t csaddr, uint64_t ceaddr, uint64_t craddr);

    // submodule (callee)
    void argCeARMRegAnalysis(uint64_t addr);
    void retCeARMRegAnalysis(uint64_t addr);
    set <uint16_t> filterArgReg(set<uint16_t> regset);
    set <uint16_t> filterRetReg(set<uint16_t> regset);

    // submodule (Inter mapping)
    void mapRegCeReadInsts(uint64_t ceaddr, map <uint16_t, vector<cs_insn>> &regReadInsts);
    void mapRegCsArgWriteInsts(uint64_t csaddr, uint64_t craddr, uint64_t ceaddr, map <uint16_t, vector<cs_insn>> &regArgReadInsts);
    void mapRegCsRetReadInsts(uint64_t csaddr, uint64_t craddr, uint64_t ceaddr, map <uint16_t, vector<cs_insn>> &regRetReadInsts);
    void mapRegCeRetWriteInsts(uint64_t csaddr, uint64_t craddr, uint64_t ceaddr, map <uint16_t, vector<cs_insn>> &regRetWriteInsts);
    void updateInstAddr2rRegSet(uint64_t faddr);

        // Subsubmodule
    const Value* mapOneRegForward(uint16_t ri, const Function *func, vector <cs_insn> &taretinsts, int maxdepth);
    const Value* mapOneRegBackward(uint16_t ri, const Function *func, vector <cs_insn> &targetinsts, int maxdepth);
    void propagatedRegReadInsts(uint64_t addr, vector <cs_insn> &insts, uint16_t tri, const Function *func);
    void propagatedRegWriteInsts(uint64_t addr, vector <cs_insn> &insts, uint16_t tri, const Function *func);
    const Value * retReg2Var(cs_insn &inst, uint16_t ri,  vector <const Value *> &irvec, char mode);

    // submodule (common, util)
    bool isDirectCallBranch(cs_insn &inst, uint64_t addrmin, uint64_t addrmax);
    bool isIndirectCallBranch(cs_insn &inst);
    bool isJmpBranch(cs_insn &inst, uint64_t addrmin, uint64_t addrmax);
    bool isCallBranch(cs_insn &inst, uint64_t addrmin, uint64_t addrmax);
    uint64_t retCalleeAddr(cs_insn &inst, uint64_t addrmin, uint64_t addrmax);
    bool isRegPropaInst(cs_insn &inst);
    bool isNormalReg(uint16_t ri);
    bool isInterPassingReg(uint16_t ri);

    inline void disPrint(cs_insn &insn){
        printf("0x%" PRIx64 ":\t%s\t%s\n", insn.address, insn.mnemonic, insn.op_str);
        return;
    }
    inline uint64_t retFuncSz(cs_insn &inst){
        return BinIRWrapper::retFuncSz((uint64_t)inst.address);
    }
    inline uint64_t retFuncSz(uint64_t addr){
        return BinIRWrapper::retFuncSz(addr);
    }
    inline string retFuncMode(cs_insn &inst){
        return BinIRWrapper::retFuncMode((uint64_t)inst.address);
    }
    inline string retFuncMode(uint64_t addr){
        return BinIRWrapper::retFuncMode(addr);
    }
    const Function *csaddr2ce(uint64_t csaddr);
    uint64_t retCSAddrFromRetVar(const Function *func, const Value *v, unsigned minidx, unsigned maxidx);

    inline string reg2str(uint16_t ri){
        return string(cs_reg_name(handle, ri));
    }
    uint16_t str2reg(string reg);

    // Result
    set <const Value *> retCSAddr2ArgCSVs(uint64_t csaddr);
    set <const Value *> retCSAddr2ArgCEVs(uint64_t csaddr);
    set <const Value *> retCSAddr2RetCSVs(uint64_t csaddr);
    const Value * retCSAddr2ArgCSV(uint64_t csaddr, uint16_t ri);
    const Value * retCSAddr2ArgCEV(uint64_t csaddr, uint16_t ri);
    const Value * retCSAddr2RetCSV(uint64_t csaddr, uint16_t ri);
    uint16_t retCSAddr2ArgCSReg(uint64_t csaddr, const Value *v);
    uint16_t retCSAddr2ArgCEReg(uint64_t csaddr, const Value *v);
    uint16_t retCSAddr2RetCSReg(uint64_t csaddr, const Value *v);
    uint16_t retCSAddr2Reg(uint64_t csaddr, const Value *v);
    const Value *retFunc2ArgCEV(const Function *func, uint16_t ri);
    const Value *retCEArgFromCSArg(uint64_t csaddr, const Value *v);
    const Value *retCSArgFromCEArg(uint64_t csaddr, const Value *v);
    const Value *retCERetFromCSRet(uint64_t csaddr, const Value *v);
    const Value *retCSRetFromCERet(uint64_t csaddr, const Value *v);
    const Value *retFunc2HeuriAggressiveArgCEV(const Function *ce, const CallInst *cs, uint16_t ri);
            // Can add more if you need
    bool isSameReg(uint64_t csaddr, const Value *v1, const Value *v2);
    void retCSRetVar2ArgVarSet(const Function *func, const Value *v, set <const Value *> &vset, unsigned si, unsigned di);
    void updateCSRet2CSAddrs(const Function *func, const Value *v, set <uint64_t> &csaddrset);
    bool isCSRetVar(const Function *func, const Value *v);

private:
    // Input
    SVFModule module;

    // Setting
    uint64_t baseoff;
    uint64_t baseaddr;
    string filename, arch;

    // Flag
    set <uint64_t> ceDoneSet, csDoneSet;
    set <const Instruction *> interAnalysisDoneSet;

    // Util value
    map <uint64_t, int> funcaddr2retidx;

    // intermedate Result
    map <uint64_t, vector <cs_insn>> funcaddr2disas;
    map <uint64_t, cs_insn> addr2disas;

    map <uint64_t, uint64_t> addr2funcaddr;
    map <uint64_t, set <uint64_t>> funcaddr2addrset;

    map <uint64_t, set <uint16_t>> ce2argset;
    map <uint64_t, set <uint16_t>> ce2retset;

    // final result
    map <uint64_t, uint64_t> csaddr2craddr;
    map <uint64_t, uint64_t> csaddr2ceaddr;
    map <uint64_t, map<uint64_t, set<uint16_t>>> funcAddr2InstAddr2rRegSet;   // for exception cs arg
    map <const Function *, map<uint64_t, set<uint16_t>>> func2InstAddr2rRegSet;   // for exception cs arg
        // Arg
            // Reg IR set
    map <uint64_t, set <const Value *>> arg_csaddr2csvs;
    map <uint64_t, set <const Value *>> arg_csaddr2cevs;
            // IR 2 IR
    map <uint64_t, map <const Value *, const Value *>> arg_csaddr2csv2cev;
    map <uint64_t, map <const Value *, const Value *>> arg_csaddr2cev2csv;
            // CS IR to CS reg
    map <uint64_t, map <const Value *, uint16_t>> arg_csaddr2csv2reg;
    map <uint64_t, map <uint16_t, const Value *>> arg_csaddr2reg2csv;
            // CE IR to CE reg
    map <uint64_t, map <const Value *, uint16_t>> arg_csaddr2cev2reg;
    map <uint64_t, map <uint16_t, const Value *>> arg_csaddr2reg2cev;
            // CS IR to AddrSet
    map <const Value *, set <uint64_t>> argvar2csaddrs;
            // Func to Reg 2 CEV arg
    map <const Function *, map <uint16_t, const Value *>> arg_cefunc2reg2cev;
        // Ret
            // Reg IR set
    map <uint64_t, set <const Value *>> ret_csaddr2csvs;
            // IR 2 IR
    map <uint64_t, map <const Value *, const Value *>> ret_csaddr2csv2cev;
    map <uint64_t, map <const Value *, const Value *>> ret_csaddr2cev2csv;
            // CS IR to CS reg
    map <uint64_t, map <const Value *, uint16_t>> ret_csaddr2csv2reg;
    map <uint64_t, map <uint16_t, const Value *>> ret_csaddr2reg2csv;
            // CS IR to AddrSet
    map <const Value *, set <uint64_t>> retvar2csaddrs;

    // Disassembler related
    csh handle;
    struct platform platform;
};

#endif

