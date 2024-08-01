#include "DisPatch/AliasFinder.h"

void AliasFinder::afmain(){
    // Variable 
    for(auto it = varmapset.begin(); it != varmapset.end(); ++it){
        string varstr = it->first;
        mapAlias(varstr);
    }
}

void AliasFinder::mapAlias(string varstr){
    set <const Value *> newvalset;
    newvalset.clear();

    // 0. Error check
    if(varmapset.find(varstr) == varmapset.end()){
        cout << "[Error]: Following key does not exist in the varmapset: " << varstr << endl;
        assert(0);
    }

    // 1. Find the alias 
    for(auto it = varmapset[varstr].begin(); it != varmapset[varstr].end(); ++it){
        const Value *v = *it;
        cout << "[DEBUG]: target var: " << inst2str(v) << endl;
        updateAliases(v, newvalset);
    }

    // 2. Merge alias
    varmapset[varstr].insert(newvalset.begin(), newvalset.end());
    newvalset.clear();

    return;
}


void AliasFinder::updateAliases(const Value *v, set <const Value *> &valset){
    PAG* pag = pta->getPAG();
    for(int i =0; i < pag->getPAGNodeNum(); i++){
        if (pag->findPAGNode((unsigned) i)) {
            const PAGNode *pn = pag->getPAGNode((unsigned) i);
            if(pn->hasValue() && (pn->getValue() == v)) {
                const Value* val = pn->getValue();
                set <const PAGNode *> pnset;
                pnset.clear();
                // PAG based inst checker
                recursiveSearchPAGNode(pn, pnset);
                for (auto pit = pnset.begin(); pit != pnset.end(); ++pit){
                    if((*pit)->hasValue()){
                        const Value *pv = (*pit)->getValue();

                        if((!pv) || (SVFUtil::dyn_cast<Function>(pv))) continue;

                        // Note: This is only for analysis at this time. Not sure whether I use this line
                        if(const Instruction *vinst = SVFUtil::dyn_cast<Instruction>(pv)){
                            // update data from here
                            cout << "\t" << inst2str(vinst) << endl;
                        }
                    }
                }
            }
        }
    }

    return;
}


void AliasFinder::recursiveSearchPAGNode(const PAGNode *n, set <const PAGNode *> &pnset){
    if(pnset.find(n) != pnset.end()) return;
    pnset.insert(n);

    for(auto sit = n->OutEdgeBegin(); sit != n->OutEdgeEnd(); ++sit){
        const PAGEdge *edge = *sit;
        const PAGNode *dsn = edge->getDstNode();

        if(dsn && dsn->hasOutgoingEdge()){
            recursiveSearchPAGNode(dsn, pnset);
        }
    }
    return ;
}

