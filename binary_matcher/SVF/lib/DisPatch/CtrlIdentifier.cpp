#include "DisPatch/CtrlIdentifier.h"

void CtrlIdentifier::cimain(){
    cout << "\n\n\n\t >>> Step1: Preprocessing\n" << endl;
    innerVarDependencyTagging();

    cout << "\n\n\n\t >>> Step2: Inner Function Role Identification\n" << endl;
    roleInnerFuncIdentification();

    cout << "\n\n\n\t >>> Step3: Unconnected Dependency Ref Input Alias\n" << endl;
    unconnectedRefAliasDetect();

    //NOTE unconnectedRefAliasDetect must be first
    cout << "\n\n\n\t >>> Step4: Cascading Transition Identification\n" << endl;
    cascadingTransition();

    cout << "\n\n\n\t >>> Step5: Inter Function Role Identification\n" << endl;
    roleInterFuncIdentification();

    cout << "\n\n\n\t >>> Step6: Role Confirmation\n" << endl;
    roleConfirm();
    return;
}


///////////////////////////////////////////////////////////////////
// innerVarDependencyTagging  phase
void CtrlIdentifier::innerVarDependencyTagging(){

    // Note: All varmapset's value must be instructions.
    for(unsigned i = 0 ; i < ctrlKeyVec.size(); i++){
        string k = ctrlKeyVec[i];   // cur, ref, err 
        set <const Value *> varset = pid->retCVset(k);

        for(auto vit = varset.begin(); vit != varset.end(); ++vit){     // NOTE: they are ptrs
            const Value *v = *vit;
            if(const Instruction *vi = SVFUtil::dyn_cast<Instruction>(v)){
                const Function *vfunc = vi->getParent()->getParent();

                var2FuncMap[v] = vfunc;
                rolemap[v] = k;
                func2TaggedvarsetMap[vfunc].insert(v);  // Variables per functions
            }
            else {
                cout << "[Error]: Var: " << inst2str(v) << " -> Is not convertiable to Instruction " << endl;
                assert(0);
            }
        }
    }


    pta = AndersenWaveDiff::createAndersenWaveDiff(module);
    AliasFinder *af = new AliasFinder(module, pta, pid->varmapset);
    af->afmain();

    // Num of controllers
    assert(pid->ctrlmapvec.size());
    for(int i = 0; i < pid->ctrlmapvec.size(); i++){
        map <string, set <const Value *>> cvmap = pid->ctrlmapvec[i];
        ctrlinfo *cobj = new ctrlinfo();
        ctrlset.insert(cobj);

        // ref 
        if(cvmap.find("ref") != cvmap.end()){
            set <const Value *> varset = cvmap["ref"];
            for(auto it = varset.begin(); it != varset.end(); ++it){
                const Value *v = *it;
                const Function *func = v2func(v);
                cobj->func = func;

                cobj->refs.insert(v);
                ref2ctrlmap[v] = cobj;
                ref2oriref[v] = v;

                if(func){
                    set <const Value *> vs = retIntraAliasGep(v, func);
                    for(auto it = vs.begin(); it != vs.end(); ++it){
                        const Value *av = *it;
                        ref2oriref[av] = v;
                    }
                }

                if(v && pid->v2ctrlinfo.find(v) != pid->v2ctrlinfo.end() && ctrlinfomap.find(cobj) == ctrlinfomap.end()){ 
                    //CtrlInfo    // pid->v2ctrlinfo[v];
                    ctrlinfomap[cobj] = pid->v2ctrlinfo[v];
                }

                    // ground truth if user provides (for verification)
                if(v != NULL && cobj->groundtruth.length() == 0 
                    && pid->v2GroundTruthController.find(v) != pid->v2GroundTruthController.end() 
                    && pid->v2GroundTruthController[v].length() > 0){
                    cobj->groundtruth = pid->v2GroundTruthController[v];
                }

                if(cobj->pidty.length() < 1 && pid->v2ctrlty.find(v) != pid->v2ctrlty.end()){
                    cobj->pidty  = pid->v2ctrlty[v];
                }
            }
        }

        // err
        if(cvmap.find("err") != cvmap.end()){
            set <const Value *> varset = cvmap["err"];
            for(auto it = varset.begin(); it != varset.end(); ++it){
                const Value *v = *it;
                cobj->errs.insert(v);
                err2ctrlmap[v] = cobj;

                if(!cobj->func){ 
                    const Function *func = v2func(v);
                    cobj->func = func;
                }

                if(v && pid->v2ctrlinfo.find(v) != pid->v2ctrlinfo.end() && ctrlinfomap.find(cobj) == ctrlinfomap.end()){ 
                    //CtrlInfo    // pid->v2ctrlinfo[v];
                    ctrlinfomap[cobj] = pid->v2ctrlinfo[v];
                }

                    // ground truth if user provides (for verification)
                if(v != NULL && cobj->groundtruth.length() == 0 && pid->v2GroundTruthController.find(v) != pid->v2GroundTruthController.end() && pid->v2GroundTruthController[v].length() > 0){
                    cobj->groundtruth = pid->v2GroundTruthController[v];
                }

                if(cobj->pidty.length() < 1 && pid->v2ctrlty.find(v) != pid->v2ctrlty.end()){
                    cobj->pidty  = pid->v2ctrlty[v];
                }
            }
        }

        // cur
        if(cvmap.find("cur") != cvmap.end()){
            set <const Value *> varset = cvmap["cur"];
            for(auto it = varset.begin(); it != varset.end(); ++it){
                const Value *v = *it;
                cobj->curs.insert(v);
                cur2ctrlmap[v] = cobj;

                if(!cobj->func){ 
                    const Function *func = v2func(v);
                    cobj->func = func;
                }

                if(v && pid->v2ctrlinfo.find(v) != pid->v2ctrlinfo.end() && ctrlinfomap.find(cobj) == ctrlinfomap.end()){ 
                    //CtrlInfo    // pid->v2ctrlinfo[v];
                    ctrlinfomap[cobj] = pid->v2ctrlinfo[v];
                }

                    // ground truth if user provides (for verification)
                if(v != NULL && cobj->groundtruth.length() == 0 && pid->v2GroundTruthController.find(v) != pid->v2GroundTruthController.end() && pid->v2GroundTruthController[v].length() > 0){
                    cobj->groundtruth = pid->v2GroundTruthController[v];
                }

                if(cobj->pidty.length() < 1 && pid->v2ctrlty.find(v) != pid->v2ctrlty.end()){
                    cobj->pidty  = pid->v2ctrlty[v];
                }
            }
        }

        // out
        if(cvmap.find("out") != cvmap.end()){
            set <const Value *> varset = cvmap["out"];
            for(auto it = varset.begin(); it != varset.end(); ++it){
                const Value *v = *it;
                cobj->mathouts.insert(v);
                mathout2ctrlmap[v] = cobj;

                if(!cobj->func){ 
                    const Function *func = v2func(v);
                    cobj->func = func;
                }

                if(v && pid->v2ctrlinfo.find(v) != pid->v2ctrlinfo.end() && ctrlinfomap.find(cobj) == ctrlinfomap.end()){ 
                    //CtrlInfo    // pid->v2ctrlinfo[v];
                    ctrlinfomap[cobj] = pid->v2ctrlinfo[v];
                }

                    // ground truth if user provides (for verification)
                if(v != NULL && cobj->groundtruth.length() == 0 && pid->v2GroundTruthController.find(v) != pid->v2GroundTruthController.end() && pid->v2GroundTruthController[v].length() > 0){
                    cobj->groundtruth = pid->v2GroundTruthController[v];
                }

                if(cobj->pidty.length() < 1 && pid->v2ctrlty.find(v) != pid->v2ctrlty.end()){
                    cobj->pidty  = pid->v2ctrlty[v];
                }
            }
        }
    }

    return;
}

///////////////////////////////////////////////////////////////////
// roleInnerFuncIdentification phase
void CtrlIdentifier::roleInnerFuncIdentification(){
    // Order based pairing
    for(auto it = func2TaggedvarsetMap.begin(); it != func2TaggedvarsetMap.end(); ++it){
        const Function *func = it->first;
        const Value *sv;
        const Value *dv;
        vector <const Value *> dvec;
        set <const Value *> srcset;

        if(!retTargetSrcVars(srcset, func)) continue; //src = err
 
        for(auto sit = srcset.begin(); sit != srcset.end(); ++sit){
            sv = *sit;
            unsigned si = inst2idx(func, sv);
            if(retTargetDstVars(sv, dvec, func)){   // dst = ref            // err's ctrl = parent , ref's ctrl = child
                for(int i = 0; i < dvec.size(); i++){
                    dv = dvec[i];
                    unsigned di = inst2idx(func, dv);

                    //if(si != 273 || di != 315) continue;

                    if(innerDepFromErr2Ref(sv, dv, si, di, func)){
                        cout << " > (Detected) Dep From: " << si << " to " << di << " in Func: " << func2name(func) << endl;
                        cout << "\tSrc: " << inst2str(sv) << " at " << inst2idx(func, sv) << endl;
                        cout << "\tDst: " << inst2str(dv) << " at " << inst2idx(func, dv) << "\n" <<  endl;

                        // To avoid Duplicated Reached Dst
                        set <const Value *> aliasset;
                        aliasset = retIntraAliasGep(sv, func);
                        for(auto ait = aliasset.begin(); ait != aliasset.end(); ++ait){
                            const Value *v = *ait;
                            checkedCVset.insert(v);
                        }
                        aliasset = retIntraAliasGep(dv, func);
                        for(auto ait = aliasset.begin(); ait != aliasset.end(); ++ait){
                            const Value *v = *ait;
                            checkedCVset.insert(v);
                        }

                        // Result update
                        err2refInnerConnMap[sv] = dv; 
                        ref2errInnerConnMap[dv] = sv;

                        // Last Idx update
                        if(func2lastCtrlVarIdx.find(func) == func2lastCtrlVarIdx.end()) 
                            func2lastCtrlVarIdx[func] = di;
                        else if(func2lastCtrlVarIdx[func] < di)
                            func2lastCtrlVarIdx[func] = di;
                        break;
                    }
                    else{
                        cout << " > (NOT Detected) Dep From: " << si << " to " << di << " in Func: " << func2name(func) << endl;
                        cout << "\tSrc: " << inst2str(sv) << " at " << inst2idx(func, sv) << endl;
                        cout << "\tDst: " << inst2str(dv) << " at " << inst2idx(func, dv) << "\n" <<  endl;
                    }
                }
            }
            else{
                cout << " > (No Valid Dst) Dep From: " << si << " in Func: " << func2name(func) << "\n" << endl;
            }
        }
    }

    // Intermediate Result Summary
    cout << "\n\n ---------------- Dep Summary (ori)--------------------- " << endl;
    for(auto it = err2refInnerConnMap.begin(); it != err2refInnerConnMap.end(); ++it){
        const Value *sv = it->first;
        const Value *dv = it->second;

        cout << "\nDEP" << endl;
        cout << "\tFrom idx: " << inst2idx(sv) << " = " << inst2str(sv) << endl;
        cout << "\tTo idx: " << inst2idx(dv) << " = " << inst2str(dv) << endl;
    }
    // Result updatea
    // - For connected ctrl
    //    map <const Value *, const Value *> out, in;    //to may not a stored variable. If that is necessary, find alias
    //    map <ctrlinfo *, ctrlinfo *> outConn, inConn
    //    set<const Value*> outs
    // - Use
    //    map <const Value *, const Value *> ref2oriref;
    //    map <const Value *, ctrlinfo *> ref2ctrlmap, err2ctrlmap;
    //    map <ctrlinfo *, bool> outConn, inConn;
    for(auto it = err2refInnerConnMap.begin(); it != err2refInnerConnMap.end(); ++it){
        const Value *sv = it->first;
        const Value *drv = it->second;
        if(ref2oriref.find(drv) != ref2oriref.end()){
            const Value *dv = ref2oriref[drv];

            if(err2ctrlmap.find(sv) == ref2ctrlmap.end()) assert(0);           
            if(ref2ctrlmap.find(dv) == ref2ctrlmap.end()) assert(0);           
            ctrlinfo *pred_ci = err2ctrlmap[sv];
            ctrlinfo *succ_ci = ref2ctrlmap[dv];

            // to var, controller
            pred_ci->out = drv;
            outConn[pred_ci] = succ_ci;
                //outs
            pred_ci->outs.insert(drv);

            // from var, controller
            succ_ci->in = sv;
            inConn[succ_ci] = pred_ci;
        }
    }

    cout << "\n\n ---------------- Intermediate Result Summary ---------------------\n" << endl;
    for(auto it = ctrlset.begin(); it != ctrlset.end(); ++it){
        ctrlinfo *ci = *it;
        cout << "==================================================================" << endl;
        for(auto mit = ci->errs.begin(); mit != ci->errs.end(); ++mit){
            const Value *v = *mit;
            const Function *curfunc = ci->func;
            cout << "\t[idx: " << inst2idx(curfunc, v) << "] Src: " << inst2str(v) << " / from func: " << func2name(curfunc) << endl;
        }
        
        if(inConn.find(ci) != inConn.end()){
            ctrlinfo *fci = inConn[ci];
            cout << "\t\tFROM Conn" << endl;
            for(auto pit = fci->refs.begin(); pit != fci->refs.end(); ++pit){
                const Value *v = *pit;
                const Function *curfunc = fci->func;
                cout << "\t\t\t[idx: " << inst2idx(curfunc, v) << "] Pred Ref: " << inst2str(v) << " / from func: " << func2name(curfunc) << endl;
            }
        }
        else cout << "\t\tFROM Conn: NULL" << endl;

        if(outConn.find(ci) != outConn.end()){
            ctrlinfo *tci = outConn[ci];
            cout << "\t\tTO Conn" << endl;
            for(auto sit = tci->refs.begin(); sit != tci->refs.end(); ++sit){
                const Value *v = *sit;
                const Function *curfunc = tci->func;
                cout << "\t\t\t[idx: " << inst2idx(curfunc, v) << "] Succ Ref: " << inst2str(v) << " / from func: " << func2name(curfunc) << endl;
            }
        }
        else cout << "\t\tTO Conn: NULL" << endl;
    }
    cout << endl;

    return;
}

bool CtrlIdentifier::retTargetSrcVars(set <const Value *> &srcset, const Function *func){
    // Note. Error = Src, must be the closest one from the controller
    if(func == NULL || func2TaggedvarsetMap.find(func) == func2TaggedvarsetMap.end()) return false;

    set <const Value*> valset = func2TaggedvarsetMap[func];
    srcset.clear();

    // Variable classification
    for(auto vit = valset.begin(); vit != valset.end(); ++vit){
        const Value *v = *vit;
        if(checkedCVset.find(v) != checkedCVset.end()) continue;
        else if((rolemap.find(v) != rolemap.end()) && (rolemap[v] == "err")){
            srcset.insert(v);
        }
    }

    if(srcset.size()) return true;
    else return false;
}



bool CtrlIdentifier::retTargetDstVars(const Value *sv, vector <const Value *> &dvec, const Function *func){
    // Note. Error = Src, must be the closest one from the controller
    if(func == NULL || func2TaggedvarsetMap.find(func) == func2TaggedvarsetMap.end()) return false;

    set <const Value*> valset = func2TaggedvarsetMap[func];
    set <const Value*> dstset, dset;
    unsigned si = inst2idx(func, sv);
    const Value *dv = NULL;
    dvec.clear();

    // Variable classification
    for(auto vit = valset.begin(); vit != valset.end(); ++vit){
        const Value *v = *vit;
        if(checkedCVset.find(v) != checkedCVset.end()) continue;
        else if((rolemap.find(v) != rolemap.end()) && (rolemap[v] == "ref")){
            if(checkedCVset.find(v) != checkedCVset.end()) continue;
            dstset.insert(v);
        }
    }
    
    // Dst Gathering
    for(auto dit = dstset.begin(); dit != dstset.end(); ++dit){
        const Value *v = *dit;
        const Value *idv = v;
        unsigned ididx = inst2idx(func, v);

        if(isForIdenticalController(sv, v) || ididx == 0) continue;

        dv = retClosestDst2Src(idv, ididx, si, func);
        if(dv){
            unsigned di = inst2idx(func, dv);
            if(si < di){        // filtering
                dset.insert(dv);
            }
        }
    }

    if(dset.size()){
        // Reordering
            // Map
        map <unsigned, const Value *> vmap;
        for(auto it = dset.begin(); it != dset.end(); ++it){
            const Value *v = *it;
            unsigned idx = inst2idx(func, v);
            vmap[idx] = v;
        }
            // Reorder
        for(auto it = vmap.begin(); it != vmap.end(); ++it){
            unsigned idx = it->first;   // not actually used since map is ordered
            const Value *v = it->second;
            dvec.push_back(v);
        }
        vmap.clear();
        return true;
    }
    else return false;
}



const Value * CtrlIdentifier::retClosestSrc2Dst(const Value *sv,  unsigned si, unsigned di, const Function *func){
    unsigned closestidx = si;
    const Value *ret = sv;

    set <const Value *> svset = retIntraAliasGep(sv, func);
    for(auto it = svset.begin(); it != svset.end(); ++it){
        const Value *v = *it;
        unsigned idx = inst2idx(func, v);

        if((idx < di && di <= closestidx) || (closestidx < idx && idx < di)){ 
            closestidx = idx;
            ret = v;
        }
    }

    return ret;
}

const Value * CtrlIdentifier::retClosestDst2Src(const Value *dv,  unsigned di, unsigned si, const Function *func){
    unsigned closestidx = di;
    const Value *ret = dv;

    set <const Value *> svset = retIntraAliasGep(dv, func);
    for(auto it = svset.begin(); it != svset.end(); ++it){
        const Value *v = *it;
        unsigned idx = inst2idx(func, v);

        if((idx > si && si >= closestidx) || (closestidx > idx && idx > si)){ 
            closestidx = idx;
            ret = v;
        }
    }

    return ret;
}

bool CtrlIdentifier::innerDepFromErr2Ref(const Value *sv, const Value *dv, unsigned si, unsigned di, const Function *func){
    unsigned maxidx;

    cout << "\t> Dep Check From: " << si << " to " << di << " in Func: " << func2name(func) << endl;
        // Dst first! to determine maximum bar
    cout << "\t  Dst: " << inst2str(dv) << " Idx: " << di << endl;
    unsigned oridi = di;
    di = initDstPoint(dv, di, func);
    dv = idx2inst(func, di);
    cout << "\t\t > Renewed Dst at '" << di << "' : " << inst2str(dv) << endl;

    cout << "\t  Src: " << inst2str(sv) << " Idx: " << si << endl;
    si = initSrcPoint(sv, si, di, func);
    sv = idx2inst(func, si);
    cout << "\t\t > Renewed Src at '" << si << "' : " << inst2str(sv) << endl;

    set <const Value *> svset = retIntraAliasGep(sv, func);

    // Inter analysis between src and sink
    batchInterAnalysis(si, oridi, func);   // Dst should be ori since Si will be loaded immediately. But, not for di which cause err

    maxidx = func2sz(func);

    cout << "\t> -> Adjusted Dep Analysis From: " << si << " to " << di << " in Func: " << func2name(func) << endl;
    if(recursiveReachabilityTrack(svset, dv, func, 0, maxidx)) return true;
    else return false;
}

unsigned CtrlIdentifier::initSrcPoint(const Value *sv, unsigned si, unsigned di, const Function *func){
    // SV's updated value (to be stored) 
    //      %z5 = getelementptr inbounds %class.AC_PosControl, %class.AC_PosControl* %this, i32 0, i32 29, i32 2
    //      store float %sub, float* %z5, align 4, !tbaa !4442
    // or loaded value (just load from sv)

    // Alias movement
    if(isGepInst(sv)){
        sv = closestAliasInst(sv, di, func);
        si = inst2idx(sv);
    }
    else if(isLoadInst(sv) || isBitCastInst(sv) || (SVFUtil::dyn_cast<ExtractElementInst>(sv) != NULL)){
        for(unsigned i = si; i < di; i++){
            const Instruction *inst = idx2inst(func, i);
            if(const ExtractElementInst *pi = SVFUtil::dyn_cast<ExtractElementInst>(inst)){
                if(pi->getVectorOperand() == sv){
                    cout << "At initSrcPoint, extract: " << inst2str(pi) << endl;
                    cout << "\t>> src var: " << inst2str(sv) << endl;
                    assert(0);
                }
            }
            else if(isLoadInst(inst)){
                if(retPtrPtr(inst) == sv){
                    si = inst2idx(sv);  //sv is same
                    break;
                }
            }
            else if(isBitCastInst(inst) || isPtrCastInst(inst)){
                if(inst->getOperand(0) == sv){
                    si = inst2idx(inst);  //sv is same
                    sv = inst;
                    break;
                }
            }
        }
    }

    // If that is argument, we will skip
    return si;
}


unsigned CtrlIdentifier::initDstPoint(const Value *dv, unsigned di, const Function *func){
    unsigned oridi = di;
    // DV's updated value (to be stored) 
    if(func2sz(func) -1 <= di) return 0xFFFFFFFF;
    else if(isVstrInst(dv)){  // it is already vstr
        dv = retPtrVal(dv);
        di = inst2idx(func, dv);
        return di;
    }
    else if(isStoreInst(dv)){
        for(unsigned i = di; i < func2sz(func); i++){
            const Instruction *inst = idx2inst(func, i);
            if(isVstrInst(inst)){   //Only Store since it will be updated -> At least ARM specific
                if(retPtrPtr(inst) == dv){
                    dv = retPtrVal(inst);
                    di = inst2idx(func, dv);
                    return di;
                }
            }
        }
        return oridi;
    }
    else if(isGepInst(dv)){
        for(unsigned i = di; i < func2sz(func); i++){
            const Instruction *inst = idx2inst(func, i);
            if(isStoreInst(inst)){   //Only Store since it will be updated
                if(retPtrPtr(inst) == dv){
                    di = initDstPoint(inst, i, func);
                    if(di != 0xFFFFFFFF){
                        return di;
                    }
                }
            }
        }
    }
    return oridi;
}



bool CtrlIdentifier::recursiveReachabilityTrack(set <const Value *> svset, const Value *dv, const Function *func, unsigned minidx, unsigned maxidx){
    if(minidx > maxidx) return false;
    set <const Value *> doneset;
    return isBackwardPropaInst(svset, doneset, dv, func, minidx, maxidx);
}

bool CtrlIdentifier::isBackwardPropaInst(set <const Value *> svset, set <const Value *> &doneset, const Value *dv, const Function *func, unsigned minidx, unsigned maxidx){
    if(!dv) return false;
    else if(doneset.find(dv) != doneset.end()) return false;

    //cout << "Debugging: dv: " << inst2str(dv) << endl;

    doneset.insert(dv);
    if(checkedCVset.find(dv) != checkedCVset.end()) return false;        // Already checked variable
    else if(svset.find(dv) != svset.end()) return true;
    else if(!SVFUtil::dyn_cast<Instruction>(dv)) return false;  // Base case (non-inst)
    else if(SVFUtil::dyn_cast<Constant>(dv)) return false;      // Base case
    else if(inst2idx(func, dv) < minidx) return false;          // Base case 
    else if(isUpdatedByCallee(func, dv)){
        set <const Value *> cevset;
        findVarsAffectingCallee(func, dv, cevset, minidx, retMaxIdx(minidx, maxidx, dv));    // = Affected callees's args


        // Alias update in Callee check 
        unsigned crAliasOff = retOffFromSVSet(svset);
        if(crAliasOff != 0xFFFFFFFF){                                  // NEW in CtrlIdentifier
                // Sub-step1: To get CS addr
            uint64_t csaddr = ipa->retCSAddrFromRetVar(func, dv, minidx, retMaxIdx(minidx, maxidx, dv)); // NEW in InterProc
                // Sub-step2: To get CE
            if(const Function *cefunc = ipa->csaddr2ce(csaddr)){                       // NEW in InterProc
                // Sub-step3: To get 
                set <const Value *> csargset = ipa->retCSAddr2ArgCSVs(csaddr);
                for(auto it = csargset.begin(); it != csargset.end(); ++it){  // Affecting src variable directly
                    const Value *v = *it;
                    unsigned passoff = gep2off(v);
                    if(passoff != 0xFFFFFFFF && crAliasOff >= passoff){
                        const Value *ceArg = ipa->retCEArgFromCSArg(csaddr, v);      // NEW in InterProc
                        if(ceArg){
                            unsigned ceAliasOff = crAliasOff - passoff;   
                            if(isRetAffectedByReadAlias(cefunc, ceAliasOff, ceArg)){        // NEW in BinWrapper
                                return true;
                            }
                        }
                    }
                }
            }
        }

        // Before Callee codes
        for(auto it = cevset.begin(); it != cevset.end(); ++it){
            const Value *v = *it;
            bool res = isBackwardPropaInst(svset, doneset, v, func, minidx, retMaxIdx(minidx, maxidx, v));
            if(res) return true;
        }

        if(const Instruction *inst = SVFUtil::dyn_cast<Instruction>(dv)){  
            for(unsigned i = 0; i < inst->getNumOperands(); i++){
                bool res = isBackwardPropaInst(svset, doneset, inst->getOperand(i), func, minidx, retMaxIdx(minidx, maxidx, inst));
                if(res) return true;
            }
        }
    }
    else if(isLoadInst(dv)){
        return isBackwardPropaInst(svset, doneset, retPtrPtr(dv), func, minidx, retMaxIdx(minidx, maxidx, dv));
    }
    else if(isVldrInst(dv)){    //%v4_805bad8 = call float @__asm_vldr(i32 3. %v3_805bad8) so works!
        unsigned vi = inst2idx(func, dv);
        return isBackwardPropaInst(svset, doneset, retPtrPtr(dv), func, minidx, retMaxIdx(minidx, maxidx, dv));
    }
    else if(isBinOpInst(dv) || isCastInst(dv)){
        const Instruction *inst = SVFUtil::dyn_cast<Instruction>(dv);
        for(unsigned i = 0; i < inst->getNumOperands(); i++){
            bool res = isBackwardPropaInst(svset, doneset, inst->getOperand(i), func, minidx, retMaxIdx(minidx, maxidx, inst));
            if(res) return true;
        }
    }

    return false;
}

////// Backtracking handler affected by Callee 
bool CtrlIdentifier::isUpdatedByCallee(const Function *func, const Value *dv){
    // Check variable list first 
    if(ipa->isCSRetVar(func, dv)){
        // NOTE PID - Check whether  -> it is not necessary here
        return true;
    }
    
    return false;
}

void CtrlIdentifier::updateInterCallee(const Function *crfunc, const Value *dv, 
                                        set <const Value *> &vset, unsigned si, unsigned di){
    // NOTE: it works only for backward analysis just like the current implementation
    vector <const Instruction*> instvec = func2insts(crfunc);
    set <uint64_t> csset, wcsset;
    set <unsigned> targetHeapOffSet;

    assert(instvec.size() > di);

    //In the backward order,
    // 1) Get read heap variables from the callee
    // 2) Get callees writing those variables
    // 3) Get args of callee set
    // Note: for 2), we don't need to care about their args since they will be handled later during the backward tracking

    // 1)
    ipa->updateCSRet2CSAddrs(crfunc, dv, csset);      
    for(auto it = csset.begin(); it != csset.end(); ++it){
        uint64_t csaddr = *it;
        if(const Function *cefunc = retcsaddr2ce(csaddr)){
            readHeapFromCallee(cefunc, targetHeapOffSet);
        }
    }

    // 2)
    for(auto it = csset.begin(); it != csset.end(); ++it){
        uint64_t csaddr = *it;
        if(const Function *cefunc = retcsaddr2ce(csaddr)){
            if(isWriteGivenHeap(cefunc, targetHeapOffSet)){
                wcsset.insert(csaddr);
            }
        }
    }

    // 3)
    for(auto it = wcsset.begin(); it != wcsset.end(); ++it){
        uint64_t csaddr = *it;
        set <const Value *> avset = ipa->retCSAddr2ArgCSVs(csaddr);
        for(auto ait = avset.begin(); ait != avset.end(); ++ait){
            const Value *v = *ait;
            if(const Instruction *inst = SVFUtil::dyn_cast<Instruction>(v)){
                unsigned idx = inst2idx(crfunc, inst);
                if(si <= idx && idx <= di) vset.insert(v);
            }
            else vset.insert(v);
        }
    }

    return;
}

void CtrlIdentifier::findVarsAffectingCallee(const Function *crfunc, const Value *dv, 
                                            set <const Value *> &vset, unsigned si, unsigned di){
    // NOTE: it works only for backward analysis just like the current implementation
    if(!ipa->isCSRetVar(crfunc, dv)) return;

    // Point 1: Caller <-> Callee effect
    ipa->retCSRetVar2ArgVarSet(crfunc, dv, vset, si, di);

    // Point 2: Callee -> Callee effect (e.g., affecting variables of Callee's heap variable) (must be cumulative)
    updateInterCallee(crfunc, dv, vset, si, di);

    return;
}

unsigned CtrlIdentifier::retOffFromSVSet(set <const Value *> &svset){
    unsigned aliasOff = 0xFFFFFFFF;
    for(auto it = svset.begin(); it != svset.end(); ++it){
        const Value *v = *it;
        aliasOff = gep2off(v);
        if(aliasOff != 0xFFFFFFFF) return aliasOff;
    }

    return aliasOff;
}

///////// Batch Inter analysis
void CtrlIdentifier::batchInterAnalysis(unsigned si, unsigned di, const Function *func){
    vector<const Instruction*> instvec = func2insts(func);

    assert(instvec.size() > di);

    for(int i = si; i <= di; i++){
        const Instruction *inst = instvec[i];
        if(isCallInst(inst)){
            const Function *ce = getCallee(inst);
            if(!ce) continue;   //Maybe due to unsolved indirect call
            ipa->interAnalysis(inst);
            binIRInit(ce);
        }
    }
    return;
}



//////////////////////////////////////////////
//######################### Step 3
//////////////////////////////////////////////////////////
void CtrlIdentifier::unconnectedRefAliasDetect(){
    // Callee -> Caller Analysis
    cout << "[INFO] # of ctrlset: " << ctrlset.size() << " from unconnectedRefAliasDetect" << endl;
    for(auto it = ctrlset.begin(); it != ctrlset.end(); ++it){
        ctrlinfo *cobj = *it;

        // arg (backward)
        if(inConn.find(cobj) == inConn.end() && inTrans.find(cobj) == inTrans.end()){
            if(!cobj->refs.size()){
                const Value *v = NULL;
                const Function *curfunc = cobj->func;

                v = *(cobj->errs.begin());
                set <const CallInst *> csset = retCallSiteSet(curfunc, module);
                if(csset.size()){
                    const CallInst *ci = *(csset.begin());
                    const Function *caller = v2func(ci);
                    uint64_t csaddr = inst2addr(ci);
                    ipa->interAnalysis(ci);
                    if(v == ipa->retCSAddr2ArgCEV(csaddr, ARM_REG_S0)){  // S0 ->"R"
                        rci = cobj;
                    }
                    else if(v == ipa->retCSAddr2ArgCEV(csaddr, ARM_REG_S1)){  // S1 ->"P"
                        pci = cobj;
                    }
                    else if(v == ipa->retCSAddr2ArgCEV(csaddr, ARM_REG_S2)){  // S2 ->"Y"
                        yci = cobj;
                    }
                }

                cout << "[Transition] Ctrl " << func2name(cobj->func) << endl;
            }
            for(auto vit = cobj->refs.begin();  vit != cobj->refs.end(); ++vit){
                const Value *v = *vit;
                const Function *curfunc = v2func(v);
                assert(curfunc);

                const Value *tv = retRecursiveReachedPtr(v, curfunc);
                if(!tv) {
                    cout << "[Unconn, NULL] Ctrl:  Ref alias: NULL from " << inst2str(v) << " at func: " << func2name(curfunc) << endl;
                    continue;
                }

                ref2alias[v] = tv;      // Final output of this function -> Unconnected controller's ref alias if ref is not ptr
                alias2ref[tv] = v;
                cout << "[Unconn] Ctrl:  Ref alias: " << inst2str(tv) << " from " << inst2str(v) << " at func: " << func2name(curfunc) << endl;
                cout << "\tOri ref: " << inst2str(v) << endl;
            }
        }
        //else NOTE: if already connect, I consider doing nothing
        //      If necessary, update "map <const Value *, const Value *> ref2alias;"
        else{
            cout << "[Conn] Ctrl:  Ref alias: " << inst2str(*(cobj->refs.begin())) << " at func: " << func2name(cobj->func) << endl;
        }
    }

    return;
}

const Value *CtrlIdentifier::retRecursiveReachedPtr(const Value *v, const Function *curfunc){
    const Value *ret = NULL;
    set <const Value *> doneset, vset;

    // init
    binIRInit(curfunc);

    // Pass if it is already ptr
    if(!v) return v;
    else if(isGepInst(v)) return v;

    // VLDR check first
    doneset.clear();
    vset.clear();
    if(backtrackVLDRLoadingAlias(v, curfunc, doneset, vset)){    // True = there is some item in "vset"
        unsigned minidx = func2sz(curfunc);
        const Value *tv = NULL;

        // VLDR check first (in the callee)
        for(auto it = vset.begin(); it != vset.end(); ++it){
            const Value *iv = *it;
            if(const Value *pv = retVLDRPtr(iv)){
                //ptr2floatMemAccess[pv].insert(iv);
                unsigned idx = inst2idx(curfunc, iv);
                if(minidx > idx){
                    tv = pv;
                    minidx = idx;
                }
            }
        }

        if(tv) return tv;
    }

    // Caller/Callee Handling
    set <const CallInst *> csset = retCallSiteSet(curfunc, module);
    if(csset.size()){
        const CallInst *ci = *(csset.begin());
        const Function *caller = v2func(ci); //ci->getCalledFunction(); 
        binIRInit(caller);
        ipa->interAnalysis(ci);
        uint64_t instaddr = inst2addr(ci);
        set <const Value *> argset = ipa->retCSAddr2ArgCEVs(instaddr);

        // Backtrack start
        // From curfunc to find corresponding argument
        doneset.clear();
        vset.clear();
        if(backtrackArgPtrAlias(v, argset, curfunc, doneset, vset)){    // True = there is some item in "vset"
            unsigned minidx = func2sz(caller);
            const Value *tv = NULL;

            // Arg check at caller
            for(auto it = vset.begin(); it != vset.end(); ++it){
                const Value *iv = *it;
                if(argset.find(iv) != argset.end()){
                    const Value *csv = ipa->retCSArgFromCEArg(instaddr, iv);
                    const Value *retv = retRecursiveReachedPtr(csv, caller);
                    if(retv){
                        unsigned idx = inst2idx(caller, retv);
                        if(minidx > idx){
                            tv = retv;
                            minidx = idx;
                        }
                    }
                }
            }
 
            if(tv) return tv;          
        }
    }

    return NULL;
}

bool CtrlIdentifier::backtrackVLDRLoadingAlias(const Value *v, const Function *func, set <const Value *> &doneset, set <const Value *> &vset){
    if(!v) return false;
    else if(doneset.find(v) != doneset.end()) return false;
    doneset.insert(v);

    if(!SVFUtil::dyn_cast<Instruction>(v)) return false;  // Base case (non-inst)
    else if(isVldrInst(v)){    //%v4_805bad8 = call float @__asm_vldr(i32 3. %v3_805bad8) so works!
        const Value *ptr = retVLDRPtr(v);
        if(ptr){
            //ptr2floatMemAccess[ptr].insert(v);
            vset.insert(v);
            return true;
        }
        else return false;
    }
    else if(isCallInst(v) || isBinOpInst(v) || isCastInst(v)){  
        bool tag = false;
        const Instruction *inst = SVFUtil::dyn_cast<Instruction>(v);
        for(unsigned i = 0; i < inst->getNumOperands(); i++){
            if(backtrackVLDRLoadingAlias(inst->getOperand(i), func, doneset, vset)) tag = true;
        }
        
        return tag;
    }

    return false;
}


bool CtrlIdentifier::backtrackArgPtrAlias(const Value *v, set <const Value *> argset, const Function *func, set <const Value *> &doneset, set <const Value *> &vset){
    if(!v) return false;
    else if(doneset.find(v) != doneset.end()) return false;
    doneset.insert(v);

    if(argset.find(v) != argset.end()){            //Argument Checker
        vset.insert(v);
        return true;
    }
    else if(!SVFUtil::dyn_cast<Instruction>(v)) return false;  // Base case (non-inst)
    else if(isCallInst(v) || isBinOpInst(v) || isCastInst(v)){ 
        bool tag = false;
        const Instruction *inst = SVFUtil::dyn_cast<Instruction>(v);
        for(unsigned i = 0; i < inst->getNumOperands(); i++){
            if(backtrackArgPtrAlias(inst->getOperand(i), argset, func, doneset, vset)) tag = true;
        }
        
        return tag;
    }

    return false;
}



//////////////////////////////////////////////
//######################### Step 4
//////////////////////////////////////////////////////////
////// Cascading transition detector
void CtrlIdentifier::cascadingTransition(){
    map <const Function *, map <const Value *, string>> transit2var2role;
    map <const Function *, map <string, set <const Value *>>> transit2role2var;

    // Step 1: Filtering
        // Transit filtering Step 1
    set <const Function *> filteredCandids;
    for(auto cit = ctrlset.begin(); cit != ctrlset.end(); ++cit){
        ctrlinfo *ci = *cit;

        getcurcscefunc(ci->curs, filteredCandids);
        getcurcscefunc(ci->refs, filteredCandids);
        getcurcscefunc(ci->errs, filteredCandids);
        getcurcscefunc(ci->outs, filteredCandids);
        getcurcscefunc(ci->mathouts, filteredCandids);
    }

        // Transit filtering Step 2
    set <const Function *> transitCandids;
    for(auto fit = filteredCandids.begin(); fit != filteredCandids.end(); ++fit){
        const Function *func = *fit;
        map <const Value *, string> var2role;
        map <string, set<const Value *>> role2var;

        var2role.clear();
        role2var.clear();
        if(filterCascadingTransition(func, var2role, role2var)){
            transitCandids.insert(func);
            transit2var2role[func] = var2role;
            transit2role2var[func] = role2var;
            cout << "[INFO]: Transition candidate func: " << func2name(func) << endl;
        }
    }

        // Outs filtering
    map <const Function *, ctrlinfo *> fromcandid2ci;
    map <const Function*, set <const Value *>> fromcandid2outs;
    set <const Function *> fromCandids;
    for(auto it = ctrlset.begin(); it != ctrlset.end(); ++it){
        ctrlinfo *ci = *it;
        if(outConn.find(ci) == outConn.end()){
            fromCandids.insert(ci->func);
            fromcandid2ci[ci->func] = ci;
            if(ci->mathouts.size()) 
                fromcandid2outs[ci->func] = ci->mathouts; 
            else
                fromcandid2outs[ci->func] = ci->errs;
            assert(ci->func);
        }
    }

    // Step 2: Dep Candids
    //  NOTE: we care only about the case where transit = callee which is the only available pattern and it is more common
    map<const Function*, vector <const CallInst *>> cr2stackPair;   // caller 2 callee (CE = transit) 
    for(auto it = fromCandids.begin(); it != fromCandids.end(); ++it){
        const Function *crfunc = *it;
        if(transitCandids.find(crfunc) != transitCandids.end()){  //Filtered Function
            vector <const CallInst *> civec;
            cr2stackPair[crfunc] = civec;
        }
        else{
            set <const CallInst *> csset = func2cssetMap[crfunc];
            for(auto it = csset.begin(); it != csset.end(); ++it){
                const CallInst *ci = *it;
                const Function *ce = getCallee(ci);
                if(ce != NULL && transitCandids.find(ce) != transitCandids.end()){
                    vector <const CallInst *> civec;
                    civec.push_back(ci);
                    cr2stackPair[crfunc] = civec;  
                }
            }
        }
    }

    // Dependency connection checking
        // prerequisite
    map <unsigned, ctrlinfo *> aliasoff2ctrl;
    for(auto it = ref2alias.begin(); it != ref2alias.end(); ++it){  
        const Value *ref = it->first;
        const Value *av = it->second;
        
        unsigned off = gep2off(av);
        ctrlinfo *ci = ref2ctrlmap[ref]; //NOTE: it must exist
        aliasoff2ctrl[off] = ci;
        //NOTE for assert
        //  I guess ref is the key to get ctrl, if wrong, change it with alias
        //  consider to use "ref2oriref" as well
        assert(ci);
    }

        // Main connectivity checking
    for(auto it = cr2stackPair.begin(); it != cr2stackPair.end(); ++it){
        const Function *crfunc = it->first;
        set <const Value *> outs = fromcandid2outs[crfunc];

        vector <const CallInst *> callstack = it->second;
        const Function *cefunc = getCallee(callstack.back());
        map <const Value *, string> var2role = transit2var2role[cefunc];
        map <string, set<const Value *>> role2varset = transit2role2var[cefunc];

        map <string, ctrlinfo *> role2ctrl;

        // Getting ctrlinfo for alias
        role2ctrl.clear();
        for(auto rit = role2varset.begin(); rit != role2varset.end(); ++rit){
            string role = rit->first;
            set <const Value *> varset = rit->second;
            recursiveAlias2Ctrl(crfunc, callstack, role, varset, aliasoff2ctrl, role2ctrl); 
        }

        if(role2ctrl.size() != 2){    
            //continue;     // Oricode
            ctrlinfo *fromci = fromcandid2ci[crfunc];
            ctrlinfo *toPitch = pci;
            ctrlinfo *toRoll = rci;

            assert(fromci);
            assert(toPitch);
            assert(toRoll);

            toPitch->role = "ap_p";
            toRoll->role = "ar_p";
            
            outTrans[fromci].insert(toPitch);
            outTrans[fromci].insert(toRoll);
            inTrans[toPitch].insert(fromci);
            inTrans[toRoll].insert(fromci);

            for(auto rit = role2varset.begin(); rit != role2varset.end(); ++rit){
                string role = rit->first;
                set <const Value *> varset = rit->second;
                for(auto vit = varset.begin(); vit != varset.end(); ++vit){
                    const Value *v = *vit;
                    if(role == "ap_p") toPitch->refs.insert(v);
                    else if(role == "ar_p") toRoll->refs.insert(v);
                }
            }

            break;
        }
        else if(taggingTransition(crfunc, callstack, outs, role2varset)){
            ctrlinfo *fromci = fromcandid2ci[crfunc];
            ctrlinfo *toPitch = role2ctrl["ap_p"];
            ctrlinfo *toRoll = role2ctrl["ar_p"];

            toPitch->role = "ap_p";
            toRoll->role = "ar_p";
            
            outTrans[fromci].insert(toPitch);
            outTrans[fromci].insert(toRoll);
            inTrans[toPitch].insert(fromci);
            inTrans[toRoll].insert(fromci);
            break;
        }
    }

    return;
}

void CtrlIdentifier::getcurcscefunc(set<const Value *> vset, set<const Function*> &fset){
    for(auto vit = vset.begin(); vit != vset.end(); ++vit){
        const Value *v = *vit;
        getcurcscefunc(v, fset);
    }

    return;
}

void CtrlIdentifier::getcurcscefunc(const Value *v, set<const Function*> &fset){
    const Function *func = v2func(v);
    if(func){
        fset.insert(func);
        //cs 
        if(ce2cssetMap.find(func) != ce2cssetMap.end()){
            set <const CallInst*> csset = ce2cssetMap[func];
            for(auto ciit = csset.begin(); ciit != csset.end(); ++ciit){
                const CallInst* cinst = *ciit;
                const Function *csfunc = cinst->getParent()->getParent();
                fset.insert(csfunc);
            }
        }
        //ce
        if(func2cssetMap.find(func) != func2cssetMap.end()){
            set <const CallInst*> ciset = func2cssetMap[func];
            for(auto ciit = ciset.begin(); ciit != ciset.end(); ++ciit){
                const CallInst* cinst = *ciit;
                if(const Function *cefunc = cinst->getCalledFunction()){
                    fset.insert(cefunc);
                }
                else if(inst2instaddr.find(cinst) != inst2instaddr.end()){
                    uint64_t addr = inst2instaddr[cinst];
                    if(const Function *cefunc = retBinIndcsaddr2ce(addr)){
                        fset.insert(cefunc);
                    }
                }
            }
        }
    }
    return;
}


void CtrlIdentifier::recursiveAlias2Ctrl(const Function *cr, vector <const CallInst *> &callstack, string role, set <const Value *> &varset, map <unsigned, ctrlinfo *> &aliasoff2ctrl, map <string, ctrlinfo *> &role2ctrl){
    // gep offset detection
    map <unsigned, int> offmap;
    for(auto it = varset.begin(); it != varset.end(); ++it){
        const Value *v = *it;
        unsigned offset = recursiveCallStackGepOff(cr, callstack, v);

        if(offset > 8){ //Too small one is not maybe proper offset
            if(offmap.find(offset) == offmap.end()) offmap[offset] = 0;
            offmap[offset] += 1;
        }
    }

    // Choose the best off
    unsigned bestoff = 0;
    int largest = 0;
    for(auto it = offmap.begin(); it != offmap.end(); ++it){
        unsigned off = it->first;
        int count = it->second;
        if(count > largest){
            largest = count;
            bestoff = off;
        }
    }

    if(bestoff){
        if(aliasoff2ctrl.find(bestoff) != aliasoff2ctrl.end()){
            role2ctrl[role] = aliasoff2ctrl[bestoff];
        }
    }

    return ;
}

unsigned CtrlIdentifier::recursiveCallStackGepOff(const Function *cr, vector <const CallInst *> &callstack, const Value *dv){
    unsigned off = 0;
    const Value *sink = dv;
    set <const Value *> argset;

    for(int i = callstack.size() - 1; i >= 0; i--){
        const CallInst *ci = callstack[i];
        const Function *curfunc = getCallee(ci);
        uint64_t csaddr = inst2addr(ci);
        assert(csaddr);

        argset.clear();
        argset = ipa->retCSAddr2ArgCEVs(csaddr);

        const Value *v = recursiveGepOff(sink, curfunc, argset, off);
        if(argset.find(v) == argset.end()) return off;

        sink = ipa->retCSArgFromCEArg(csaddr, v);
    }

    argset.clear();
    recursiveGepOff(sink, cr, argset, off);

    return off;
}


bool CtrlIdentifier::taggingTransition(const Function *cr, vector <const CallInst *> &callstack, set <const Value *> &outs, map <string, set<const Value *>> &role2varset){
    bool r = false;
    bool p = false;
    for(auto it = role2varset.begin(); it != role2varset.end(); ++it){
        set <const Value *> vset = it->second;
        string role = it->first;

        for(auto vit = vset.begin(); vit != vset.end(); ++vit){
            const Value *v = *vit;
            if(recursiveCallStackReachabilityTrack(outs, v, cr, callstack)){
                if(role == "ap_p") p = true;
                else if(role == "ap_r") r = true; 
                else assert(0);
            }
            if(r && p) return true;
        }
        if(r && p) return true;
    }
    
    return false;
}

bool CtrlIdentifier::recursiveCallStackReachabilityTrack(set <const Value *> svset, const Value *dv, const Function *cr, vector <const CallInst *> &callstack){
    unsigned si, di;
    const Value *src;
    const Value *sink;
    bool found;
    set <const Value *> argset;
    set <const Value *> csargset;
    set <const Value *> sinkset;
    set <const Value *> srcset;

    sinkset.insert(dv);

    for(int i = callstack.size() - 1; i >= 0; i++){
        const CallInst *ci = callstack[i];
        const Function *curfunc = getCallee(ci);
        uint64_t csaddr = inst2addr(ci);
        assert(csaddr);

        argset.clear();
        csargset.clear();

        argset = ipa->retCSAddr2ArgCEVs(csaddr);
        found = false;
        for(auto sit = sinkset.begin(); sit != sinkset.end(); ++sit){
            sink = *sit;
            for(auto ait = argset.begin(); ait != argset.end(); ++ait){
                src = *ait;
                srcset.clear();
                srcset.insert(src);

                si = inst2idx(curfunc, src);    // = src = srcset semantically
                di = inst2idx(curfunc, sink);

                if(recursiveReachabilityTrack(srcset, sink, curfunc, si, di)){
                    const Value *csarg = ipa->retCEArgFromCSArg(csaddr, src);
                    csargset.insert(csarg);
                    found = true;
                }
            }
        }

        if(!found) return false;

        sinkset.clear();
        sinkset = csargset;
    }

    for(auto sit = sinkset.begin(); sit != sinkset.end(); ++sit){
        sink = *sit;
        si = insts2maxidx(cr, svset);
        di = inst2idx(cr, sink);
        if(recursiveReachabilityTrack(svset, dv, cr, si, di)) return true;
    }
    return false;
}



bool CtrlIdentifier::filterCascadingTransition(const Function *func, map <const Value *, string> &var2role, map <string, set<const Value*>> &role2var){
    // This one should not be found with binarywrapper
    // But Binary operator does not rely much on binary wrapper's processing
    bool fmul = false, fdiv = false, fsub = false, fadd = false, atanf = false;
    unsigned offset = 0;
    set <const Value *> rollcandids, pitchcandids;

    if(!func->size()) return false;
    vector<const Instruction *> instvec = func2insts(func);

    // preprocessing, signature-based filtering
    for(int i = 0; i < instvec.size(); i++){
        const Instruction *inst = instvec[i];
        if(isBinOpInst(inst)){
            if(retBinOpTy(inst) == Instruction::FAdd) fadd = true;
            else if(retBinOpTy(inst) == Instruction::FSub) fsub = true;
            else if(retBinOpTy(inst) == Instruction::FMul){
                const Value *cst = inst->getOperand(1);
                if(isProcFP(cst)){
                    float fpv = retProcFP(cst);
                    fmul = true;
                }
            }
            else if(retBinOpTy(inst) == Instruction::FDiv){
                const Value *cst = inst->getOperand(1);
                if(isProcFP(cst)){
                    float fpv = retProcFP(cst);
                    fdiv = true;
                }
            }
            else if(retBinOpTy(inst) == VFMS){
                fsub = true;
                fmul = true;
            }
            else if(retBinOpTy(inst) == VFMA){
                fadd = true;
                fmul = true;
            }
        }
        else if(const CallInst *ci = SVFUtil::dyn_cast<CallInst>(inst)){
            if(const Function *ce = ci->getCalledFunction()){ 
                if(pid->isSemanticFunc("atanf", ce)){
                    atanf = true;
                }
            }
        }
    }

    if(!(fmul && fdiv && fsub && fadd && atanf)) return false;
    // (Mostly) filtered. Let's rely on binIRInit
    binIRInit(func);

    bool f1 = false, f2 = false, f3 = false;
    bool cosf = false;

    // offset setting by checking ctrl w/o to ctrl
    for(auto it = ctrlset.begin(); it != ctrlset.end(); ++it){
        ctrlinfo *ci = *it;
        if(ci->func == func && outConn.find(ci) == outConn.end()){
            for(auto iit = ci->outs.begin(); iit != ci->outs.end(); ++iit){
                int idx = inst2idx(func, *iit);
                if(idx > offset) offset = idx;
            }

            for(auto iit = ci->mathouts.begin(); iit != ci->mathouts.end(); ++iit){
                int idx = inst2idx(func, *iit);
                if(idx > offset) offset = idx;
            }

            for(auto iit = ci->errs.begin(); iit != ci->errs.end(); ++iit){
                int idx = inst2idx(func, *iit);
                if(idx > offset) offset = idx;
            }
        }
    }

    // Checking (1) remaining signatures and (2) roll/pitch cadidates
    for(int i = offset; i < instvec.size(); i++){
        const Instruction *inst = instvec[i];

        // instruction-level signature checking
        if(isBinOpInst(inst)){
            if(retBinOpTy(inst) == Instruction::FAdd){
                fadd = true;
            }
            else if(retBinOpTy(inst) == Instruction::FSub){
                fsub = true;
            }
            else if(retBinOpTy(inst) == Instruction::FMul){
                for(int i = 0; i < inst->getNumOperands(); i++){
                    const Value *cst = inst->getOperand(i);
                    if(isProcFP(cst)){
                        double fdvalue = fabs(retProcFP(cst));

                        if(isPI(fdvalue)) f1 = true;
                        else if(is180(fdvalue)) f2 = true;
                    }
                }
            }
            else if(retBinOpTy(inst) == Instruction::FDiv){
                for(int i = 0; i < inst->getNumOperands(); i++){
                    const Value *cst = inst->getOperand(i);
                    if(isProcFP(cst)){
                        double fdvalue = fabs(retProcFP(cst));

                        if(isG(fdvalue)) f3 = true;
                        else if(isPI(fdvalue)) f1 = true;
                        else if(is180(fdvalue)) f2 = true;
                    }
                }
            }
            else if(retBinOpTy(inst) == VFMS){
                for(int i = 0; i < inst->getNumOperands(); i++){
                    const Value *cst = inst->getOperand(i);

                    if(isProcFP(cst)){
                        double fdvalue = fabs(retProcFP(cst));

                        if(isG(fdvalue)) f3 = true;
                        else if(isPI(fdvalue)) f1 = true;
                        else if(is180(fdvalue)) f2 = true;
                    }
                }
            }
            else if(retBinOpTy(inst) == VFMA){
                for(int i = 0; i < inst->getNumOperands(); i++){
                    const Value *cst = inst->getOperand(i);

                    if(isProcFP(cst)){
                        double fdvalue = fabs(retProcFP(cst));

                        if(isG(fdvalue)) f3 = true;
                        else if(isPI(fdvalue)) f1 = true;
                        else if(is180(fdvalue)) f2 = true;
                    }
                }
            }
        }
        else if(isVstrInst(inst)){  
            const Value * ptr = retPtrPtr(inst);
            if(isGepInst(ptr) || SVFUtil::dyn_cast<Argument>(ptr) != NULL){
                if(!cosf)   pitchcandids.insert(inst);
                else        rollcandids.insert(inst);
            }
        }
        else if(isCallInst(inst)){
            assert(pid->semanticFuncSetMap.find("cosf") != pid->semanticFuncSetMap.end()); //check prerequisite is ready
            set <const Function *> cosset = pid->semanticFuncSetMap["cosf"];
            const Function *ce = getCallee(inst);

            if(ce != NULL && cosset.find(ce) != cosset.end()){
                cosf = true;
            }
        }
    }

    if(f1 && f2 && f3){
        bool found = false;
        for(auto rit = rollcandids.begin(); rit != rollcandids.end(); ++rit){
            const Value *rinst = *rit;
            const Value *dv = retPtrVal(rinst);
            const Value *dptr = retPtrPtr(rinst);
            for(auto pit = pitchcandids.begin(); pit != pitchcandids.end(); ++pit){
                const Value *pinst = *pit;
                const Value *sv = retPtrVal(pinst);
                const Value *sptr = retPtrPtr(pinst);
                set <const Value *> svset;
                unsigned si = inst2idx(func, sv);
                unsigned di = inst2idx(func, dv);
                svset.clear();
                svset.insert(sv);

                if(recursiveReachabilityTrack(svset, dv, func, si, di)){
                    var2role[sptr] = "ap_p"; // 09/04 check // originally P
                    var2role[dptr] = "ar_p";
                    role2var["ap_p"].insert(sptr);         // originally P
                    role2var["ar_p"].insert(dptr);

                    found = true;
                }
            }
        }

        return found;
    }
    else return false;
}

////// Util
bool CtrlIdentifier::isForIdenticalController(const Value *sv, const Value *dv){
    set <const Value *> cvset = pid->retCtrlVset(sv);
    if(cvset.find(dv) != cvset.end()) return true;
    else return false;
}


//////////////////////////////////////////////
//######################### Step 5
//////////////////////////////////////////////////////////
void CtrlIdentifier::roleInterFuncIdentification(){
    // Step1 : missIn, missOut, inset update
    map <const Value *, ctrlinfo *> missIn;
    map <const Value *, ctrlinfo *> missOut;
    set <const Value *> inset;
    set <const Value *> outset;

    // Step 1: Missing Part detection
    for(auto it = ctrlset.begin(); it != ctrlset.end(); ++it){
        ctrlinfo *cobj = *it;

        // missIn
        if(inConn.find(cobj) == inConn.end() && inTrans.find(cobj) == inTrans.end()){
            if(!cobj->refs.size()){     // Or connect it with err?
                cout << "[DEBUG] inset Entry: " << func2name(cobj->func) << " input Ref: NULL" << endl;  
                if(cobj->groundtruth.length()) cout << "   >> GroundTruth: " << cobj->groundtruth << endl;
                else cout << "  >> No GroundTruth" << endl;
            }
            else{
                for(auto vit = cobj->refs.begin();  vit != cobj->refs.end(); ++vit){
                    const Value *v = *vit;
                    if(ref2alias.find(v) != ref2alias.end()) v = ref2alias[v];

                    missIn[v] = cobj;
                    inset.insert(v);
                    cout << "[DEBUG] inset Entry: " << func2name(cobj->func) << " input Ref: " << inst2str(v) << endl;
                    if(cobj->groundtruth.length()) cout << "   >> GroundTruth: " << cobj->groundtruth << endl;
                    else cout << "  >> No GroundTruth" << endl;
                }
            }
        }

        // missOut
        if(outConn.find(cobj) == outConn.end() && outTrans.find(cobj) == outTrans.end()){
            if(cobj->outs.size()){     // Or connect it with err?
                for(auto vit = cobj->outs.begin();  vit != cobj->outs.end(); ++vit){
                    const Value *v = *vit;
                    missOut[v] = cobj;
                    outset.insert(v);
                    cout << "[DEBUG] outset Entry: " << func2name(cobj->func) << " outv: " << inst2str(v) << endl;
                    if(cobj->groundtruth.length()) cout << "   >> GroundTruth: " << cobj->groundtruth << endl;
                    else cout << "  >> No GroundTruth" << endl;
                }
            }
            else if(cobj->mathouts.size()){     // Or connect it with err?
                for(auto vit = cobj->mathouts.begin();  vit != cobj->mathouts.end(); ++vit){
                    const Value *v = *vit;
                    missOut[v] = cobj;
                    outset.insert(v);
                    cout << "[DEBUG] outset Entry: " << func2name(cobj->func) << " mathoutv: " << inst2str(v) << endl;
                    if(cobj->groundtruth.length()) cout << "   >> GroundTruth: " << cobj->groundtruth << endl;
                    else cout << "  >> No GroundTruth" << endl;
                }
            }
            else assert(0);
        }
    }
    set <unsigned> offset = gep2off(inset);
    assert(offset.size() > 0);


    // Step 2: Inter argument update (if the offset is 0)
    // Update it (inter) -> To solve the following problem
    //[DEBUG] V:   %v3_805aa94 = inttoptr i32 %v2_805aa94 to i32*, !addr !11
    //   >> Offset: 112
    //   >> ROffset: 0 of Inst:   %tmp11 = call float @__decompiler_undefined_function_3()
    //       >> ROffset's func: function_8059bd8

    //ref2ctrlmap[v] = cobj;        //erase and update
    //ref2oriref[v] = v;           // you can update ctrlinfo as well. But, at this time, I don't 
    // Out is ok w/o having gep. But, in (=ref) requires being refined 
        // Deep copy
    map <const Value *, ctrlinfo *> oldref2ctrlmap;
    for(auto it = ref2ctrlmap.begin(); it != ref2ctrlmap.end(); ++it){
        const Value *v = it->first;
        ctrlinfo *cobj = it->second;
        oldref2ctrlmap[v] = cobj;
        ctrl2intervmap[cobj].insert(v);
    }
    for(auto it = oldref2ctrlmap.begin(); it != oldref2ctrlmap.end(); ++it){
        const Value *dv = it->first;
        ctrlinfo *cobj = it->second;
        unsigned roff = gep2off(dv);
        unsigned maxidx; 
        if(roff <= 12){  // Because ref should be argument, it must be somewhere in the caller -> 12 = considering 3 axis
            if(!dv) continue;
            else if(SVFUtil::dyn_cast<Instruction>(dv)) maxidx = inst2idx(dv);
            else if(SVFUtil::dyn_cast<Argument>(dv)) maxidx = 0;
            else assert(0);

            const Function *func = v2func(dv);
            vector <const CallInst *> calllist;
            calllist.clear();

            cout << "1: Cur target potentially problematic func: " << func2name(func) << " for off: " << roff <<  endl;
            cout << "\t> dv: " << inst2str(dv) << " at  func: " << func2name(v2func(dv)) << endl;
            cout << "\t\t> idx: " << maxidx << endl;

            if(const Function *caller = recursiveToSrcFunc(func, offset, calllist, 2, maxidx)){    //2 = max depth
                set <const Value *> doneset;
                set <const Value *> tvset;
                vector <const Value *> srcvec = retSrcVec(calllist.back(), offset); 

                // Find matched one
                tvset.clear();
                for(int i = 0; i < srcvec.size(); i++){
                    const Value *tv = srcvec[i];
                    if(isVldrInst(tv)) tv = retPtrVal(tv);

                    unsigned off = gep2off(tv);
                    unsigned minidx, maxidx;
                    const Function *crfunc;
                    if(calllist.size()){
                        crfunc = v2func(calllist.back()); //getCallee(calllist.back());//v2func(calllist.back());
                        minidx = inst2idx(crfunc, tv);
                        maxidx = inst2idx(crfunc, calllist.back());
                    }
                    else{
                        crfunc = func;
                        minidx = inst2idx(crfunc, tv);
                        maxidx = inst2idx(crfunc, dv);
                    }

                    doneset.clear();
                    if(recursiveInterBacktrackFromArg(func, calllist, tv, dv, doneset, minidx, maxidx)){
                        tvset.insert(tv);
                    }
                }

                // Choose the closetest one 
                unsigned lidx = 0;
                const Value *tv = NULL;
                for(auto tit = tvset.begin(); tit != tvset.end(); ++tit){
                    const Value *v = *tit;
                    unsigned idx = inst2idx(v);
                    if(idx != 0 && lidx < idx){
                        lidx = idx;
                        tv = v;
                    }
                }
                if(!tv) cout << "\t> tv: NULL" << endl;
                else{
                    if(!gep2off(tv)) tv = retPtrOffPtr(tv);
                    cout << "\t> tv: " << inst2str(tv) << " at  func: " << func2name(v2func(tv)) << endl;
                    cout << "\t> dv: " << inst2str(dv) << " at  func: " << func2name(v2func(dv)) << endl;
                    cout << "\t\t> idx: " << lidx << endl;
                    cout << "\t\t> off: " << gep2off(tv) << endl;
                    inset.erase(dv);
                    inset.insert(tv);
                    ref2ctrlmap.erase(dv);
                    ref2ctrlmap[tv] = cobj;
                    ctrl2intervmap[cobj].erase(dv);
                    ctrl2intervmap[cobj].insert(tv);
                    cearg2csargptr[dv] = tv;
                }
            }
        }
    }
    // Step3: Caller exploration from missOut inst, while using inset. 
    //        If it meets the any variable inset (distace based), gather all variable and distance from that variable
    //        Then, backtrack to check the reachability for missOut
    //        - Direction: Sink(dv) (Caller) ->  Src(tv) (Callee)
    //        - Callist structure: 
    cout << "[DEBUG] outset sz: " << outset.size() << endl;
    cout << "[DEBUG] inset sz: " << inset.size() << endl;
    cout << "[DEBUG] offset sz: " << offset.size() << endl;
    for(auto it = offset.begin(); it != offset.end(); ++it){
        cout << "\t>>offset: " << *it << endl;
    }

    for(auto it = outset.begin(); it != outset.end(); ++it){
        const Value *tv = *it;
        const Function *func = v2func(tv);
        unsigned minidx;

        if(!tv) continue;
        else if(SVFUtil::dyn_cast<Instruction>(tv)) minidx = inst2idx(func, tv);
        else if(SVFUtil::dyn_cast<Argument>(tv)) minidx = 0;
        else assert(0);

        vector <const CallInst *> calllist;
        calllist.clear();

            //recursiveToSinkFunc: Check only direct caller 
        if(const Function *caller = recursiveToSinkFunc(func, offset, calllist, 2, minidx)){    //2 = max depth
            set <const Value *> doneset;
 
            if(!calllist.size()) continue;  // Sub bug
            for(auto oit = offset.begin(); oit != offset.end(); ++oit){
                unsigned off = *oit;
                // NOTE: Could be "set" Then sinkvec will be ordered for all "offset"
                vector <const Value *> sinkvec = retSinkVec(calllist.back(), off);
                bool found = false;
                // Calllist = Low idx -> callee/ High idx -> caller
                for(int i = 0; i < sinkvec.size(); i++){
                    const Value *dv = sinkvec[i];
                    if(isVstrInst(dv)) dv = retPtrVal(dv);

                    unsigned minidx, maxidx;
                    const Function *curfunc;
                    if(calllist.size()){
                        curfunc = v2func(calllist.back());
                        minidx = inst2idx(curfunc, calllist.back());
                        maxidx = inst2idx(curfunc, dv);
                    }
                    else{
                        curfunc = func;
                        minidx = inst2idx(curfunc, tv);
                        maxidx = inst2idx(curfunc, dv);
                    }

                    doneset.clear();
                    if(recursiveInterBacktrackToRet(curfunc, calllist, tv, dv, doneset, minidx, maxidx)){
                        ctrlinfo *inctrl = off2refctrl(off);//ref2ctrl(sinkvec[i]); //ref2ctrl(dv);
                        ctrlinfo *outctrl = missOut[tv];
                        if(inctrl == NULL || outctrl == NULL) break;

                        // update
                        outConn[outctrl] = inctrl;
                        inConn[inctrl] = outctrl;
                        outctrl->out = tv;
                        inctrl->in = sinkvec[i]; // dv;

                        // Used for Step 6 to indicate controllers
                        if(ctrl2intervmap.find(outctrl) == ctrl2intervmap.end()){
                            //ptr2floatMemAccess[retVSTRPtr(sinkvec[i])].insert(sinkvec[i]);
                            ctrl2intervmap[outctrl].insert(retVSTRPtr(sinkvec[i]));
                        }

                        // From controller perspective tv -> dv is correct although we used backtracking
                        cout << "\t\tFrom (out) " << inst2str(tv) << "  =>  To (in) " << inst2str(sinkvec[i]) << endl;
                        cout << "\t\t\tOut func " << func2name(outctrl->func) << " in func " << func2name(inctrl->func) << endl;
                        ceout2csoutptr[tv] = sinkvec[i];

                        if(outctrl->groundtruth.length()) cout << "\t\t\t\t>> (Out) GroundTruth: " << outctrl->groundtruth << endl;
                        if(inctrl->groundtruth.length()) cout << "\t\t\t\t>> (In) GroundTruth: " << inctrl->groundtruth << endl;

                        cout << "\t\t--------------------------------" << endl;
                        found = true;
                        break;
                    }
                }
                if(found) break;
            }
        }
    }

    return;
}


const Function *CtrlIdentifier::recursiveToSinkFunc(const Function *func, set <unsigned> &offset, vector <const CallInst *> &calllist, int depth, unsigned minidx){
    if(depth <= 0) return NULL;
    vector <const Instruction *> instvec = func2insts(func);
    binIRInit(func);

    // Inner check
    for(unsigned i = minidx; i < instvec.size(); i++){
        const Instruction *inst = instvec[i];
        if(isVstrInst(inst)){
            const Value *ptr = retVSTRPtr(inst);

            if(!ptr) continue;   
            //ptr2floatMemAccess[ptr].insert(inst);

            unsigned off = gep2off(ptr);
            if(offset.find(off) != offset.end()){
                return func;
            }
        }
    }

    // Caller check
    set <const CallInst *> csset = retCallSiteSet(func, module);
    if(csset.size()){
        const CallInst *ci = *(csset.begin());
        if(csset.size() == 2){
             ci = *(++(csset.begin()));
        }
        const Function *caller = v2func(ci); 
        binIRInit(caller);
        ipa->interAnalysis(ci);

        unsigned newminidx = inst2idx(caller, ci);
        calllist.push_back(ci);                    // call stack update
        return recursiveToSinkFunc(caller, offset, calllist, depth-1, newminidx);
    }
    else{
        return NULL;
    }
}

const Function *CtrlIdentifier::recursiveToSrcFunc(const Function *func, set<unsigned> &offset, vector <const CallInst *> &calllist, int depth, int maxidx){
    if(depth <= 0) return NULL;
    vector <const Instruction *> instvec = func2insts(func);
    binIRInit(func);

    // Inner check
    for(int i = maxidx - 1; i >= 0; i--){
        const Instruction *inst = instvec[i];
        if(isVldrInst(inst)){
            const Value *ptr = retVLDRPtr(inst);
            if(!ptr) continue;
            //ptr2floatMemAccess[ptr].insert(inst);

            unsigned off = gep2off(ptr);
            if(offset.find(off) != offset.end()){
                return func;
            }
        }
    }

    // Caller check
    set <const CallInst *> csset = retCallSiteSet(func, module);
    if(csset.size()){
        const CallInst *ci = *(csset.begin());
        const Function *caller = v2func(ci); 
        binIRInit(caller);
        ipa->interAnalysis(ci);

        unsigned newmaxidx = inst2idx(caller, ci);
        calllist.push_back(ci);                    // call stack update
        return recursiveToSrcFunc(caller, offset, calllist, depth-1, newmaxidx);
    }
    else{
        return NULL;
    }
}

vector <const Value *> CtrlIdentifier::retSinkVec(const CallInst *ci, set <unsigned> &offset){
    vector <const Value *> orderedvec;
    const Function *func = v2func(ci);
    vector <const Instruction *> instvec = func2insts(func);
    unsigned minidx = inst2idx(func, ci);
    set <const Instruction *> tvset;

    // Gather all inst
    for(unsigned i = minidx; i < instvec.size(); i++){
        const Instruction *inst = instvec[i];
        if(isVstrInst(inst)){
            const Value *ptr = retVSTRPtr(inst);

            if(!ptr) continue; 

            unsigned off = gep2off(ptr);
            if(offset.find(off) != offset.end()){
                tvset.insert(inst);
            }
        }
    }

    // ordering 
    map <unsigned, const Value *> ordermap;
    for(auto it = tvset.begin(); it != tvset.end(); ++it){
        const Instruction *inst = *it;
        unsigned idx = inst2idx(func, inst);
        ordermap[idx] = inst;
    }

    for(auto it = ordermap.begin(); it != ordermap.end(); ++it){
        const Value *v = it->second;
        orderedvec.push_back(v);
    }

    return orderedvec;
}

vector <const Value *> CtrlIdentifier::retSinkVec(const CallInst *ci, unsigned offset){
    vector <const Value *> orderedvec;
    const Function *func = v2func(ci);
    vector <const Instruction *> instvec = func2insts(func);
    unsigned minidx = inst2idx(func, ci);
    set <const Instruction *> tvset;

    // Gather all inst
    for(unsigned i = minidx; i < instvec.size(); i++){
        const Instruction *inst = instvec[i];
        if(isVstrInst(inst)){
            const Value *ptr = retVSTRPtr(inst);

            if(!ptr) continue;  
            //ptr2floatMemAccess[ptr].insert(inst);

            unsigned off = gep2off(ptr);
            if(offset == off){
                tvset.insert(inst);
            }
        }
    }

    // ordering 
    map <unsigned, const Value *> ordermap;
    for(auto it = tvset.begin(); it != tvset.end(); ++it){
        const Instruction *inst = *it;
        unsigned idx = inst2idx(func, inst);
        ordermap[idx] = inst;
    }

    for(auto it = ordermap.begin(); it != ordermap.end(); ++it){
        const Value *v = it->second;
        orderedvec.push_back(v);
    }

    return orderedvec;
}


vector <const Value *> CtrlIdentifier::retSrcVec(const CallInst *ci, set <unsigned> &offset){
    vector <const Value *> orderedvec;
    const Function *func = v2func(ci);
    vector <const Instruction *> instvec = func2insts(func);
    unsigned maxidx = inst2idx(func, ci);
    set <const Instruction *> tvset;

    // Gather all inst
    for(int i = maxidx; i >= 0; i--){
        const Instruction *inst = instvec[i];
        if(isVldrInst(inst)){
            const Value *ptr = retVLDRPtr(inst);

            if(!ptr) continue; 

            unsigned off = gep2off(ptr);
            if(offset.find(off) != offset.end()){
                tvset.insert(inst);
            }
        }
    }

    // ordering 
    map <unsigned, const Value *> ordermap;
    for(auto it = tvset.begin(); it != tvset.end(); ++it){
        const Instruction *inst = *it;
        unsigned idx = inst2idx(func, inst);
        ordermap[idx] = inst;
    }

    for(auto it = ordermap.begin(); it != ordermap.end(); ++it){
        const Value *v = it->second;
        orderedvec.push_back(v);
    }

    return orderedvec;
}

vector <const Value *> CtrlIdentifier::retSrcVec(const CallInst *ci, unsigned offset){
    vector <const Value *> orderedvec;
    const Function *func = v2func(ci);
    vector <const Instruction *> instvec = func2insts(func);
    unsigned maxidx = inst2idx(func, ci);
    set <const Instruction *> tvset;

    // Gather all inst
    for(int i = maxidx; i >= 0; i--){
        const Instruction *inst = instvec[i];
        if(isVldrInst(inst)){
            const Value *ptr = retVLDRPtr(inst);

            if(!ptr) continue;

            unsigned off = gep2off(ptr);
            if(offset == off){
                tvset.insert(inst);
            }
        }
    }

    // ordering 
    map <unsigned, const Value *> ordermap;
    for(auto it = tvset.begin(); it != tvset.end(); ++it){
        const Instruction *inst = *it;
        unsigned idx = inst2idx(func, inst);
        ordermap[idx] = inst;
    }

    for(auto it = ordermap.begin(); it != ordermap.end(); ++it){
        const Value *v = it->second;
        orderedvec.push_back(v);
    }

    return orderedvec;
}

CtrlIdentifier::ctrlinfo * CtrlIdentifier::ref2ctrl(const Value *v){
    ctrlinfo *ci = NULL;
    unsigned off = gep2off(v);
    assert(off != 0xFFFFFFFF);

    for(auto it = ref2ctrlmap.begin(); it != ref2ctrlmap.end(); ++it){
        const Value *ref = it->first;
        unsigned roff = gep2off(ref);

        assert(off != 0xFFFFFFFF);
        if(roff == off){
            ci = it->second;
            break;
        }
    }

    return ci;
}

CtrlIdentifier::ctrlinfo * CtrlIdentifier::off2refctrl(unsigned off){
    if(!off) return NULL;
    ctrlinfo *ci = NULL;
    for(auto it = ref2ctrlmap.begin(); it != ref2ctrlmap.end(); ++it){
        const Value *ref = it->first;
        unsigned roff = gep2off(ref);
        if(roff == off){
            ci = it->second;
            break;
        }
    }

    return ci;
}


bool CtrlIdentifier::recursiveInterBacktrackToRet(const Function *func, vector <const CallInst *> &calllist, const Value *tv, const Value *dv, set<const Value *> &doneset, unsigned minidx, unsigned maxidx){
    // Src (Callee) tv - executed first <- Sink (Caller) dv - executed last
    //  0. This is backgracking, dv -> tv  = cr -> ce
    //  1. tv = keeping changing while going to the caller
    //  2. calllist != stack = high idx -> low idx
    //  3. Calllist = Low idx -> callee/ High idx -> caller
    if(dv == NULL || tv == NULL) return false;
    else if(tv == dv){
        return true;
    }
    else if(doneset.find(dv) != doneset.end()) return false;
    doneset.insert(dv);

    if(calllist.size()){
        const CallInst *ci = calllist.back();
        uint64_t csaddr = inst2addr(ci);
        set <const Value *> csrets = ipa->retCSAddr2RetCSVs(csaddr);
        assert(csaddr);

        if(csrets.find(dv) != csrets.end()){        // Caller check, its ret found-> Callee's ret
            const Value *cev = ipa->retCERetFromCSRet(csaddr, dv);
            const Function *crfunc = v2func(ci);
            const Function *cefunc = getCallee(ci);
            unsigned newminidx, newmaxidx;

            // Callee = Upper stack item = callllist.[back() - 1] 
            if(calllist.size() > 1)         newminidx = inst2idx(cefunc, calllist[calllist.size() - 2]);   // not happen
            else if(calllist.size() == 1)   newminidx = inst2idx(cefunc, tv);
            else assert(0);                 // Impossible
            newmaxidx = inst2idx(cefunc, cev);

            bool res = false;
            calllist.pop_back();
            res = recursiveInterBacktrackToRet(cefunc, calllist, tv, cev, doneset, newminidx, newmaxidx);//or newcalllist
            calllist.push_back(ci); // Recovery
            return res;
        }
    }

    if(!SVFUtil::dyn_cast<Instruction>(dv)) return false;  // Base case (non-inst)
    else if(isLoadInst(dv) || isVldrInst(dv)){        // Not sure it is necessary
        return recursiveInterBacktrackToRet(func, calllist, tv, retPtrPtr(dv), doneset, minidx, maxidx);
    }
    else if(isBinOpInst(dv) || isCastInst(dv)){  
        const Instruction *inst = SVFUtil::dyn_cast<Instruction>(dv);
        for(unsigned i = 0; i < inst->getNumOperands(); i++){
            if(recursiveInterBacktrackToRet(func, calllist, tv, inst->getOperand(i), doneset, minidx, maxidx)) return true;
        }
    }

    return false;
}


bool CtrlIdentifier::recursiveInterBacktrackFromArg(const Function *func, vector <const CallInst *> &calllist, const Value *tv, const Value *dv, set<const Value *> &doneset, unsigned minidx, unsigned maxidx){
    // Src (Callee) tv - executed first <- Sink (Caller) dv - executed last
    //  0. This is backgracking, dv -> tv  = cr -> ce
    //  1. tv = keeping changing while going to the caller
    //  2. calllist != stack = high idx -> low idx
    //  3. Calllist = Low idx -> callee/ High idx -> caller
    if(dv == NULL || tv == NULL) return false;
    else if(tv == dv) return true;
    else if(doneset.find(dv) != doneset.end()) return false;
    doneset.insert(dv);

    if(calllist.size()){
        const CallInst *ci = calllist.back();//front();
        uint64_t csaddr = inst2addr(ci);
        ipa->interAnalysis(ci);
        set <const Value *> ceargs = ipa->retCSAddr2ArgCEVs(csaddr);
        assert(csaddr);

        if(ceargs.find(dv) != ceargs.end()){        // Caller check, its ret found-> Callee's ret
            const Value *crv = ipa->retCSArgFromCEArg(csaddr, dv);
            const Function *crfunc = v2func(ci);
            unsigned newminidx, newmaxidx;

            // Callee = Upper stack item = callllist.[back() - 1] 
            if(calllist.size() > 1)         newmaxidx = inst2idx(crfunc, calllist[calllist.size() - 2]);   // not happen
            else if(calllist.size() == 1)   newmaxidx = inst2idx(crfunc, crv);
            else assert(0);                 // Impossible
            newminidx = inst2idx(crfunc, tv);

            bool res = false;
            calllist.pop_back();
            res = recursiveInterBacktrackFromArg(crfunc, calllist, tv, crv, doneset, newminidx, newmaxidx);//or newcalllist 
            calllist.push_back(ci); // Recovery
            if(res) return res;
        }
    }

    if(!SVFUtil::dyn_cast<Instruction>(dv)) return false;  // Base case (non-inst)
    else if(isLoadInst(dv)){
        return recursiveInterBacktrackFromArg(func, calllist, tv, retPtrPtr(dv), doneset, minidx, maxidx);
    }
    else if(isVldrInst(dv)){
        return recursiveInterBacktrackFromArg(func, calllist, tv, retVLDRPtr(dv), doneset, minidx, maxidx);
    }
    else if(isBinOpInst(dv) || isCastInst(dv)){   
        const Instruction *inst = SVFUtil::dyn_cast<Instruction>(dv);
        for(unsigned i = 0; i < inst->getNumOperands(); i++){
            if(recursiveInterBacktrackFromArg(func, calllist, tv, inst->getOperand(i), doneset, minidx, maxidx)) return true;
        }
    }

    return false;
}




//////////////////////////////////////////////
//######################### Step 6
//////////////////////////////////////////////////////////
void CtrlIdentifier::roleConfirm(){
    //pid->pidInfoMap //Key: axis (x,y,z, (_p,_v, _a) ar, ap, ay (_p, _v, _a)) <-> Mapped: AxisInfo

    map <string, ctrlinfo *> foundmap; // top_x, bot_x, ... top_ar, bot_ar ....
    set <ctrlinfo *> taggedctrls;

    // Step 1: Lowest controller guess (w/ Transition + # of primitive controllers) 
    vector <ctrlinfo*> xycandid;    // bottom to top 
    vector <ctrlinfo*> arpcandid;   // top to bottom
    map <string, set <ctrlinfo*>> type2ctrlset; // P, PI, PID, PIDFF, BOTCANDID (BOTCANDID can be either PID and PIDFF)

    for(auto it = ctrl2intervmap.begin(); it != ctrl2intervmap.end(); ++it){
        ctrlinfo *cobj = it->first;

        if(outConn.find(cobj) != outConn.end() && outConn[cobj]->groundtruth.length() > 0) 
            cout << "   >> outconn: " << outConn[cobj]->groundtruth << endl;
        if(inConn.find(cobj) != inConn.end() && inConn[cobj]->groundtruth.length() > 0) 
            cout << "   >> inconn: " << inConn[cobj]->groundtruth << endl;
        if(outTrans.find(cobj) != outTrans.end()){
            for(auto cit = outTrans[cobj].begin(); cit != outTrans[cobj].end(); ++cit){
                if((*cit)->groundtruth.length() > 0)
                    cout << "   >> outtrans: " << (*cit)->groundtruth << endl;
            }
        }
        if(inTrans.find(cobj) != inTrans.end()){
            for(auto cit = inTrans[cobj].begin(); cit != inTrans[cobj].end(); ++cit){
                if((*cit)->groundtruth.length() > 0)
                    cout << "   >> intrans: " << (*cit)->groundtruth << endl;
            }
        }
        // Only for xy
        if(outConn.find(cobj) == outConn.end() && outTrans.find(cobj) != outTrans.end()){ // = out to transition formula == inTrans
            xycandid.push_back(cobj);
            if(cobj->groundtruth.length() > 0)
                cout << "> xy: " << cobj->groundtruth << endl;
        }

        // Only for roll pitch
        if(inConn.find(cobj) == inConn.end() && inTrans.find(cobj) != inTrans.end()){
            arpcandid.push_back(cobj);
            if(cobj->groundtruth.length() > 0)
                cout << "> arp: " << cobj->groundtruth << endl;
        }

        // Type classification
        type2ctrlset[cobj->pidty].insert(cobj);
        if(cobj->pidty == "PID" || cobj->pidty == "PIDFF"){
            type2ctrlset["BOTCANDID"].insert(cobj);
        }
    }

            // Offset guess
    if(xycandid.size() == 1 && retNumInOff(xycandid[0]) == 2){
        if(hasXYCtrlStructFromBot(xycandid[0])){     // if transition logic can determine it
            tagAxisFromBot(xycandid[0], "xy", taggedctrls);
            foundmap["top_x"] = botctrl2topctrl(xycandid[0]);
            foundmap["bot_x"] = xycandid[0];
            foundmap["top_y"] = botctrl2topctrl(xycandid[0]);
            foundmap["bot_y"] = xycandid[0];
        }
        else{
            tagCtrl(xycandid[0], "xy", pid->pidInfoMap["xy"].getNumCtrls() - 1, taggedctrls);
            foundmap["bot_x"] = xycandid[0];
            foundmap["bot_y"] = xycandid[0];
        }
    }
    else if(xycandid.size() == 2 && retNumInOff(xycandid[0]) == 1 && retNumInOff(xycandid[1]) == 1){
        if(hasXCtrlStructFromBot(xycandid[1])){     // if transition logic can determine it
            tagAxisFromBot(xycandid[1], "x", taggedctrls);
            tagAxisFromBot(xycandid[0], "y", taggedctrls);
            foundmap["top_x"] = botctrl2topctrl(xycandid[1]);
            foundmap["bot_x"] = xycandid[1];
            foundmap["top_y"] = botctrl2topctrl(xycandid[0]);
            foundmap["bot_y"] = xycandid[0];
        }
        else if(hasXCtrlStructFromBot(xycandid[0])){        // if transition logic can determine it
            tagAxisFromBot(xycandid[0], "x", taggedctrls);
            tagAxisFromBot(xycandid[1], "y", taggedctrls);
            foundmap["top_x"] = botctrl2topctrl(xycandid[0]);
            foundmap["bot_x"] = xycandid[0];
            foundmap["top_y"] = botctrl2topctrl(xycandid[1]);
            foundmap["bot_y"] = xycandid[1];
        }
        else if(getInOff(xycandid[0]) > getInOff(xycandid[1])){
            if(hasCtrlStructFromBot(xycandid[1], "x") && hasCtrlStructFromBot(xycandid[0], "y")){
                tagAxisFromBot(xycandid[1], "x", taggedctrls);
                tagAxisFromBot(xycandid[0], "y", taggedctrls);
                foundmap["top_x"] = botctrl2topctrl(xycandid[1]);
                foundmap["bot_x"] = xycandid[1];
                foundmap["top_y"] = botctrl2topctrl(xycandid[0]);
                foundmap["bot_y"] = xycandid[0];
            }
            else{
                tagCtrl(xycandid[1], "x", pid->pidInfoMap["x"].getNumCtrls() - 1, taggedctrls);
                tagCtrl(xycandid[0], "y", pid->pidInfoMap["y"].getNumCtrls() - 1, taggedctrls);
                foundmap["bot_x"] = xycandid[1];
                foundmap["bot_y"] = xycandid[0];
            }
        }
        else{
            if(hasCtrlStructFromBot(xycandid[0], "x") && hasCtrlStructFromBot(xycandid[1], "y")){
                tagAxisFromBot(xycandid[0], "x", taggedctrls);
                tagAxisFromBot(xycandid[1], "y", taggedctrls);
                foundmap["top_x"] = botctrl2topctrl(xycandid[0]);
                foundmap["bot_x"] = xycandid[0];
                foundmap["top_y"] = botctrl2topctrl(xycandid[1]);
                foundmap["bot_y"] = xycandid[1];
            }
            else{
                tagCtrl(xycandid[0], "x", pid->pidInfoMap["x"].getNumCtrls() - 1, taggedctrls);
                tagCtrl(xycandid[1], "y", pid->pidInfoMap["y"].getNumCtrls() - 1, taggedctrls);
                foundmap["bot_x"] = xycandid[0];
                foundmap["bot_y"] = xycandid[1];
            }
        }
    }

    if(arpcandid.size() == 2 && retNumInOff(arpcandid[0]) == 1 && retNumInOff(arpcandid[1]) == 1){
        if(hasARCtrlStructFromTop(arpcandid[1])){   // if transition logic can determine it
            tagAxisFromTop(arpcandid[0], "ar", taggedctrls);
            tagAxisFromTop(arpcandid[1], "ap", taggedctrls);
            foundmap["top_ar"] = arpcandid[0];
            foundmap["bot_ar"] = topctrl2botctrl(arpcandid[0]);
            foundmap["top_ap"] = arpcandid[1];
            foundmap["bot_ap"] = topctrl2botctrl(arpcandid[1]);
        }
        else if(hasARCtrlStructFromTop(arpcandid[1])){      // if transition logic can determine it
            tagAxisFromTop(arpcandid[1], "ar", taggedctrls);
            tagAxisFromTop(arpcandid[0], "ap", taggedctrls);
            foundmap["top_ar"] = arpcandid[1];
            foundmap["bot_ar"] = topctrl2botctrl(arpcandid[1]);
            foundmap["top_ap"] = arpcandid[0];
            foundmap["bot_ap"] = topctrl2botctrl(arpcandid[0]);
        }
        else if(getInOff(arpcandid[0]) > getInOff(arpcandid[1])){
            if(hasCtrlStructFromTop(arpcandid[1], "ar") && hasCtrlStructFromTop(arpcandid[0], "ap")){
                tagAxisFromTop(arpcandid[1], "ar", taggedctrls);
                tagAxisFromTop(arpcandid[0], "ap", taggedctrls);
                foundmap["top_ar"] = arpcandid[1];
                foundmap["bot_ar"] = topctrl2botctrl(arpcandid[1]);
                foundmap["top_ap"] = arpcandid[0];
                foundmap["bot_ap"] = topctrl2botctrl(arpcandid[0]);
            }
            else{
                tagCtrl(arpcandid[1], "ar", 0, taggedctrls);
                tagCtrl(arpcandid[0], "ap", 0, taggedctrls);
                foundmap["top_ar"] = arpcandid[1];
                foundmap["top_ap"] = arpcandid[0];
            }
        }
        else{
            if(hasCtrlStructFromTop(arpcandid[0], "ar") && hasCtrlStructFromTop(arpcandid[1], "ap")){
                tagAxisFromTop(arpcandid[0], "ar", taggedctrls);
                tagAxisFromTop(arpcandid[1], "ap", taggedctrls);
                foundmap["top_ar"] = arpcandid[0];
                foundmap["bot_ar"] = topctrl2botctrl(arpcandid[0]);
                foundmap["top_ap"] = arpcandid[1];
                foundmap["bot_ap"] = topctrl2botctrl(arpcandid[1]);
            }
            else{
                tagCtrl(arpcandid[0], "ar", 0, taggedctrls);
                tagCtrl(arpcandid[1], "ap", 0, taggedctrls);
                foundmap["top_ar"] = arpcandid[0];
                foundmap["top_ap"] = arpcandid[1];
            }
        }
    }

        // for z, yaw
    for(auto it = type2ctrlset["BOTCANDID"].begin(); it != type2ctrlset["BOTCANDID"].end(); ++it){// have 6 candids even w/o filter
        ctrlinfo* ci = *it;
            // z
        if(hasCtrlStructFromBot(ci, "ay") == false &&  hasXYCtrlStructFromBot(ci) == false && hasCtrlStructFromBot(ci, "x") == false
            && foundmap.find("bot_x") != foundmap.end() && foundmap.find("bot_y") != foundmap.end()
            && taggedctrls.find(ci) == taggedctrls.end()){
            if(hasCtrlStructFromBot(ci, "z") == true){
                tagAxisFromBot(ci, "z", taggedctrls);
                foundmap["top_z"] = botctrl2topctrl(ci);
                foundmap["bot_z"] = ci;
            }
            else if((foundmap.find("bot_ar") != foundmap.end() && foundmap.find("bot_ap") != foundmap.end()) 
                 || (foundmap.find("bot_y") != foundmap.end() && getInOff(foundmap["bot_y"]) + 4 == getInOff(ci)) 
                ){
                tagCtrl(ci, "z", pid->pidInfoMap["z"].getNumCtrls() - 1, taggedctrls);
                foundmap["bot_z"] = ci;
            }
        }
            // ay 
        else if(hasCtrlStructFromBot(ci, "x") == false && hasXYCtrlStructFromBot(ci) == false 
            && taggedctrls.find(ci) == taggedctrls.end()
            && (hasCtrlStructFromBot(ci, "ay") == true || 
             (foundmap.find("bot_ar") != foundmap.end() && foundmap.find("bot_ap") != foundmap.end()))){
            if(hasCtrlStructFromBot(ci, "ay") == true){
                tagAxisFromBot(ci, "ay", taggedctrls);
                foundmap["top_ay"] = botctrl2topctrl(ci);
                foundmap["bot_ay"] = ci;
            }
            else if((foundmap.find("bot_x") != foundmap.end() && foundmap.find("bot_y") != foundmap.end()) 
                 || (foundmap.find("bot_r") != foundmap.end() && getInOff(foundmap["bot_p"]) + 4 == getInOff(ci)) 
                ){
                tagCtrl(ci, "ay", pid->pidInfoMap["ay"].getNumCtrls() - 1, taggedctrls);
                foundmap["bot_ay"] = ci;
            }
        }
    }

    // Step 2: Check worst case (No XYZ, No RPY) -> if (XYZ PID and RPY PIDFF)  
    if(type2ctrlset["BOTCANDID"].size() >= 6 && type2ctrlset["BOTCANDID"].size() <= 9){
        // map in offset
        ctrlinfo* off0 = NULL;
        ctrlinfo* off4 = NULL;
        ctrlinfo* off8 = NULL;
        bool rpy = false;
        bool xyz = false;
        for(auto it = type2ctrlset["BOTCANDID"].begin(); it != type2ctrlset["BOTCANDID"].end(); ++it){
            ctrlinfo *ci = *it;
            if(hasOffCtrl(ci, -4, type2ctrlset["BOTCANDID"]) == false 
                && hasOffCtrl(ci, 4, type2ctrlset["BOTCANDID"]) == true 
                && hasOffCtrl(ci, 8, type2ctrlset["BOTCANDID"]) == true){
                ctrlinfo *ci4 = retOffCtrl(ci, 4, type2ctrlset["BOTCANDID"]);
                ctrlinfo *ci8 = retOffCtrl(ci, 8, type2ctrlset["BOTCANDID"]);

                if(ci->role.length() > 0 || ci4->role.length() > 0 || ci8->role.length() > 0){
                    if(ci->role.find("r") == 0 || ci4->role.find("p") == 0 || ci8->role.find("y") == 0){
                        if(taggedctrls.find(ci) != taggedctrls.end()) 
                            tagCtrl(ci, "ar", pid->pidInfoMap["ar"].getNumCtrls() - 1, taggedctrls);
                        if(taggedctrls.find(ci4) != taggedctrls.end()) 
                            tagCtrl(ci4, "ap", pid->pidInfoMap["ap"].getNumCtrls() - 1, taggedctrls);
                        if(taggedctrls.find(ci8) != taggedctrls.end()) 
                            tagCtrl(ci8, "ay", pid->pidInfoMap["ay"].getNumCtrls() - 1, taggedctrls);
                        rpy = true;
                    }
                    else if(ci->role.find("x") == 0 || ci4->role.find("y") == 0 || ci8->role.find("z") == 0){
                        if(taggedctrls.find(ci) != taggedctrls.end()) 
                            tagCtrl(ci, "x", pid->pidInfoMap["x"].getNumCtrls() - 1, taggedctrls);
                        if(taggedctrls.find(ci4) != taggedctrls.end()) 
                            tagCtrl(ci4, "y", pid->pidInfoMap["y"].getNumCtrls() - 1, taggedctrls);
                        if(taggedctrls.find(ci8) != taggedctrls.end()) 
                            tagCtrl(ci8, "z", pid->pidInfoMap["z"].getNumCtrls() - 1, taggedctrls);
                        xyz = true;      
                    }
                    else if(ci->role.find("xy") == 0 || ci8->role.find("z") == 0){  
                        if(taggedctrls.find(ci) != taggedctrls.end()) 
                            tagCtrl(ci, "xy", pid->pidInfoMap["xy"].getNumCtrls() - 1, taggedctrls);
                        if(taggedctrls.find(ci8) != taggedctrls.end()) 
                            tagCtrl(ci8, "z", pid->pidInfoMap["z"].getNumCtrls() - 1, taggedctrls);
                        xyz = true;      
                    }
                    else if(ci->role.find("xy") == 0 || ci4->role.find("z") == 0){  
                        if(taggedctrls.find(ci) != taggedctrls.end()) 
                            tagCtrl(ci, "xy", pid->pidInfoMap["xy"].getNumCtrls() - 1, taggedctrls);
                        if(taggedctrls.find(ci4) != taggedctrls.end()) 
                            tagCtrl(ci4, "z", pid->pidInfoMap["z"].getNumCtrls() - 1, taggedctrls);
                        xyz = true;      
                    }
                }
                else{
                    off0 = ci;
                    off4 = ci4;
                    off8 = ci8;
                }
            }
        }
        // 1 found 1 not found
        if((rpy == true && xyz == false)){
                // X
            if(hasCtrlStructFromBot(off0, "x")) tagAxisFromBot(off0, "x", taggedctrls); 
            else  tagCtrl(off0, "x", pid->pidInfoMap["x"].getNumCtrls() - 1, taggedctrls);
                // Y
            if(hasCtrlStructFromBot(off4, "y")) tagAxisFromBot(off4, "y", taggedctrls);
            else  tagCtrl(off4, "y", pid->pidInfoMap["y"].getNumCtrls() - 1, taggedctrls);
                // Z
            if(hasCtrlStructFromBot(off8, "z")) tagAxisFromBot(off8, "z", taggedctrls);
            else  tagCtrl(off8, "z", pid->pidInfoMap["z"].getNumCtrls() - 1, taggedctrls);
        }
        else if(rpy == false && xyz == true){
                // Roll
            if(hasCtrlStructFromBot(off0, "ar")) tagAxisFromBot(off0, "ar", taggedctrls);
            else  tagCtrl(off0, "ar", pid->pidInfoMap["ar"].getNumCtrls() - 1, taggedctrls);
                // Pitch 
            if(hasCtrlStructFromBot(off4, "ap")) tagAxisFromBot(off4, "ap", taggedctrls);
            else  tagCtrl(off4, "ap", pid->pidInfoMap["ap"].getNumCtrls() - 1, taggedctrls);
                // Yaw
            if(hasCtrlStructFromBot(off8, "ay")) tagAxisFromBot(off8, "ay", taggedctrls);
            else  tagCtrl(off8, "ay", pid->pidInfoMap["ay"].getNumCtrls() - 1, taggedctrls);
        }
    }

        //  -> try the others as well if they are connected to each other
    if(ctrlset.size() - taggedctrls.size() > 0){
        set <string> untagged = retUntaggedCtrl(taggedctrls);
        set <ctrlinfo*> untaggedctrlset;
        for(auto it = ctrlset.begin(); it != ctrlset.end(); ++it){
            ctrlinfo *ci = *it;
            if(taggedctrls.find(ci) == taggedctrls.end()){
                untaggedctrlset.insert(ci);
            }
        }

        if(untagged.size() == 1 && untaggedctrlset.size() == 1){
            tagCtrl(*(untaggedctrlset.begin()), *(untagged.begin()), taggedctrls);
        }
        else if(untaggedctrlset.size() <= 3){
            for(auto it = untaggedctrlset.begin(); it != untaggedctrlset.end(); ++it){
                ctrlinfo *ci = *it;
                string prefix = retPrefix_SameLevelCtrlTy(untagged);
                string postfix = retPostfix_SameLevelCtrlTy(untagged);
                if(postfix.length() > 0){   // prefix can be "" 
                    if(hasOffCtrl(ci, -4, ctrlset) == false 
                        && hasOffCtrl(ci, 4, ctrlset) == true 
                        && hasOffCtrl(ci, 8, ctrlset) == true 
                        && hasOffCtrl(ci, 12, ctrlset) == false){   // rpy or xyz
                        ctrlinfo *ci4 = retOffCtrl(ci, 4, ctrlset);
                        ctrlinfo *ci8 = retOffCtrl(ci, 8, ctrlset);   

                        if(taggedctrls.find(ci) != taggedctrls.end()){
                            string name;
                            if(prefix.find("a") == 0)   name = prefix + "r" + postfix;
                            else                        name = "x" + postfix;
                            tagCtrl(ci, name, taggedctrls);
                        }
                        if(taggedctrls.find(ci4) != taggedctrls.end()){
                            string name;
                            if(prefix.find("a") == 0)   name = prefix + "p" + postfix;
                            else                        name = "y" + postfix;
                            tagCtrl(ci4, name, taggedctrls);
                        }
                        if(taggedctrls.find(ci8) != taggedctrls.end()) {
                            string name;
                            if(prefix.find("a") == 0)   name = prefix + "y" + postfix;
                            else                        name = "z" + postfix;
                            tagCtrl(ci8, name, taggedctrls);
                        }
                    }
                    else if(hasOffCtrl(ci, -4, ctrlset) == true 
                        && hasOffCtrl(ci, 4, ctrlset) == true 
                        && hasOffCtrl(ci, 8, ctrlset) == false
                        && hasOffCtrl(ci, 12, ctrlset)){          //py or yz: Otherwise, it should be handled in the above
                        ctrlinfo *ci4 = retOffCtrl(ci, 4, ctrlset);
                        if(taggedctrls.find(ci) != taggedctrls.end()){
                            string name;
                            if(prefix.find("a") == 0)   name = prefix + "p" + postfix;
                            else                        name = "y" + postfix;
                            tagCtrl(ci, name, taggedctrls);
                        }
                        if(taggedctrls.find(ci4) != taggedctrls.end()) {
                            string name;
                            if(prefix.find("a") == 0)   name = prefix + "y" + postfix;
                            else                        name = "z" + postfix;
                            tagCtrl(ci4, name, taggedctrls);
                        }                   
                    }
                }
            }
        }
    }

        // Otherwise -> fail....
    set <string> finaluntagged = retUntaggedCtrl(taggedctrls);
    if(finaluntagged.size()){
        cout << "Failed to find all of the controllers" << endl;
        for(auto it = finaluntagged.begin(); it != finaluntagged.end(); ++it){
            cout << "Untagged: " << *it << endl;
        }
        for(auto cit = taggedctrls.begin(); cit != taggedctrls.end(); ++cit){
            ctrlinfo *ci = *cit;
            cout << " Tagged: " << ci->role << endl;
        }
    }
    else{
        cout << "Perfect!" << endl;
    }

    // final variable determination
    for(auto vit = ctrl2intervmap.begin(); vit != ctrl2intervmap.end(); ++vit){
        ctrlinfo *ci = vit->first;
        set <const Value *> vset = vit->second;
        cout << "-----------" << endl;

        set <const Value *> refset, errset, curset, outset;
        refset.clear();
        curset.clear();
        errset.clear();
        outset.clear();

        cout << "> ci: " << ci->role << " w/ refs sz: " << ci->refs.size() << endl;
        for(auto sit = ci->refs.begin(); sit != ci->refs.end(); ++sit){
            const Value *v = *sit;
            if(!v) continue;
            if(alias2ref.find(v) != alias2ref.end()){
                cout << " > REF PTR\t: " << inst2str(v) << endl;
                if(const Value *finst = refptr2vldr(v, v, ci)){
                    cout << "\t> Float Inst\t: " << inst2str(finst) << endl;
                    refset.insert(finst);
                    //ctrl2finalref[ci] = finst;
                }
            }
            else if(ref2ctrlmap.find(v) != ref2ctrlmap.end()){
                cout << " > REF\t: " << inst2str(v) << endl;
                if(const Value *finst = refptr2vldr(v, v, ci)){
                    cout << "\t> Float Inst\t: " << inst2str(finst) << endl;
                    refset.insert(finst);
                    //ctrl2finalref[ci] = finst;
                }
                else if(SVFUtil::dyn_cast<Argument>(v) == NULL && isHeuriArg(v) == false){
                    refset.insert(v);
                    //ctrl2finalref[ci] = v;
                }
                else{
                    if(ctrlinfomap[ci]->ref.length() > 0
                            && ctrlinfomap[ci]->ref.find("S") == string::npos && ctrlinfomap[ci]->ref.find("arg") == string::npos){
                        uint64_t addr = stoull(ctrlinfomap[ci]->ref, nullptr, 16);
                        vector <const Instruction *> instvec = retInstvecFromAddr(addr);
                        for(int i = 0; i < instvec.size(); i++){
                            const Instruction *inst = instvec[i];
                            if(isVldrInst(inst)){
                                refset.insert(inst);
                                break;
                            }
                        }
                    }
                }
            }
            else{
                cout << " > Unknown\t: " << inst2str(v) << endl;
                if(const Value *finst = refptr2vldr(v, v, ci)){
                    cout << "\t> Float Inst\t: " << inst2str(finst) << endl;
                    refset.insert(finst);
                    //ctrl2finalref[ci] = finst;
                }
                else if(SVFUtil::dyn_cast<Argument>(v) == NULL && isHeuriArg(v) == false){
                    refset.insert(v);
                    //ctrl2finalref[ci] = v;
                }
                else{
                    if(ctrlinfomap[ci]->ref.length() > 0
                            && ctrlinfomap[ci]->ref.find("S") == string::npos && ctrlinfomap[ci]->ref.find("arg") == string::npos){
                        uint64_t addr = stoull(ctrlinfomap[ci]->ref, nullptr, 16);
                        vector <const Instruction *> instvec = retInstvecFromAddr(addr);
                        for(int i = 0; i < instvec.size(); i++){
                            const Instruction *inst = instvec[i];
                            if(isVldrInst(inst)){
                                refset.insert(inst);
                                break;
                            }
                        }
                    }
                }
            }
        }
        
        if(refset.size() == 1){
            ctrl2finalref[ci] = *(refset.begin());
        }
        else if(refset.size() == 2){
            const Value *v1 = *(refset.begin());
            const Value *v2 = *(++(refset.begin()));
            if(retVMEMPtr(v1) != NULL && retVMEMPtr(v2) != NULL && gep2off(retVMEMPtr(v1)) < gep2off(retVMEMPtr(v2))){
                ctrl2finalref[ci] = v1;
                ctrl2finalref2[ci] = v2;
            }
            else if(retVMEMPtr(v1) != NULL && retVMEMPtr(v2) != NULL && gep2off(retVMEMPtr(v1)) > gep2off(retVMEMPtr(v2))){
                ctrl2finalref[ci] = v2;
                ctrl2finalref2[ci] = v1;
            }
            else{
                ctrl2finalref[ci] = v1;
                ctrl2finalref2[ci] = v2;
            }
        }

        cout << "> ci: " << ci->role << " w/ curs sz: " << ci->curs.size() << endl;
        for(auto sit = ci->curs.begin(); sit != ci->curs.end(); ++sit){
            const Value *v = *sit;
            if(!v) continue;
            cout << " > Unknown\t: " << inst2str(v) << " at func: " << v2func(v)->getName().str() << endl;
            if(const Value *finst = curptr2vldr(v, v, ci)){
                cout << "\t> Float Inst\t: " << inst2str(finst) << endl;
                curset.insert(finst);
            }
            else if(SVFUtil::dyn_cast<Argument>(v) == NULL && isHeuriArg(v) == false){
                curset.insert(v);
            }
            else{
                if(ctrlinfomap[ci]->cur.length() > 0
                        && ctrlinfomap[ci]->cur.find("S") == string::npos && ctrlinfomap[ci]->cur.find("arg") == string::npos){
                    uint64_t addr = stoull(ctrlinfomap[ci]->cur, nullptr, 16);
                    vector <const Instruction *> instvec = retInstvecFromAddr(addr);
                    for(int i = 0; i < instvec.size(); i++){
                        const Instruction *inst = instvec[i];
                        if(isVldrInst(inst)){
                            curset.insert(inst);
                            break;
                        }
                    }
                }
            }
        }
        if(curset.size() == 1){
            ctrl2finalcur[ci] = *(curset.begin());
        }
        else if(curset.size() == 2){
            const Value *v1 = *(curset.begin());
            const Value *v2 = *(++(curset.begin()));
            if(retVMEMPtr(v1) != NULL && retVMEMPtr(v2) != NULL && gep2off(retVMEMPtr(v1)) < gep2off(retVMEMPtr(v2))){
                ctrl2finalcur[ci] = v1;
                ctrl2finalcur2[ci] = v2;
            }
            else if(retVMEMPtr(v1) != NULL && retVMEMPtr(v2) != NULL && gep2off(retVMEMPtr(v1)) > gep2off(retVMEMPtr(v2))){
                ctrl2finalcur[ci] = v2;
                ctrl2finalcur2[ci] = v1;
            }
            else{
                ctrl2finalcur[ci] = v1;
                ctrl2finalcur2[ci] = v2;
            }
        }


        cout << "> ci: " << ci->role << " w/ errs sz: " << ci->errs.size() << endl;
        for(auto sit = ci->errs.begin(); sit != ci->errs.end(); ++sit){
            const Value *v = *sit;
            if(!v) continue;
            if(err2ctrlmap.find(v) != err2ctrlmap.end()){
                cout << " > ERR\t: " << inst2str(v) << endl;
                if(const Value *finst = errptr2vldr(v, v, ci)){
                    cout << "\t> Float Inst\t: " << inst2str(finst) << endl;
                    errset.insert(finst);
                }
                else if(SVFUtil::dyn_cast<Argument>(v) == NULL && isHeuriArg(v) == false){
                    cout << "\t> Norm Inst\t: " << inst2str(v) << endl;
                    errset.insert(v);
                }
                else{
                    if(ctrlinfomap[ci]->err.length() > 0
                            && ctrlinfomap[ci]->err.find("S") == string::npos && ctrlinfomap[ci]->err.find("arg") == string::npos){
                        uint64_t addr = stoull(ctrlinfomap[ci]->err, nullptr, 16);
                        vector <const Instruction *> instvec = retInstvecFromAddr(addr);
                        for(int i = 0; i < instvec.size(); i++){
                            const Instruction *inst = instvec[i];
                            if(isVldrInst(inst)){
                                errset.insert(inst);
                                break;
                            }
                            else if(isVldrInst(inst)){
                                errset.insert(inst);
                                break;
                            }
                        }
                    }
                }
            }
            else{
                cout << " > Unknown\t: " << inst2str(v) << endl;
                if(const Value *finst = errptr2vldr(v, v, ci)){
                    cout << "\t> Float Inst\t: " << inst2str(finst) << endl;
                    errset.insert(finst);
                }
                else if(SVFUtil::dyn_cast<Argument>(v) == NULL && isHeuriArg(v) == false){
                    errset.insert(v);
                }
                else{
                    if(ctrlinfomap[ci]->err.length() > 0
                            && ctrlinfomap[ci]->err.find("S") == string::npos && ctrlinfomap[ci]->err.find("arg") == string::npos){
                        uint64_t addr = stoull(ctrlinfomap[ci]->err, nullptr, 16);
                        vector <const Instruction *> instvec = retInstvecFromAddr(addr);
                        for(int i = 0; i < instvec.size(); i++){
                            const Instruction *inst = instvec[i];
                            if(isVldrInst(inst)){
                                errset.insert(inst);
                                break;
                            }
                            else if(isVldrInst(inst)){
                                errset.insert(inst);
                                break;
                            }
                        }
                    }
                }

            }
        }
        if(errset.size() == 1){
            ctrl2finalerr[ci] = *(errset.begin());
        }
        else if(errset.size() == 2){
            const Value *v1 = *(errset.begin());
            const Value *v2 = *(++(errset.begin()));
            if(retVMEMPtr(v1) != NULL && retVMEMPtr(v2) != NULL &&  gep2off(retVMEMPtr(v1)) < gep2off(retVMEMPtr(v2))){
                ctrl2finalerr[ci] = v1;
                ctrl2finalerr2[ci] = v2;
            }
            else if(retVMEMPtr(v1) != NULL && retVMEMPtr(v2) != NULL &&  gep2off(retVMEMPtr(v1)) > gep2off(retVMEMPtr(v2))){
                ctrl2finalerr[ci] = v2;
                ctrl2finalerr2[ci] = v1;
            }
            else{
                ctrl2finalerr[ci] = v1;
                ctrl2finalerr2[ci] = v2;
            }
        }

        cout << "- OUT -" << endl;
        if(ci->outs.size()){
            for(auto sit = ci->outs.begin(); sit != ci->outs.end(); ++sit){
                const Value *v = *sit;
                if(!v) continue;
                if(alias2ref.find(v) != alias2ref.end()){
                    cout << " > OUT REF PTR\t: " << inst2str(v) << endl;
                    if(const Value *finst = ptr2vstr(v, v)){
                        cout << "\t> Float Inst\t: " << inst2str(finst) << endl;
                        outset.insert(finst);
                    }
                }
                else if(ref2ctrlmap.find(v) != ref2ctrlmap.end()){
                    cout << " > OUT REF\t: " << inst2str(v) << endl;
                    if(const Value *finst = ptr2vstr(v, v)){
                        cout << "\t> Float Inst\t: " << inst2str(finst) << endl;
                        outset.insert(finst);
                    }
                    else if(SVFUtil::dyn_cast<Argument>(v) == NULL && isHeuriArg(v) == false){
                        outset.insert(v);
                    }
                }
                else{
                    cout << " > OUT Unknown\t: " << inst2str(v) << endl;
                    if(const Value *finst = ptr2vstr(v, v)){
                        cout << "\t> Float Inst\t: " << inst2str(finst) << endl;
                        outset.insert(finst);
                    }
                    else if(SVFUtil::dyn_cast<Argument>(v) == NULL && isHeuriArg(v) == false){
                        //ctrl2finalout[ci] = v;
                        outset.insert(v);
                    }
                }
            }
            if(outset.size() == 1){
                ctrl2finalout[ci] = *(outset.begin());
            }
            else if(outset.size() == 2){
                const Value *v1 = *(outset.begin());
                const Value *v2 = *(++(outset.begin()));
                if(retVMEMPtr(v1) != NULL && retVMEMPtr(v2) != NULL && gep2off(retVMEMPtr(v1)) < gep2off(retVMEMPtr(v2))){
                    ctrl2finalout[ci] = v1;
                    ctrl2finalout2[ci] = v2;
                }
                else if(retVMEMPtr(v1) != NULL && retVMEMPtr(v2) != NULL && gep2off(retVMEMPtr(v1)) > gep2off(retVMEMPtr(v2))){
                    ctrl2finalout[ci] = v2;
                    ctrl2finalout2[ci] = v1;
                }
                else{
                    ctrl2finalout[ci] = v1;
                    ctrl2finalout2[ci] = v2;
                }
            }
        }
        else if(ci->mathouts.size()){
            for(auto sit = ci->mathouts.begin(); sit != ci->mathouts.end(); ++sit){
                const Value *v = *sit;
                if(!v) continue;
                if(alias2ref.find(v) != alias2ref.end()){
                    cout << " > MATH OUT REF PTR\t: " << inst2str(v) << endl;
                    if(const Value *finst = ptr2vstr(v, v)){
                        cout << "\t> Float Inst\t: " << inst2str(finst) << endl;
                        //ctrl2finalout[ci] = finst;
                        outset.insert(finst);
                    }
                }
                else if(ref2ctrlmap.find(v) != ref2ctrlmap.end()){
                    cout << " > MATH OUT REF\t: " << inst2str(v) << endl;
                    if(const Value *finst = ptr2vstr(v, v)){
                        cout << "\t> Float Inst\t: " << inst2str(finst) << endl;
                        //ctrl2finalout[ci] = finst;
                        outset.insert(finst);
                    }
                    else if(SVFUtil::dyn_cast<Argument>(v) == NULL && isHeuriArg(v) == false){
                        //ctrl2finalout[ci] = v;
                        outset.insert(v);
                    }
                }
                else{
                    cout << " > MATH OUT Unknown\t: " << inst2str(v) << endl;
                    if(const Value *finst = ptr2vstr(v, v)){
                        cout << "\t> Float Inst\t: " << inst2str(finst) << endl;
                        //ctrl2finalout[ci] = finst;
                        outset.insert(finst);
                    }
                    else if(SVFUtil::dyn_cast<Argument>(v) == NULL && isHeuriArg(v) == false){
                        //ctrl2finalout[ci] = v;
                        outset.insert(v);
                    }
                }
            }
            if(outset.size() == 1){
                ctrl2finalout[ci] = *(outset.begin());
            }
            else if(outset.size() == 2){
                const Value *v1 = *(outset.begin());
                const Value *v2 = *(++(outset.begin()));
                if(retVMEMPtr(v1) != NULL && retVMEMPtr(v2) != NULL && gep2off(retVMEMPtr(v1)) < gep2off(retVMEMPtr(v2))){
                    ctrl2finalout[ci] = v1;
                    ctrl2finalout2[ci] = v2;
                }
                else if(retVMEMPtr(v1) != NULL && retVMEMPtr(v2) != NULL && gep2off(retVMEMPtr(v1)) > gep2off(retVMEMPtr(v2))){
                    ctrl2finalout[ci] = v2;
                    ctrl2finalout2[ci] = v1;
                }
                else{
                    ctrl2finalout[ci] = v1;
                    ctrl2finalout2[ci] = v2;
                }
            }
        }
    }

    // ordering
    vector <ctrlinfo *> orderctrls;
    for(auto cit = taggedctrls.begin(); cit != taggedctrls.end(); ++cit){
        ctrlinfo *ci = *cit;
        if((ci->role.find("x") != string::npos || ci->role.find("y") != string::npos) && ci->role.find("ay") == string::npos
            && ci->role.find("_p") != string::npos)
            orderctrls.push_back(ci);
    }
    for(auto cit = taggedctrls.begin(); cit != taggedctrls.end(); ++cit){
        ctrlinfo *ci = *cit;
        if((ci->role.find("x") != string::npos || ci->role.find("y") != string::npos) && ci->role.find("ay") == string::npos 
            && ci->role.find("_v") != string::npos)
            orderctrls.push_back(ci);
    }
    for(auto cit = taggedctrls.begin(); cit != taggedctrls.end(); ++cit){
        ctrlinfo *ci = *cit;
        if((ci->role.find("x") != string::npos || ci->role.find("y") != string::npos) && ci->role.find("ay") == string::npos
            && ci->role.find("_a") != string::npos)
            orderctrls.push_back(ci);
    }
    for(auto cit = taggedctrls.begin(); cit != taggedctrls.end(); ++cit){
        ctrlinfo *ci = *cit;
        if((ci->role.find("ar_") != string::npos || ci->role.find("ap_") != string::npos || ci->role.find("ay_") != string::npos) 
            && ci->role.find("_p") != string::npos)
            orderctrls.push_back(ci);
    }

    for(auto cit = taggedctrls.begin(); cit != taggedctrls.end(); ++cit){
        ctrlinfo *ci = *cit;
        if(ci->role.find("z_") != string::npos && ci->role.find("_p") != string::npos)
            orderctrls.push_back(ci);
    }
    for(auto cit = taggedctrls.begin(); cit != taggedctrls.end(); ++cit){
        ctrlinfo *ci = *cit;
        if(ci->role.find("z_") != string::npos && ci->role.find("_v") != string::npos)
            orderctrls.push_back(ci);
    }
    for(auto cit = taggedctrls.begin(); cit != taggedctrls.end(); ++cit){
        ctrlinfo *ci = *cit;
        if(ci->role.find("z_") != string::npos && ci->role.find("_a") != string::npos)
            orderctrls.push_back(ci);
    }
    for(auto cit = taggedctrls.begin(); cit != taggedctrls.end(); ++cit){
        ctrlinfo *ci = *cit;
        if((ci->role.find("ar_") != string::npos || ci->role.find("ap_") != string::npos || ci->role.find("ay_") != string::npos) 
            && ci->role.find("_v") != string::npos)
            orderctrls.push_back(ci);
    }
    for(auto cit = taggedctrls.begin(); cit != taggedctrls.end(); ++cit){
        ctrlinfo *ci = *cit;
        if((ci->role.find("ar_") != string::npos || ci->role.find("ap_") != string::npos || ci->role.find("ay_") != string::npos) 
            && ci->role.find("_a") != string::npos)
            orderctrls.push_back(ci);
    }

    // result
    class ctrlfinalres{
        public:
            string role;
            uint64_t ref;
            uint64_t cur;
            uint64_t err;
            uint64_t out;
            uint64_t p;
            uint64_t i;
            uint64_t d;
            uint64_t ff;
    };
    map <ctrlinfo *, ctrlfinalres> resmap;
    map <uint64_t, uint64_t> instaddr2csmap;   // Note: this part has a naive design. However, it can be extended easily
                                                // if cs does not belong to the ci's function 


    for(int i = 0; i < orderctrls.size(); i++){
        ctrlinfo *ci = orderctrls[i];
        string role; 
        if("xy_p" == ci->role) role = "POS_X";
        else if("xy_v" == ci->role) role = "VEL_X";
        else if("xy_a" == ci->role) role = "ACC_X";
        else if("x_p" == ci->role) role = "POS_X";
        else if("x_v" == ci->role) role = "VEL_X";
        else if("x_a" == ci->role) role = "ACC_X";
        else if("y_p" == ci->role) role = "POS_Y";
        else if("y_v" == ci->role) role = "VEL_Y";
        else if("y_a" == ci->role) role = "ACC_Y";
        else if("z_p" == ci->role) role = "POS_Z";
        else if("z_v" == ci->role) role = "VEL_Z";
        else if("z_a" == ci->role) role = "ACC_Z";
        else if("ar_p" == ci->role) role = "ANG_ROLL";
        else if("ap_p" == ci->role) role = "ANG_PITCH";
        else if("ay_p" == ci->role) role = "ANG_YAW";
        else if("ar_v" == ci->role) role = "ANGRATE_ROLL";
        else if("ap_v" == ci->role) role = "ANGRATE_PITCH";
        else if("ay_v" == ci->role) role = "ANGRATE_YAW";
        else if("ar_a" == ci->role) role = "ANGACC_ROLL";
        else if("ap_a" == ci->role) role = "ANGACC_PITCH";
        else if("ay_a" == ci->role) role = "ANGACC_YAW";
        else continue;

        resmap[ci].role = role;
        // final result update in header (in consideration of xy like controllers)
        // ref
        if(ctrl2finalref.find(ci) != ctrl2finalref.end()){
            if(const Instruction *inst = SVFUtil::dyn_cast<Instruction>(ctrl2finalref[ci])){
                resmap[ci].ref = inst2addr(inst);
            }
        }
        // cur
        if(ctrl2finalcur.find(ci) != ctrl2finalcur.end()){
            if(const Instruction *inst = SVFUtil::dyn_cast<Instruction>(ctrl2finalcur[ci]))
                resmap[ci].cur = inst2addr(inst);
        }
        // err
        if(ctrl2finalerr.find(ci) != ctrl2finalerr.end()){
            if(const Instruction *inst = SVFUtil::dyn_cast<Instruction>(ctrl2finalerr[ci]))
                resmap[ci].err = inst2addr(inst);
        }
        // out
        if(ctrl2finalout.find(ci) != ctrl2finalout.end()){
            if(const Instruction *inst = SVFUtil::dyn_cast<Instruction>(ctrl2finalout[ci]))
                resmap[ci].out = inst2addr(inst);
        }

        if(ctrlinfomap[ci]->p.length()){
            uint64_t varaddr = stoull(ctrlinfomap[ci]->p, nullptr, 16);
            resmap[ci].p = varaddr;
        }
        if(ctrlinfomap[ci]->i.length()){
            uint64_t varaddr = stoull(ctrlinfomap[ci]->i, nullptr, 16);
            resmap[ci].i = varaddr;
        }
        if(ctrlinfomap[ci]->d.length()){
            uint64_t varaddr = stoull(ctrlinfomap[ci]->d, nullptr, 16);
            resmap[ci].d = varaddr;
        }
        if(ctrlinfomap[ci]->ff.length()){
            uint64_t varaddr = stoull(ctrlinfomap[ci]->ff, nullptr, 16);
            resmap[ci].ff = varaddr;
        }


        if("xy_p" == ci->role) role = "POS_Y";
        else if("xy_v" == ci->role) role = "VEL_Y";
        else if("xy_a" == ci->role) role = "ACC_Y";
        else continue;

        resmap[ci].role = role;
        // final result update in header (2 dimension)
        // ref
        if(ctrl2finalref2.find(ci) != ctrl2finalref2.end()){
            if(const Instruction *inst = SVFUtil::dyn_cast<Instruction>(ctrl2finalref2[ci]))
                resmap[ci].ref = inst2addr(inst);
        }
        // cur
        if(ctrl2finalcur2.find(ci) != ctrl2finalcur2.end()){
            if(const Instruction *inst = SVFUtil::dyn_cast<Instruction>(ctrl2finalcur2[ci]))
                resmap[ci].cur = inst2addr(inst);
        }
        // err
        if(ctrl2finalerr2.find(ci) != ctrl2finalerr2.end()){
            if(const Instruction *inst = SVFUtil::dyn_cast<Instruction>(ctrl2finalerr2[ci]))
                resmap[ci].err = inst2addr(inst);
        }
        // out
        if(ctrl2finalout2.find(ci) != ctrl2finalout2.end()){
            if(const Instruction *inst = SVFUtil::dyn_cast<Instruction>(ctrl2finalout2[ci]))
                resmap[ci].out = inst2addr(inst);
        }

        if(ctrlinfomap[ci]->p.length()){
            uint64_t varaddr = stoull(ctrlinfomap[ci]->p, nullptr, 16);
            resmap[ci].p = varaddr;
        }
        if(ctrlinfomap[ci]->i.length()){
            uint64_t varaddr = stoull(ctrlinfomap[ci]->i, nullptr, 16);
            resmap[ci].i = varaddr;
        }
        if(ctrlinfomap[ci]->d.length()){
            uint64_t varaddr = stoull(ctrlinfomap[ci]->d, nullptr, 16);
            resmap[ci].d = varaddr;
        }
        if(ctrlinfomap[ci]->ff.length()){
            uint64_t varaddr = stoull(ctrlinfomap[ci]->ff, nullptr, 16);
            resmap[ci].ff = varaddr;
        }
    }


    map<ctrlinfo *, map <string, set<const Instruction*>>> cicxtcandidmap;
    map<ctrlinfo *, map <string, const Instruction*>> cicxtmap;
    for(int i = 0; i < orderctrls.size(); i++){
        ctrlinfo *ci = orderctrls[i];
        if(("xy_p" == ci->role) || ("xy_v" == ci->role) || ("xy_a" == ci->role) || ("x_p" == ci->role) || ("x_v" == ci->role) || ("x_a" == ci->role) || ("y_p" == ci->role) || ("y_v" == ci->role) || ("y_a" == ci->role) || ("z_p" == ci->role) || ("z_v" == ci->role) || ("z_a" == ci->role) || ("ar_p" == ci->role) || ("ap_p" == ci->role) || ("ay_p" == ci->role) || ("ar_v" == ci->role) || ("ap_v" == ci->role) || ("ay_v" == ci->role) || ("ar_a" == ci->role) || ("ap_a" == ci->role) || ("ay_a" == ci->role)){
            retPatchCxtCandid(ci, 1, cicxtcandidmap[ci]);

            if(("xy_p" == ci->role) || ("xy_v" == ci->role) || ("xy_a" == ci->role)){
                retPatchCxtCandid(ci, 2, cicxtcandidmap[ci]);
            }
        }
    }

    cout << "------------------" << endl;
    for(int i = 0; i < orderctrls.size(); i++){
        ctrlinfo *ci = orderctrls[i];
        if(("xy_p" == ci->role) || ("xy_v" == ci->role) || ("xy_a" == ci->role) || ("x_p" == ci->role) || ("x_v" == ci->role) || ("x_a" == ci->role) || ("y_p" == ci->role) || ("y_v" == ci->role) || ("y_a" == ci->role) || ("z_p" == ci->role) || ("z_v" == ci->role) || ("z_a" == ci->role) || ("ar_p" == ci->role) || ("ap_p" == ci->role) || ("ay_p" == ci->role) || ("ar_v" == ci->role) || ("ap_v" == ci->role) || ("ay_v" == ci->role) || ("ar_a" == ci->role) || ("ap_a" == ci->role) || ("ay_a" == ci->role)){
            retPatchCxt(ci, 1, cicxtcandidmap, cicxtmap[ci]);

            if(("xy_p" == ci->role) || ("xy_v" == ci->role) || ("xy_a" == ci->role)){
                retPatchCxt(ci, 2, cicxtcandidmap, cicxtmap[ci]);
            }
        }
    }

    cout << std::hex;
    for(int i = 0; i < orderctrls.size(); i++){
        ctrlinfo *ci = orderctrls[i];
        if("xy_p" == ci->role) cout << "POS_X" << endl;
        else if("xy_v" == ci->role) cout << "VEL_X" << endl;
        else if("xy_a" == ci->role) cout << "ACC_X" << endl;
        else if("x_p" == ci->role) cout << "POS_X" << endl;
        else if("x_v" == ci->role) cout << "VEL_X" << endl;
        else if("x_a" == ci->role) cout << "ACC_X" << endl;
        else if("y_p" == ci->role) cout << "POS_Y" << endl;
        else if("y_v" == ci->role) cout << "VEL_Y" << endl;
        else if("y_a" == ci->role) cout << "ACC_Y" << endl;
        else if("z_p" == ci->role) cout << "POS_Z" << endl;
        else if("z_v" == ci->role) cout << "VEL_Z" << endl;
        else if("z_a" == ci->role) cout << "ACC_Z" << endl;
        else if("ar_p" == ci->role) cout << "ANG_ROLL" << endl;
        else if("ap_p" == ci->role) cout << "ANG_PITCH" << endl;
        else if("ay_p" == ci->role) cout << "ANG_YAW" << endl;
        else if("ar_v" == ci->role) cout << "ANGRATE_ROLL" << endl;
        else if("ap_v" == ci->role) cout << "ANGRATE_PITCH" << endl;
        else if("ay_v" == ci->role) cout << "ANGRATE_YAW" << endl;
        else if("ar_a" == ci->role) cout << "ANGACC_ROLL" << endl;
        else if("ap_a" == ci->role) cout << "ANGACC_PITCH" << endl;
        else if("ay_a" == ci->role) cout << "ANGACC_YAW" << endl;

        map <string, const Instruction*> cxtmap = cicxtmap[ci];

        // final result update in header (in consideration of xy like controllers)
        // ref
        if(ctrl2finalref.find(ci) != ctrl2finalref.end()){
            if(const Instruction *inst = SVFUtil::dyn_cast<Instruction>(ctrl2finalref[ci])){
                cout << ">> ref: 0x" << inst2addr(inst) << endl;
            }
        }
        // cur
        if(ctrl2finalcur.find(ci) != ctrl2finalcur.end()){
            if(const Instruction *inst = SVFUtil::dyn_cast<Instruction>(ctrl2finalcur[ci])){
                cout << ">> cur: 0x" << inst2addr(inst) << endl;
            }
        }
        // err
        if(ctrl2finalerr.find(ci) != ctrl2finalerr.end()){
            if(const Instruction *inst = SVFUtil::dyn_cast<Instruction>(ctrl2finalerr[ci])){
                cout << ">> err: 0x" << inst2addr(inst) << endl;
            }
        }
        // out
        if(ctrl2finalout.find(ci) != ctrl2finalout.end()){
            if(const Instruction *inst = SVFUtil::dyn_cast<Instruction>(ctrl2finalout[ci])){
                cout << ">> out: 0x" << inst2addr(inst) << endl;
            }
        }

        if(ctrlinfomap[ci]->p.length()){  
            cout << ">> p: 0x" << ctrlinfomap[ci]->p << endl;   
            if(cxtmap.find("p") != cxtmap.end()){
                uint64_t csaddr = inst2addr(cxtmap["p"]);
                cout << "\t>> cxt cs: 0x" << csaddr << endl;
            }
        }
        if(ctrlinfomap[ci]->i.length()){
            cout << ">> i: 0x" << ctrlinfomap[ci]->i << endl;
            if(cxtmap.find("i") != cxtmap.end()){
                uint64_t csaddr = inst2addr(cxtmap["i"]);
                cout << "\t>> cxt cs: 0x" << csaddr << endl;
            }
        }
        if(ctrlinfomap[ci]->d.length()){
            cout << ">> d: 0x" << ctrlinfomap[ci]->d << endl;
            if(cxtmap.find("d") != cxtmap.end()){
                uint64_t csaddr = inst2addr(cxtmap["d"]);
                cout << "\t>> cxt cs: 0x" << csaddr << endl;
            }
        }
        if(ctrlinfomap[ci]->ff.length()){
            cout << ">> ff: 0x" << ctrlinfomap[ci]->ff << endl;
            if(cxtmap.find("ff") != cxtmap.end()){
                uint64_t csaddr = inst2addr(cxtmap["ff"]);
                cout << "\t>> cxt cs: 0x" << csaddr << endl;
            }
        }

        cout << "\n" << endl;

        if("xy_p" == ci->role) cout << "POS_Y" << endl;
        else if("xy_v" == ci->role) cout << "VEL_Y" << endl;
        else if("xy_a" == ci->role) cout << "ACC_Y" << endl;
        else continue;

        // final result update in header (2 dimension)
        // ref
        if(ctrl2finalref2.find(ci) != ctrl2finalref2.end()){
            if(const Instruction *inst = SVFUtil::dyn_cast<Instruction>(ctrl2finalref2[ci])){
                cout << ">> ref: 0x" << inst2addr(inst) << endl;
            }
        }
        // cur
        if(ctrl2finalcur2.find(ci) != ctrl2finalcur2.end()){
            if(const Instruction *inst = SVFUtil::dyn_cast<Instruction>(ctrl2finalcur2[ci])){
                cout << ">> cur: 0x" << inst2addr(inst) << endl;
            }
        }
        // err
        if(ctrl2finalerr2.find(ci) != ctrl2finalerr2.end()){
            if(const Instruction *inst = SVFUtil::dyn_cast<Instruction>(ctrl2finalerr2[ci])){
                cout << ">> err: 0x" << inst2addr(inst) << endl;
            }
        }
        // out
        if(ctrl2finalout2.find(ci) != ctrl2finalout2.end()){
            if(const Instruction *inst = SVFUtil::dyn_cast<Instruction>(ctrl2finalout2[ci])){
                cout << ">> out: 0x" << inst2addr(inst) << endl;
            }
        }

        if(ctrlinfomap[ci]->p.length()){  
            cout << ">> p: 0x" << ctrlinfomap[ci]->p << endl;   
            if(cxtmap.find("p") != cxtmap.end()){
                uint64_t csaddr = inst2addr(cxtmap["p"]);
                cout << "\t>> cxt cs: 0x" << csaddr << endl;
            }
        }
        if(ctrlinfomap[ci]->i.length()){
            cout << ">> i: 0x" << ctrlinfomap[ci]->i << endl;
            if(cxtmap.find("i") != cxtmap.end()){
                uint64_t csaddr = inst2addr(cxtmap["i"]);
                cout << "\t>> cxt cs: 0x" << csaddr << endl;
            }
        }
        if(ctrlinfomap[ci]->d.length()){
            cout << ">> d: 0x" << ctrlinfomap[ci]->d << endl;
            if(cxtmap.find("d") != cxtmap.end()){
                uint64_t csaddr = inst2addr(cxtmap["d"]);
                cout << "\t>> cxt cs: 0x" << csaddr << endl;
            }
        }
        if(ctrlinfomap[ci]->ff.length()){
            cout << ">> ff: 0x" << ctrlinfomap[ci]->ff << endl;
            if(cxtmap.find("ff") != cxtmap.end()){
                uint64_t csaddr = inst2addr(cxtmap["ff"]);
                cout << "\t>> cxt cs: 0x" << csaddr << endl;
            }
        }

        cout << "\n" << endl;
    }
    cout << std::dec;

    cout << "--------------------------------" << endl;
    return;
}

void CtrlIdentifier::retPatchCxtCandid(ctrlinfo *ci, int axis, map <string, set<const Instruction*>> &cxtmap){
    set <const Function*> crset, ceset, allset;
    const Function *cr = NULL;

    map <string, const Function*> cemap;
    map <const Instruction *, const Function*> cscemap;
    map <const Function*, set<const Instruction *>> cecssetmap;

    // final result update in header (in consideration of xy like controllers)
    if(axis == 1){
        // ref
        if(ctrl2finalref.find(ci) != ctrl2finalref.end()){
            if(const Instruction *inst = SVFUtil::dyn_cast<Instruction>(ctrl2finalref[ci])){
                crset.insert(inst->getParent()->getParent());
                allset.insert(inst->getParent()->getParent());
            }
        }
        // cur
        if(ctrl2finalcur.find(ci) != ctrl2finalcur.end()){
            if(const Instruction *inst = SVFUtil::dyn_cast<Instruction>(ctrl2finalcur[ci])){
                crset.insert(inst->getParent()->getParent());
                allset.insert(inst->getParent()->getParent());
            }
        }
        // err
        if(ctrl2finalerr.find(ci) != ctrl2finalerr.end()){
            if(const Instruction *inst = SVFUtil::dyn_cast<Instruction>(ctrl2finalerr[ci])){
                crset.insert(inst->getParent()->getParent());
                allset.insert(inst->getParent()->getParent());
            }
        }
        // out
        if(ctrl2finalout.find(ci) != ctrl2finalout.end()){
            if(const Instruction *inst = SVFUtil::dyn_cast<Instruction>(ctrl2finalout[ci])){
                crset.insert(inst->getParent()->getParent());
                allset.insert(inst->getParent()->getParent());
            }
        }
    }
    else if(axis == 2){
        // ref
        if(ctrl2finalref2.find(ci) != ctrl2finalref2.end()){
            if(const Instruction *inst = SVFUtil::dyn_cast<Instruction>(ctrl2finalref2[ci])){
                crset.insert(inst->getParent()->getParent());
                allset.insert(inst->getParent()->getParent());
            }
        }
        // cur
        if(ctrl2finalcur2.find(ci) != ctrl2finalcur2.end()){
            if(const Instruction *inst = SVFUtil::dyn_cast<Instruction>(ctrl2finalcur2[ci])){
                crset.insert(inst->getParent()->getParent());
                allset.insert(inst->getParent()->getParent());
            }
        }
        // err
        if(ctrl2finalerr2.find(ci) != ctrl2finalerr2.end()){
            if(const Instruction *inst = SVFUtil::dyn_cast<Instruction>(ctrl2finalerr2[ci])){
                crset.insert(inst->getParent()->getParent());
                allset.insert(inst->getParent()->getParent());
            }
        }
        // out
        if(ctrl2finalout2.find(ci) != ctrl2finalout2.end()){
            if(const Instruction *inst = SVFUtil::dyn_cast<Instruction>(ctrl2finalout2[ci])){
                crset.insert(inst->getParent()->getParent());
                allset.insert(inst->getParent()->getParent());
            }
        }
    }

    if(ctrlinfomap[ci]->p.length()){  
        uint64_t addr = stoull(ctrlinfomap[ci]->p, nullptr, 16);
        if(!addr) assert(0);
        ceset.insert(retFuncFromInstAddr(addr));
        allset.insert(retFuncFromInstAddr(addr));
        cemap["p"] = retFuncFromInstAddr(addr);
    }
    if(ctrlinfomap[ci]->i.length()){
        uint64_t addr = stoull(ctrlinfomap[ci]->i, nullptr, 16);
        if(!addr) assert(0);
        ceset.insert(retFuncFromInstAddr(addr));
        allset.insert(retFuncFromInstAddr(addr));
        cemap["i"] = retFuncFromInstAddr(addr);
    }
    if(ctrlinfomap[ci]->d.length()){
        uint64_t addr = stoull(ctrlinfomap[ci]->d, nullptr, 16);
        if(!addr) assert(0);
        ceset.insert(retFuncFromInstAddr(addr));
        allset.insert(retFuncFromInstAddr(addr));
        cemap["d"] = retFuncFromInstAddr(addr);
    }
    if(ctrlinfomap[ci]->ff.length()){
        uint64_t addr = stoull(ctrlinfomap[ci]->ff, nullptr, 16);
        if(!addr) assert(0);
        ceset.insert(retFuncFromInstAddr(addr));
        allset.insert(retFuncFromInstAddr(addr));
        cemap["ff"] = retFuncFromInstAddr(addr);
    }


    if(allset.size() >= 2){
        map<const Function *, set<const Function*>> funccallmap;
        map<const Function *, set<const Instruction*>> funccsmap;

        for(auto it = crset.begin(); it != crset.end(); ++it){
            const Function *func = *it;
            for(auto iit = inst_begin(func); iit != inst_end(func); ++iit){
                const Instruction *inst = &*iit;
                if(const CallInst *cinst = SVFUtil::dyn_cast<CallInst>(inst)){
                    const Function *ce = cinst->getCalledFunction();
                    funccallmap[func].insert(ce);
                    cscemap[inst] = ce; // assume direct call
                    cecssetmap[ce].insert(inst);
                }
            }
        }

        for(auto it = funccallmap.begin(); it != funccallmap.end(); ++it){
            const Function *candid = it->first;
            set <const Function*> allceset = it->second;
            if(!ceset.size()) continue; 

            bool notfound;
            notfound = true;

            for(auto cit = ceset.begin(); cit != ceset.end(); ++cit){
                const Function *ce = *cit;
                if(allceset.find(ce) == allceset.end()){
                    notfound = false;
                    break;
                }
            }

            if(notfound){
                cr = candid;
                break;
            }
        }
    }

    if(cr){
        // only pid controller part is shared. So, it has different meaning according to context. But, variables such as current, ref, err are not shared.
        if(cemap.find("p") != cemap.end()){
            const Function *ce = cemap["p"];
            if(cecssetmap.find(ce) == cecssetmap.end()) assert(0);

            if(ce != ci->func){ // it is not actually ce
                set <const Instruction *> csset = cecssetmap[ce];
                if(csset.size() == 1){
                    const Instruction *inst = *(csset.begin());
                    cxtmap["p"].insert(inst);
                }
                else{
                    for(auto it = csset.begin(); it != csset.end(); ++it){
                        const Instruction *inst = *it;
                        unsigned idx = inst2idx(inst);
                        cxtmap["p"].insert(inst);
                    }
                }
            }
        }
        if(cemap.find("i") != cemap.end()){
            const Function *ce = cemap["i"];
            if(cecssetmap.find(ce) == cecssetmap.end()) assert(0);

            if(ce != ci->func){ // it is not actually ce
                set <const Instruction *> csset = cecssetmap[ce];
                if(csset.size() == 1){
                    const Instruction *inst = *(csset.begin());
                    cxtmap["i"].insert(inst);
                }
                else{
                    for(auto it = csset.begin(); it != csset.end(); ++it){
                        const Instruction *inst = *it;
                        unsigned idx = inst2idx(inst);
                        cxtmap["i"].insert(inst);
                    }
                }
            }
        }
        if(cemap.find("d") != cemap.end()){
            const Function *ce = cemap["d"];
            if(cecssetmap.find(ce) == cecssetmap.end()) assert(0);

            if(ce != ci->func){ // it is not actually ce
                set <const Instruction *> csset = cecssetmap[ce];
                if(csset.size() == 1){
                    const Instruction *inst = *(csset.begin());
                    cxtmap["d"].insert(inst);
                }
                else{
                    for(auto it = csset.begin(); it != csset.end(); ++it){
                        const Instruction *inst = *it;
                        unsigned idx = inst2idx(inst);
                        cxtmap["d"].insert(inst);
                    }
                }
            }
        }
        if(cemap.find("ff") != cemap.end()){
            const Function *ce = cemap["ff"];
            if(cecssetmap.find(ce) == cecssetmap.end()) assert(0);

            if(ce != ci->func){ // it is not actually ce
                set <const Instruction *> csset = cecssetmap[ce];
                if(csset.size() == 1){
                    const Instruction *inst = *(csset.begin());
                    cxtmap["ff"].insert(inst);
                }
                else{
                    for(auto it = csset.begin(); it != csset.end(); ++it){
                        const Instruction *inst = *it;
                        unsigned idx = inst2idx(inst);
                        cxtmap["ff"].insert(inst);
                    }
                }
            }
        }
    }
}



void CtrlIdentifier::retPatchCxt(ctrlinfo *ci, int axis, map<ctrlinfo*, map <string, set<const Instruction*>>> &cicxtcandidmap, map <string, const Instruction*> &cxtmap){
    map <string, set<const Instruction*>> cxtcandidmap = cicxtcandidmap[ci];

    if(cxtcandidmap["p"].size() == 1){ // p must exist
        cxtmap["p"] = *(cxtcandidmap["p"].begin()); // p must exist
        if(cxtcandidmap.find("i") != cxtcandidmap.end()) cxtmap["i"] = *(cxtcandidmap["i"].begin());
        if(cxtcandidmap.find("d") != cxtcandidmap.end()) cxtmap["d"] = *(cxtcandidmap["d"].begin());
        if(cxtcandidmap.find("ff") != cxtcandidmap.end()) cxtmap["ff"] = *(cxtcandidmap["ff"].begin());
    }
    else if(cxtcandidmap.size() > 1){
        const Function *targetcr = (*(cxtcandidmap["p"].begin()))->getParent()->getParent();    // p must exist/ must be in the same function
        const Instruction *targetoi = NULL;
        if(axis == 1){
            if(ctrl2finalout.find(ci) != ctrl2finalout.end()){
                if(const Instruction *inst = SVFUtil::dyn_cast<Instruction>(ctrl2finalout[ci])) targetoi = inst;
            }
        }
        else if(axis == 2){
            if(ctrl2finalout2.find(ci) != ctrl2finalout2.end()){
                if(const Instruction *inst = SVFUtil::dyn_cast<Instruction>(ctrl2finalout2[ci])) targetoi = inst;
            }
        }
        assert(targetoi);

        // for target ci
        map<unsigned, const Instruction *> csidx2pcandid; // call inst
        set<const Instruction*> pcandidset = cicxtcandidmap[ci]["p"]; //p must exist
        for(auto it = pcandidset.begin(); it != pcandidset.end(); ++it){
            const Instruction *inst = *it;
            unsigned oidx = inst2idx(inst);
            csidx2pcandid[oidx] = inst;
        }

        map<unsigned, const Instruction *> csidx2icandid;
        set<const Instruction*> icandidset;
        if(cicxtcandidmap[ci].find("i") != cicxtcandidmap[ci].end()){
            set<const Instruction*> icandidset = cicxtcandidmap[ci]["i"]; //p must exist
            for(auto it = icandidset.begin(); it != icandidset.end(); ++it){
                const Instruction *inst = *it;
                unsigned oidx = inst2idx(inst);
                csidx2icandid[oidx] = inst;
            }
        }

        map<unsigned, const Instruction *> csidx2dcandid;
        set<const Instruction*> dcandidset;
        if(cicxtcandidmap[ci].find("d") != cicxtcandidmap[ci].end()){
            set<const Instruction*> dcandidset = cicxtcandidmap[ci]["d"]; //p must exist
            for(auto it = dcandidset.begin(); it != dcandidset.end(); ++it){
                const Instruction *inst = *it;
                unsigned oidx = inst2idx(inst);
                csidx2dcandid[oidx] = inst;
            }
        }

        map<unsigned, const Instruction *> csidx2ffcandid;
        set<const Instruction*> ffcandidset;
        if(cicxtcandidmap[ci].find("ff") != cicxtcandidmap[ci].end()){
            set<const Instruction*> ffcandidset = cicxtcandidmap[ci]["ff"]; //p must exist
            for(auto it = ffcandidset.begin(); it != ffcandidset.end(); ++it){
                const Instruction *inst = *it;
                unsigned oidx = inst2idx(inst);
                csidx2ffcandid[oidx] = inst;
            }
        }

        // ordering pidff
        vector <unsigned> cspidxvec;
        for(auto it = csidx2pcandid.begin(); it != csidx2pcandid.end(); ++it){
            unsigned idx = it->first;
            cspidxvec.push_back(idx);
        }

        vector <unsigned> csiidxvec;
        for(auto it = csidx2icandid.begin(); it != csidx2icandid.end(); ++it){
            unsigned idx = it->first;
            csiidxvec.push_back(idx);
        }

        vector <unsigned> csdidxvec;
        for(auto it = csidx2dcandid.begin(); it != csidx2dcandid.end(); ++it){
            unsigned idx = it->first;
            csdidxvec.push_back(idx);
        }

        vector <unsigned> csffidxvec;
        for(auto it = csidx2ffcandid.begin(); it != csidx2ffcandid.end(); ++it){
            unsigned idx = it->first;
            csffidxvec.push_back(idx);
        }

        // oidx 
        map<unsigned, ctrlinfo*> oidx2ci; // for output inst
        for(auto it = cicxtcandidmap.begin(); it != cicxtcandidmap.end(); ++it){
            ctrlinfo *ici = it->first;

            if(it->second.find("p") == it->second.end()) continue;
            else if( (*(it->second["p"].begin()))->getParent()->getParent() == targetcr){    // p must exist/ must be in the same function
                unsigned oidx = 0;
                if(axis == 1){
                    if(ctrl2finalout.find(ici) != ctrl2finalout.end()){
                        if(const Instruction *inst = SVFUtil::dyn_cast<Instruction>(ctrl2finalout[ici])) oidx = inst2idx(inst);
                    }
                }
                else if(axis == 2){
                    if(ctrl2finalout2.find(ici) != ctrl2finalout2.end()){
                        if(const Instruction *inst = SVFUtil::dyn_cast<Instruction>(ctrl2finalout2[ici])) oidx = inst2idx(inst);
                    }
                }
                oidx2ci[oidx] = ici;
            }
        }

        // ordering oidx
        vector <unsigned> oidxvec;
        for(auto it = oidx2ci.begin(); it != oidx2ci.end(); ++it){
            unsigned idx = it->first;
            oidxvec.push_back(idx);
        }

        // determine
        for(unsigned i = 0; i < oidxvec.size(); i++){
            unsigned idx = oidxvec[i];
            if(oidx2ci[idx] == ci){
                unsigned cspidx = cspidxvec[i];
                cxtmap["p"] = csidx2pcandid[cspidx];   // p

                if(csidx2icandid.size() > 1){
                    unsigned csiidx = csiidxvec[i];
                    cxtmap["i"] = csidx2icandid[csiidx];   // i
                }

                if(csidx2dcandid.size() > 1){
                    unsigned csdidx = csdidxvec[i];
                    cxtmap["d"] = csidx2dcandid[csdidx];   // d
                }
                
                if(csidx2ffcandid.size() > 1){
                    unsigned csffidx = csffidxvec[i];
                    cxtmap["ff"] = csidx2ffcandid[csffidx];   // ff
                }
                break;
            }
        }

        if(cspidxvec.size() != oidxvec.size()) assert(0);
    }
}



const Value *CtrlIdentifier::ptr2vstr(const Value *v, const Value *basev){ // can return multi inst (multiple inst is ok)
    int thres = 30;                                                             // because it just increases instrumentation targets
    const Function *func = v2func(v);
    const Instruction *sinst = SVFUtil::dyn_cast<Instruction>(v);
    const Instruction *binst = SVFUtil::dyn_cast<Instruction>(basev);
    
    // Inner analysis
    if(isCastInst(sinst) && func != NULL){
        cout << "[DEBUG]: VSTR Inner: " << inst2str(sinst) << endl;
        vector<const Instruction*> instvec = func2insts(func);
        const Instruction *tinst = NULL;
        int sidx = inst2idx(binst);
        int eidx = instvec.size();
        if(eidx > sidx + thres) eidx = sidx + thres;

        for(int i = sidx; i < eidx; i++){
            if(isVstrInst(instvec[i])){
                if(retVSTRPtr(instvec[i]) == sinst){
                    tinst = instvec[i];
                    break;
                }
            }
        }

        if(tinst) return retLateVstr(tinst);
    }

    // Inter analysis -> return 
    // 1. possible all vmov detection (set)
    // 2. ce -> cs for set
    // 3. detect VSTR within cs ~ cs + thres 
    if(ceout2csoutptr.find(v) != ceout2csoutptr.end()){
        cout << "[DEBUG]: out: " << inst2str(v) << " -> vstr: " << inst2str(ceout2csoutptr[v]) << endl;
        return retLateVstr(ceout2csoutptr[v]);
    }

    return NULL;
}

const Value *CtrlIdentifier::retLateVstr(const Value *v){
    const Function *func = v2func(v);
    if(isVstrInst(v)){
        const Instruction *vstr = SVFUtil::dyn_cast<Instruction>(v);
        const Instruction *latevstr = NULL;
        const Value *ptrv = retVSTRPtr(v);
        vector<const Instruction*> instvec = func2insts(func);
        int sidx = inst2idx(vstr);
        int eidx = inst2idx(&(vstr->getParent()->back()));

        for(int i = sidx; i <= eidx; i++){
            if(isVstrInst(instvec[i])){
                if(retVSTRPtr(instvec[i]) == ptrv){
                    latevstr = instvec[i];
                }
            }
        }

        if(latevstr) return latevstr;
    }

    return v;
}


       

void CtrlIdentifier::backtrackPtrVLDR(const Value *v, set <const Value *> &doneset, set <const Value *> &vset){
    if(!v) return;
    else if(doneset.find(v) != doneset.end()) return;
    doneset.insert(v);

    if(!SVFUtil::dyn_cast<Instruction>(v)) return;
    else if(isVldrInst(v)){    //%v4_805bad8 = call float @__asm_vldr(i32 3. %v3_805bad8) so works!
        const Value *ptr = retVLDRPtr(v);
        if(ptr){
            vset.insert(v);
        }
        else{
            const Instruction *inst = SVFUtil::dyn_cast<Instruction>(v);           
            backtrackPtrVLDR(inst->getOperand(0), doneset, vset);
        }
    }
    else if(isLoadInst(v)){
        const Instruction *inst = SVFUtil::dyn_cast<Instruction>(v);           
        backtrackPtrVLDR(inst->getOperand(0), doneset, vset);
    }
    else if(isCastInst(v)){
        const Instruction *inst = SVFUtil::dyn_cast<Instruction>(v);
        vset.insert(v);
        for(unsigned i = 0; i < inst->getNumOperands(); i++){
            backtrackPtrVLDR(inst->getOperand(i), doneset, vset);
        }
    }

    return;
}


const Value *CtrlIdentifier::refptr2vldr(const Value *v, const Value *basev, ctrlinfo *ci){
    const Function *func = v2func(v);
    const Value *ret = NULL;

    // Inner analysis
    if(func != NULL && pid->initcv2cvaddr.find(v) != pid->initcv2cvaddr.end()){
        uint64_t addr = pid->initcv2cvaddr[v];
 
        //cout << ">> ref: 0x" << std::hex << addr  << std::dec << endl;
        vector <const Instruction *> instvec = retInstvecFromAddr(addr);

        for(int i = 0; i < instvec.size(); i++){
            const Instruction *inst = instvec[i];
            if(isVldrInst(inst)){
                set <const Value *> doneset, vset;
                backtrackPtrVLDR(inst, doneset, vset);

                if(vset.find(v) != vset.end()){
                    return inst;
                }
            }
        }
    }

    ret = ptr2vldr(v, basev, ci);
    return ret;
}

const Value *CtrlIdentifier::curptr2vldr(const Value *v, const Value *basev, ctrlinfo *ci){
    const Function *func = v2func(v);
    const Value *ret = NULL;

    // Inner analysis
    if(func != NULL && pid->initcv2cvaddr.find(v) != pid->initcv2cvaddr.end()){
        uint64_t addr = pid->initcv2cvaddr[v];
 
        //cout << ">> ref: 0x" << std::hex << addr  << std::dec << endl;
        vector <const Instruction *> instvec = retInstvecFromAddr(addr);

        for(int i = 0; i < instvec.size(); i++){
            const Instruction *inst = instvec[i];
            if(isVldrInst(inst)){
                set <const Value *> doneset, vset;
                backtrackPtrVLDR(inst, doneset, vset);

                if(vset.find(v) != vset.end()){
                    return inst;
                }
            }
        }
    }

    ret = ptr2vldr(v, basev, ci);
    return ret;
}

const Value *CtrlIdentifier::errptr2vldr(const Value *v, const Value *basev, ctrlinfo *ci){
    return ptr2vldr(v, basev, ci);
}

const Value *CtrlIdentifier::ptr2vldr(const Value *v, const Value *basev, ctrlinfo *ci){ // can return multi inst 
                                                                                         // (multiple inst is ok)
    int thres = 30;                                                                      // because it just increases 
                                                                                         // instrumentation targets
    const Function *func = v2func(v);
    const Instruction *sinst = SVFUtil::dyn_cast<Instruction>(v);
    const Instruction *binst = SVFUtil::dyn_cast<Instruction>(basev);
 
    // Inner analysis
    if(isCastInst(sinst) && func != NULL){
        cout << "[DEBUG]: VLDR Inner: " << inst2str(sinst) << endl;
        vector<const Instruction*> instvec = func2insts(func);
        const Instruction *tinst = NULL;
        int sidx = inst2idx(binst);
        int eidx = instvec.size();
        if(eidx > sidx + thres) eidx = sidx + thres;

        for(int i = sidx; i < eidx; i++){
            if(isVldrInst(instvec[i])){
                if(retVLDRPtr(instvec[i]) == sinst){
                    tinst = instvec[i];
                    break;
                }
            }
        }

        if(!tinst){ // Sometimes it is from vstr
            sidx = inst2idx(binst);
            eidx = instvec.size();
            if(eidx > sidx + thres) eidx = sidx + thres;

            for(int i = sidx; i < eidx; i++){
                if(isVstrInst(instvec[i])){
                    if(retVSTRPtr(instvec[i]) == sinst){
                        tinst = instvec[i];
                        break;
                    }
                }
            }
        }

        if(tinst) return tinst;
    }


    // Inter analysis -> ag
    // 1. related arg detection
    // 2. ce -> cs (- not a set)
    // 3. detect VLDR within cs -thres ~ cs
        // Step 1: Use the saved one 
    if(cearg2csargptr.find(v) != cearg2csargptr.end()){
        cout << "[DEBUG]: arg: " << inst2str(v) << " -> vldr: " << inst2str(cearg2csargptr[v]) << endl;
        return cearg2csargptr[v];
    }
        // Step 2: Find the new one.
    else if(isHeuriArg(v) || SVFUtil::dyn_cast<Argument>(v) != NULL){
        const Value *csArg = NULL;
        set <const CallInst *> csset = retCallSiteSet(func, module);
        cout << "[DEBUG]: step2-1 cssetsz: " << csset.size() << endl;
        if(csset.size() <= 2){  // heuristic rule-> can be improved w/ universal rule
            cout << "[DEBUG]: step2 arg: " << inst2str(v) << " / func: " << func2name(v2func(v)) << endl;

            for(auto it = csset.begin(); it != csset.end(); ++it){
                const CallInst *ci = *it;//*(csset.begin());
                const Function *caller = v2func(ci);
                vector <const Instruction *> instvec = func2insts(func);

                binIRInit(func);
                binIRInit(caller);
                ipa->interAnalysis(ci);

                uint64_t csaddr = inst2instaddr[ci];
                csArg = ipa->retCSArgFromCEArg(csaddr, v);
                if(csArg){
                    cout << "[DEBUG]: step2.1 csarg: " << inst2str(csArg) << " / func: " << func2name(v2func(csArg)) << endl;
                    if(isVldrInst(csArg)) return csArg;        // We are looking for vldr - not ptra
                    else if(isBinOpInst(csArg) == true && retBinOpTy(csArg) == Instruction::Add){
                        vector<const Instruction*> instvec = func2insts(func);
                        for(int i = 0; i < instvec.size(); i++){
                            if(isVldrInst(instvec[i])){
                                if(retVLDRPtr(instvec[i]) == v || instvec[i]->getOperand(0) == v){
                                    cout << "\t> step2.1.1 t vldr: " << inst2str(instvec[i]) << endl;
                                    return instvec[i];
                                }
                            }
                            else if(isVstrInst(instvec[i])){
                                if(retVSTRPtr(instvec[i]) == v || instvec[i]->getOperand(1) == v){
                                    cout << "\t> step2.1.2 t vstr: " << inst2str(instvec[i]) << endl;
                                    return instvec[i];
                                }
                            }
                        } 
                    }
                }

                // Additional analysis
                if(v == ipa->retCSAddr2ArgCEV(csaddr, ARM_REG_S0)){
                    csArg = ipa->retCSAddr2ArgCSV(csaddr, ARM_REG_S0);
                    cout << "[DEBUG]: step2.2 csarg: " << inst2str(csArg) << " / func: " << func2name(v2func(csArg)) << endl;
                    if(isVldrInst(csArg)) return csArg;        // We are looking for vldr - not ptr
                }
                else if(v == ipa->retCSAddr2ArgCEV(csaddr, ARM_REG_S1)){
                    csArg = ipa->retCSAddr2ArgCSV(csaddr, ARM_REG_S1);
                    cout << "[DEBUG]: step2.3 csarg: " << inst2str(csArg) << " / func: " << func2name(v2func(csArg)) << endl;
                    if(isVldrInst(csArg)) return csArg;        // We are looking for vldr - not ptr
                }
                else if(v == ipa->retCSAddr2ArgCEV(csaddr, ARM_REG_S2)){
                    csArg = ipa->retCSAddr2ArgCSV(csaddr, ARM_REG_S2);
                    cout << "[DEBUG]: step2.4 csarg: " << inst2str(csArg) << " / func: " << func2name(v2func(csArg)) << endl;
                    if(isVldrInst(csArg)) return csArg;        // We are looking for vldr - not ptr
                }
            }
        }
    }

    // Nothing found
    return NULL;
}

void CtrlIdentifier::tagAxisCtrls(vector <ctrlinfo *> &ctrlvec, string axis, set <ctrlinfo *> &taggedctrls){
    for(int i = 0; i < ctrlvec.size(); i++){
        tagCtrl(ctrlvec[i], axis, i, taggedctrls);
    }
    return;
}

void CtrlIdentifier::tagAxisFromBot(ctrlinfo *ci, string axis, set <ctrlinfo *> &taggedctrls){
    vector <ctrlinfo *> ctrlvec = retCtrlVecFromBot(ci);
    int numPrimitive = pid->pidInfoMap[axis].getNumCtrls(); 
    
    if(numPrimitive == ctrlvec.size()){
        tagAxisCtrls(ctrlvec, axis, taggedctrls);
    }
    else{
        cout << "[Error]: Not matched. Should be handled by the previous logic" << endl;
        assert(0);
    }
    return;
}

void CtrlIdentifier::tagAxisFromTop(ctrlinfo *ci, string axis, set <ctrlinfo *> &taggedctrls){
    vector <ctrlinfo *> ctrlvec = retCtrlVecFromTop(ci);
    int numPrimitive = pid->pidInfoMap[axis].getNumCtrls();

    if(numPrimitive == ctrlvec.size()){
        tagAxisCtrls(ctrlvec, axis, taggedctrls);
    }
    else{
        cout << "[Error]: Not matched. Should be handled by the previous logic" << endl;
        assert(0);
    }
    return;
}

void CtrlIdentifier::tagCtrl(ctrlinfo *ci, string axis, int differential_level, set <ctrlinfo *> &taggedctrls){
    string post_fix;
    if(differential_level == 0) post_fix = "_p";
    else if(differential_level == 1) post_fix = "_v";
    else if(differential_level == 2) post_fix = "_a";

    string name = axis + post_fix;
    tagCtrl(ci, name, taggedctrls);

    return;
}

void CtrlIdentifier::tagCtrl(ctrlinfo *ci, string name, set <ctrlinfo *> &taggedctrls){
    ci->role = name;
    taggedctrls.insert(ci);
    return;
}

CtrlIdentifier::ctrlinfo *CtrlIdentifier::retInCtrl(ctrlinfo *ci){
    if(inConn.find(ci) != inConn.end()) return inConn[ci];
    else return NULL;
}

CtrlIdentifier::ctrlinfo *CtrlIdentifier::retOutCtrl(ctrlinfo *ci){
    if(outConn.find(ci) != outConn.end()) return outConn[ci];
    else return NULL;
}


int CtrlIdentifier::getInOff_IdxedIncreasingOrder(ctrlinfo *ci, int idx){ 
    if(ctrl2intervmap.find(ci) != ctrl2intervmap.end()){
        map <int, const Value *> ret;
        for(auto it = ctrl2intervmap[ci].begin(); it != ctrl2intervmap[ci].end(); ++it){
            const Value *v = *it;
            if(gep2off(v) > 12) ret[gep2off(v)] = v;
        }
        if(idx < ret.size()){
            vector <int> retvec;
            for(auto it = ret.begin(); it != ret.end(); ++it){
                int voff = it->first;
                const Value *v = it->second;
                retvec.push_back(voff);
            }
            return retvec[idx];
        }
        else assert(0);
    }
    return 0;
}


int CtrlIdentifier::getInOff(ctrlinfo *ci){ 
    if(ctrl2intervmap.find(ci) != ctrl2intervmap.end()){
        set <int> ret;
        for(auto it = ctrl2intervmap[ci].begin(); it != ctrl2intervmap[ci].end(); ++it){
            const Value *v = *it;
            if(gep2off(v) > 12) ret.insert(gep2off(v));
        }
        if(ret.size() == 1) return (*ret.begin());       // only if there is one Inoff
        else{
            cout << "[DEBUG]: cur ci's role: " << ci->role << endl;
            for(auto it = ctrl2intervmap[ci].begin(); it != ctrl2intervmap[ci].end(); ++it){
                const Value *v = *it;
                cout << "[DEBUG] related v: " << inst2str(v) << endl;
                cout << "\t off: " << std::hex << gep2off(v) << std::dec << endl;
            }
            assert(0);
        }
    }
    return 0;
}

int CtrlIdentifier::retNumInOff(ctrlinfo *ci){ // Replace "hasOffCtrl" 
    if(ctrl2intervmap.find(ci) != ctrl2intervmap.end()){
        set <int> ret;
        for(auto it = ctrl2intervmap[ci].begin(); it != ctrl2intervmap[ci].end(); ++it){
            const Value *v = *it;
            if(gep2off(v) > 12) ret.insert(gep2off(v));
        }
        return ret.size();
    }
    return 0;
}


bool CtrlIdentifier::hasOffCtrl(ctrlinfo* ci, int off, set <ctrlinfo*> &ctrlset){   //off could be 4 -4 and so on
    int refoff = getInOff(ci);
    for(auto it = ctrlset.begin(); it != ctrlset.end(); ++it){
        int chkoff = getInOff(*it);
        if(chkoff + off == refoff) return true;
    }
    return false;
}

CtrlIdentifier::ctrlinfo *CtrlIdentifier::retOffCtrl(ctrlinfo* ci, int off, set <ctrlinfo*> &ctrlset){
    int refoff = getInOff(ci);
    for(auto it = ctrlset.begin(); it != ctrlset.end(); ++it){
        ctrlinfo* chkci = *it;
        int chkoff = getInOff(chkci);
        if(chkoff + off == refoff) return chkci;
    }
    return NULL;
}

vector <CtrlIdentifier::ctrlinfo *> CtrlIdentifier::retCtrlVecFromBot(ctrlinfo *ci){
    ctrlinfo *ci1 = NULL;   // Should have at least ci 
    ctrlinfo *ci2 = NULL;
    vector <ctrlinfo *> ctrlvec;

    if(!ci){
        ctrlvec.clear();
        return ctrlvec;
    }
    ctrlvec.push_back(ci);

    if(inConn.find(ci) != inConn.end()){  // inconn
        ci1 = inConn[ci];
        if(ci1){
            ctrlvec.push_back(ci1);

            if(inConn.find(ci1) != inConn.end()){
                ci2 = inConn[ci1];
                if(ci2){
                    ctrlvec.push_back(ci2);
                }
            }
        }
    }

    vector <ctrlinfo *> newvec;
    for(int i = ctrlvec.size() - 1; i >= 0; i--){
        newvec.push_back(ctrlvec[i]);
    }

    return newvec;
}

vector <CtrlIdentifier::ctrlinfo *> CtrlIdentifier::retCtrlVecFromTop(ctrlinfo *ci){
    ctrlinfo *ci1 = NULL;
    ctrlinfo *ci2 = NULL;
    vector <ctrlinfo *> ctrlvec;

    if(!ci){
        ctrlvec.clear();
        return ctrlvec;
    }
    ctrlvec.push_back(ci);

    if(outConn.find(ci) != outConn.end()){
        ci1 = outConn[ci];
        if(ci1){
            ctrlvec.push_back(ci1);

            if(outConn.find(ci1) != outConn.end()){
                ci2 = outConn[ci1];
                if(ci2){
                    ctrlvec.push_back(ci2);
                }
            }
        }
    }

    return ctrlvec;
}

bool CtrlIdentifier::hasXCtrlStructFromBot(ctrlinfo *ci){   // if transition logic can determine it
    int numPrimitive = pid->pidInfoMap["x"].getNumCtrls();
    vector <ctrlinfo *> ctrlvec = retCtrlVecFromBot(ci);        

    if(ctrlvec.size() == numPrimitive){
        for(int i = 0; i < ctrlvec.size(); i++){
            if(ctrlvec[i]->pidty != pid->pidInfoMap["x"].getCtrl(i)) return false;
        }   
        return true;
    }
    return false;
} 

bool CtrlIdentifier::hasXYCtrlStructFromBot(ctrlinfo *ci){   // if transition logic can determine it
    int numPrimitive = pid->pidInfoMap["xy"].getNumCtrls();
    vector <ctrlinfo *> ctrlvec = retCtrlVecFromBot(ci);        

    if(ctrlvec.size() == numPrimitive){
        for(int i = 0; i < ctrlvec.size(); i++){
            if(ctrlvec[i]->pidty != pid->pidInfoMap["xy"].getCtrl(i)) return false;
        }   
        return true;
    }
    return false;
} 

bool CtrlIdentifier::hasARCtrlStructFromTop(ctrlinfo *ci){  // if transition logic can determine it
    int numPrimitive = pid->pidInfoMap["ar"].getNumCtrls();
    vector <ctrlinfo *> ctrlvec = retCtrlVecFromTop(ci);        
    if(ctrlvec.size() == numPrimitive){
        for(int i = 0; i < ctrlvec.size(); i++){
            if(ctrlvec[i]->pidty != pid->pidInfoMap["ar"].getCtrl(i)){
                return false;
            }
        }   
        return true;
    }
    return false;
} 

bool CtrlIdentifier::hasCtrlStructFromBot(ctrlinfo *ci, string target){
    int numPrimitive = pid->pidInfoMap[target].getNumCtrls();
    vector <ctrlinfo *> ctrlvec = retCtrlVecFromBot(ci);
    if(ctrlvec.size() == numPrimitive){
        for(int i = 0; i < ctrlvec.size(); i++){
            if(ctrlvec[i]->pidty != pid->pidInfoMap[target].getCtrl(i)) return false;
        }   
        return true;
    }
    return false;
}

bool CtrlIdentifier::hasCtrlStructFromTop(ctrlinfo *ci, string target){
    int numPrimitive = pid->pidInfoMap[target].getNumCtrls();
    vector <ctrlinfo *> ctrlvec = retCtrlVecFromTop(ci);
    if(ctrlvec.size() == numPrimitive){
        for(int i = 0; i < ctrlvec.size(); i++){
            if(ctrlvec[i]->pidty != pid->pidInfoMap[target].getCtrl(i)) return false;
        }   
        return true;
    }
    return false;
}


CtrlIdentifier::ctrlinfo *CtrlIdentifier::botctrl2topctrl(ctrlinfo *ci){
    vector <ctrlinfo *> ctrlvec = retCtrlVecFromBot(ci);
    return ctrlvec.back();
}

CtrlIdentifier::ctrlinfo *CtrlIdentifier::topctrl2botctrl(ctrlinfo *ci){
    vector <ctrlinfo *> ctrlvec = retCtrlVecFromTop(ci);
    return ctrlvec[0];
}
            
set <string> CtrlIdentifier::retUntaggedCtrl(set <ctrlinfo*> &taggedctrls){
    set <string> untagged;
    for(auto it = pid->pidInfoMap.begin(); it != pid->pidInfoMap.end(); ++it){
        string axis = it->first;
        InitPIDObj::AxisInfo ai = it->second;
        if(ai.getNumCtrls() >= 1){
            string ctrl = axis + "_p";
            bool found = false;
            for(auto cit = taggedctrls.begin(); cit != taggedctrls.end(); ++cit){
                ctrlinfo *ci = *cit;
                if(ci->role == ctrl){
                    found = true;
                    break;
                }
            }
            if(!found) untagged.insert(ctrl);
        }
        if(ai.getNumCtrls() >= 2){
            string ctrl = axis + "_v";
            bool found = false;
            for(auto cit = taggedctrls.begin(); cit != taggedctrls.end(); ++cit){
                ctrlinfo *ci = *cit;
                if(ci->role == ctrl){
                    found = true;
                    break;
                }
            }
            if(!found) untagged.insert(ctrl);
        }
        if(ai.getNumCtrls() >= 3){
            string ctrl = axis + "_a";
            bool found = false;
            for(auto cit = taggedctrls.begin(); cit != taggedctrls.end(); ++cit){
                ctrlinfo *ci = *cit;
                if(ci->role == ctrl){
                    found = true;
                    break;
                }
            }
            if(!found) untagged.insert(ctrl);
        }
    }
    return untagged;
}

string CtrlIdentifier::retPrefix_SameLevelCtrlTy(set <string> &untagged){
    string axis_prefix = "";
    string postfix = "";
    for(auto it = untagged.begin(); it != untagged.end(); ++it){
        string ctrl = *it;
        if(!postfix.length()){
            if(ctrl.find("a") == 0) axis_prefix = "a";

            if(ctrl.find("_p") != string::npos) postfix = "_p";
            else if(ctrl.find("_v") != string::npos) postfix = "_v";
            else if(ctrl.find("_a") != string::npos) postfix = "_a";
        }
        else{
            //err will be handled by the zero length of retPostfix_SameLevelCtrlTy's return
                //axis prefix
            if(ctrl.find("a") == 0 && axis_prefix.length() == 0) return ""; //err
            else if(ctrl.find("a") != 0 && axis_prefix == "a") return "";   //err
                //postfix
            else if(ctrl.find(postfix) == string::npos) return "";          //err
        }
    }
    return axis_prefix;  // ok
}

string CtrlIdentifier::retPostfix_SameLevelCtrlTy(set <string> &untagged){
    string axis_prefix = "";
    string postfix = "";
    for(auto it = untagged.begin(); it != untagged.end(); ++it){
        string ctrl = *it;
        if(!postfix.length()){
            if(ctrl.find("a") == 0) axis_prefix = "a";

            if(ctrl.find("_p") != string::npos) postfix = "_p";
            else if(ctrl.find("_v") != string::npos) postfix = "_v";
            else if(ctrl.find("_a") != string::npos) postfix = "_a";
        }
        else{
            //err will be handled by the zero length of retPostfix_SameLevelCtrlTy's return
                //axis prefix
            if(ctrl.find("a") == 0 && axis_prefix.length() == 0) return ""; //err
            else if(ctrl.find("a") != 0 && axis_prefix == "a") return "";   //err
                //postfix
            else if(ctrl.find(postfix) == string::npos) return "";          //err
        }
    }
    return postfix;  // ok
}



