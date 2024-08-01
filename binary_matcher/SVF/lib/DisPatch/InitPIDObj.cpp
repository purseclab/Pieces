#include "DisPatch/InitPIDObj.h"

// Step 1: Intepreter 
void InitPIDObj::interpreter(){
    if(config != "NULL"){
        ifstream fs;
        string word;
        string delim = "##";
        string subdelim = "~~";
        string subsubdelim = "_";
        string pidty = "";
        string funcname = "";
        string cur = "";
        string ref = "";
        string err = "";
        string out = "";
        string p_p = "";
        string p_i = "";
        string p_d = "";
        string p_ff = "";
        string gt = "";
        string mode = "";
        string funcdef = "";
        bool skip = false;
        unsigned controlidx = 0;

        fs.open(config, fstream::in);

        for(; fs >> word; ){
            // framework selector
            if(word.find("/*") == 0){
                skip = true;
                continue;
            }
            else if(word.find("*/") == 0){
                skip = false;
                continue;
            }
            else if(skip) {
                continue;
            }
            else if(word.find("//") == 0) continue;
            else if(word.find("ARCH##") == 0){
                size_t pos = word.find(delim);
                if(pos != std::string::npos){
                    arch = word.substr(pos + delim.length(), word.length() - (pos + delim.length()));
                }
                else assert(0);
            }
            else if(word.find("BIN##") == 0){
                size_t pos = word.find(delim);
                if(pos != std::string::npos){
                    binname = word.substr(pos + delim.length(), word.length() - (pos + delim.length()));
                }
                else assert(0);
            }
            else if(word.find("BASEOFF##") == 0){
                size_t pos = word.find(delim);
                if(pos != std::string::npos){
                    string baseoffstr = word.substr(pos + delim.length(), word.length() - (pos + delim.length()));
                    baseoff = stoull(baseoffstr, nullptr, 16);
                }
                else assert(0);
            }
            else if(word.find("BASEADDR##") == 0){
                size_t pos = word.find(delim);
                if(pos != std::string::npos){
                    string baseaddrstr = word.substr(pos + delim.length(), word.length() - (pos + delim.length()));
                    baseaddr = stoull(baseaddrstr, nullptr, 16);
                }
                else assert(0);
            }
            else if(word.find("FUNCRANGE##") == 0){
                size_t pos = word.find(delim);
                if(pos != std::string::npos){
                    pos = word.find(subdelim);      // Want to skip function name
                    word = word.substr(pos + subdelim.length(), word.length() - (pos + subdelim.length()));

                    pos = word.find(subdelim);
                    string startaddrstr = word.substr(0, pos);
                    word = word.substr(pos + subdelim.length(), word.length() - (pos + subdelim.length()));
                    uint64_t startaddr = stoull(startaddrstr, nullptr, 16);

                    pos = word.find(subdelim);
                    string endaddrstr = word.substr(0, pos);
                    word = word.substr(pos + subdelim.length(), word.length() - (pos + subdelim.length()));
                    uint64_t endaddr = stoull(endaddrstr, nullptr, 16);

                    string exemode;
                    if(word.length()) exemode = word;
                    else exemode = "";

                    binFuncRangeUpdate(startaddr, endaddr, exemode);
                }
                else assert(0);
            }
            // semantic
            else if(word.find("PID_SEMANTIC##") == 0){
                size_t pos = word.find(delim);
                word = word.substr(pos + delim.length(), word.length() - (pos + delim.length()));
                pos = word.find(subdelim);
                string role = word.substr(0, pos);
                string pids = word.substr(pos + subdelim.length(), word.length() - (pos + subdelim.length()));

                pidInfoMap[role] = AxisInfo(role, pids);
            }
            else if(word.find("CTRLFUNC##") == 0 || word.find("MATHFUNC##") == 0){
                size_t pos = word.find(delim);
                word = word.substr(pos + delim.length(), word.length() - (pos + delim.length()));
                pos = word.find(subdelim);
                string ctrlfunc = word.substr(0, pos);
                string role = word.substr(pos + subdelim.length(), word.length() - (pos + subdelim.length()));

                const Function *tf = module.getFunction(ctrlfunc);
                assert(tf);
                semanticFuncSetMap[role].insert(tf);
            }
            // Control var handler
            else if(word.find("$$") == 0){
                if(pidty != "" && funcname != "" && err != ""){  // Minimally, err should exist
                    const Function *func = module.getFunction(funcname);
                    if(!func){
                        const Function *closest = NULL;
                        size_t tpos = funcname.find("function_");
                        string tfuncname = funcname.substr(tpos + strlen("function_"));
                        int64_t taddr = stoll(tfuncname, nullptr, 16); 
                        int64_t mindiff = 0xFFFFFFFF;
                        for(auto it = module.begin(); it != module.end(); ++it){
                            const Function *cfunc = *it;
                            string cfuncname = cfunc->getName().str();
                            size_t cpos = cfuncname.find("function_");

                            if(cpos == string::npos) continue;

                            cfuncname = cfuncname.substr(cpos  + strlen("function_"));
                            
                            if(cfuncname.length() >= 16) continue;  // LLVM lifter bug handler

                            int64_t caddr = stoll(cfuncname, nullptr, 16); 
                            if(mindiff > abs(taddr - caddr)){
                                mindiff = abs(taddr - caddr);
                                closest = cfunc;
                            }
                        }

                        if(!closest){
                            cout << "[DEBUG]: function (" << funcname << ") does not exist" << endl;
                            assert(0);
                        }
                        func = closest;
                    }
                    else if(pidty == "") assert(0);

                    CtrlInfo ctrl_info;

                    ctrl_info.func = func;
                    ctrl_info.cur = cur;
                    ctrl_info.ref = ref;
                    ctrl_info.err = err;
                    ctrl_info.out = out;

                    ctrl_info.p = p_p;
                    ctrl_info.i = p_i;
                    ctrl_info.d = p_d;
                    ctrl_info.ff = p_ff;

                    ctrl_info.gt = gt;
                    ctrl_info.pidty = pidty;
                    ctrl_info.controlidx = controlidx;

                    ctrlVec.push_back(ctrl_info);

                    controlidx++;
                }
                pidty = "";
                funcname = "";
                cur = "";
                ref = "";
                err = "";
                out = "";

                p_p = "";
                p_i = "";
                p_d = "";
                p_ff = "";

                gt = "";
                mode = "";
            }
            else if(word.find("PIDType##") == 0){
                size_t pos = word.find(delim);
                if(pos != std::string::npos){
                    string key = word.substr(0, pos);
                    pidty = word.substr(pos + delim.length(), word.length() - (pos + delim.length()));
                }
                else assert(0);
            }
            else if(word.find("FUNC##") == 0){
                size_t pos = word.find(delim);
                if(pos != std::string::npos){
                    string key = word.substr(0, pos);
                    funcname = word.substr(pos + delim.length(), word.length() - (pos + delim.length()));
                }
                else assert(0);
            }
            else if(word.find("GROUNDTRUTH##") == 0){
                size_t pos = word.find(delim);
                if(pos != std::string::npos){
                    string key = word.substr(0, pos);
                    gt = word.substr(pos + delim.length(), word.length() - (pos + delim.length()));
                }
                else assert(0);
            }
            else if(word.find("cur##") == 0 || word.find("ref##") == 0 || word.find("err##") == 0 || word.find("out##") == 0
                 || word.find("p##") == 0 || word.find("i##") == 0 || word.find("d##") == 0 || word.find("ff##") == 0){
                size_t pos = word.find(delim);
                if(pos != std::string::npos){
                    string key = word.substr(0, pos);
                    if(word.find("cur##") == 0){
                        cur = word.substr(pos + delim.length(), word.length() - (pos + delim.length()));
                    }
                    else if(word.find("ref##") == 0){
                        ref = word.substr(pos + delim.length(), word.length() - (pos + delim.length()));
                    }
                    else if(word.find("err##") == 0){
                        err = word.substr(pos + delim.length(), word.length() - (pos + delim.length()));
                    }
                    else if(word.find("out##") == 0){
                        out = word.substr(pos + delim.length(), word.length() - (pos + delim.length()));
                    }
                    else if(word.find("p##") == 0){
                        p_p = word.substr(pos + delim.length(), word.length() - (pos + delim.length()));
                    }
                    else if(word.find("i##") == 0){
                        p_i = word.substr(pos + delim.length(), word.length() - (pos + delim.length()));
                    }
                    else if(word.find("d##") == 0){
                        p_d = word.substr(pos + delim.length(), word.length() - (pos + delim.length()));
                    }
                    else if(word.find("ff##") == 0){
                        p_ff = word.substr(pos + delim.length(), word.length() - (pos + delim.length()));
                    }
                    else {
                        cout << "Word: " << word << endl;
                        assert(0);
                    }
                }
                else assert(0);
            }
        }
        fs.close();
    }
    else assert(0);

    // Func add csce
    // Points-to analysis's problem compensation. This can be improved through the combination with some binary analyzer (e.g., angr)
    uint64_t csaddr;
    uint64_t cefunc;
    csaddr = 0x8059a44; //copter        //function_80599ec  // _ZN24AC_AttitudeControl_Multi19rate_controller_runEv
    cefunc = 0x8059bd8; //    _ZN18AC_AttitudeControl24rate_target_to_motor_yawEff
    mapBinIndcs2ce(csaddr, cefunc);

    csaddr = 0x8057014; //heli          //function_8056f74 // _ZN23AC_AttitudeControl_Heli19rate_controller_runEv
    cefunc = 0x80568d8;
    mapBinIndcs2ce(csaddr, cefunc);

    csaddr = 0x80454ec; //sub           //function_8045494 // 
    cefunc = 0x8045b04;    //function_8045b02 / 0x8045b04
    mapBinIndcs2ce(csaddr, cefunc);

    csaddr = 0x805b518; //plane          //function_805b4c0 // 
    cefunc = 0x805b6f4;
    mapBinIndcs2ce(csaddr, cefunc);

    return ;
}

// Step 2: updateAllCtrlVars
void InitPIDObj::updateAllCtrlVars(InterProcAnal *inipa){
    if(!ctrlVec.size()) assert(0);
    else if(!inipa) assert(0);

    ipa = inipa;
    for(int i = 0; i < ctrlVec.size(); i++){
        reflectInputConfig(ctrlVec[i].func, &(ctrlVec[i]), ctrlVec[i].cur, ctrlVec[i].ref, ctrlVec[i].err, ctrlVec[i].out, ctrlVec[i].gt, ctrlVec[i].pidty, ctrlVec[i].controlidx);
        if(ctrlVec[i].gt.length())
            cout << "\t\t============= End of GT: " << ctrlVec[i].gt  << "\n" << endl;
    }
    return;
}


int InitPIDObj::CtrlVar2Idx(const Function *func, string word){
    if(word == "-") return -1;
    binIRInit(func);

    string delim = "~~";

    int idx = 0;
    bool found = false;
    size_t pos = word.find(delim);

    if(pos != string::npos){
        string addrstr = word.substr(0, pos);

        string type = word.substr(pos + delim.length(), word.length() - (pos + delim.length()));
        if(type == "arg"){
            uint16_t ri = ipa->str2reg(addrstr);
            idx = ri * -1;
            found = true;
        }
    }
    else{
        string addrstr = word;
        boost::to_lower(addrstr);
        uint64_t addr = stoull(addrstr, nullptr, 16);

        if(instaddr2instvec.find(addr) != instaddr2instvec.end()){
            vector<const Instruction *> instvec = instaddr2instvec[addr];

            // inttoptrinst // highest
            for(int i = 0; i < instvec.size(); i++){
                const Instruction *inst = instvec[i];
                if(SVFUtil::dyn_cast<IntToPtrInst>(inst)){
                    found = true;
                    idx = inst2idx(inst);
                    break;               
                }
            }


            // load  // next highest
            if(!found){
                for(int i = 0; i < instvec.size(); i++){
                    const Instruction *inst = instvec[i];
                    if(SVFUtil::dyn_cast<LoadInst>(inst)){
                        const LoadInst *li = SVFUtil::dyn_cast<LoadInst>(inst);
                        const Instruction *pinst = SVFUtil::dyn_cast<Instruction>(li->getPointerOperand());
                        if(pinst){
                            idx = inst2idx(pinst);
                            found = true;
                            break;
                        }
                    }
                }
            }

            // binop // next highest
            if(!found){
                for(int i = 0; i < instvec.size(); i++){
                    const Instruction *inst = instvec[i];
                    if(SVFUtil::dyn_cast<BinaryOperator>(inst) != NULL || isBinOpInst(inst)){
                        found = true;
                        idx = inst2idx(inst);
                        break;
                    }
                }
            }

            // vmov // next highest
            if(!found){
                for(int i = 0; i < instvec.size(); i++){
                    const Instruction *inst = instvec[i];
                    if(const CallInst *ci = SVFUtil::dyn_cast<CallInst>(inst)){
                        string inst_str = retAsmStr(ci);
                        if(inst_str == "vmov"){
                            found = true;
                            idx = inst2idx(inst);
                            break;
                        }
                    }
                }
            }

            // phinode  // next highest
            if(!found){
                for(int i = 0; i < instvec.size(); i++){
                    const Instruction *inst = instvec[i];
                    if(SVFUtil::dyn_cast<PHINode>(inst)){
                        found = true;
                        idx = inst2idx(inst);
                        break;               
                    }
                }
            }

        }
    }

    return idx;
}


vector <const Value *> InitPIDObj::updateCtrlVars(string word, const Function *func, unsigned controlidx, string kind){
    vector <const Value *> ret;
    vector <int> keyvec;
    vector <int> wordvec;

    // not identifed variable
    ret.clear();
    if(word == "") return ret;

    cout << "\n\n\t word: " << word << " for func: " << func->getName().str() << endl;

    // Data preprocessing
    while(1){
        int num;
        if(word.length() == 0) assert(0);
        else if(word.find(",") != string::npos){
            size_t poscmt = word.find("//");    // comment removal
            if(poscmt != string::npos){
                word = word.substr(0, poscmt);
            }

            string subword = word.substr(0, word.find(","));
            word.erase(0, word.find(",") + 1);

            num = CtrlVar2Idx(func, subword);
            if(word != "-") assert(num != -1);          // Need to fix error return value if it causes assert (only in this func)
            keyvec.push_back(num);

            boost::to_lower(subword);
            if(subword.find("~") == string::npos && subword.find("-") == string::npos){   // -: nothing / ~: related reg
                uint64_t addr = stoull(subword, nullptr, 16);
                wordvec.push_back(addr);
            }
            else wordvec.push_back(0);
        }
        else{
            size_t poscmt = word.find("//");        // comment removal
            if(poscmt != string::npos){
                word = word.substr(0, poscmt);
            }

            num = CtrlVar2Idx(func, word);
            if(word != "-") assert(num != -1);          // Need to fix error return value if it causes assert (only in this func)
            keyvec.push_back(num);

            boost::to_lower(word);
            if(word.find("~") == string::npos && word.find("-") == string::npos){   // -: nothing / ~: related reg
                uint64_t addr = stoull(word, nullptr, 16);
                wordvec.push_back(addr);
            }
            else wordvec.push_back(0);
            break;
        }
    }

    // Data update
    for(int i = 0; i < keyvec.size(); i++){
        const Value *v, *ptr;
        cout << " \t\tkeyvec: " << keyvec[i] << endl;
        cout << " \t\twordvec: " << std::hex << wordvec[i] << std::dec << endl;
        if(keyvec[i] > 0){
            v = idx2inst(func, keyvec[i]);
            cout << " \t\t init v: " << inst2str(v) << endl;
            cout << " \t\t func: " << v2func(v)->getName().str() << endl;
            // inst to vec

        }

        if(keyvec[i] != -1){
            if(keyvec[i] < 0){
                uint16_t ri = keyvec[i] * -1;
                set <const CallInst *> csset = retCallSiteSet(func, module);
                if(!csset.size()){
                    cout << "[DEBUG] function targeted by retCallSiteSet: " << func->getName().str() << endl;
                    assert(csset.size());
                }
                ipa->interAnalysis(*(csset.begin()));      // Only one at this time since I am interested only in ce

                v = ipa->retFunc2ArgCEV(func, ri);
                ptr = v;

                if(v == NULL || SVFUtil::dyn_cast<Instruction>(v) == NULL){
                    v = ipa->retFunc2HeuriAggressiveArgCEV(func, *(csset.begin()), ri); // Register Argument Exception handler
                    ptr = v;
                    if(v == NULL || SVFUtil::dyn_cast<Instruction>(v) == NULL){
                        ret.push_back(NULL);
                        keyvec[i] = -1;
                        cout << "\n[INFO]: init Empty: " << endl;
                        cout << "\tKeyvec: " << keyvec[i] << endl;
                        cout << "\tKIND: " << kind << endl;
                        cout << "\t > NULL" << endl;
                        continue;
                    }
                }
            }
            else{
                v = idx2inst(func, keyvec[i]);
                if(kind == "err"){
                    ptr = initBackwardDstPoint(v, maxdist, func);   // Return = getPtr or ptrcast (from the this + offset ptr)
                }
                else if(kind == "out"){ // Mostly return as a return value of PID controller
                    ptr = v;
                }
                else{
                    ptr = initForwardSrcPoint(v, maxdist, func);    // Return = getPtr or ptrcast (from the this + offset ptr)
                }
            }
            ret.push_back(ptr);
            varmapset[kind].insert(ptr);
            ctrlidx2varset[controlidx].insert(ptr);
            var2ctrlidx[ptr] = controlidx;
            if(wordvec[i] != 0)
                initcv2cvaddr[ptr] = wordvec[i];

            if(keyvec[i] < 0)   // reg
                cout << "\tKeyvec: " << ipa->reg2str(keyvec[i] * -1) << endl;
            else
                cout << "\tKeyvec: " << keyvec[i] << endl;
            cout << "\tKIND: " << kind << endl;
            cout << "\t > v: " << inst2str(v) << " at idx: " << inst2idx(func, v) <<  " at func: " << func2name(func) << endl;
            cout << "\t > ptr: " << inst2str(ptr) << " at idx: " << inst2idx(func, ptr) << " at func: " <<  func2name(func)  << endl;
        }
        else{
            ret.push_back(NULL);
            cout << "\n[INFO]: init Empty: " << endl;
            cout << "\tKeyvec: " << keyvec[i] << endl;
            cout << "\tKIND: " << kind << endl;
            cout << "\t > NULL" << endl;
        }
    }

#if 0
    cout << " ret ---------" << endl;
    for(int i = 0; i < ret.size(); i++){
        cout << "\t\t " << inst2str(ret[i]) << endl;
    }
#endif
    return ret;
}

//os : out
void InitPIDObj::reflectInputConfig(const Function *func, CtrlInfo *ci, string cs, string rs, string es, string os, string gt, string pidty, unsigned controlidx){
    if(gt.length())
        cout << "\t\t============= Start of GT: " << gt  << " w/ func: " << func << endl;

    vector <const Value *> curvec = updateCtrlVars(cs, func, controlidx, "cur");
    vector <const Value *> refvec = updateCtrlVars(rs, func, controlidx, "ref");
    vector <const Value *> errvec = updateCtrlVars(es, func, controlidx, "err");
    vector <const Value *> outvec = updateCtrlVars(os, func, controlidx, "out");
    map<string, set <const Value *>> cvmap;

    if(curvec.size() != refvec.size() || curvec.size() != errvec.size() || refvec.size() != errvec.size()){
        cout << "[DEBUG]: " << gt << endl;
        cout << "  >> cursz: " << curvec.size() << endl;
        cout << "  >> refsz: " << refvec.size() << endl;
        cout << "  >> errsz: " << errvec.size() << endl;
        assert(0);
    }

    // ctrl map update // out maynot be done here
    for(unsigned i = 0; i < curvec.size(); i++){
        if(curvec[i] != NULL){
            cvmap["cur"].insert(curvec[i]);
            if(refvec.size() > 0 && refvec[i] != NULL)
                ctrlvarset[curvec[i]].insert(refvec[i]);
            if(errvec.size() > 0 && errvec[i] != NULL)   
                ctrlvarset[curvec[i]].insert(errvec[i]);
        }
    }

    for(unsigned i = 0; i < refvec.size(); i++){
        if(refvec[i] != NULL){
            cvmap["ref"].insert(refvec[i]);
            if(curvec.size() > 0 && curvec[i] != NULL)
                ctrlvarset[refvec[i]].insert(curvec[i]);
            if(errvec.size() > 0 && errvec[i] != NULL)   
                ctrlvarset[refvec[i]].insert(errvec[i]);
        }
    }

    for(unsigned i = 0; i < errvec.size(); i++){
        if(errvec[i] != NULL){
            cvmap["err"].insert(errvec[i]);
            if(curvec.size() > 0 && curvec[i] != NULL)
                ctrlvarset[errvec[i]].insert(curvec[i]);
            if(refvec.size() > 0 && refvec[i] != NULL)   
                ctrlvarset[errvec[i]].insert(refvec[i]);
        }
    }

    for(unsigned i = 0; i < outvec.size(); i++){
        if(outvec[i] != NULL){
            cvmap["out"].insert(outvec[i]);
        }
    }

    // Grountruth update only if there is for verification purpose
    for(unsigned i = 0; i < curvec.size(); i++){
        if(curvec[i] != NULL){
            if(gt.length() != 0 && gt != "-") v2GroundTruthController[curvec[i]] = gt;
            v2ctrlty[curvec[i]] = pidty;
            v2ctrlinfo[curvec[i]] = ci;
        }
    }
    for(unsigned i = 0; i < refvec.size(); i++){
        if(refvec[i] != NULL){
            if(gt.length() != 0 && gt != "-") v2GroundTruthController[refvec[i]] = gt;
            v2ctrlty[refvec[i]] = pidty;
            v2ctrlinfo[refvec[i]] = ci;
        }
    }
    for(unsigned i = 0; i < errvec.size(); i++){
        if(errvec[i] != NULL){
            if(gt.length() != 0 && gt != "-") v2GroundTruthController[errvec[i]] = gt;
            v2ctrlty[errvec[i]] = pidty;
            v2ctrlinfo[errvec[i]] = ci;
        }
    }
    for(unsigned i = 0; i < outvec.size(); i++){
        if(outvec[i] != NULL){
            if(gt.length() != 0 && gt != "-") v2GroundTruthController[outvec[i]] = gt;
            v2ctrlty[outvec[i]] = pidty;
            v2ctrlinfo[outvec[i]] = ci;
        }
    }

    if(cvmap.size())
        ctrlmapvec.push_back(cvmap);

    return;
}

//////////////////////////// Forward Recursive
const Value* InitPIDObj::initForwardSrcPoint(const Value *sv, int dist, const Function *func){
    cout << "\n[INFO]: initForwardSrcPoint: " << inst2str(sv) << " at func: " << func2name(func) << endl;
    const Value *ret = NULL;
    if(isGepInst(sv)) return sv;
    else if(isVldrInst(sv) || isVstrInst(sv)){
        const Value *ssv = retPtrPtr(sv);
        if(isLoadInst(ssv) || isStoreInst(ssv)){
            const Value *ptr = retPtrPtr(ssv);
            if(isGepInst(ptr)){
                return ptr;
            }
        }
    }

    if(isLoadInst(sv) || isStoreInst(sv)){
        const Value *ptr = retPtrPtr(sv);
        if(isGepInst(ptr)){
            return ptr;
        }
    }
    ret = recursiveForwardSrcPoint(sv, dist, func);
    if(ret) return ret;
    else return sv; //Not found, use it as variable
}

const Value* InitPIDObj::recursiveArgForward(const Argument *av, int dist, const Function *func){
    const Value *ret = NULL;
    set <const Instruction *> instset = getCalleeCSset(func);

    for(auto it = instset.begin(); it != instset.end(); ++it){
        const Instruction *callerinst = *it;
        const Function *caller = callerinst->getParent()->getParent();
        if(caller == func) continue;    //recursive prevention

//        cout << "\t>> Cur Func: " << func2name(func) << " -> Caller: " << func2name(caller) << endl;
        ret = recursiveForwardSrcPoint(callerinst->getOperand(arg2idx(func, av)), dist, caller);
        if(ret) return ret;
    }
    return NULL;    //not matched
}

const Value* InitPIDObj::recursiveForwardSrcPoint(const Value *sv, int dist, const Function *func){
    cout << "\t\t recursive sv: " << inst2str(sv) << endl;
    if(SVFUtil::dyn_cast<Function>(sv) || SVFUtil::dyn_cast<Constant>(sv) || SVFUtil::dyn_cast<GlobalVariable>(sv)) return NULL;
    else if(const Argument *arg = SVFUtil::dyn_cast<Argument>(sv)){
        return recursiveArgForward(arg, dist, func);
    }
    else if(SVFUtil::dyn_cast<Instruction>(sv) == NULL){
        cout << "[Error]: Unhandled pattern, GV? then add related logics: " << inst2str(sv) << endl;
        assert(0);
    }
    else if(isGepInst(sv)) return sv;
    //else if(isPhiInst(sv)){  // Can be improved by handling all phi nodes
    else if(isPtrCastInst(sv)){
        if(isBinOpInst(sv)){
            if(retBinOpTy(sv) == Instruction::Add){
                const Instruction *inst = SVFUtil::dyn_cast<Instruction>(sv);
                const Value *constv = inst->getOperand(1);
                if(const ConstantInt *ci = SVFUtil::dyn_cast<ConstantInt>(constv)){
                    if(ci->getZExtValue() >= 8) return sv;
                }
            }
        }
    }
    else{
        const Value *ret = NULL;
        int maxidx, minidx;
        int newdist;
        if(inst2idx(sv) <= 0) return NULL;

        maxidx = inst2idx(sv);  //start from one previous inst
        if(maxidx > dist) minidx = maxidx - dist;
        else minidx = 0;


        for(int i = maxidx; i >= minidx; i--){
            const Instruction *inst = idx2inst(func, i);
            if(isStoreInst(inst)){
                const Value *spv = retPtrVal(inst);
                newdist = dist - (maxidx - i);
                if(inst2idx(spv) >= inst2idx(inst)) continue;  //prevent wrong looping
                ret = recursiveForwardSrcPoint(spv, newdist, func); //not si as an arg
                if(ret) return ret;
            }
            else if(isLoadInst(inst) && inst == sv){
                return retPtrVal(inst);
            }
            else if(isBinOpInst(inst) || isCastInst(inst) || isCallInst(inst) || 
                    (SVFUtil::dyn_cast<ExtractElementInst>(inst) != NULL)){
                for(unsigned ii = 0; ii < inst->getNumOperands(); ii++){
                    newdist = dist - (maxidx - i);
                    if(const Argument *arg = SVFUtil::dyn_cast<Argument>(inst->getOperand(ii))){
                        // don't need to check arg
                        ret = recursiveArgForward(arg, newdist, func);
                        if(ret) return ret;
                    }
                    else if(SVFUtil::dyn_cast<Instruction>(inst->getOperand(ii))){
                        if(inst2idx(inst->getOperand(ii)) >= inst2idx(inst)) continue;  //prevent wrong looping
                        ret = recursiveForwardSrcPoint(inst->getOperand(ii), newdist, func); //not si as an arg
                        if(ret) return ret;
                    }
                }
            }
        }
    }

    return NULL;
}


//////////////////////////// Backward recursive
const Value* InitPIDObj::initBackwardDstPoint(const Value *sv, int dist, const Function *func){
    cout << "\n[INFO]: initBackwardDstPoint: " << inst2str(sv) << " at func: " << func2name(func) << endl;
    const Value *ret = NULL;
    if(isGepInst(sv)) return sv;
    else if(isVldrInst(sv) || isVstrInst(sv)){
        const Value *ssv = retPtrPtr(sv);
        if(isLoadInst(ssv) || isStoreInst(ssv)){
            const Value *ptr = retPtrPtr(ssv);
            if(isGepInst(ptr)){
                return ptr;
            }
        }
    }

    if(isLoadInst(sv) || isStoreInst(sv)){
        const Value *ptr = retPtrPtr(sv);
        if(isGepInst(ptr)){
            return ptr;
        }
    }

    ret = recursiveBackwardDstPoint(sv, dist, func);

    if(ret != NULL && ret != sv) return ret;
    else{
        ret = recursiveBackwardArgCopyFinder(sv, func); 
        if(ret) return ret;
        else return sv; //Not found, use it as variable
    }
}


const Value* InitPIDObj::recursiveBackwardArgCopyFinder(const Value *sv, const Function *func){   
    const Value *ret = NULL;
    if(isHeuriArg(sv)){
        return sv;
    }
    else if(isVmovInst(sv)){
        if(const Instruction *inst = SVFUtil::dyn_cast<Instruction>(sv)){
            ret = recursiveBackwardArgCopyFinder(inst->getOperand(0), func);
            return ret;
        }
    }
    return ret;
}

const Value* InitPIDObj::recursiveRetBackward(int dist, const Function *func){
    const Value *ret = NULL;
    set <const Instruction *> instset = getCalleeCSset(func);

    for(auto it = instset.begin(); it != instset.end(); ++it){
        const Instruction *callerinst = *it;
        const Function *caller = callerinst->getParent()->getParent();
        if(caller == func) continue;    //recursive prevention

        ret = recursiveBackwardDstPoint(callerinst, dist, caller);
        if(ret) return ret;
    }
    return NULL;    //not matched
}

const Value* InitPIDObj::recursiveBackwardDstPoint(const Value *sv, int dist, const Function *func){
    if(SVFUtil::dyn_cast<Function>(sv) || SVFUtil::dyn_cast<Constant>(sv) || SVFUtil::dyn_cast<GlobalVariable>(sv)) return NULL;
    else if(SVFUtil::dyn_cast<Instruction>(sv) == NULL){
        cout << "[Error]: Unhandled pattern, GV? then add related logics: " << inst2str(sv) << endl;
        assert(0);
    }
    else if(isGepInst(sv)) return sv;
    else{
        const Value *ret = NULL;
        int maxidx, minidx;
        int newdist;
        if(inst2idx(sv) <= 0) return NULL;

        minidx = inst2idx(sv);  //start from one previous inst
        maxidx = func2sz(func);
        if(maxidx > (dist + minidx)) maxidx = dist + minidx;

        for(int i = 0; i < maxidx; i++){
            const Instruction *inst = idx2inst(func, i);
            if(const ReturnInst *ri = SVFUtil::dyn_cast<ReturnInst>(inst)){
                if(i == func2sz(func) - 1){
                    newdist = dist - (i - minidx);
                    if(ri->getReturnValue() == sv) return recursiveRetBackward(newdist, func);
                    else return NULL;
                }
            }
            else if(isStoreInst(inst)){
                const Value *spv = retPtrVal(inst);
                if(spv == sv) return spv;
            }
            else if(isBinOpInst(inst) || isCastInst(inst) || isCallInst(inst) || 
                    (SVFUtil::dyn_cast<ExtractElementInst>(inst) != NULL)){
                bool matched = false;
                for(unsigned ii = 0; ii < inst->getNumOperands(); ii++){
                    if(inst->getOperand(ii) == sv){
                        matched = true;
                        break;
                    }
                }
                if(matched){
                    newdist = dist - (i - minidx);
                    ret = recursiveBackwardDstPoint(inst, newdist, func); //not si as an arg
                    if(ret) return ret;
                }
            }
        }
    }

    return NULL;
}




// Util
set <const Instruction *> InitPIDObj::getCalleeCSset(const Function *callee){

    if(callee2csset.find(callee) == callee2csset.end()){
        for(auto it = module.begin(); it != module.end(); ++it){
            const Function *func = *it;
            for(auto iit = inst_begin(func); iit != inst_end(func); ++iit){
                const Instruction *inst = &*iit;
                if(const CallInst *ci = SVFUtil::dyn_cast<CallInst>(inst)){
                    if(const Function *ce = getCallee(ci)){
                        callee2csset[ce].insert(ci);
                    }
                }
            }
        }
    }
    return callee2csset[callee];
}

