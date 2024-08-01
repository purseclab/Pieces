#ifndef CTRLIDENTIFIER_H_
#define CTRLIDENTIFIER_H_

#include "UtilDef.h"
#include "BinIRWrapper.h"
#include "InitPIDObj.h"
#include "InterProcAnal.h"
#include "AliasFinder.h"
#include "WPA/Andersen.h"

class AliasFinder;

class CtrlIdentifier: public BinIRWrapper {
public:
    CtrlIdentifier(){ assert(0); };
    CtrlIdentifier(SVFModule &inmodule, InitPIDObj *inpid, InterProcAnal *inipa, unsigned inmaxdist, unsigned inmaxcallercxt){
        module = inmodule;
        pid = inpid;
        ipa = inipa;
        maxdist = inmaxdist;
        maxcallercxt = inmaxcallercxt;
        binInit(module);
    };

    // main
    void cimain();

    // Type
    typedef struct _ctrlinfo{
        set <const Value *> curs;
        set <const Value *> refs;
        set <const Value *> errs;
        set <const Value *> outs;   //ctrl out
        set <const Value *> mathouts;

        const Function *func;
        const Value * out;   // originally to
        const Value * in;    //to is not usually a stored variable. If that is necessary, find alias  // originally from
        string role;
        string groundtruth; 
        string pidty;
    } ctrlinfo;

private:
////////////////////// Variable
    // Input
    SVFModule module;
    InitPIDObj *pid;
    InterProcAnal *ipa;
    PointerAnalysis* pta;
    AliasFinder *af;

    // Processing metadata
    vector<string> ctrlKeyVec { "cur", "ref", "err"};

    // Setting 
    unsigned maxdist;
    unsigned maxcallercxt;

    // Util data
    map <const Function *, unsigned> func2lastCtrlVarIdx;
    ctrlinfo *rci, *pci, *yci; 

    // Intermediate result
    map <const Value *, const Value *> err2refInnerConnMap, ref2errInnerConnMap;
    map <const Value *, const Function *> var2FuncMap;
    map <const Function *, set <const Value*>> func2TaggedvarsetMap;
    set <const Value*> checkedCVset;
    map <const Value *, const Value *> ref2oriref;
    map <const Value *, string> rolemap;
    map <const Value *, set <const Value *>> ptr2floatMemAccess;

    // Inter arg / ret
    map <const Value *, const Value *> cearg2csargptr, ceout2csoutptr;

    // Final dependency result
    map <const Value *, ctrlinfo *> ref2ctrlmap, err2ctrlmap, cur2ctrlmap, mathout2ctrlmap;
    map <ctrlinfo *, set<const Value *>> ctrl2intervmap;
    map <ctrlinfo *, ctrlinfo *> outConn, inConn;  
    map <ctrlinfo *, set <ctrlinfo *> > outTrans, inTrans;
    set <ctrlinfo *> ctrlset;
    map <const Value *, const Value *> ref2alias;
    map <const Value *, const Value *> alias2ref;

    // Role determination
    map <int, ctrlinfo *> inoff2ctrl;
            // ctrlinfo * -> initpid->CtrlInfo*
    map <ctrlinfo *, InitPIDObj::CtrlInfo*> ctrlinfomap;

    // Special instruction handling
        // For isPropaInst
    map <const Instruction *, set <unsigned>> handledOffMap; //e.g., ExtractElementInst, %9 = extractelement <2 x float> %8, 'i32 0'

    // Final result // low high?
    map <ctrlinfo *, const Value *> ctrl2finalref, ctrl2finalerr, ctrl2finalcur, ctrl2finalout;
    map <ctrlinfo *, const Value *> ctrl2finalref2, ctrl2finalerr2, ctrl2finalcur2, ctrl2finalout2; // for 2 dimension

//////////////////// Functions
    // Module Functions
    void innerVarDependencyTagging();
    void roleInnerFuncIdentification();
    void unconnectedRefAliasDetect();
    void cascadingTransition();
    void roleInterFuncIdentification(); //+ dependency Connecting operation
    void roleConfirm();

        // Submodules of roleInnerFuncIdentification
    bool retTargetSrcVars(set <const Value *> &srcset, const Function *func);
    bool retTargetDstVars(const Value *sv, vector <const Value *> &dvec, const Function *func);
    bool innerDepFromErr2Ref(const Value *sv, const Value *dv, unsigned si, unsigned di, const Function *func);
    unsigned initSrcPoint(const Value *sv, unsigned si, unsigned di, const Function *func);
    unsigned initDstPoint(const Value *dv, unsigned di, const Function *func);
    bool recursiveReachabilityTrack(set <const Value *> svset, const Value *dv, const Function *func, unsigned minidx, unsigned maxidx);
    const Value *retClosestSrc2Dst(const Value *sv,  unsigned si, unsigned di, const Function *func);
    const Value *retClosestDst2Src(const Value *dv,  unsigned di, unsigned si, const Function *func);

            // Submodule of recursiveReachabilityTrack
    bool isBackwardPropaInst(set <const Value *> svset, set <const Value *> &doneset, const Value *dv, const Function *func, unsigned minidx, unsigned maxidx);
    bool isUpdatedByCallee(const Function *func, const Value *dv);
    void batchInterAnalysis(unsigned si, unsigned di, const Function *func);
    void updateInterCallee(const Function *crfunc, const Value *dv, set <const Value *> &vset, unsigned si, unsigned di);
    void findVarsAffectingCallee(const Function *crfunc, const Value *dv, set <const Value *> &vset, unsigned si, unsigned di);
    unsigned retOffFromSVSet(set <const Value *> &svset);

                // Subsubmodule of isBackwardPropaInst
    unsigned retMaxIdx(unsigned minidx, unsigned maxidx, const Value *v){
        unsigned newmaxidx = maxidx;
        if(const Instruction *inst = SVFUtil::dyn_cast<Instruction>(v)) newmaxidx = inst2idx(inst);

        if(newmaxidx < minidx) newmaxidx = maxidx;
        else if(newmaxidx > maxidx) newmaxidx = maxidx;

        return newmaxidx;
    }
        // Submodule of unconnectedRefAliasDetect
    const Value *retRecursiveReachedPtr(const Value *v, const Function *curfunc);
    bool backtrackVLDRLoadingAlias(const Value *v, const Function *func, set <const Value *> &doneset, set <const Value *> &vset);
    bool backtrackArgPtrAlias(const Value *v, set <const Value *> argset, const Function *func, set <const Value *> &doneset, set <const Value *> &vset);

        // Submodule of cascadingTransition
    bool filterCascadingTransition(const Function *func, map <const Value *, string> &var2role, map <string, set<const Value*>> &role2var);
    bool taggingTransition(const Function *cr, vector <const CallInst *> &callstack, set <const Value *> &outs, map <string, set<const Value *>> &role2varset);
    bool recursiveCallStackReachabilityTrack(set <const Value *> svset, const Value *dv, const Function *cr, vector <const CallInst *> &callstack);
    void recursiveAlias2Ctrl(const Function *cr, vector <const CallInst *> &callstack, string role, set <const Value *> &varset, map <unsigned, ctrlinfo *> &aliasoff2ctrl, map <string, ctrlinfo *> &role2ctrl);
    unsigned recursiveCallStackGepOff(const Function *cr, vector <const CallInst *> &callstack, const Value *dv);
    void getcurcscefunc(set<const Value *> vset, set<const Function*> &fset);
    void getcurcscefunc(const Value *v, set<const Function*> &fset);

        // Submodule of roleInterFuncIdentification
    const Function *recursiveToSrcFunc(const Function *func, set <unsigned> &offset, vector <const CallInst*> &calllist, int depth, int maxidx);
    const Function *recursiveToSinkFunc(const Function *func, set <unsigned> &offset, vector <const CallInst*> &calllist, int depth, unsigned minidx);
    bool recursiveInterBacktrackToRet(const Function *func, vector <const CallInst *> &calllist, const Value *tv, const Value *dv, set<const Value *> &doneset, unsigned minidx, unsigned maxidx);
    bool recursiveInterBacktrackFromArg(const Function *func, vector <const CallInst *> &calllist, const Value *tv, const Value *dv, set<const Value *> &doneset, unsigned minidx, unsigned maxidx);
    vector <const Value *> retSinkVec(const CallInst *ci, set <unsigned> &offset);
    vector <const Value *> retSinkVec(const CallInst *ci, unsigned offset);
    vector <const Value *> retSrcVec(const CallInst *ci, set <unsigned> &offset);
    vector <const Value *> retSrcVec(const CallInst *ci, unsigned offset);
    ctrlinfo *ref2ctrl(const Value *v);
    ctrlinfo *off2refctrl(unsigned off);

        // submodule
    void tagAxisCtrls(vector <ctrlinfo *> &ctrlvec, string axis, set <ctrlinfo *> &taggedctrls);
    void tagAxisFromBot(ctrlinfo *ci, string axis, set <ctrlinfo *> &taggedctrls);
    void tagAxisFromTop(ctrlinfo *ci, string axis, set <ctrlinfo *> &taggedctrls);
    void tagCtrl(ctrlinfo *ci, string axis, int differential_level, set <ctrlinfo *> &taggedctrls);
    void tagCtrl(ctrlinfo *ci, string name, set <ctrlinfo *> &taggedctrls);
    ctrlinfo *retInCtrl(ctrlinfo *ci);
    ctrlinfo *retOutCtrl(ctrlinfo *ci);
    int getInOff(ctrlinfo *ci);
    int getInOff_IdxedIncreasingOrder(ctrlinfo *ci, int idx);
    int retNumInOff(ctrlinfo *ci);
    bool hasOffCtrl(ctrlinfo* ci, int off, set <ctrlinfo*> &ctrlset);
    ctrlinfo *retOffCtrl(ctrlinfo* ci, int off, set <ctrlinfo*> &ctrlset);
    vector <ctrlinfo *> retCtrlVecFromBot(ctrlinfo *ci);
    vector <ctrlinfo *> retCtrlVecFromTop(ctrlinfo *ci);
    bool hasXCtrlStructFromBot(ctrlinfo *ci);
    bool hasXYCtrlStructFromBot(ctrlinfo *ci);
    bool hasARCtrlStructFromTop(ctrlinfo *ci);
    bool hasCtrlStructFromBot(ctrlinfo *ci, string target);
    bool hasCtrlStructFromTop(ctrlinfo *ci, string target);
    ctrlinfo *botctrl2topctrl(ctrlinfo *ci);
    ctrlinfo *topctrl2botctrl(ctrlinfo *ci);
    set <string> retUntaggedCtrl(set <ctrlinfo*> &taggedctrls);
    string retPrefix_SameLevelCtrlTy(set <string> &untagged);
    string retPostfix_SameLevelCtrlTy(set <string> &untagged);
    const Value *ptr2vstr(const Value *v, const Value *basev); // v != basev if basev is call inst and that is from callee
    const Value *retLateVstr(const Value *v);
    const Value *ptr2vldr(const Value *v, const Value *basev, ctrlinfo *ci); // v != basev if basev is call inst and that is from callee
    const Value *refptr2vldr(const Value *v, const Value *basev, ctrlinfo *ci);
    const Value *curptr2vldr(const Value *v, const Value *basev, ctrlinfo *ci);
    const Value *errptr2vldr(const Value *v, const Value *basev, ctrlinfo *ci);
    void backtrackPtrVLDR(const Value *v, set <const Value *> &doneset, set <const Value *> &vset);
    void retPatchCxtCandid(ctrlinfo *ci, int axis, map <string, set<const Instruction*>> &cxtmap);
    void retPatchCxt(ctrlinfo *ci, int axis, map<ctrlinfo*, map <string, set<const Instruction*>>> &cicxtcandidmap, map <string, const Instruction*> &cxtmap);

            // Util
    bool isForIdenticalController(const Value *sv, const Value *dv);
    bool isPI(double v){
        if(fabs(v - 3.14) < 0.01){
            return true;
        }
        return false;
    }
    bool is180(double v){
        if((fabs(v - 180.0) < 0.01) || (fabs(v - 1800.0) < 0.01)  || (fabs(v - 18000.0) < 0.01) || (fabs(v -180000.0) < 0.01) || (fabs(v - 1.8) < 0.01)){
            return true;
        }
        return false;
    }
    bool isG(double v){
        if((fabs(v - 98.0665) < 0.01) || (fabs(v - 980.665) < 0.01) || (fabs(v - 9.8066) < 0.01)){
            return true;
        }
        return false;
    }
};

#endif

