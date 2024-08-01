#ifndef _CRT_H_
#define _CRT_H_
extern map<unsigned int,desc> svdmap;
extern map<unsigned int, vector<Function*>, std::greater<unsigned int>> iomap;
typedef struct {
        string  name;
        string  file;
        string  dir;
        unsigned int addr;
        unsigned int refcount;
} refinfo;
//refcount[address][file] = access count;
extern map<unsigned int,map<string, int>> refcount;
/* Filtered version of svdmap */
extern map<int, string> device;
/* We use this dummy struct because LLVM Constant Range doesn't have a null arg constructor */
/* Device-Driver map */
extern map<int, refinfo> drivers;
typedef struct {
        APInt Lower;
        APInt Upper;
} constRange;
/* We use this dummy struct because LLVM Constant Range doesn't have a null arg constructor */
/* Device-Driver map */
extern map<int, refinfo> drivers;
int driverIsolation();
int taskKernelVoilations();
int taskTaskVoilations();

#endif 
