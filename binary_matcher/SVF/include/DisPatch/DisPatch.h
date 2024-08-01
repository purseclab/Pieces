#ifndef DISPATCH_H_
#define DISPATCH_H_

#include <set>
#include <vector>
#include "UtilDef.h"
#include "AliasFinder.h"
#include "CtrlIdentifier.h"
#include "InitPIDObj.h"
#include "InterProcAnal.h"
#include "Util/BasicTypes.h"
#include "WPA/Andersen.h"
#include "Util/SVFUtil.h"


class DisPatch: public ModulePass, public UtilDef {

public:
    /// Pass ID
    static char ID;

    static ModulePass* modulePass;

    /// Constructor
    DisPatch();

    /// Destructor
    virtual ~DisPatch();

    /// We start the pass here
    virtual bool runOnModule(Module& inmodule);

    /// Pass name
    virtual StringRef getPassName() const {
        return "DisPatch Analyzer";
    }

private:
    PointerAnalysis* pta;
    SVFModule module;

    void debug_instprint(string fname);
    void debug_asmprint();
};

#endif
