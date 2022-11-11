#ifndef COPROJ_GENERATE_H
#define COPROJ_GENERATE_H

#include "global.h"

namespace gen{
    int AddExp(std::string &);
    void ConstDecl();
    void VarDecl();
    void Block();
}
struct SymbolEntry2{
    std::string name;
    std::string rename;
    char kind; //v var, c const, p parameter, f function
    char type; //i int, v void, a array, s string
    int lev;
    int dmn; //dimension of array
};
class MIDCode{
public:
    std::string type;
    //label print jump branch getint push call add sub mul div mod
    //equal arrDecl constDecl varDecl funcDecl para endFunc
    //returnInt returnVoid fPush
    std::string code;//raw code
    //var names that need to be processed:
    //var_*,RET
    std::string arg1;
    std::string arg2;
    std::string arg3;
    void quickSet(std::string T,std::string a1,std::string a2,std::string a3){
        type=T;arg1=a1;arg2=a2;arg3=a3;
    }
    std::string generateCode(){
        if(type=="constDecl") code="const "+arg1+" "+arg2+" = "+arg3;
        if(type=="varDecl") code="var int "+arg1;
        if(type=="equal") code=arg1+" = "+arg2;
        if(type=="funcDecl") code=arg1+" "+arg2+"()";
        if(type=="para") code="para "+arg1+" "+arg2;
        if(type=="add") code=arg1+" = "+arg2+" + "+arg3;
        if(type=="sub") code=arg1+" = "+arg2+" - "+arg3;
        if(type=="mul") code=arg1+" = "+arg2+" * "+arg3;
        if(type=="div") code=arg1+" = "+arg2+" / "+arg3;
        if(type=="mod") code=arg1+" = "+arg2+" % "+arg3;
        if(type=="returnInt") code="ret "+arg1;
        if(type=="returnVoid") code="return";
        if(type=="endFunc") code="endFunc";
        if(type=="call") code="call "+arg1;
        if(type=="push") code="push "+arg1;
        if(type=="print") code="print "+arg1;
        if(type=="getint") code="scanf "+arg1;
        if(type=="fPush") code="fPush";
        return code;
    }
};
extern int tmpCount,strCount;
extern std::vector<SymbolEntry2>ST;
extern std::vector<MIDCode>midCode;

class MIPSData{
public:
    std::string name;
    std::string type;
    std::string val;
    std::string code;
    void quickSet(std::string n,std::string t,std::string v){
        name=n;val=v;
        if(t=="int") type="word";
        if(t=="str") type="asciiz";
    }
    std::string generateCode(){
        code=name+": ."+type+" "+val;
        return code;
    }
};
class MIPSText{
public:
    std::string cmd;
    std::string arg1;
    std::string arg2;
    std::string arg3;
    std::string code;
    void quickSet(std::string c,std::string a1,std::string a2,std::string a3){
        cmd=c;arg1=a1;arg2=a2;arg3=a3;
    }
    std::string generateCode(){
        if(cmd=="label") code=arg1+":";
        if(cmd=="li") code=cmd+" "+arg1+","+arg2;
        if(cmd=="syscall") code=cmd;
        if(cmd=="addi") code=cmd+" "+arg1+","+arg2+","+arg3;
        if(cmd=="la") code=cmd+" "+arg1+","+arg2;
        if(cmd=="lw") code=cmd+" "+arg1+","+arg2;
        if(cmd=="LW") code="lw "+arg1+","+arg2+"("+arg3+")";
        if(cmd=="sw") code=cmd+" "+arg1+","+arg2;
        if(cmd=="SW") code="sw "+arg1+","+arg2+"("+arg3+")";
        if(cmd=="add") code=cmd+" "+arg1+","+arg2+","+arg3;
        if(cmd=="sub") code=cmd+" "+arg1+","+arg2+","+arg3;
        if(cmd=="mult") code=cmd+" "+arg1+","+arg2;
        if(cmd=="div") code=cmd+" "+arg1+","+arg2+","+arg3;
        if(cmd=="mflo") code=cmd+" "+arg1;
        if(cmd=="mfhi") code=cmd+" "+arg1;
        if(cmd=="jr") code=cmd+" "+arg1;
        if(cmd=="j") code=cmd+" "+arg1;
        if(cmd=="jal") code=cmd+" "+arg1;
        return code;
    }
};
int genPntStr(std::string,int);
std::string getRename(std::string);
int getConstVal(std::string);

void generateMiddle();
void generateMIPS();
#endif //COPROJ_GENERATE_H
