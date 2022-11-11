#ifndef COPROJ_ANALYSIS_H
#define COPROJ_ANALYSIS_H

#include "co_head.h"
struct LexEntry{
    std::string src;
    std::string type;
    int line;
};
extern std::vector<LexEntry>lexTable;
struct SymbolEntry{
    std::string name;
    char kind; //v var, c const, p parameter, f function
    char type; //i int, v void, a array
    int lev;
    int params; //num of parameters
    int dmn; //dimension of array
};
extern std::vector<SymbolEntry>SymbolTable;

namespace syn{
    int AddExp();
    void ConstDecl();
    void VarDecl();
    void Block(bool x=false);
}
//ANA_AUX_FUNC
bool isExpFST(std::string,std::string);
bool isLval_Eq(int);
bool checkCaseA(LexEntry);
bool checkCaseB(SymbolEntry);
bool checkCaseC(std::string);
bool checkCaseD(std::string,int);
int getFuncPosInSymbolTable(std::string);
int getArrDmn(std::string);
char getIdentKind(std::string);
int getArgsNum(std::string);
int getReturnPosInBlock(int);

void lexicalAnalysis();
void syntacticAnalysis();

#endif //COPROJ_ANALYSIS_H
