#include "DisPatch/InterProcAnal.h"
//#define DEBUG

void InterProcAnal::ipainit(){
    // Arch
    transform(arch.begin(), arch.end(), arch.begin(), ::toupper); 
    if(arch == "THUMB"){
        platform.arch = CS_ARCH_ARM;
        platform.mode = CS_MODE_THUMB;
    }
    else if(arch == "ARM"){
        platform.arch = CS_ARCH_ARM;
        platform.mode = CS_MODE_ARM;
    }
    else if(arch == "ARM_MIXED"){
        platform.arch = CS_ARCH_ARM;
        platform.mode = CS_MODE_ARM;
    }
    else if(arch == "ARM64"){
        platform.arch = CS_ARCH_ARM64;
        platform.mode = CS_MODE_ARM;
        assert(0);  // Not planned to be tested
    }
    else if(arch == "X86_32"){
        platform.arch = CS_ARCH_X86;
        platform.mode = CS_MODE_32;
    }
    else if(arch == "X86_64"){
        platform.arch = CS_ARCH_X86;
        platform.mode = CS_MODE_64;
    }
    else assert(0);

    // Code
    FILE * file = fopen(filename.c_str(), "rb");
    if (file == NULL) assert(0);
    fseek(file, 0, SEEK_END);
    platform.size = ftell(file);
    platform.code = (unsigned char *)malloc(platform.size);
    rewind(file);
    unsigned readsz = fread(platform.code, sizeof(unsigned char), platform.size, file);
    if(readsz <= 0) assert(0);
    fclose(file);

    platform.code = (platform.code) + baseoff;

    // Setup
    cs_err err = cs_open(platform.arch, platform.mode, &handle);
    if (err) {
        cout << "Failed on cs_open() with error returned: " << err << endl; 
        assert(0);
    }

    if(platform.arch == CS_ARCH_X86){
        cs_option(handle, CS_OPT_SYNTAX, CS_OPT_SYNTAX_INTEL);
    }
    else if(platform.mode == CS_MODE_ARM){
        cs_option(handle, CS_OPT_DETAIL, CS_OPT_ON);
        cs_option(handle, CS_OPT_MODE, CS_MODE_ARM);
    }
    else if(platform.mode == CS_MODE_THUMB){
        cs_option(handle, CS_OPT_DETAIL, CS_OPT_ON);
        cs_option(handle, CS_OPT_MODE, CS_MODE_THUMB);
    }
    else assert(0);

    return;
}

//#################################################################
//############################# Func disas
//#################################################################
void InterProcAnal::disFunc(uint64_t addr, uint64_t sz, string mode){
    // disassemble
	cs_insn *insn;
	size_t count;

    count = cs_disasm(handle, platform.code + (addr - baseaddr), sz, addr, 0, &insn);
    if(count) {
#ifdef DEBUG
        cout << "Disasm at addr: " << std::hex << addr << std::dec << endl;
        for (size_t j = 0; j < count; j++) {
            disPrint(insn[j]);
        }
#endif
    } 
    else {
        cout << "[ERROR] Failed to disasm given code!" << endl;
        assert(0);
    }

    // Mapping
    for (size_t i = 0; i < count; i++) {
        funcaddr2disas[addr].push_back(insn[i]);
        addr2disas[addr]= insn[i];
        addr2funcaddr[insn[i].address] = addr;
        funcaddr2addrset[addr].insert(insn[i].address);
    }

    return;
}

//#################################################################
//############################# Callee Analysis
//#################################################################
void InterProcAnal::calleeAnalysis(uint64_t addr, uint64_t sz, string mode){
    // Check done
    if(ceDoneSet.find(addr) != ceDoneSet.end()) return;

       // Option for this function
    if(platform.arch == CS_ARCH_ARM){
        if(mode == "ARM") cs_option(handle, CS_OPT_MODE, CS_MODE_ARM);
        else if(mode == "THUMB") cs_option(handle, CS_OPT_MODE, CS_MODE_THUMB);
        else cs_option(handle, CS_OPT_MODE, CS_MODE_THUMB); //default = Thumb   
    }

    // Disas if necessary
    if(funcaddr2disas.find(addr) == funcaddr2disas.end()){
        disFunc(addr, sz, mode);
    }

    // BinIRinit  (just in case)
    const Function *func = retIRFunc(addr);
    binIRInit(func);

    // Argument/Ret analysis
    if(mode == "ARM" || mode == "THUMB"){
        argCeARMRegAnalysis(addr);
        retCeARMRegAnalysis(addr);
    }
    else{
        cout << " mode: " << mode << endl;
        cout << " addr: " << std::hex << addr << endl;
        assert(0); // x86 is out of scope
    }
    ceDoneSet.insert(addr);

    return ;
}


void InterProcAnal::argCeARMRegAnalysis(uint64_t addr){
    // arm.h or x86.h for capstone
    cs_regs regs_read, regs_write;
    uint8_t regs_read_count, regs_write_count;
    set<uint16_t> rrset, wrset;
    vector <cs_insn> instvec = funcaddr2disas[addr];

    for (int i = 0; i < instvec.size(); i++) {
        if (!cs_regs_access(handle, &instvec[i], regs_read, &regs_read_count, regs_write, &regs_write_count)) {
            if (regs_read_count) {
                for(int j = 0;  j < regs_read_count; j++) {
                    uint16_t ri = regs_read[j];
                    if(isInterPassingReg(ri)){
                        if(wrset.find(ri) == wrset.end() && rrset.find(ri) == rrset.end()){
                            rrset.insert(ri);
                        }
                    }
                }
            }

            if (regs_write_count) {
                for(int j = 0; j < regs_write_count; j++) {
                    uint16_t ri = regs_write[j];
                    if(isInterPassingReg(ri)){
                        wrset.insert(ri); // Not necessary to maintain instaddr2init"w"r 
                    }
                }
            }
        }
    }

    // arg summary
#ifdef DEBUG
    cout << ">> Arg summenry at addr " << std::hex << addr << std::dec << endl;
#endif
    if(rrset.size()){
        rrset = filterArgReg(rrset);
        for(auto it = rrset.begin(); it != rrset.end(); ++it){
            uint16_t ri = *it;
#ifdef DEBUG
            cout << "\t" << cs_reg_name(handle, ri) << "  ";
#endif
        }
#ifdef DEBUG
        cout << "\n" << endl;
#endif
        ce2argset[addr] = rrset;
    }
#ifdef DEBUG
    else cout << "\t >> Nothing" << endl;
#endif

    return;  
}

void InterProcAnal::retCeARMRegAnalysis(uint64_t addr){
    // arm.h or x86.h for capstone
    cs_regs regs_read, regs_write;
    uint8_t regs_read_count, regs_write_count;
    set <uint16_t> rrset, wrset;
    vector <cs_insn> instvec = funcaddr2disas[addr];
    uint64_t addrmax = instvec.back().address + instvec.back().size -1;
    uint64_t addrmin = instvec[0].address;

    // Ret inst location detect
    int retidx = instvec.size() - 1;
    bool returnfound = false;
    bool isPrevCall = false;
    for (int i = instvec.size() - 1; i >= 0; i--) {
        if(instvec[i].id == ARM_INS_POP){ // POP based return
            // poped, recovered register set. This should be excluded from ret pass since it is not affacted by the func operation
            if (!cs_regs_access(handle, &instvec[i], regs_read, &regs_read_count, regs_write, &regs_write_count)) {
                if (regs_write_count) {
                    for(int j = 0; j < regs_write_count; j++) {
                        uint16_t ri = regs_write[j];
                        if(ARM_REG_PC == ri){
                            retidx = i;
                            returnfound = true;
                            break;
                        }
                        else if(ARM_REG_LR == ri && isPrevCall == true){
                            retidx = i;
                            returnfound = true;
                            break;
                        }
                    }
                }
            }
            if(returnfound) break;
            isPrevCall = false;
        }
        // Function call
        else if(isCallBranch(instvec[i], addrmin, addrmax)){
            isPrevCall = true;
            continue;
        }
        // Jump 
        else if(isJmpBranch(instvec[i], addrmin, addrmax)) isPrevCall = false;
        else isPrevCall = false;
    }

    // Return finalization
    if(!returnfound){
        retidx = instvec.size() - 1;
    }
    funcaddr2retidx[addr] = retidx;

   
    // Check the ret reg
    for (int i = retidx; i >= 0; i--) {
        if (!cs_regs_access(handle, &instvec[i], regs_read, &regs_read_count, regs_write, &regs_write_count)) {
            if (regs_write_count) {
                for(int j = 0; j < regs_write_count; j++) {
                    uint16_t ri = regs_write[j];
                    if(isInterPassingReg(ri)){
                        wrset.insert(ri);
                    }
                }
            }
        }

        if(retidx - i >= 8 || retidx - i > retidx / 2 ) break;
        else if(isDirectCallBranch(instvec[i], addrmin, addrmax)) break;
    }


    // Ret summary
#ifdef DEBUG
    cout << ">> Ret summenry at addr " << std::hex << instvec[retidx].address << std::dec << endl;
    cout << "   -> Ret idx: [" << retidx << "/" << instvec.size() 
         << "] / inst addr " << std::hex << instvec[retidx].address << std::dec << endl;
#endif
    if(wrset.size()){
        wrset = filterRetReg(wrset);
        for(auto it = wrset.begin(); it != wrset.end(); ++it){
            uint16_t ri = *it;
#ifdef DEBUG
            cout << "\t" << cs_reg_name(handle, ri) << "  ";
#endif
        }
#ifdef DEBUG
        cout << "\n" << endl;
#endif
        ce2retset[addr] = wrset;
    }
#ifdef DEBUG
    else cout << "\t >> Nothing" << endl;
#endif

    return;
}

void InterProcAnal::callsiteAnalysis(uint64_t csaddr, uint64_t ceaddr, uint64_t craddr, uint64_t crsz, string mode){
    if(csDoneSet.find(csaddr) != csDoneSet.end()) return;

    // Option for this function
    if(platform.arch == CS_ARCH_ARM){
        if(mode == "ARM") cs_option(handle, CS_OPT_MODE, CS_MODE_ARM);
        else if(mode == "THUMB") cs_option(handle, CS_OPT_MODE, CS_MODE_THUMB);
        else assert(0);
    }

    // Disas if necessary
    if(funcaddr2disas.find(craddr) == funcaddr2disas.end()){
        disFunc(craddr, crsz, mode);
    }

    // Bin IR Init 
    const Function *func = retIRFunc(craddr);
    binIRInit(func);

    csDoneSet.insert(csaddr);

    return;
}

//#################################################################
//############################# Util
//#################################################################
bool InterProcAnal::isDirectCallBranch(cs_insn &inst, uint64_t addrmin, uint64_t addrmax){
    bool retflag = false;
    uint64_t jmpaddr = 0;

    if(inst.id == ARM_INS_B || inst.id == ARM_INS_BL){
        cs_arm *arm = &(inst.detail->arm);
        for (int j = 0; j < arm->op_count; j++) {
            cs_arm_op *op = &(arm->operands[j]);
            switch((int)op->type){
                case ARM_OP_IMM:{
                    jmpaddr = (uint64_t)op->imm;
                    if(jmpaddr < addrmin || jmpaddr > addrmax) retflag = true;
                    break;
                }
                default:
                    break;
            }
        }
    }

    // functionrange 
    if(retflag){
        if(addr2funcInfo.find(jmpaddr) != addr2funcInfo.end()) return true;
        else return false;
    }
    else return false;
}

bool InterProcAnal::isIndirectCallBranch(cs_insn &inst){
    if(inst.id == ARM_INS_BX || inst.id == ARM_INS_BLX) return true;
    else return false;
}

bool InterProcAnal::isJmpBranch(cs_insn &inst, uint64_t addrmin, uint64_t addrmax){
    bool retflag = false;
    uint64_t jmpaddr = 0;

    if(inst.id == ARM_INS_B || inst.id == ARM_INS_BL){
        cs_arm *arm = &(inst.detail->arm);
        for (int j = 0; j < arm->op_count; j++) {
            cs_arm_op *op = &(arm->operands[j]);
            switch((int)op->type){
                case ARM_OP_IMM:{
                    jmpaddr = (uint64_t)op->imm;
                    if(jmpaddr < addrmin || jmpaddr > addrmax) retflag = true;
                    break;
                }
                default:
                    break;
            }
        }
    }

    if(retflag){
        if(addr2funcInfo.find(jmpaddr) != addr2funcInfo.end()) return false;
        else return true;
    }
    else return true;
}

bool InterProcAnal::isCallBranch(cs_insn &inst, uint64_t addrmin, uint64_t addrmax){
    return (isDirectCallBranch(inst, addrmin, addrmax) || isIndirectCallBranch(inst));
}

uint64_t InterProcAnal::retCalleeAddr(cs_insn &inst, uint64_t addrmin, uint64_t addrmax){
    uint64_t calleeaddr;

    // Error exception
    if(inst.id == ARM_INS_BXJ) assert(0);    // not handled yet for java
    
    // Direct call
    if(platform.arch == CS_ARCH_ARM){
        if(inst.id == ARM_INS_B || inst.id == ARM_INS_BL){
            cs_arm *arm = &(inst.detail->arm);
            for (int j = 0; j < arm->op_count; j++) {
                cs_arm_op *op = &(arm->operands[j]);
                switch((int)op->type){
                    case ARM_OP_IMM:{
                        calleeaddr = (uint64_t)op->imm;
                        if(calleeaddr < addrmin || calleeaddr > addrmax) return calleeaddr;
                        else return 0;
                    }
                    default:{
                        break;
                    }
                }
            }
            return 0;
        }

        // indirect call 
        if(inst.id == ARM_INS_BX || inst.id == ARM_INS_BLX){   
            return retBinIndcsaddr2ceaddr(inst.address);
        }
    }
    else assert(0);//Not handled arch

    cout << "[Error]: Error case " << endl;
    assert(0);
    return calleeaddr;
}


bool InterProcAnal::isRegPropaInst(cs_insn &inst){
	if (inst.detail == NULL) assert(0);
    int r = 0, w = 0, fr = 0, fw = 0;

    if(platform.arch == CS_ARCH_ARM){
        cs_arm *arm;
        arm = &(inst.detail->arm);
        for (int i = 0; i < arm->op_count; i++) {
            cs_arm_op *op = &(arm->operands[i]);
            switch((int)op->type) {
                case ARM_OP_REG:{
                    switch(op->access) {
                        case CS_AC_READ:
                            r++;
                            break;
                        case CS_AC_WRITE:
                            w++;
                            break;
                        case CS_AC_READ | CS_AC_WRITE:
                            r++;
                            w++;
                            break;
                        default:
                            break;
                    }
                    break;
                }
                case ARM_OP_FP:
                    switch(op->access) {
                        case CS_AC_READ:
                            fr++;
                            break;
                        case CS_AC_WRITE:
                            fw++;
                            break;
                        case CS_AC_READ | CS_AC_WRITE:
                            fr++;
                            fw++;
                            break;
                        default:
                            break;
                    }
                    break;
                case ARM_OP_IMM:
                case ARM_OP_MEM:
                case ARM_OP_PIMM:
                case ARM_OP_CIMM:
                case ARM_OP_SETEND:
                case ARM_OP_SYSREG:
                default:
                    return false;
            }
        }
    }
    else assert(0);

    //loose condition. Typecasting (from int to FP is ok with the current implementation)
    if((fr > 0 && fw > 0) || (r > 0 && w > 0)) return true;
    else return false;
}

bool InterProcAnal::isNormalReg(uint16_t ri){
    if(platform.arch == CS_ARCH_ARM){
        if((ARM_REG_D0 <= ri && ri <= ARM_REG_D31) || (ARM_REG_Q0 <= ri && ri <= ARM_REG_Q15)
           || (ARM_REG_R0 <= ri && ri <= ARM_REG_R12) || (ARM_REG_S0 <= ri && ri <= ARM_REG_S31)) return true;
        else return false;
    }
    else assert(0);
}

bool InterProcAnal::isInterPassingReg(uint16_t ri){
    if(platform.arch == CS_ARCH_ARM){
        if((ARM_REG_D0 <= ri && ri <= ARM_REG_D3) || (ARM_REG_Q0 <= ri && ri <= ARM_REG_Q3) 
           || (ARM_REG_R0 <= ri && ri <= ARM_REG_R3) || (ARM_REG_S0 <= ri && ri <= ARM_REG_S3)) return true;
        else return false;
    }
    else assert(0);
}

set<uint16_t> InterProcAnal::filterArgReg(set<uint16_t> regset){
    set <uint16_t> retset;

    // Normal reg
    if(regset.find(ARM_REG_R0) != regset.end()){
        retset.insert(ARM_REG_R0);
        if(regset.find(ARM_REG_R1) != regset.end()){
            retset.insert(ARM_REG_R1);
            if(regset.find(ARM_REG_R2) != regset.end()){
                retset.insert(ARM_REG_R2);
                if(regset.find(ARM_REG_R3) != regset.end()){
                    retset.insert(ARM_REG_R3);
                }
            }
        }
    }

    // S floating reg
    if(regset.find(ARM_REG_S0) != regset.end()){
        retset.insert(ARM_REG_S0);
        if(regset.find(ARM_REG_S1) != regset.end()){
            retset.insert(ARM_REG_S1);
            if(regset.find(ARM_REG_S2) != regset.end()){
                retset.insert(ARM_REG_S2);
                if(regset.find(ARM_REG_S3) != regset.end()){
                    retset.insert(ARM_REG_S3);
                }
            }
        }
    }

    // D floating reg
    if(regset.find(ARM_REG_D0) != regset.end()){
        retset.insert(ARM_REG_D0);
        if(regset.find(ARM_REG_D1) != regset.end()){
            retset.insert(ARM_REG_D1);
            if(regset.find(ARM_REG_D2) != regset.end()){
                retset.insert(ARM_REG_D2);
                if(regset.find(ARM_REG_D3) != regset.end()){
                    retset.insert(ARM_REG_D3);
                }
            }
        }
    }

    // D floating reg
    if(regset.find(ARM_REG_Q0) != regset.end()){
        retset.insert(ARM_REG_Q0);
        if(regset.find(ARM_REG_Q1) != regset.end()){
            retset.insert(ARM_REG_Q1);
            if(regset.find(ARM_REG_Q2) != regset.end()){
                retset.insert(ARM_REG_Q2);
                if(regset.find(ARM_REG_Q3) != regset.end()){
                    retset.insert(ARM_REG_Q3);
                }
            }
        }
    }

    return retset;
}

set<uint16_t> InterProcAnal::filterRetReg(set<uint16_t> regset){
    set <uint16_t> retset;

    // Normal reg
    if(regset.find(ARM_REG_R0) != regset.end()){
        retset.insert(ARM_REG_R0);
        if(regset.find(ARM_REG_R1) != regset.end()){
            retset.insert(ARM_REG_R1);
            if(regset.find(ARM_REG_R2) != regset.end()){
                retset.insert(ARM_REG_R2);
                if(regset.find(ARM_REG_R3) != regset.end()){
                    retset.insert(ARM_REG_R3);
                }
            }
        }
    }

    // S floating reg
    if(regset.find(ARM_REG_S0) != regset.end()){
        retset.insert(ARM_REG_S0);
        if(regset.find(ARM_REG_S1) != regset.end()){
            retset.insert(ARM_REG_S1);
            if(regset.find(ARM_REG_S2) != regset.end()){
                retset.insert(ARM_REG_S2);
                if(regset.find(ARM_REG_S3) != regset.end()){
                    retset.insert(ARM_REG_S3);
                }
            }
        }
    }

    // D floating reg
    if(regset.find(ARM_REG_D0) != regset.end()){
        retset.insert(ARM_REG_D0);
        if(regset.find(ARM_REG_D1) != regset.end()){
            retset.insert(ARM_REG_D1);
            if(regset.find(ARM_REG_D2) != regset.end()){
                retset.insert(ARM_REG_D2);
                if(regset.find(ARM_REG_D3) != regset.end()){
                    retset.insert(ARM_REG_D3);
                }
            }
        }
    }

    // D floating reg
    if(regset.find(ARM_REG_Q0) != regset.end()){
        retset.insert(ARM_REG_Q0);
        if(regset.find(ARM_REG_Q1) != regset.end()){
            retset.insert(ARM_REG_Q1);
            if(regset.find(ARM_REG_Q2) != regset.end()){
                retset.insert(ARM_REG_Q2);
                if(regset.find(ARM_REG_Q3) != regset.end()){
                    retset.insert(ARM_REG_Q3);
                }
            }
        }
    }


    return retset;
}

const Value * InterProcAnal::retReg2Var(cs_insn &inst, uint16_t ri,  vector <const Value *> &irvec, char mode){
    const Value *ret = NULL;
    cs_regs regs_read, regs_write;
    uint8_t regs_read_count, regs_write_count;
    vector <uint16_t> regvec;

    if(cs_regs_access(handle, &inst, regs_read, &regs_read_count, regs_write, &regs_write_count)) return NULL;

    if(arch == "ARM" || arch == "ARM_MIXED" || arch == "THUMB"){
        if(mode == 'R' || mode == 'r'){
            for (int j = 0; j < regs_read_count; j++) {
                uint16_t r = regs_read[j];
                if(isNormalReg(r)) regvec.push_back(r);
            }
        }
        else if(mode == 'W' || mode == 'w'){
            for (int j = 0; j < regs_write_count; j++) {
                uint16_t r = regs_write[j];
                if(isNormalReg(r)) regvec.push_back(r);
            }
        }
        else assert(0);
#ifdef DEBUG
        cout << "\taccess reg: " << regvec.size() << endl;
        for (int j = 0; j < regvec.size(); j++) {
            cout << "\t\t>>idx: " << j << " rreg " << reg2str(regvec[j]) << endl;
        }
        cout << "\tir: " << irvec.size() << endl;
        for (int j = 0; j < irvec.size(); j++) {
            cout << "\t\t>>idx: " << j << " var " << inst2str(irvec[j]) << endl;
        }
#endif
        // idx identification
        for (int j = 0; j < regvec.size(); j++) {
            if(regvec[j] == ri){
                ret = irvec[j];
                break;
            }
        }

        return ret;
    }
    else if(arch == "X86_64") assert(0);
    else if(arch == "X86_32") assert(0);
    else if(arch == "ARM64") assert(0);
    else{
        cout << "[Error] unknown arch: " << arch << endl;
        assert(0); 
    }

    assert(0);
    return ret;
}


uint16_t InterProcAnal::str2reg(string reg){
    if(reg == "S0") return ARM_REG_S0;
    else if(reg == "S1") return ARM_REG_S1;
    else if(reg == "S2") return ARM_REG_S2;
    else if(reg == "S3") return ARM_REG_S3;
    else if(reg == "S4") return ARM_REG_S4;
    else if(reg == "R0") return ARM_REG_R0;
    else if(reg == "R1") return ARM_REG_R1;
    else if(reg == "R2") return ARM_REG_R2;
    else if(reg == "R3") return ARM_REG_R3;
    else if(reg == "R4") return ARM_REG_R4;
    else{
        cout << "[Error]: Simply add more registers. Then, it should work" << endl;
        assert(0);
    }
}
//#################################################################
//############################# Caller <-> Callee Util
//#################################################################
const Function *InterProcAnal::csaddr2ce(uint64_t csaddr){
    if(csaddr2ceaddr.find(csaddr) != csaddr2ceaddr.end()){
        uint64_t ceaddr = csaddr2ceaddr[csaddr];
        return retIRFunc(ceaddr);
    }
    return NULL;
}

uint64_t InterProcAnal::retCSAddrFromRetVar(const Function *func, const Value *v, unsigned minidx, unsigned maxidx){
    uint64_t funcaddr = retFuncAddr(func);
    if(!funcaddr) return 0;
    else if(funcaddr2addrset.find(funcaddr) == funcaddr2addrset.end()) return 0;
    else if(retvar2csaddrs.find(v) == retvar2csaddrs.end()) return 0;

    set <uint64_t> csaddrs = retvar2csaddrs[v];
    set <uint64_t> funcaddrs = funcaddr2addrset[funcaddr];
    vector <const Instruction*> instvec = func2insts(func);
    uint64_t csaddr = 0;
    uint64_t nearaddr = 0;


    // Near address
    if(inst2instaddr.find(instvec[maxidx]) != inst2instaddr.end()) nearaddr = inst2instaddr[instvec[maxidx]];
    else{
        for(int i = maxidx; i < instvec.size(); i++){
            if(inst2instaddr.find(instvec[i]) != inst2instaddr.end()){
                nearaddr = inst2instaddr[instvec[i]];
                break;
            }
        }
    }
    assert(nearaddr);

#ifdef DEBUG
    cout << std::hex << "\t\t\t near: " << nearaddr << " / # of csaddr: " << csaddrs.size() << endl;
    cout << "\t\t\t\t given inst: " << inst2str(v) << endl;
#endif


    // csaddr get
    for(auto it = csaddrs.begin(); it != csaddrs.end(); ++it){  // this is ordered
        uint64_t addr = *it;
#ifdef DEBUG
        cout << "\t\t\t\t addr: " << addr << endl;
#endif
        if(!csaddr) csaddr = addr;
        else{
            uint64_t curdiff;
            uint64_t prediff;
            if(csaddr > nearaddr) prediff = csaddr - nearaddr;
            else                   prediff = nearaddr - csaddr;

            if(addr > nearaddr) curdiff = addr - nearaddr;
            else                   curdiff = nearaddr - addr;

            if(curdiff < prediff) csaddr = addr;
        }

    }
    cout << std::dec;
    return csaddr;
}

//#################################################################
//############################# Inter mapping (arg/ret)
//#################################################################
void InterProcAnal::mapRegCeReadInsts(uint64_t ceaddr, map <uint16_t, vector<cs_insn>> &regReadInsts){
    if(ce2argset.find(ceaddr) == ce2argset.end()) assert(0);

    set <uint16_t> argRegs = ce2argset[ceaddr];
    set <uint16_t> wRegs;
    vector <cs_insn> insts = funcaddr2disas[ceaddr];
	cs_regs regs_read, regs_write;
	uint8_t regs_read_count, regs_write_count;

    for(int i = 0; i < insts.size(); i++){
        cs_insn inst = insts[i];
        if(!cs_regs_access(handle, &inst, regs_read, &regs_read_count, regs_write, &regs_write_count)) {
            if(regs_read_count){
                for(int j = 0; j < regs_read_count; j++){
                    uint16_t ri = regs_read[j];
                    if(argRegs.find(ri) != argRegs.end() && wRegs.find(ri) == wRegs.end()){ // Read overlap is ok
                        regReadInsts[ri].push_back(inst);
#ifdef DEBUG
                        cout << "[Matched]: " << reg2str(ri) << endl;
                        disPrint(inst);
#endif
                    }
                }
            }

            if(regs_write_count){       // Dirty reg must be excluded
                for(int j = 0; j < regs_write_count; j++){
                    uint16_t ri = regs_write[j];
                    if(wRegs.find(ri) == wRegs.end()){
                        wRegs.insert(ri);
                    }
                }
            }
        }
    }
#ifdef DEBUG
    cout << " === " << endl;
#endif
    if(argRegs.size() != regReadInsts.size()){
#ifdef DEBUG
        cout << " Not matched " << argRegs.size() << " VS " << regReadInsts.size() << endl;
        cout << "\tRegs: " << endl;
#endif
        for(auto it = argRegs.begin(); it != argRegs.end(); ++it){
            uint16_t r = *it;
#ifdef DEBUG
            cout << "\t" << reg2str(r) << endl;
#endif
            if(regReadInsts.find(r) != regReadInsts.end()){
                vector <cs_insn> rinsts = regReadInsts[r];
                for(int i = 0; i < rinsts.size(); i++){
#ifdef DEBUG
                    cout << "\t\t >> ";
                    disPrint(rinsts[i]);
#endif
                }
            }
#ifdef DEBUG
            else cout << "\t\t >> None";
#endif
        }

#ifdef DEBUG
        cout << " === " << endl;
#endif
        assert(0);
    }

    return;
}

void InterProcAnal::mapRegCsArgWriteInsts(uint64_t csaddr, uint64_t craddr, uint64_t ceaddr, map <uint16_t, vector<cs_insn>> &regArgWriteInsts){
    // Note: cearg has the complete set of reg info. So, we use its list instead of cs side
    set <uint16_t> argRegs = ce2argset[ceaddr];
    set <uint16_t> wRegs;
    vector <cs_insn> insts; 
	cs_regs regs_read, regs_write;
	uint8_t regs_read_count, regs_write_count;

    if(funcaddr2disas.find(craddr) == funcaddr2disas.end()){
        uint64_t crsz;
        if(!(crsz = retFuncSz(craddr))){
            const Function *crfunc = retIRFunc(craddr);
            uint64_t startaddr = craddr;
            uint64_t endaddr = inferMissingEndAddr(crfunc);
            string exemode = inferMissingExemode(crfunc);
            binFuncRangeUpdate(startaddr, endaddr, exemode);
            crsz = retFuncSz(craddr);
        }
        string crexe = retFuncMode(craddr);
        disFunc(craddr, crsz, crexe);
    }
    insts = funcaddr2disas[craddr];

    for(int i = insts.size() - 1; i >= 0; i--){
        cs_insn inst = insts[i];

        if(inst.address >= csaddr) continue;
        else if(!cs_regs_access(handle, &inst, regs_read, &regs_read_count, regs_write, &regs_write_count)) {
            if(regs_write_count){
                for(int j = 0; j < regs_write_count; j++){
                    uint16_t ri = regs_write[j];
                    if(argRegs.find(ri) != argRegs.end() && wRegs.find(ri) == wRegs.end()){
                        regArgWriteInsts[ri].push_back(inst);
                        wRegs.insert(ri);
#ifdef DEBUG
                        cout << "[Matched]: " << reg2str(ri) << std::hex 
                             << " from " << inst.address << " w/ csaddr: " << csaddr << std::dec <<  endl;
                        disPrint(inst);
#endif
                    }
                }
            }
        }
    }
#ifdef DEBUG
    cout << " === " << endl;
#endif
    if(argRegs.size() != regArgWriteInsts.size()){
#ifdef DEBUG
        cout << " Not matched " << argRegs.size() << " VS " << regArgWriteInsts.size() << endl;
        cout << "\tRegs: " << endl;
#endif
        for(auto it = argRegs.begin(); it != argRegs.end(); ++it){
            uint16_t r = *it;
#if 0
            cout << "\t" << reg2str(r) << endl;                         // Arg Reg printing
#endif
            if(regArgWriteInsts.find(r) != regArgWriteInsts.end()){
                vector <cs_insn> rinsts = regArgWriteInsts[r];
#ifdef DEBUG
                for(int i = 0; i < rinsts.size(); i++){
                    cout << "\t\t >> ";
                    disPrint(rinsts[i]);
                }
#endif
            }
#ifdef DEBUG
            else cout << "\t\t >> None" << endl;
#endif
        }
#ifdef DEBUG
        cout << " === " << endl;
#endif
    }

    return;

}

void InterProcAnal::mapRegCsRetReadInsts(uint64_t csaddr, uint64_t craddr, uint64_t ceaddr, map <uint16_t, vector<cs_insn>> &regRetReadInsts){
    // Note: cearg has the complete set of reg info. So, we use its list instead of cs side
    set <uint16_t> retRegs = ce2retset[ceaddr];
    set <uint16_t> wRegs;
    vector <cs_insn> insts;
	cs_regs regs_read, regs_write;
	uint8_t regs_read_count, regs_write_count;

    if(funcaddr2disas.find(craddr) == funcaddr2disas.end()){
        uint64_t crsz;
        if(!(crsz = retFuncSz(craddr))){
            const Function *crfunc = retIRFunc(craddr);
            uint64_t startaddr = craddr;
            uint64_t endaddr = inferMissingEndAddr(crfunc);
            string exemode = inferMissingExemode(crfunc);
            binFuncRangeUpdate(startaddr, endaddr, exemode);
            crsz = retFuncSz(craddr);
        }
        string crexe = retFuncMode(craddr);
        disFunc(craddr, crsz, crexe);
    }
    insts = funcaddr2disas[craddr];

    for(int i = 0; i < insts.size(); i++){
        cs_insn inst = insts[i];

        if(inst.address <= csaddr) continue;
        else if(!cs_regs_access(handle, &inst, regs_read, &regs_read_count, regs_write, &regs_write_count)) {
            if(regs_read_count){
                for(int j = 0; j < regs_read_count; j++){
                    uint16_t ri = regs_read[j];
                    if(retRegs.find(ri) != retRegs.end() && wRegs.find(ri) == wRegs.end()){
                        regRetReadInsts[ri].push_back(inst);
#ifdef DEBUG
                        cout << "[Matched]: " << reg2str(ri) << std::hex 
                             << " from " << inst.address << " w/ csaddr: " << csaddr << std::dec <<  endl;
                        disPrint(inst);
#endif
                    }
                }
            }

            if(regs_write_count){       // Dirty reg must be excluded
                for(int j = 0; j < regs_write_count; j++){
                    uint16_t ri = regs_write[j];
                    if(wRegs.find(ri) == wRegs.end()){
                        wRegs.insert(ri);
                    }
                }
            }
        }
    }
#ifdef DEBUG
    cout << " === " << endl;
    if(retRegs.size() != regRetReadInsts.size()){
        cout << " Not matched " << retRegs.size() << " VS " << regRetReadInsts.size() << endl;
        cout << " === " << endl;
    }
#endif

    return;
}


void InterProcAnal::mapRegCeRetWriteInsts(uint64_t csaddr, uint64_t craddr, uint64_t ceaddr, map <uint16_t, vector<cs_insn>> &regRetWriteInsts){
    // Note: cearg has the complete set of reg info. So, we use its list instead of cs side
    set <uint16_t> retRegs = ce2retset[ceaddr];
    set <uint16_t> wRegs;
    vector <cs_insn> insts;
	cs_regs regs_read, regs_write;
	uint8_t regs_read_count, regs_write_count;
    int retidx = funcaddr2retidx[ceaddr];

    if(funcaddr2disas.find(ceaddr) == funcaddr2disas.end()){
        uint64_t cesz;
        if(!(cesz = retFuncSz(ceaddr))){
            const Function *cefunc = retIRFunc(ceaddr);
            uint64_t startaddr = ceaddr;
            uint64_t endaddr = inferMissingEndAddr(cefunc);
            string exemode = inferMissingExemode(cefunc);
            binFuncRangeUpdate(startaddr, endaddr, exemode);
            cesz = retFuncSz(ceaddr);
        }
        string ceexe = retFuncMode(ceaddr);
        disFunc(ceaddr, cesz, ceexe);
    }
    insts = funcaddr2disas[ceaddr];

    for(int i = retidx - 1; i >= 0; i--){
        cs_insn inst = insts[i];

        if(!cs_regs_access(handle, &inst, regs_read, &regs_read_count, regs_write, &regs_write_count)) {
            if(regs_write_count){
                for(int j = 0; j < regs_write_count; j++){
                    uint16_t ri = regs_write[j];
                    if(retRegs.find(ri) != retRegs.end() && wRegs.find(ri) == wRegs.end()){
                        regRetWriteInsts[ri].push_back(inst);
                        wRegs.insert(ri);
#ifdef DEBUG
                        cout << "[Matched]: " << reg2str(ri) << std::hex 
                             << " from " << inst.address << " w/ csaddr: " << csaddr << std::dec <<  endl;
                        disPrint(inst);
#endif
                    }
                }
            }
        }
    }

#ifdef DEBUG
    cout << " === " << endl;
    if(retRegs.size() != regRetWriteInsts.size()){
        cout << " Not matched " << retRegs.size() << " VS " << regRetWriteInsts.size() << endl;
        cout << " === " << endl;
    }
#endif

    return;
}



// This is first
void InterProcAnal::propagatedRegReadInsts(uint64_t addr, vector <cs_insn> &insts, uint16_t tri, const Function *func){
    uint64_t faddr = retFuncAddr(func);
    assert(faddr);
    insts.clear();
    vector <cs_insn> finsts = funcaddr2disas[faddr];
    cs_regs regs_read, regs_write;
    uint8_t regs_read_count, regs_write_count;
    bool dirty = false;

    for(int i = 0; i < finsts.size(); i++){
        cs_insn inst = finsts[i];
        if(inst.address > addr){        // not ">="
            if(!cs_regs_access(handle, &inst, regs_read, &regs_read_count, regs_write, &regs_write_count)) {
                if(regs_read_count){
                    for(int j = 0; j < regs_read_count; j++){
                        uint16_t ri = regs_read[j];
                        if(tri == ri && dirty == false){
                            insts.push_back(inst);
                        }
                    }
                }

                if(regs_write_count){       // Dirty reg must be excluded
                    for(int j = 0; j < regs_write_count; j++){
                        uint16_t ri = regs_write[j];
                        if(tri == ri){
                            dirty = true;
                            break;
                        }
                    }
                }
            }
        }

        if(dirty) break;
    }
    return;
}


// Later one: Backward
void InterProcAnal::propagatedRegWriteInsts(uint64_t addr, vector <cs_insn> &insts, uint16_t tri, const Function *func){
    uint64_t faddr = retFuncAddr(func);
    assert(faddr);
    insts.clear();
    vector <cs_insn> finsts = funcaddr2disas[faddr];
    cs_regs regs_read, regs_write;
    uint8_t regs_read_count, regs_write_count;
    bool dirty = false;

    for(int i = finsts.size() - 1; i >= 0; i--){
        cs_insn inst = finsts[i];
        if(inst.address < addr){        // not ">="
            if(!cs_regs_access(handle, &inst, regs_read, &regs_read_count, regs_write, &regs_write_count)) {
                if(regs_write_count){
                    for(int j = 0; j < regs_write_count; j++){
                        uint16_t ri = regs_write[j];
                        if(tri == ri){ //&& dirty == false){
                            insts.push_back(inst);
                            dirty = true; //?
                            break;  //??
                        }
                    }
                }
            }
        }

        if(dirty) break;
    }
    return;
}

// From caller arg (written later)
const Value * InterProcAnal::mapOneRegBackward(uint16_t ri, const Function *func, vector <cs_insn> &targetinsts, int maxdepth){
    const Value *ret = NULL;
    if(maxdepth <= 0) return ret;
    else if(!targetinsts.size()) return ret;

    uint64_t curfuncaddr = retFuncAddr(func);
    if(funcaddr2disas.find(curfuncaddr) == funcaddr2disas.end()) assert(0);

#ifdef DEBUG
    cout << "[INFO]: mapOneRegBackward for reg: " << reg2str(ri) << " from " << std::hex << curfuncaddr << std::dec <<endl;
#endif

    for(int i = targetinsts.size() - 1; i >= 0; i--){
        uint64_t instaddr = targetinsts[i].address;
#ifdef DEBUG
        disPrint(targetinsts[i]);
#endif
        if(addr2IRwrvec.find(instaddr) != addr2IRwrvec.end()){
            vector <const Value *> wrvec = addr2IRwrvec[instaddr];
            if(wrvec.size() == 1){
                ret = wrvec[0];
#ifdef DEBUG
                cout << "Addr: " << std::hex << instaddr << std::dec << endl;
                cout << "\t>> Map " << reg2str(ri) << " -> IR: " << inst2str(ret) << endl;
#endif
                break;
            }
            else if(wrvec.size() == 2){  // inVar? Or Error case
                ret = retReg2Var(targetinsts[i], ri, wrvec, 'W');
#ifdef DEBUG
                cout << "Addr: " << std::hex << instaddr << std::dec << endl;
                cout << "\t>> Map " << reg2str(ri) << " -> IR: " << inst2str(ret) << endl;
#endif
                if(wrvec.size() > 3) assert(0); 
                break;
            }
        }
#ifdef DEBUG
        else{ 
            cout << "\t>> Not matched " << endl;
        }
#endif
    }

    // Detection ok? if fails ... generate new insts
    if(ret) return ret;
    else{
        vector <cs_insn> insts = funcaddr2disas[curfuncaddr];
        vector <cs_insn> newinsts;
        cs_regs regs_read, regs_write;
        uint8_t regs_read_count, regs_write_count;
        
        assert(insts.size());
        for(int i = insts.size() - 1; i >= 0; i--){
            cs_insn inst = insts[i];
            if(targetinsts.back().address < insts[i].address) continue;     // Note: Processed inst can be done repeatedly?
            else if(!isRegPropaInst(inst)) continue;
            else if(!cs_regs_access(handle, &inst, regs_read, &regs_read_count, regs_write, &regs_write_count)) {
                for(int j = 0; j < regs_write_count; j++){
                    uint16_t r = regs_write[j];
                    if(r == ri && regs_read_count > 0){  
                        vector <uint16_t> rvec;
                        for(int j = 0; j < regs_read_count; j++){
                            if(isNormalReg(regs_read[j])) rvec.push_back(regs_read[j]);
                        }
                        if(rvec.size() == 1){    // care about normal writing = one normal reg write
                            vector<cs_insn> nestedWriteInsts;
                            propagatedRegWriteInsts(inst.address, nestedWriteInsts, rvec.back(), func);
                            if(ret = mapOneRegBackward(rvec.back(), func, nestedWriteInsts, maxdepth - 1)) return ret;
                        }
                        rvec.clear();
                    }
                }
            }
        }
        return NULL;
    }
}

// From callee arg or caller ret
const Value * InterProcAnal::mapOneRegForward(uint16_t ri, const Function *func, vector <cs_insn> &targetinsts, int maxdepth){
    const Value *ret = NULL;
    if(maxdepth <= 0) return ret;
    else if(!targetinsts.size()) return ret;

    uint64_t curfuncaddr = retFuncAddr(func);
    if(funcaddr2disas.find(curfuncaddr) == funcaddr2disas.end()) assert(0);

#ifdef DEBUG
    cout << "[INFO]: mapOneRegForward for reg: " << reg2str(ri) << " from " << std::hex << curfuncaddr << std::dec <<endl;
#endif
    for(int i = 0; i < targetinsts.size(); i++){
        uint64_t instaddr = targetinsts[i].address;
#ifdef DEBUG
        disPrint(targetinsts[i]);
#endif
        if(addr2IRrrvec.find(instaddr) != addr2IRrrvec.end()){
            vector <const Value *> rrvec = addr2IRrrvec[instaddr];
            if(rrvec.size() == 1){
                ret = rrvec[0];
#ifdef DEBUG
                cout << "Addr: " << std::hex << instaddr << std::dec << endl;
                cout << "\t>> Map " << reg2str(ri) << " -> IR: " << inst2str(ret) << endl;
#endif
                break;
            }
            else if(rrvec.size() >= 2){  // inVar? Or Error case
                ret = retReg2Var(targetinsts[i], ri, rrvec, 'R');
#ifdef DEBUG
                cout << "Addr: " << std::hex << instaddr << std::dec << endl;
                cout << "\t>> Map " << reg2str(ri) << " -> IR: " << inst2str(ret) << endl;
#endif
                break;
            }
        }
#ifdef DEBUG
        else{ 
            cout << "\t>> Not matched " << endl;
        }
#endif
    }

    // if detection fails, generate new insts to explore further candidates
    if(ret) return ret;
    else{
        vector <cs_insn> insts = funcaddr2disas[curfuncaddr];
        vector <cs_insn> newinsts;
        cs_regs regs_read, regs_write;
        uint8_t regs_read_count, regs_write_count;

        assert(insts.size());
        for(int i = 0; i < insts.size(); i++){
            cs_insn inst = insts[i];
            if(targetinsts[0].address > insts[i].address) continue;         // Note: Processed inst can be done repeatedly?
            else if(!isRegPropaInst(inst)) continue;
            else if(!cs_regs_access(handle, &inst, regs_read, &regs_read_count, regs_write, &regs_write_count)) {
                for(int j = 0; j < regs_read_count; j++){
                    uint16_t r = regs_read[j];
                    if(r == ri && regs_write_count > 0){  
                        vector <uint16_t> wvec;
                        for(int j = 0; j < regs_write_count; j++){
                            if(isNormalReg(regs_write[j])) wvec.push_back(regs_write[j]);
                        }
                        if(wvec.size() == 1){    // care about normal read = one normal reg read
                            vector<cs_insn> nestedReadInsts;
                            propagatedRegReadInsts(inst.address, nestedReadInsts, wvec.back(), func);
                            if(ret = mapOneRegForward(wvec.back(), func, nestedReadInsts, maxdepth - 1)) return ret;
                        }
                        wvec.clear();
                    }
                }
            }
        }
        return NULL;
    }
}


void InterProcAnal::mapInterPass(uint64_t csaddr, uint64_t ceaddr, uint64_t craddr){
    // Callee Analysis (IR var 2 Reg)
#ifdef DEBUG
    cout << "------------------ Callee (InterPass)---------------" << endl;
#endif
    const Function *cefunc = retIRFunc(ceaddr);
    assert(cefunc);
    map <uint16_t, vector<cs_insn>> regReadCeInsts;
    map <uint16_t, const Value *> reg2ceargvar;
    mapRegCeReadInsts(ceaddr, regReadCeInsts);      // Written inst is excluded

        // Mapping to LLVM IR 
    for(auto mit = regReadCeInsts.begin(); mit != regReadCeInsts.end(); ++mit){
        uint16_t ri = mit->first;
        vector <cs_insn> insts = mit->second;

        // Recursive Detect
        const Value *mapv = mapOneRegForward(ri, cefunc, insts, 3);
        if(mapv){
            reg2ceargvar[ri] = mapv;
#ifdef DEBUG
            cout << "[Good]: ce arg var at csaddr " << std::hex << csaddr << std::dec << " for reg: " << reg2str(ri) << "\n" << endl;
#endif
        }
        else{
#ifdef DEBUG
            cout << "[Warning]: no ce arg var at csaddr " << std::hex << csaddr << std::dec << " for reg: " << reg2str(ri) << "\n" << endl;
#endif
        }
    }

    // Caller Analysis (IR var 2 Reg)
#ifdef DEBUG
    cout << "\n------------------ Caller Arg (InterPass)---------------" << endl;
#endif
    const Function *crfunc = retIRFunc(craddr);
    assert(crfunc);
    map <uint16_t, const Value *> reg2csargvar;
    map <uint16_t, vector<cs_insn>> regArgWriteInsts;
    mapRegCsArgWriteInsts(csaddr, craddr, ceaddr, regArgWriteInsts);
        // Mapping to LLVM IR  (Arg)
    for(auto mit = regArgWriteInsts.begin(); mit != regArgWriteInsts.end(); ++mit){
        uint16_t ri = mit->first;
        vector <cs_insn> insts = mit->second;

        // Recursive Detect
        const Value *mapv = mapOneRegBackward(ri, crfunc, insts, 3);

        if(mapv) {
            reg2csargvar[ri] = mapv;
#ifdef DEBUG
            cout << "[Good]: cs arg var at csaddr " << std::hex << csaddr << std::dec << " for reg: " << reg2str(ri) << "\n" << endl;
#endif
        }
#ifdef DEBUG
        else{
            cout << "[Warning]: no cs arg var at csaddr " << std::hex << csaddr << std::dec << " for reg: " << reg2str(ri) << "\n" << endl;
        }
#endif
    }

#ifdef DEBUG
    cout << "\n------------------ Caller Ret (InterPass)---------------" << endl;
#endif
    map <uint16_t, const Value *> reg2csretvar;
    map <uint16_t, vector<cs_insn>> regRetReadInsts;
    mapRegCsRetReadInsts(csaddr, craddr, ceaddr, regRetReadInsts);

        // Mapping to LLVM IR  (Ret)
    for(auto mit = regRetReadInsts.begin(); mit != regRetReadInsts.end(); ++mit){
        uint16_t ri = mit->first;
        vector <cs_insn> insts = mit->second;

        // Recursive Detect
        const Value *mapv = mapOneRegForward(ri, crfunc, insts, 3);

        if(mapv) {
            reg2csretvar[ri] = mapv;
#ifdef DEBUG
            cout << "[Good]: ret var at csaddr " << std::hex << csaddr << std::dec << " for reg: " << reg2str(ri) << "\n" << endl;
#endif
        }
#ifdef DEBUG
        else{
            cout << "[Warning]: no ret var at csaddr " << std::hex << csaddr << std::dec << " for reg: " << reg2str(ri) << "\n" << endl;
        }
#endif
    }

#ifdef DEBUG
    cout << "\n------------------ Callee Ret (InterPass)---------------" << endl;
#endif
    map <uint16_t, const Value *> reg2ceretvar;
    map <uint16_t, vector<cs_insn>> regRetWriteInsts;
        // Mapping to LLVM IR  (Ret)     
    mapRegCeRetWriteInsts(csaddr, craddr, ceaddr, regRetWriteInsts);
    for(auto mit = regRetWriteInsts.begin(); mit != regRetWriteInsts.end(); ++mit){
        uint16_t ri = mit->first;
        vector <cs_insn> insts = mit->second;

        // Recursive Detect
        const Value *mapv = mapOneRegBackward(ri, cefunc, insts, 3);

        if(mapv) {
            reg2ceretvar[ri] = mapv;
#ifdef DEBUG
            cout << "[Good]: ret var at csaddr " << std::hex << csaddr << std::dec << " for reg: " << reg2str(ri) << "\n" << endl;
#endif
        }
#ifdef DEBUG
        else{
            cout << "[Warning]: no ret var at csaddr " << std::hex << csaddr << std::dec << " for reg: " << reg2str(ri) << "\n" << endl;
        }
#endif
    }

    // final mapping
    csaddr2craddr[csaddr] = craddr; // map <uint64_t, uint64_t>
    csaddr2ceaddr[csaddr] = ceaddr; // map <uint64_t, uint64_t>

        // CS mapping
    for(auto it = reg2csargvar.begin(); it != reg2csargvar.end(); ++it){
        uint16_t ri = it->first;
        const Value *csv = it->second;
        if(reg2ceargvar.find(ri) == reg2ceargvar.end()) continue;

        const Value *cev = reg2ceargvar[ri];

            // Reg IR set
        arg_csaddr2csvs[csaddr].insert(csv);
        arg_csaddr2cevs[csaddr].insert(cev);
            // IR 2 IR
        arg_csaddr2csv2cev[csaddr][csv] = cev;  // map <uint64_t, map <const Value *, const Value *>>
        arg_csaddr2cev2csv[csaddr][cev] = csv;  // map <uint64_t, map <const Value *, const Value *>>
            // CS IR to CS reg
        arg_csaddr2csv2reg[csaddr][csv] = ri; // map <uint64_t, map <const Value *, uint16_t>>
        arg_csaddr2reg2csv[csaddr][ri] = csv; // map <uint64_t, map <uint16_t, const Value *>>
            // CE IR to CE reg
        arg_csaddr2cev2reg[csaddr][cev] = ri; // map <uint64_t, map <const Value *, uint16_t>>
        arg_csaddr2reg2cev[csaddr][ri] = cev; // map <uint64_t, map <uint16_t, const Value *>>
            // CS IR to AddrSet
        argvar2csaddrs[csv].insert(csaddr);
            // Func to Reg 2 CEV arg
        arg_cefunc2reg2cev[cefunc][ri] = cev;
    }
        // CE mapping (need improvement if necessary)
    for(auto it = reg2csretvar.begin(); it != reg2csretvar.end(); ++it){
        uint16_t ri = it->first;
        const Value *csv = it->second;
        if(reg2ceretvar.find(ri) == reg2ceretvar.end()) continue;
        const Value *cev = reg2ceretvar[ri];
            // IR 2 IR
        ret_csaddr2csv2cev[csaddr][csv] = cev;  // map <uint64_t, map <const Value *, const Value *>>
        ret_csaddr2cev2csv[csaddr][cev] = csv;  // map <uint64_t, map <const Value *, const Value *>>

        ret_csaddr2csvs[csaddr].insert(csv);
        ret_csaddr2csv2reg[csaddr][csv] = ri;
        ret_csaddr2reg2csv[csaddr][ri] = csv;
            // CS IR to AddrSet
        retvar2csaddrs[csv].insert(csaddr);
    }

        // extra mapping
    updateInstAddr2rRegSet(craddr);
    updateInstAddr2rRegSet(ceaddr);

    return;
}

void InterProcAnal::updateInstAddr2rRegSet(uint64_t faddr){
    if(funcAddr2InstAddr2rRegSet.find(faddr) != funcAddr2InstAddr2rRegSet.end()) return;

    //cout << " ----------- faddr: " << std::hex << faddr << std::dec << endl;
    const Function* func = retIRFunc(faddr);
    vector <cs_insn> insts = funcaddr2disas[faddr];
    cs_regs regs_read, regs_write;
    uint8_t regs_read_count, regs_write_count;

    for(int i = 0; i < insts.size(); i++){
        cs_insn inst = insts[i];

        if(!cs_regs_access(handle, &inst, regs_read, &regs_read_count, regs_write, &regs_write_count)) {
            if(regs_read_count){
                //cout << "----------------- " << endl;
                //disPrint(inst);
                for(int j = 0; j < regs_read_count; j++){
                    uint16_t ri = regs_read[j];
                    //cout << "\t\t >> rReg: " << reg2str(ri) << std::hex << endl;
                    funcAddr2InstAddr2rRegSet[faddr][inst.address].insert(ri);
                    func2InstAddr2rRegSet[func][inst.address].insert(ri);
                }
            }
        }
    }
 
    return;
}


//#################################################################
//############################# Result return
//#################################################################
set <const Value *> InterProcAnal::retCSAddr2ArgCSVs(uint64_t csaddr){
    if(arg_csaddr2csvs.find(csaddr) != arg_csaddr2csvs.end()){
        return arg_csaddr2csvs[csaddr];
    }
    set <const Value *> dummyset;
    return dummyset;
}

set <const Value *> InterProcAnal::retCSAddr2ArgCEVs(uint64_t csaddr){
    if(arg_csaddr2cevs.find(csaddr) != arg_csaddr2cevs.end()){
        return arg_csaddr2cevs[csaddr];
    }
    set <const Value *> dummyset;
    return dummyset;
}

set <const Value *> InterProcAnal::retCSAddr2RetCSVs(uint64_t csaddr){
    if(ret_csaddr2csvs.find(csaddr) != ret_csaddr2csvs.end()){
        return ret_csaddr2csvs[csaddr];
    }
    set <const Value *> dummyset;
    return dummyset;
}

const Value *InterProcAnal::retCSAddr2ArgCSV(uint64_t csaddr, uint16_t ri){
    if(arg_csaddr2reg2csv.find(csaddr) != arg_csaddr2reg2csv.end()){
        if(arg_csaddr2reg2csv[csaddr].find(ri) != arg_csaddr2reg2csv[csaddr].end()) return arg_csaddr2reg2csv[csaddr][ri];
    }
    return NULL;
}

const Value *InterProcAnal::retCSAddr2ArgCEV(uint64_t csaddr, uint16_t ri){
    if(arg_csaddr2reg2cev.find(csaddr) != arg_csaddr2reg2cev.end()){
        if(arg_csaddr2reg2cev[csaddr].find(ri) != arg_csaddr2reg2cev[csaddr].end()) return arg_csaddr2reg2cev[csaddr][ri];
    }
    return NULL;
}

const Value *InterProcAnal::retCSAddr2RetCSV(uint64_t csaddr, uint16_t ri){
    if(ret_csaddr2reg2csv.find(csaddr) != ret_csaddr2reg2csv.end()){
        if(ret_csaddr2reg2csv[csaddr].find(ri) != ret_csaddr2reg2csv[csaddr].end()) return ret_csaddr2reg2csv[csaddr][ri];
    }
    return NULL;
}

uint16_t InterProcAnal::retCSAddr2ArgCSReg(uint64_t csaddr, const Value *v){
    if(arg_csaddr2csv2reg.find(csaddr) != arg_csaddr2csv2reg.end()){
        if(arg_csaddr2csv2reg[csaddr].find(v) != arg_csaddr2csv2reg[csaddr].end()) return arg_csaddr2csv2reg[csaddr][v];
    }
    return 0xFFFF;
}

uint16_t InterProcAnal::retCSAddr2ArgCEReg(uint64_t csaddr, const Value *v){
    if(arg_csaddr2cev2reg.find(csaddr) != arg_csaddr2cev2reg.end()){
        if(arg_csaddr2cev2reg[csaddr].find(v) != arg_csaddr2cev2reg[csaddr].end()) return arg_csaddr2cev2reg[csaddr][v];
    }
    return 0xFFFF;
}

uint16_t InterProcAnal::retCSAddr2RetCSReg(uint64_t csaddr, const Value *v){
    if(ret_csaddr2csv2reg.find(csaddr) != ret_csaddr2csv2reg.end()){
        if(ret_csaddr2csv2reg[csaddr].find(v) != ret_csaddr2csv2reg[csaddr].end()) return ret_csaddr2csv2reg[csaddr][v];
    }
    return 0xFFFF;
}

const Value *InterProcAnal::retFunc2ArgCEV(const Function *func, uint16_t ri){
    if(arg_cefunc2reg2cev.find(func) != arg_cefunc2reg2cev.end()){
        if(arg_cefunc2reg2cev[func].find(ri) != arg_cefunc2reg2cev[func].end()) return arg_cefunc2reg2cev[func][ri];
    }
    return NULL;
}

const Value *InterProcAnal::retFunc2HeuriAggressiveArgCEV(const Function *ce, const CallInst *cs, uint16_t ri){
    const Function *cr = cs->getParent()->getParent();
    uint64_t maxaddr = inst2instaddr[cs];
    uint64_t minaddr = retFuncAddr(cr); 
    const Value *v = NULL;

    // ri (register) valid check
    uint64_t ceaddr = retFuncAddr(ce);
    if(ce2argset.find(ceaddr) == ce2argset.end()){
        checkFunc(ce);
    }

    set <uint16_t> argRegSet = ce2argset[ceaddr];
    if(argRegSet.find(ri) == argRegSet.end()) return v;

    // minaddr update
    bool call = false;
    for(auto iit = inst_begin(cr); iit != inst_end(cr); ++iit){
        const Instruction *inst = &*iit;
        if(inst2instaddr.find(inst) != inst2instaddr.end()){
            uint64_t addr = inst2instaddr[inst];
            if(addr >= maxaddr) break;
            else if(const CallInst *ci = SVFUtil::dyn_cast<CallInst>(inst)){
                if(const Function *f = ci->getCalledFunction()){
                    if(f->getName().str().find("function") == 0){
                        call = true; 
                        continue;
                    }
                }
            }
            if(call){
                call = false;
                minaddr = addr;
            }
        }
    }

    // find related inst 
    if(func2InstAddr2rRegSet.find(cr) != func2InstAddr2rRegSet.end()){
        map <uint64_t, set<uint16_t>> instaddr2rRegSet = func2InstAddr2rRegSet[cr];
        for(auto it = instaddr2rRegSet.begin(); it != instaddr2rRegSet.end(); ++it){    // map -> ordered
            uint64_t addr = it->first;
            set <uint16_t> rrs = it->second;
            if(minaddr <= addr && maxaddr > addr && rrs.find(ri) != rrs.end()){
                vector <const Instruction *> instvec = retInstvecFromAddr(addr);
                if(instvec.size()){
                    v = instvec.back();
                }
            }
        }
    }

    return v;
}

uint16_t InterProcAnal::retCSAddr2Reg(uint64_t csaddr, const Value *v){
    uint16_t reg;
    if((reg = retCSAddr2ArgCSReg(csaddr, v)) != 0xFFFF) return reg;
    else if((reg = retCSAddr2ArgCEReg(csaddr, v)) != 0xFFFF) return reg;
    else if((reg = retCSAddr2RetCSReg(csaddr, v)) != 0xFFFF) return reg;
    else return 0xFFFF;
}

const Value *InterProcAnal::retCEArgFromCSArg(uint64_t csaddr, const Value *v){
    if(arg_csaddr2csv2cev.find(csaddr) != arg_csaddr2csv2cev.end()){
        if(arg_csaddr2csv2cev[csaddr].find(v) != arg_csaddr2csv2cev[csaddr].end()) return arg_csaddr2csv2cev[csaddr][v];
    }
    return NULL;
}

const Value *InterProcAnal::retCSArgFromCEArg(uint64_t csaddr, const Value *v){
    if(arg_csaddr2cev2csv.find(csaddr) != arg_csaddr2cev2csv.end()){
        if(arg_csaddr2cev2csv[csaddr].find(v) != arg_csaddr2cev2csv[csaddr].end()) return arg_csaddr2cev2csv[csaddr][v];
    }
    return NULL;
}

const Value *InterProcAnal::retCERetFromCSRet(uint64_t csaddr, const Value *v){
    if(ret_csaddr2csv2cev.find(csaddr) != ret_csaddr2csv2cev.end()){
        if(ret_csaddr2csv2cev[csaddr].find(v) != ret_csaddr2csv2cev[csaddr].end()) return ret_csaddr2csv2cev[csaddr][v];
    }
    return NULL;
}

const Value *InterProcAnal::retCSRetFromCERet(uint64_t csaddr, const Value *v){
    if(ret_csaddr2cev2csv.find(csaddr) != ret_csaddr2cev2csv.end()){
        if(ret_csaddr2cev2csv[csaddr].find(v) != ret_csaddr2cev2csv[csaddr].end()) return ret_csaddr2cev2csv[csaddr][v];
    }
    return NULL;
}

bool InterProcAnal::isSameReg(uint64_t csaddr, const Value *v1, const Value *v2){
    uint16_t r1 = retCSAddr2Reg(csaddr, v1);
    uint16_t r2 = retCSAddr2Reg(csaddr, v2);
    if(r1 != 0XFFFF && r2 != 0XFFFF && r1 == r2) return true;
    else return false;
}

bool InterProcAnal::isCSRetVar(const Function *func, const Value *v){
    uint64_t funcaddr = retFuncAddr(func);
    if(!funcaddr) return false;
    else if(funcaddr2addrset.find(funcaddr) == funcaddr2addrset.end()){
        return false;
    }
    else if(retvar2csaddrs.find(v) == retvar2csaddrs.end()){
        return false;
    }

    set <uint64_t> csaddrs = retvar2csaddrs[v];
    set <uint64_t> funcaddrs = funcaddr2addrset[funcaddr];

    for(auto it = csaddrs.begin(); it != csaddrs.end(); ++it){
        uint64_t csaddr = *it;
        if(funcaddrs.find(csaddr) != funcaddrs.end()) return true;
    }

    return false;
}

void InterProcAnal::retCSRetVar2ArgVarSet(const Function *func, const Value *v, set <const Value *> &vset, unsigned si, unsigned di){
    uint64_t funcaddr = retFuncAddr(func);
    if(!funcaddr) return;
    else if(funcaddr2addrset.find(funcaddr) == funcaddr2addrset.end()) return;
    else if(retvar2csaddrs.find(v) == retvar2csaddrs.end()) return;

    set <uint64_t> csaddrs = retvar2csaddrs[v];
    set <uint64_t> funcaddrs = funcaddr2addrset[funcaddr];
    for(auto it = csaddrs.begin(); it != csaddrs.end(); ++it){
        uint64_t csaddr = *it;
        if(funcaddrs.find(csaddr) != funcaddrs.end()){
            if(arg_csaddr2csvs.find(csaddr) != arg_csaddr2csvs.end()){
                set <const Value *> argset = arg_csaddr2csvs[csaddr];
                for(auto it = argset.begin(); it != argset.end(); ++it){
                    const Value *av = *it;

                    if(const Instruction *inst = SVFUtil::dyn_cast<Instruction>(av)){
                        unsigned curidx = inst2idx(inst);
                        if(si <= curidx && curidx <= di){
                            vset.insert(av);
                        }
                    }
                }
            }
        }
    }

    return ;
}

void InterProcAnal::updateCSRet2CSAddrs(const Function *func, const Value *v, set <uint64_t> &csaddrset){
    uint64_t funcaddr = retFuncAddr(func);
    if(!funcaddr) return;
    else if(funcaddr2addrset.find(funcaddr) == funcaddr2addrset.end()) return;
    else if(retvar2csaddrs.find(v) == retvar2csaddrs.end()) return;

    set <uint64_t> csaddrs = retvar2csaddrs[v];
    set <uint64_t> funcaddrs = funcaddr2addrset[funcaddr];

    for(auto it = csaddrs.begin(); it != csaddrs.end(); ++it){
        uint64_t csaddr = *it;
        if(funcaddrs.find(csaddr) != funcaddrs.end()){
            csaddrset.insert(csaddr);                       // update here
        }
    }

    return;
}

//#################################################################
//############################# Main analyzer
//#################################################################
void InterProcAnal::interAnalysis(const Instruction *cs){
    uint64_t csaddr, craddr, ceaddr;
    uint64_t crsz, cesz;
    string crexe, ceexe;
    const Function *crfunc, *cefunc;
    assert(cs);

    // Duplicated processing avoid
    if(interAnalysisDoneSet.find(cs) != interAnalysisDoneSet.end()) return;
    interAnalysisDoneSet.insert(cs);

    // cr init
    crfunc = cs->getParent()->getParent();
    craddr = retFuncAddr(crfunc);
    if(!(crsz = retFuncSz(craddr))){
        uint64_t startaddr = craddr;
        uint64_t endaddr = inferMissingEndAddr(crfunc);
        string exemode = inferMissingExemode(crfunc);
        binFuncRangeUpdate(startaddr, endaddr, exemode);
        crsz = retFuncSz(craddr);
    }
    crexe = retFuncMode(craddr);

    assert(craddr);
    assert(crsz);

    // cs init 
    if(inst2instaddr.find(cs) == inst2instaddr.end()){
        binIRInit(crfunc);
        if(inst2instaddr.find(cs) == inst2instaddr.end()) assert(0);
    }
    csaddr = inst2instaddr[cs];
    assert(csaddr);

    // ce init
    cefunc = getCallee(cs);

    if(!cefunc) return; // unsolved callee. So skip
    ceaddr = retFuncAddr(cefunc);
    if(!(cesz = retFuncSz(ceaddr))){
        uint64_t startaddr = ceaddr;
        uint64_t endaddr = inferMissingEndAddr(cefunc);
        string exemode = inferMissingExemode(cefunc);
        binFuncRangeUpdate(startaddr, endaddr, exemode);
        cesz = retFuncSz(ceaddr);
    }
    ceexe = retFuncMode(ceaddr);
    if(ceexe.length() == 0) ceexe = "THUMB";
    assert(ceaddr);
    assert(cesz);

    // Callee Anaylsis
    //  NOTE: callee must be first
    calleeAnalysis(ceaddr, cesz, ceexe);

    // Caller Analysis
    callsiteAnalysis(ceaddr, csaddr, craddr, crsz, crexe);

    // Value mapping
    mapInterPass(csaddr, ceaddr, craddr);

    return;
}


