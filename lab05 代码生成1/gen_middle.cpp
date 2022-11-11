#include "global.h"
#include "analysis.h"
#include "generate.h"
using namespace std;
using namespace gen;

static int pos,lev;
static string type,src;
static LexEntry lexT;

static bool printEql=false;
static string expT,retStr;
static string inFuncName="global";
int tmpCount=0,strCount=0;
vector<SymbolEntry2>ST;
vector<MIDCode>midCode;
static map<string,string>var_name;

static void getSym(){
    if(pos+1==lexTable.size()) return;
    lexT=lexTable[++pos];
    type=lexT.type;
    src=lexT.src;
}
static void retreat(){
    lexT=lexTable[--pos];
    type=lexT.type;
    src=lexT.src;
}
static void outLex(){
    fso << type << " " << src << endl;
}
static int Exp(string &S){
    return AddExp(S);
}
static int LVal(string &S){
    int dmn,val=0;
    if(type=="IDENFR"){
        S=getRename(src);
        val=getConstVal(S);
        dmn = getArrDmn(src);
        outLex(),getSym();
        while(src=="["){
            outLex(),getSym();
            Exp(S);
            if(src=="]"){
                dmn--;
                outLex(),getSym();
            }
        }
    }
    return val;
}
static int Number(string &S){
    int val=stoi(src);
    S=src;
    outLex(),getSym();
    return val;
}
static int PrimaryExp(string &S){
    string L="",R="";
    int val=0;
    if(src=="("){
        outLex(),getSym();
        val=Exp(L);
        if(src==")"){
            outLex(),getSym();
        }
    }
    else if(type=="IDENFR"){
        val=LVal(L);
    }
    else if(type=="INTCON"){
        val=Number(L);
    }
    S=L;
    return val;
}
static void UnaryOp(){
    outLex(),getSym();
}
static void FuncRParams(){
    string L;
    MIDCode raw;
    Exp(L);
    raw.quickSet("push",L,"","");
    midCode.push_back(raw);
    while(src==","){
        outLex(),getSym();
        Exp(L);
        raw.quickSet("push",L,"","");
        midCode.push_back(raw);
    }
}
static int UnaryExp(string &S){
    string tmpVar,L="",R="";
    MIDCode raw;
    int val=0;
    if(src=="("||type=="INTCON"){
        val=PrimaryExp(L);
    }
    else if(type=="IDENFR"){
        string func=src;
        getSym();
        if(src=="("){
            retreat();
            outLex();
            getSym(),outLex(),getSym();
            raw.quickSet("fPush","","","");
            midCode.push_back(raw);
            if(src==")"){
                outLex(),getSym();
            }
            else{
                FuncRParams();
                if(src==")"){
                    outLex(),getSym();
                }
            }
            raw.quickSet("call",func,"","");
            midCode.push_back(raw);
            string tmpVar="pvar_"+ to_string(tmpCount++);
            raw.quickSet("equal",tmpVar,"RET","");
            midCode.push_back(raw);
            L=tmpVar;
        }
        else{
            retreat();
            val=PrimaryExp(L);
        }
    }
    else if(src=="+"||src=="-"||src=="!"){
        string tmpOp=src;
        UnaryOp();
        L="0";
        if(expT!="const") tmpVar="var_"+ to_string(tmpCount++);
        if(tmpOp=="+"){
            val=UnaryExp(R);
            raw.quickSet("add",tmpVar,L,R);
        }
        if(tmpOp=="-"){
            val=-UnaryExp(R);
            raw.quickSet("sub",tmpVar,L,R);
        }
        if(tmpOp=="!") val=!UnaryExp(R);
        L=tmpVar;
        if(expT!="const") midCode.push_back(raw);
    }
    S=L;
    return val;
}
static int MulExp(string &S){
    string tmpVar,L="",R="";
    MIDCode raw;
    int val=0;
    val=UnaryExp(L);
    while(src=="*"||src=="/"||src=="%"){
        string tmpOp=src;
        outLex(),getSym();
        if(expT!="const") tmpVar="var_"+ to_string(tmpCount++);
        if(tmpOp=="*"){
            val*=UnaryExp(R);
            raw.quickSet("mul",tmpVar,L,R);
        }
        if(tmpOp=="/"){
            int v=UnaryExp(R);
            if(v!=0) val/=v;
            raw.quickSet("div",tmpVar,L,R);
        }
        if(tmpOp=="%"){
            int v=UnaryExp(R);
            if(v!=0) val%=v;
            raw.quickSet("mod",tmpVar,L,R);
        }
        L=tmpVar;
        if(expT!="const") midCode.push_back(raw);
    }
    S=L;
    return val;
}
int gen::AddExp(string &S){
    string tmpVar,L="",R="";
    MIDCode raw;
    int val=0;
    val=MulExp(L);
    while(src=="+"||src=="-"){
        string tmpOp=src;
        outLex(),getSym();
        if(expT!="const") tmpVar="var_"+ to_string(tmpCount++);
        if(tmpOp=="+") {
            val+=MulExp(R);
            raw.quickSet("add",tmpVar,L,R);
        }
        if(tmpOp=="-") {
            val-=MulExp(R);
            raw.quickSet("sub",tmpVar,L,R);
        }
        L=tmpVar;
        if(expT!="const") midCode.push_back(raw);
    }
    S=L;
    return val;
}
static int ConstExp(){
    string t;
    expT="const";
    int val=AddExp(t);
    expT="";
    return val;
}
static int ConstInitVal(){
    int val=0;
    if(src=="{"){
        outLex(),getSym();
        if(src!="}"){
            ConstInitVal();
            while(src==","){
                outLex(),getSym();
                ConstInitVal();
            }
            if(src=="}"){
                outLex(),getSym();
            }
        }
        else{
            outLex(),getSym();
        }
    }
    else if(isExpFST(type,src)){
        val=ConstExp();
    }
    return val;
}
static void ConstDef(){
    string varName=inFuncName+"_"+ to_string(lev)+"_"+src;
    int val=0;
    MIDCode raw;
    ST.push_back({src,varName,'c','i',lev,0});
    outLex(),getSym();
    int dmn=0;
    while(src=="["){
        outLex(),getSym();
        if(true) ConstExp();
        if(src=="]"){
            dmn++;
            outLex(),getSym();
        }
    }
    ST[ST.size()-1].dmn=dmn;
    if(src=="="){
        outLex(),getSym();
        val=ConstInitVal();
    }
    raw.quickSet("constDecl","int",varName, to_string(val));
    midCode.push_back(raw);
}
static void FuncFParam(){
    string paraName,paraType;
    MIDCode raw;
    if(src=="int"){
        outLex(),getSym();
        if(type=="IDENFR"){
            paraName=inFuncName+"_para_"+src;
            ST.push_back({src,paraName,'p','i',lev+1,0});
            outLex(),getSym();
            int dmn=0;
            if(src=="["){
                outLex(),getSym();
                if(src=="]"){
                    outLex(),getSym();
                    dmn=1;
                }
                if(src=="["){
                    outLex(),getSym();
                    ConstExp();
                    if(src=="]"){
                        outLex(),getSym();
                        dmn++;
                    }
                }
            }
            ST[ST.size()-1].dmn=dmn;
            if(dmn==0) paraType="int";
            if(dmn==1) paraType="int[]";
            if(dmn==2) paraType="int[][]";
            raw.quickSet("para",paraType,paraName,"");
            midCode.push_back(raw);
        }
    }
}
static void FuncFParams(){
    int cnt=1;
    FuncFParam();
    while(src==","){
        outLex(),getSym();
        FuncFParam();
        cnt++;
    }
}
static void RelExp(){
    string t;
    if(isExpFST(type,src)){
        AddExp(t);
        while(src=="<"||src=="<="||src==">"||src==">="){
            fso << "<RelExp>" << endl;
            outLex(),getSym();
            AddExp(t);
        }
    }
}
static void EqExp(){
    if(isExpFST(type,src)){
        RelExp();
        while(src=="=="||src=="!="){
            fso << "<EqExp>" << endl;
            outLex(),getSym();
            RelExp();
        }
    }
}
static void LAndExp(){
    if(isExpFST(type,src)){
        EqExp();
        while(src=="&&"){
            fso << "<LAndExp>" << endl;
            outLex(),getSym();
            EqExp();
        }
    }
}
static void LOrExp(){
    if(isExpFST(type,src)){
        LAndExp();
        while(src=="||"){
            outLex(),getSym();
            LAndExp();
        }
    }
}
static void Cond(){
    LOrExp();
}
static void Stmt(){
    MIDCode raw;
    if(src==";"){
        outLex(),getSym();
    }
    else if(src=="{"){
        Block();
    }
    else if(src=="if"){
        outLex(),getSym();
        if(src=="("){
            outLex(),getSym();
            Cond();
            if(src==")"){
                outLex(),getSym();
            }
        }
        Stmt();
        if(src=="else"){
            outLex(),getSym();
            Stmt();
        }
    }
    else if(src=="while"){
        outLex(),getSym();
        if(src=="("){
            outLex(),getSym();
            Cond();
            if(src==")"){
                outLex(),getSym();
            }
        }
        Stmt();
    }
    else if(src=="break"){
        outLex(),getSym();
        if(src==";"){
            outLex(),getSym();
        }
    }
    else if(src=="continue"){
        outLex(),getSym();
        if(src==";"){
            outLex(),getSym();
        }
    }
    else if(src=="return"){
        outLex(),getSym();
        if(isExpFST(type,src)){
            string L="";
            Exp(L);
            raw.quickSet("returnInt",L,"","");
            midCode.push_back(raw);
        }
        else{
            raw.quickSet("returnVoid","","","");
            midCode.push_back(raw);
        }
        if(src==";"){
            outLex(),getSym();
        }
    }
    else if(src=="printf"){
        int args=0;
        outLex(),getSym();
        if(src=="("){
            outLex(),getSym();
            if(type=="STRCON"){
                int cnt=genPntStr(src,lev),pos;
                pos=ST.size()-cnt;
                outLex(),getSym();
                raw.quickSet("print",ST[pos++].name,"","");
                midCode.push_back(raw);
                while(src==","){
                    string L;
                    outLex(),getSym();
                    Exp(L);
                    raw.quickSet("print",L,"","");
                    midCode.push_back(raw);
                    if(pos!=ST.size()){
                        raw.quickSet("print",ST[pos++].name,"","");
                        midCode.push_back(raw);
                    }
                }
                if(src==")"){
                    outLex(),getSym();
                }
                if(src==";"){
                    outLex(),getSym();
                }
            }
        }
    }
    else if(isLval_Eq(pos)){
        string L,varName=getRename(src);
        LVal(L);
        outLex(),getSym();
        if(src=="getint"){
            outLex(),getSym();
            if(src=="("){
                outLex(),getSym();
                if(src==")"){
                    outLex(),getSym();
                }
            }
            string tmpVar="var_"+ to_string(tmpCount++);
            raw.quickSet("getint",tmpVar,"","");
            midCode.push_back(raw);
            raw.quickSet("equal",varName,tmpVar,"");
            midCode.push_back(raw);
        }
        else if(isExpFST(type,src)){
            Exp(L);
            raw.quickSet("equal",varName,L,"");
            midCode.push_back(raw);
        }
        if(src==";"){
            outLex(),getSym();
        }
    }
    else if(isExpFST(type,src)){
        string t;
        Exp(t);
        if(src==";"){
            outLex(),getSym();
        }
    }
}
static void BlockItem(){
    if(src=="const"){
        ConstDecl();
    }
    else if(src=="int"){
        VarDecl();
    }
    else{
        Stmt();
    }
}
void gen::Block(){
    if(src=="{"){
        lev++;
        outLex(),getSym();
        while(src!="}"){
            BlockItem();
        }
        if(src=="}"){
            while(ST[ST.size()-1].lev==lev){
                ST.pop_back();
            }
            lev--;
            outLex(),getSym();
        }
    }
}
void gen::ConstDecl(){
    outLex(),getSym();
    if(src=="int"){
        outLex(),getSym();
        if(type=="IDENFR") ConstDef();
        while(src==","){
            outLex(),getSym();
            if(type=="IDENFR") ConstDef();
        }
        if(src==";"){
            outLex(),getSym();
        }
    }
}
static void FuncType(){
    if(src=="void"||src=="int"){
        outLex(),getSym();
    }
}
static void FuncDef(){
    string functype=src,funcName;
    MIDCode raw;
    FuncType();
    if(type=="IDENFR"){
        inFuncName=src;
        funcName=src;
        ST.push_back({src,funcName,'f',lexTable[pos-1].src[0],lev,0});
        raw.quickSet("funcDecl",functype,funcName,"");
        midCode.push_back(raw);
        outLex(),getSym();
        if(src=="("){
            outLex(),getSym();
            if(src=="int"){
                FuncFParams();
            }
            if(src==")"){
                outLex(),getSym();
            }
            Block();
            raw.quickSet("endFunc","","","");
            midCode.push_back(raw);
        }
    }
    inFuncName="global";
}
static void MainFuncDef(){
    inFuncName="main";
    MIDCode raw;
    outLex(),getSym();//int
    outLex(),getSym();//main
    outLex(),getSym();//(
    outLex(),getSym();//)
    raw.quickSet("funcDecl","int","main","");
    midCode.push_back(raw);
    Block();
    raw.quickSet("endFunc","","","");
    midCode.push_back(raw);
    inFuncName="global";
}
static int InitVal(string &S){
    int val=0;
    if(src=="{"){
        outLex(),getSym();
        InitVal(S);
        while(src==","){
            outLex(),getSym();
            InitVal(S);
        }
        if(src=="}"){
            outLex(),getSym();
        }
    }
    else{
        val=Exp(S);
    }
    return val;
}
static void VarDef(){
    int val=0;
    string varName=inFuncName+"_"+ to_string(lev)+"_"+src;
    MIDCode raw;
    raw.quickSet("varDecl",varName,"","");
    midCode.push_back(raw);
    ST.push_back({src,varName,'v','i',lev,0});
    outLex(),getSym();
    int dmn=0;
    while(src=="["){
        outLex(),getSym();
        ConstExp();
        if(src=="]") {
            outLex(),getSym();
            dmn++;
        }
        else if(src=="["||src=="="||src==";"||src==")"||src==","){
            dmn++;
        }
    }
    ST[ST.size()-1].dmn=dmn;
    if(src=="="){
        string L="";
        outLex(),getSym();
        printEql= true;
        InitVal(L);
        printEql= false;
        raw.quickSet("equal",varName,L,"");
        midCode.push_back(raw);
    }
}
void gen::VarDecl(){
    outLex(),getSym();
    if(type=="IDENFR"){
        VarDef();
        while(src==","){
            outLex(),getSym();
            VarDef();
        }
        if(src==";") outLex(),getSym();
    }
}
static void CompUnit(){
    while(true){
        if(src=="const") ConstDecl();
        else if(src=="void") FuncDef();
        else if(src=="int"){
            getSym();
            if(src=="main"){
                ST.push_back({"main","main",'f','i',lev,0});
                retreat();
                MainFuncDef();
                break;
            }
            else if(type=="IDENFR"){
                getSym();
                if(src=="("){
                    retreat(),retreat();
                    FuncDef();
                }
                else if(src=="["||src==","||src==";"||src=="="){
                    retreat(),retreat();
                    VarDecl();
                }
            }
        }
    }
}
void generateMiddle(){
    pos=-1;
    getSym();
    CompUnit();
    for(int i=0;i<midCode.size();i++){
        fmo<<midCode[i].generateCode()<<endl;
    }
}