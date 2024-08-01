#include "DisPatch/UtilDef.h"

map <const Function *, unsigned> UtilDef::func2szMap;
map <const Function *, map <const Instruction *, unsigned>> UtilDef::func2inst2idxmap;
map <const Function *, map <unsigned, const Value *>> UtilDef::func2idx2instmap;
map <const Function *, map <const Value *, unsigned>> UtilDef::func2arg2idxmap;
map <const Function *, map <unsigned, const Value *>> UtilDef::func2idx2argmap;
map <const Value *, vector <unsigned>> UtilDef::var2offvec;
map <const Value *, unsigned> UtilDef::var2offsum;
map <unsigned, set <const Value *>> UtilDef::off2varset;
map <const Function *, vector<const Instruction*>> UtilDef::func2instvec;
map <const Function *, map <const Value *, vector <unsigned>>> UtilDef::func2var2offvec;
map <const Function *, map <const Value *, unsigned>> UtilDef::func2var2offsum;
map <const Function *, map <unsigned, set <const Value *>>> UtilDef::func2off2varset;    //off = offsum
map <const Function *, set <unsigned>> UtilDef::func2offsums;    //off = offsum
