#include "co_head.h"
#include "analysis.h"
using namespace std;
using namespace syn;

static int pos;
static string type,src;
static LexData lexT;

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
    cout<<type<<" "<<src<<endl;
}
void error(int id=-1){
    printf("Error ID:%d Lex: ",pos,id);
    outLex();
}
void Exp(){
    AddExp();
    cout<<"<Exp>"<<endl;
}
void LVal(){
    if(type=="IDENFR"){
        outLex(),getSym();
        while(src=="["){
            outLex(),getSym();
            Exp();
            if(src=="]"){
                outLex(),getSym();
            }
        }
    }
    cout<<"<LVal>"<<endl;
}
void Number(){
    outLex(),getSym();
    cout<<"<Number>"<<endl;
}
void PrimaryExp(){
    if(src=="("){
        outLex(),getSym();
        Exp();
        if(src==")"){
            outLex(),getSym();
        }
    }
    else if(type=="IDENFR"){
        LVal();
    }
    else if(type=="INTCON"){
        Number();
    }
    cout<<"<PrimaryExp>"<<endl;
}
void UnaryOp(){
    outLex(),getSym();
    cout<<"<UnaryOp>"<<endl;
}
void FuncRParams(){
    Exp();
    while(src==","){
        outLex(),getSym();
        Exp();
    }
    cout<<"<FuncRParams>"<<endl;
}
void UnaryExp(){
    if(src=="("||type=="INTCON"){
        PrimaryExp();
    }
    else if(type=="IDENFR"){
        getSym();
        if(src=="("){
            retreat(),outLex();
            getSym(),outLex(),getSym();
            if(src==")"){
                outLex(),getSym();
            }
            else{
                FuncRParams();
                if(src==")"){
                    outLex(),getSym();
                }
            }
        }
        else{
            retreat();
            PrimaryExp();
        }
    }
    else if(src=="+"||src=="-"||src=="!"){
        UnaryOp();
        UnaryExp();
    }
    cout<<"<UnaryExp>"<<endl;
}
void MulExp(){
    UnaryExp();
    while(src=="*"||src=="/"||src=="%"){
        cout<<"<MulExp>"<<endl;
        outLex(),getSym();
        UnaryExp();
    }
    cout<<"<MulExp>"<<endl;
}
void syn::AddExp(){
    MulExp();
    while(src=="+"||src=="-"){
        cout<<"<AddExp>"<<endl;
        outLex(),getSym();
        MulExp();
    }
    cout<<"<AddExp>"<<endl;
}
void ConstExp(){
    AddExp();
    cout<<"<ConstExp>"<<endl;
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
    cout<<"<ConstInitVal>"<<endl;
}
void ConstDef(){
    outLex(),getSym();
    while(src=="["){
        outLex(),getSym();
        if(true) ConstExp();
        else error();
        if(src=="]"){
            outLex(),getSym();
        }
        else error();
    }
    if(src=="="){
        outLex(),getSym();
        if(true) ConstInitVal();
        else error();
    }
    else error();
    cout<<"<ConstDef>"<<endl;
}
void FuncFParam(){
    if(src=="int"){
        outLex(),getSym();
        if(type=="IDENFR"){
            outLex(),getSym();
            if(src=="["){
                outLex(),getSym();
                if(src=="]"){
                    outLex(),getSym();
                    while(src=="["){
                        outLex(),getSym();
                        ConstExp();
                        if(src=="]"){
                            outLex(),getSym();
                        }
                    }
                }
            }
        }
    }
    cout<<"<FuncFParam>"<<endl;
}
void FuncFParams(){
    FuncFParam();
    while(src==","){
        outLex(),getSym();
        FuncFParam();
    }
    cout<<"<FuncFParams>"<<endl;
}
void RelExp(){
    if(isExpFST(type,src)){
        AddExp();
        while(src=="<"||src=="<="||src==">"||src==">="){
            cout<<"<RelExp>"<<endl;
            outLex(),getSym();
            AddExp();
        }
    }
    cout<<"<RelExp>"<<endl;
}
void EqExp(){
    if(isExpFST(type,src)){
        RelExp();
        while(src=="=="||src=="!="){
            cout<<"<EqExp>"<<endl;
            outLex(),getSym();
            RelExp();
        }
    }
    cout<<"<EqExp>"<<endl;
}
void LAndExp(){
    if(isExpFST(type,src)){
        EqExp();
        while(src=="&&"){
            cout<<"<LAndExp>"<<endl;
            outLex(),getSym();
            EqExp();
        }
    }
    cout<<"<LAndExp>"<<endl;
}
void LOrExp(){
    if(isExpFST(type,src)){
        LAndExp();
        while(src=="||"){
            cout<<"<LOrExp>"<<endl;
            outLex(),getSym();
            LAndExp();
        }
    }
    cout<<"<LOrExp>"<<endl;
}
void Cond(){
    LOrExp();
    cout<<"<Cond>"<<endl;
}
void Stmt(){
    if(src==";"){
        outLex(),getSym();
    }
    else if(src=="{"){
        Block();
    }
    else if(isLval_Eq(pos)){
        LVal();
        outLex(),getSym();
        if(src=="getint"){
            outLex(),getSym();
            if(src=="("){
                outLex(),getSym();
                if(src==")"){
                    outLex(),getSym();
                }
            }
        }
        else if(isExpFST(type,src)){
            Exp();
        }
        if(src==";"){
            outLex(),getSym();
        }
    }
    else if(isExpFST(type,src)){
        Exp();
        outLex(),getSym();
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
            Exp();
        }
        if(src==";"){
            outLex(),getSym();
        }
    }
    else if(src=="printf"){
        outLex(),getSym();
        if(src=="("){
            outLex(),getSym();
            if(type=="STRCON"){
                outLex(),getSym();
                while(src==","){
                    outLex(),getSym();
                    Exp();
                }
                if(src==")"){
                    outLex(),getSym();
                    if(src==";"){
                        outLex(),getSym();
                    }
                }
            }
        }
    }
    cout<<"<Stmt>"<<endl;
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
    //cout<<"<BlockItem>"<<endl;
}
void syn::Block(){
    if(src=="{"){
        outLex(),getSym();
        while(src!="}"){
            BlockItem();
        }
        if(src=="}"){
            outLex(),getSym();
        }
    }
    cout<<"<Block>"<<endl;
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
        else error(1);
    }
    else error(2);
    cout<<"<ConstDecl>"<<endl;
}
void FuncType(){
    if(src=="void"||src=="int"){
        outLex(),getSym();
    }
    else error();
    cout<<"<FuncType>"<<endl;
}
void FuncDef(){
    FuncType();
    if(type=="IDENFR"){
        outLex(),getSym();
        if(src=="("){
            outLex(),getSym();
            if(src=="int"){
                FuncFParams();
            }
            if(src==")"){
                outLex(),getSym();
                Block();
            }
            else error();
        }
        else error();
    }
    else error();
    cout<<"<FuncDef>"<<endl;
}
void MainFuncDef(){
    outLex(),getSym();
    outLex(),getSym();
    outLex(),getSym();
    if(src==")"){
        outLex(),getSym();
        Block();
    }
    else error();
    cout<<"<MainFuncDef>"<<endl;
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
    cout<<"<InitVal>"<<endl;
}
void VarDef(){
    outLex(),getSym();
    while(src=="["){
        outLex(),getSym();
        ConstExp();
        if(src=="]") {
            outLex(),getSym();
        }
    }
    if(src=="="){
        outLex(),getSym();
        InitVal();
    }
    cout<<"<VarDef>"<<endl;
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
        else error();
    }
    else error();
    cout<<"<VarDecl>"<<endl;
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
                else error(4);
            }
            else error();
        }
        else error();
        //printf("Analysis %d finished.\n",cnt);
    }
    cout<<"<CompUnit>"<<endl;
}
void syntacticAnalysis(){
    lexicalAnalysis();
    pos=-1;
    getSym();
    CompUnit();
}