#ifndef ALIASFINDER_H_
#define ALIASFINDER_H_

#include "UtilDef.h"
#include "DisPatch.h"

class AliasFinder: public UtilDef {
public:
    AliasFinder(){ assert(0); };
    AliasFinder(SVFModule inmodule, PointerAnalysis* inpta, map <string, set<const Value *>> invarmapset){
        module = inmodule;
        pta = inpta;
        varmapset = invarmapset; 
    };

    // Main
    void afmain();

private:
    SVFModule module;
    PointerAnalysis* pta;
    map <string, set<const Value *>> varmapset;

    // Module
    void mapAlias(string varstr);
    void updateAliases(const Value *v, set <const Value *> &valset);
    void recursiveSearchPAGNode(const PAGNode *n, set <const PAGNode *> &pnset);    
};

#endif

