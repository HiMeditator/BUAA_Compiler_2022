#include "co_head.h"
#include "analysis.h"
using namespace std;
using namespace syn;

static int pos,lev;
static string type,src;
static LexEntry lexT;
vector<SymbolEntry>SymbolTable;

static int params,paraDmn,preErrLine=0,inLoopDep=0;
static char inFuncType = 'n'; //i int, v void, n not in a function
static int hasReturn;
static int endFuncLine;

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
void outLex(){
    fso << type << " " << src << endl;
}
void outSym(string desc=""){
//    cout<<"------------"<<desc<<" lev: "<<lev<<"------------\n";
//    cout<<"name\ttype\tkind\tlev\tpara\tdmn"<<endl;
//    for(int i=0;i<SymbolTable.size();i++){
//        SymbolEntry t = SymbolTable[i];
//        cout<<t.name<<"\t"<<t.type<<"\t"<<t.kind<<"\t"<<t.lev
//            <<"\t"<<t.params<<"\t"<<t.dmn<<endl;
//    }
}
void outError(char Case,int line){
    if(preErrLine==line) return;
    preErrLine=line;
    feo << line << " " << Case << endl;
}
void error(int id=-1){
    printf("Error ID:%d Src: ",id);
    cout<<src<<endl;
    outLex();
}
int Exp(){
    int dmn = AddExp();
    fso << "<Exp>" << endl;
    return dmn;
}
int LVal(char *kind = nullptr){
    //feo<<"LVal :"<<lexT.line<<"\n";
    int dmn;
    if(type=="IDENFR"){
        if(kind != nullptr){
            *kind = getIdentKind(src);
        }
        dmn = getArrDmn(src);
        if(checkCaseC(src)){
            outError('c',lexT.line);
        }
        outLex(),getSym();
        while(src=="["){
            outLex(),getSym();
            Exp();
            if(src=="]"){
                dmn--;
                outLex(),getSym();
            }
            else if(src=="["||src=="="||src==";"||src==")"||src==","){
                dmn--;
                outError('k',lexT.line);
            }
        }
    }
    fso << "<LVal>" << endl;
    return dmn;
}
void Number(){
    outLex(),getSym();
    fso << "<Number>" << endl;
}
int PrimaryExp(){
    int dmn;
    if(src=="("){
        outLex(),getSym();
        dmn = Exp();
        if(src==")"){
            outLex(),getSym();
        }
    }
    else if(type=="IDENFR"){
        dmn = LVal();
    }
    else if(type=="INTCON"){
        dmn=0;
        Number();
    }
    fso << "<PrimaryExp>" << endl;
    return dmn;
}
void UnaryOp(){
    outLex(),getSym();
    fso << "<UnaryOp>" << endl;
}
bool FuncRParams(string func){
    params=1;
    bool caseEError = false;
    int paraP = getFuncPosInSymbolTable(func) + 1;
    paraDmn = SymbolTable[paraP++].dmn;
    int expDmn = Exp();
    if(expDmn != paraDmn){
        caseEError = true;
    }
    while(src==","){
        outLex(),getSym();
        paraDmn = SymbolTable[paraP++].dmn;
        expDmn = Exp();
        if(expDmn != paraDmn){
            caseEError = true;
        }
        params++;
    }
    fso << "<FuncRParams>" << endl;
    return caseEError;
}
int UnaryExp(){
    int dmn;
    if(src=="("||type=="INTCON"){
        dmn = PrimaryExp();
    }
    else if(type=="IDENFR"){
        string func=src;
        getSym();
        if(src=="("){
            int fpos= getFuncPosInSymbolTable(func);
            dmn = SymbolTable[fpos].type=='i'?0:-1;
            retreat();
            if(checkCaseC(src)){
                outError('c',lexT.line);
            }
            outLex();
            getSym(),outLex(),getSym();
            if(src==")"){
                outLex(),getSym();
                if(checkCaseD(func,0)){
                    outError('d',lexT.line);
                }
            }
            else if(isExpFST(type,src)){//Handle case E,D
                bool caseEError = FuncRParams(func);
                if(checkCaseD(func,params)){
                    outError('d',lexT.line);
                }
                else if(caseEError){
                    outError('e', lexT.line);
                }
                if(src==")"){
                    outLex(),getSym();
                }
                else{
                    outError('j',lexTable[pos-1].line);
                }
            }
            else{
                outError('j',lexTable[pos-1].line);
            }
        }
        else{
            retreat();
            dmn = PrimaryExp();
        }
    }
    else if(src=="+"||src=="-"||src=="!"){
        UnaryOp();
        dmn = UnaryExp();
    }
    fso << "<UnaryExp>" << endl;
    return dmn;
}
int MulExp(){
    int rDmn = UnaryExp(),dmn;
    while(src=="*"||src=="/"||src=="%"){
        fso << "<MulExp>" << endl;
        outLex(),getSym();
        dmn=UnaryExp();
        if(rDmn!=dmn) rDmn=-1;
    }
    fso << "<MulExp>" << endl;
    return rDmn;
}
int syn::AddExp(){
    int rDmn = MulExp(),dmn;
    while(src=="+"||src=="-"){
        fso << "<AddExp>" << endl;
        outLex(),getSym();
        dmn=MulExp();
        if(rDmn!=dmn) rDmn=-1;
    }
    fso << "<AddExp>" << endl;
    return rDmn;
}
void ConstExp(){
    AddExp();
    fso << "<ConstExp>" << endl;
}
void ConstInitVal(){
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
        ConstExp();
    }
    fso << "<ConstInitVal>" << endl;
}
void ConstDef(){
    if(checkCaseB({src,'c','i',lev})){
        outError('b', lexT.line);
    }
    else{
        SymbolTable.push_back({src,'c','i',lev,0,0});
    }
    outLex(),getSym();
    int dmn=0;
    while(src=="["){
        outLex(),getSym();
        if(true) ConstExp();
        else error();
        if(src=="]"){
            dmn++;
            outLex(),getSym();
        }
        else if(src=="["||src=="="||src==","||src==";"||src==")"){
            dmn++;
            outError('k',lexT.line);
        }
        else error();
    }
    SymbolTable[SymbolTable.size()-1].dmn=dmn;
    if(src=="="){
        outLex(),getSym();
        if(true) ConstInitVal();
        else error();
    }
    else error();
    fso << "<ConstDef>" << endl;
}
void FuncFParam(){
    if(src=="int"){
        outLex(),getSym();
        if(type=="IDENFR"){
            if(checkCaseB({src,'p','i',lev+1})){
                outError('b',lexT.line);
            }
            SymbolTable.push_back({src,'p','i',lev+1,0,0});
            outLex(),getSym();
            int dmn=0;
            if(src=="["){
                outLex(),getSym();
                if(src=="]"){
                    outLex(),getSym();
                    dmn=1;
                }
                else{
                    dmn=1;
                    outError('k',lexT.line);
                }
                if(src=="["){
                    outLex(),getSym();
                    ConstExp();
                    if(src=="]"){
                        outLex(),getSym();
                        dmn++;
                    }
                    else{
                        dmn++;
                        outError('k',lexT.line);
                    }
                }
            }
            SymbolTable[SymbolTable.size()-1].dmn=dmn;
        }
    }
    fso << "<FuncFParam>" << endl;
}
void FuncFParams(){
    int cnt=1;
    FuncFParam();
    while(src==","){
        outLex(),getSym();
        FuncFParam();
        cnt++;
    }
    SymbolTable[SymbolTable.size()-cnt-1].params=cnt;
    fso << "<FuncFParams>" << endl;
}
void RelExp(){
    if(isExpFST(type,src)){
        AddExp();
        while(src=="<"||src=="<="||src==">"||src==">="){
            fso << "<RelExp>" << endl;
            outLex(),getSym();
            AddExp();
        }
    }
    fso << "<RelExp>" << endl;
}
void EqExp(){
    if(isExpFST(type,src)){
        RelExp();
        while(src=="=="||src=="!="){
            fso << "<EqExp>" << endl;
            outLex(),getSym();
            RelExp();
        }
    }
    fso << "<EqExp>" << endl;
}
void LAndExp(){
    if(isExpFST(type,src)){
        EqExp();
        while(src=="&&"){
            fso << "<LAndExp>" << endl;
            outLex(),getSym();
            EqExp();
        }
    }
    fso << "<LAndExp>" << endl;
}
void LOrExp(){
    if(isExpFST(type,src)){
        LAndExp();
        while(src=="||"){
            fso << "<LOrExp>" << endl;
            outLex(),getSym();
            LAndExp();
        }
    }
    fso << "<LOrExp>" << endl;
}
void Cond(){
    LOrExp();
    fso << "<Cond>" << endl;
}
void Stmt(){
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
            else{
                outError('j',lexTable[pos-1].line);
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
            else{
                outError('j',lexTable[pos-1].line);
            }
        }
        inLoopDep++;
        Stmt();
        inLoopDep--;
    }
    else if(src=="break"){
        if(!inLoopDep){
            outError('m',lexT.line);
        }
        outLex(),getSym();
        if(src==";"){
            outLex(),getSym();
        }
        else{
            outError('i', lexTable[pos-1].line);
        }
    }
    else if(src=="continue"){
        if(!inLoopDep){
            outError('m',lexT.line);
        }
        outLex(),getSym();
        if(src==";"){
            outLex(),getSym();
        }
        else{
            outError('i', lexTable[pos-1].line);
        }
    }
    else if(src=="return"){
        outLex(),getSym();
        if(isExpFST(type,src)){
            Exp();
            if(inFuncType=='v'){
                outError('f',lexT.line);
            }
        }
        if(src==";"){
            outLex(),getSym();
        }
        else{
            outError('i', lexTable[pos-1].line);
        }
        if(src=="}") hasReturn = 1;
    }
    else if(src=="printf"){
        int args=0;
        outLex(),getSym();
        if(src=="("){
            outLex(),getSym();
            if(type=="STRCON"){
                if(checkCaseA(lexT)){
                    outError('a',lexT.line);
                }
                args = getArgsNum(src);
                outLex(),getSym();
                while(src==","){
                    outLex(),getSym();
                    Exp();
                    args--;
                }
                if(args!=0){
                    outError('l',lexT.line);
                }
                if(src==")"){
                    outLex(),getSym();
                }
                else{
                    outError('j',lexTable[pos-1].line);
                }
                if(src==";"){
                    outLex(),getSym();
                }
                else{
                    outError('i', lexTable[pos-1].line);
                }
            }
        }
    }
    else if(isLval_Eq(pos)){
        char lvalKind;
        LVal(&lvalKind);
        outLex(),getSym();
        if(src=="getint"){
            if(lvalKind=='c'){
                outError('h',lexT.line);
            }
            outLex(),getSym();
            if(src=="("){
                outLex(),getSym();
                if(src==")"){
                    outLex(),getSym();
                }
                else{
                    outError('j', lexTable[pos-1].line);
                }
            }
        }
        else if(isExpFST(type,src)){
            if(lvalKind=='c'){
                outError('h',lexT.line);
            }
            Exp();
        }
        if(src==";"){
            outLex(),getSym();
        }
        else{
            outError('i', lexTable[pos-1].line);
        }
    }
    else if(isExpFST(type,src)){
        Exp();
        if(src==";"){
            outLex(),getSym();
        }
        else{
            outError('i', lexTable[pos-1].line);
        }
    }
    fso << "<Stmt>" << endl;
}
void BlockItem(){
    if(src=="const"){
        ConstDecl();
    }
    else if(src=="int"){
        VarDecl();
    }
    else{
        Stmt();
    }
    //fso<<"<BlockItem>"<<endl;
}
void syn::Block(bool isFuncBlock){
    if(src=="{"){
        outSym("=>");
        lev++;
        outLex(),getSym();
        while(src!="}"){
            BlockItem();
        }
        if(src=="}"){
            endFuncLine = lexT.line;
            while(SymbolTable[SymbolTable.size()-1].lev==lev
                &&SymbolTable[SymbolTable.size()-1].kind!='p')
                SymbolTable.pop_back();
            outSym("<=");
            lev--;
            if(hasReturn==1&&isFuncBlock){
                hasReturn=getReturnPosInBlock(pos-1);
            }
            outLex(),getSym();
        }
    }
    fso << "<Block>" << endl;
}
void syn::ConstDecl(){
    outLex(),getSym();
    if(src=="int"){
        outLex(),getSym();
        if(type=="IDENFR") ConstDef();
        else error();
        while(src==","){
            outLex(),getSym();
            if(type=="IDENFR") ConstDef();
            else error();
        }
        if(src==";"){
            outLex(),getSym();
        }
        else{
            outError('i', lexTable[pos-1].line);
        }
    }
    else error(2);
    fso << "<ConstDecl>" << endl;
}
void FuncType(){
    if(src=="void"||src=="int"){
        outLex(),getSym();
    }
    else error();
    fso << "<FuncType>" << endl;
}
void FuncDef(){
    FuncType();
    if(type=="IDENFR"){
        if(checkCaseB({src,'f',lexTable[pos-1].src[0],lev})){
            outError('b',lexT.line);
        }
        else{
            SymbolTable.push_back({src,'f',lexTable[pos-1].src[0],lev,0,0});
        }
        inFuncType = lexTable[pos-1].src[0];
        outLex(),getSym();
        if(src=="("){
            outLex(),getSym();
            if(src=="int"){
                FuncFParams();
            }
            if(src==")"){
                outLex(),getSym();
            }
            else {
                outError('j',lexTable[pos-1].line);
            }
            hasReturn = 0;
            Block(true);
            if(inFuncType=='i' && hasReturn!=2){
                outError('g', endFuncLine);
            }
            inFuncType = 'n';
        }
        else error();
    }
    else error();
    fso << "<FuncDef>" << endl;
}
void MainFuncDef(){
    outLex(),getSym();
    outLex(),getSym();
    outLex(),getSym();
    if(src==")"){
        outLex(),getSym();
    }
    else{
        outError('j',lexTable[pos-1].line);
    }
    inFuncType = 'i';
    hasReturn = 0;
    Block(true);
    if(inFuncType=='i' && hasReturn!=2){
        outError('g', endFuncLine);
    }
    inFuncType = 'n';
    fso << "<MainFuncDef>" << endl;
}
void InitVal(){
    if(src=="{"){
        outLex(),getSym();
        InitVal();
        while(src==","){
            outLex(),getSym();
            InitVal();
        }
        if(src=="}"){
            outLex(),getSym();
        }
    }
    else{//以后添加 isExpFIRST(src)
        Exp();
    }
    fso << "<InitVal>" << endl;
}
void VarDef(){
    if(checkCaseB({src,'v','i',lev})){
        outError('b',lexT.line);
    }
    else {
        SymbolTable.push_back({src,'v','i',lev,0,0});
    }
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
            outError('k',lexT.line);
        }
    }
    SymbolTable[SymbolTable.size()-1].dmn=dmn;
    if(src=="="){
        outLex(),getSym();
        InitVal();
    }
    fso << "<VarDef>" << endl;
}
void syn::VarDecl(){
    outLex(),getSym();
    if(type=="IDENFR"){
        VarDef();
        while(src==","){
            outLex(),getSym();
            VarDef();
        }
        if(src==";") outLex(),getSym();
        else{
            outError('i', lexTable[pos-1].line);
        }
    }
    else error();
    fso << "<VarDecl>" << endl;
}
void CompUnit(){
    int cnt=0;
    //while(++cnt<=5){
    while(true){
        if(src=="const") ConstDecl();
        else if(src=="void") FuncDef();
        else if(src=="int"){
            getSym();
            if(src=="main"){
                SymbolTable.push_back({"main",'f','i',lev,0,0});
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
                else{
                    outError('i',lexTable[pos-1].line);
                }
            }
            else error();
        }
        else error();
        //printf("Analysis %d finished.\n",cnt);
    }
    fso << "<CompUnit>" << endl;
}
void syntacticAnalysis(){
    //feo<<"Test Error"<<endl;
    lexicalAnalysis();
    pos=-1;
    getSym();
    CompUnit();
}