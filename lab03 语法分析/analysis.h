#ifndef COPROJ_ANALYSIS_H
#define COPROJ_ANALYSIS_H

#include "co_head.h"
struct LexData{
    std::string src;
    std::string type;
};
extern std::vector<LexData>lexTable;

namespace syn{
    void AddExp();
    void ConstDecl();
    void VarDecl();
    void Block();
}
bool isExpFST(std::string,std::string);
bool isLval_Eq(int);

void lexicalAnalysis();
void syntacticAnalysis();

#endif //COPROJ_ANALYSIS_H
