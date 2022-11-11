#include "global.h"
#include "generate.h"
using namespace std;
static int p;
static bool isInFunc=false;
vector<MIPSData>Data;
vector<MIPSText>TextFunc,TextGlb;
map<string,string>const_type;
map<string,string>const_val;
MIPSData tD;
MIPSText tT;
queue<string>ret;
void pushData(string name,string type,string val){
    tD.quickSet(name,type,val);
    Data.push_back(tD);
    //cout<<"data "<<name<<"="<<val<<" at "<<p+1<<"\n";
}
void pushText(string cmd,string arg1,string arg2,string arg3){
    tT.quickSet(cmd,arg1,arg2,arg3);
    if(isInFunc) TextFunc.push_back(tT);
    else TextGlb.push_back(tT);
}
void loadVarOrNum(string rt,string rs){
    if(isdigit(rs[0]))
        pushText("li",rt,rs,"");
    else if(const_type[rs]=="int") pushText("li",rt,const_val[rs],"");
    else pushText("lw",rt,rs,"");
}
void constDecl(MIDCode m){
    const_type[m.arg2]=m.arg1;
    const_val[m.arg2]=m.arg3;
    if(m.arg1=="str") pushData(m.arg2,m.arg1,m.arg3);
}
void varDecl(MIDCode m){
    pushData(m.arg1,"int","0");
}
void Print(MIDCode m){
    if(isdigit(m.arg1[0])) {
        pushText("li","$a0",m.arg1,"");
        pushText("li","$v0","1","");
        pushText("syscall","","","");
    }else{
        if(const_type[m.arg1]=="str"){
            pushText("la","$a0",m.arg1,"");
            pushText("li","$v0","4","");
        }
        else if(const_type[m.arg1]=="int"){
            pushText("li","$a0",const_val[m.arg1],"");
            pushText("li","$v0","1","");
        }
        else{
            pushText("lw","$a0",m.arg1,"");
            pushText("li","$v0","1","");
        }
        pushText("syscall","","","");
    }
}
void Equal(MIDCode m){
    if(m.arg1.substr(0,4)=="str_"&&isdigit(m.arg1[4])){
        pushData(m.arg1,"int","0");
    }
    if(m.arg1.substr(0,5)=="pvar_"&&isdigit(m.arg1[5])){
        pushData(m.arg1,"int","0");
    }
    if(isdigit(m.arg2[0]))
        pushText("li","$t0",m.arg2,"");
    else if(const_type[m.arg2]=="int")
        pushText("li","$t0",const_val[m.arg2],"");
    else pushText("lw","$t0",m.arg2,"");
    pushText("sw","$t0",m.arg1,"");
}
void Calc(MIDCode m,string cmd){
    pushData(m.arg1,"int","0");

    if(isdigit(m.arg2[0]))
        pushText("li","$t0",m.arg2,"");
    else if(const_type[m.arg2]=="int")
        pushText("li","$t0",const_val[m.arg2],"");
    else
        pushText("lw","$t0",m.arg2,"");
    if(isdigit(m.arg3[0]))
        pushText("li","$t1",m.arg3,"");
    else if(const_type[m.arg3]=="int")
        pushText("li","$t1",const_val[m.arg3],"");
    else
        pushText("lw","$t1",m.arg3,"");

    if(cmd=="mult"||cmd=="div"||cmd=="mod"){
        if(cmd=="mult") {
            pushText("mult","$t0","$t1","");
            pushText("mflo","$t2","","");
        }
        if(cmd=="div"){
            pushText("div","$t0","$t1","");
            pushText("mflo","$t2","","");
        }
        if(cmd=="mod"){
            pushText("div","$t0","$t1","");
            pushText("mfhi","$t2","","");
        }
    }
    else{
        pushText(cmd,"$t2","$t0","$t1");
    }
    pushText("sw","$t2",m.arg1,"");
}
void getInt(MIDCode m){
    pushData(m.arg1,"int","0");
    pushText("li","$v0","5","");
    pushText("syscall","","","");
    pushText("sw","$v0",m.arg1,"");
}
void Return(MIDCode m,string funcName){
    if(funcName=="main"){
        pushText("li","$v0","10","");
        pushText("syscall","","","");
        return;
    }
    if(m.type=="returnInt"){
        if(isdigit(m.arg1[0]))
            pushText("li","$v0",m.arg1,"");
        else pushText("lw","$v0",m.arg1,"");
        pushText("sw","$v0","RET","");
    }
    pushText("LW","$ra","0","$sp");
    pushText("li","$t0","4","");
    pushText("add","$sp","$sp","$t0");
    pushText("jr","$ra","","");
}
void Push(){
    vector<MIDCode>paras;
    MIDCode m;
    ++p;//skip fPush
    while(true){
        m=midCode[p];
        if(m.type=="fPush") {
            Push();
        }
        else if(m.type=="push"){
            paras.push_back(m);
        }
        else if(m.type=="call"){
            break;//call
        }
        else{
            if(m.type=="equal") Equal(m);
            if(m.type=="add") Calc(m,"add");
            if(m.type=="sub") Calc(m,"sub");
            if(m.type=="mul") Calc(m,"mult");
            if(m.type=="div") Calc(m,"div");
            if(m.type=="mod") Calc(m,"mod");
        }
        p++;
    }
    int d=0;
    for(int pos=1;pos<=paras.size();pos++){
        m=paras[pos-1];
        if(pos==1) loadVarOrNum("$a0",m.arg1);
        else if(pos==2) loadVarOrNum("$a1",m.arg1);
        else if(pos==3) loadVarOrNum("$a2",m.arg1);
        else if(pos==4) loadVarOrNum("$a3",m.arg1);
        else{
            d=(pos-4)*(-4);
            if(isdigit(m.arg1[0])){
                pushText("li","$t0", m.arg1,"");
                pushText("SW","$t0", to_string(d),"$sp");
            }
            else{
                pushText("lw","$t0", m.arg1,"");
                pushText("SW","$t0", to_string(d),"$sp");
            }
        }
    }
    pushText("li","$t0", to_string(d),"");
    pushText("add","$sp","$sp","$t0");//$sp-=(para-4)*4
    pushText("jal",midCode[p].arg1,"","");
    pushText("li","$t0", to_string(d),"");
    pushText("sub","$sp","$sp","$t0");
    p++;//pvar_x = RET
    if(!isdigit(midCode[p].arg1[0]))
        pushData(midCode[p].arg1,"int","1");
    pushText("sw","$v0",midCode[p].arg1,"");
}
void Para(){
    MIDCode m;
    int paraNum=0,pos=p;
    while(midCode[pos++].type=="para") paraNum++;
    pos=0;
    while(midCode[p].type=="para"){
        m=midCode[p++];
        pos++;
        pushData(m.arg2,"int","0");
        if(pos==1) pushText("sw","$a0",m.arg2,"");
        else if(pos==2) pushText("sw","$a1",m.arg2,"");
        else if(pos==3) pushText("sw","$a2",m.arg2,"");
        else if(pos==4) pushText("sw","$a3",m.arg2,"");
        else{
            int d=(paraNum-pos)*4;
            pushText("LW","$t0", to_string(d),"$sp");
            pushText("sw","$t0", m.arg2,"");
        }
    }
    p--;
}
void parseFunc(){
    isInFunc=true;
    MIDCode m=midCode[p];
    string funcName=m.arg2;
    pushText("label",funcName,"","");
    if(funcName!="main"){
        pushText("SW","$ra","-4","$sp");
        pushText("li","$t0","-4","");
        pushText("add","$sp","$sp","$t0");
    }
    m=midCode[++p];
    while(m.type!="endFunc"){
        if(m.type=="constDecl") constDecl(m);
        if(m.type=="varDecl") varDecl(m);
        if(m.type=="print") Print(m);
        if(m.type=="equal") Equal(m);
        if(m.type=="add") Calc(m,"add");
        if(m.type=="sub") Calc(m,"sub");
        if(m.type=="mul") Calc(m,"mulcot");
        if(m.type=="div") Calc(m,"div");
        if(m.type=="mod") Calc(m,"mod");
        if(m.type=="getint") getInt(m);
        if(m.type=="returnInt") Return(m,funcName);
        if(m.type=="returnVoid") Return(m,funcName);
        if(m.type=="fPush") Push();
        if(m.type=="para") Para();
        m=midCode[++p];
    }
    if(funcName!="main"){
        pushText("LW","$ra","0","$sp");
        pushText("li","$t0","4","");
        pushText("add","$sp","$sp","$t0");
        pushText("jr","$ra","","");
    }
    isInFunc= false;
}
void generateMIPS(){
    string type;
    pushData("RET","int","0");
    for(p=0;p<midCode.size();p++){
        type=midCode[p].type;
        if(type=="funcDecl") parseFunc();
        if(type=="constDecl") constDecl(midCode[p]);
        if(type=="varDecl") varDecl(midCode[p]);
        if(type=="equal") Equal(midCode[p]);
        if(type=="add") Calc(midCode[p],"add");
        if(type=="sub") Calc(midCode[p],"sub");
        if(type=="mul") Calc(midCode[p],"mult");
        if(type=="div") Calc(midCode[p],"div");
        if(type=="mod") Calc(midCode[p],"mod");
    }
    fmips<<".data"<<endl;
    for(int i=0;i<Data.size();i++){
        fmips<<Data[i].generateCode()<<endl;
    }
    fmips<<".text"<<endl;
    for(int i=0;i<TextGlb.size();i++){
        fmips<<TextGlb[i].generateCode()<<endl;
    }
    fmips<<"j main"<<endl;
    for(int i=0;i<TextFunc.size();i++){
        fmips<<TextFunc[i].generateCode()<<endl;
    }
}