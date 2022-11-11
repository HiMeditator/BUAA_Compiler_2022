#include "global.h"
#include "analysis.h"
using namespace std;

bool isExpFST(string type,string src){
    return  src=="+"||src=="-"||src=="!"||src=="("||
            type=="INTCON"||type=="IDENFR";
}
bool isLval_Eq(int pos){
    while(lexTable[pos].src!=";"){
        //cout<<lexTable[pos].src<<" ";
        if(lexTable[pos].src=="=") {
            //cout<<"  end\n";
            return true;
        }
        pos++;
    }
//    if(lexTable[pos++].type=="IDENFR"){
//        int cnt=0;
//        if(lexTable[pos].src=="[") {
//            cnt++;
//            while (++pos) {
//                if (lexTable[pos].src == "[") cnt++;
//                if (lexTable[pos].src == "]") cnt--;
//                if (cnt == 0 && lexTable[pos + 1].src != "[") {
//                    pos++;
//                    break;
//                }
//
//            }
//        }
//        return lexTable[pos].src=="=";
//    }
    return false;
}
bool checkFormatString(char chr,char chr2){
    if(chr=='%'&&chr2=='d') return true;
    if(chr=='\\'){
        if(chr2=='n') return true;
        else return false;
    }
    if(chr==' '||chr=='!'||(40<=chr&&chr<=126))
        return true;
    return false;
}
bool checkCaseA(LexEntry lext){
    for(int i=1;i<lext.src.size()-1;i++){
        if(!checkFormatString(lext.src[i],lext.src[i+1]))
            return true;
    }
    return false;
}
bool checkCaseB(SymbolEntry sym){
    for(int i=SymbolTable.size()-1;i>=0;i--){
        if(SymbolTable[i].lev!=sym.lev) return false;
        if(SymbolTable[i].name==sym.name){
            return true;
        }
    }
    return false;
}
bool checkCaseC(string ident){
    bool outLev = false;
    for(int i=SymbolTable.size()-1;i>=0;i--){
        if(SymbolTable[i].name==ident&&
          !(SymbolTable[i].kind=='p'&&outLev))
            return false;
        if(SymbolTable[i].kind=='f') outLev=true;
    }
    return true;
}
bool checkCaseD(string func,int params){
    for(int i=SymbolTable.size()-1;i>=0;i--){
        if(SymbolTable[i].name==func)
            return SymbolTable[i].params!=params;
    }
    return true;
}
int getFuncPosInSymbolTable(string func){
    for(int i=SymbolTable.size()-1;i>=0;i--)
        if(SymbolTable[i].name==func)
            return i;
    return -1;
}
int getArrDmn(string ident){
    for(int i=SymbolTable.size()-1;i>=0;i--)
        if(SymbolTable[i].name==ident)
            return SymbolTable[i].dmn;
    return 0;
}
char getIdentKind(string ident){
    for(int i=SymbolTable.size()-1;i>=0;i--)
        if(SymbolTable[i].name==ident)
            return SymbolTable[i].kind;
    return ' ';
}
int getArgsNum(string str){
    int num=0;
    for(int i=0;i<str.size();i++)
        if(str[i]=='%'&&str[i+1]=='d')
            num++;
    return num;
}
int getReturnPosInBlock(int pos){
    if(lexTable[pos--].src!=";") return 1;
    while(1){
        //feo<<lexTable[pos].src<<" ";
        if(lexTable[pos].src=="return"){
            //feo<<" END\n";
            return 2;
        }
        if(lexTable[pos].src==";"||lexTable[pos].src=="}"){
            return 1;
        }
        pos--;
    }
}