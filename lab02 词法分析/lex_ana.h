#ifndef COPROJ_LEX_ANA_H
#define COPROJ_LEX_ANA_H

struct LexData{
    std::string src;
    std::string type;
};
int getsym();
void lexicalAnalysis();

#endif //COPROJ_LEX_ANA_H
