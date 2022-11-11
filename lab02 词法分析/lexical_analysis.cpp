#include "co_head.h"
#include "lex_ana.h"
using namespace std;

string file,token;
char chr;
int pos;
LexData lexT;

void getFile(){
    string buf;
    while(getline(cin,buf)) {
        file += buf+"\n";
    }
}
void clearToken(){
    token = "";
}
void getChar(){
    chr = file[++pos];
    if(pos==file.size()) chr=0;
}
void catToken(){
    token += chr;
}
void retreat(){
    chr = file[--pos];
}
string reserver(){
    if(token=="main") return "MAINTK";
    if(token=="const") return "CONSTTK";
    if(token=="int") return "INTTK";
    if(token=="break") return "BREAKTK";
    if(token=="continue") return "CONTINUETK";
    if(token=="if") return "IFTK";
    if(token=="else") return "ELSETK";
    if(token=="while") return "WHILETK";
    if(token=="getint") return "GETINTTK";
    if(token=="printf") return "PRINTFTK";
    if(token=="return") return "RETURNTK";
    if(token=="void") return "VOIDTK";
    return "IDENFR";
}
int getsym(){
    clearToken();
    getChar();
    while(isspace(chr)) getChar();
    if(isalpha(chr)||chr=='_'){
        while(isalpha(chr)|| isdigit(chr)||chr=='_'){
            catToken();
            getChar();
        }
        retreat();
        lexT.type=reserver();
    }
    else if(isdigit(chr)){
        while(isdigit(chr)){
            catToken();
            getChar();
        }
        retreat();
        lexT.type="INTCON";
    }
    else if(chr=='\"'){
        do{
            catToken();
            getChar();
        }while(chr!='\"');
        catToken();
        lexT.type="STRCON";
    }
    else if(chr=='!'){
        catToken();
        getChar();
        if(chr=='=') catToken(),lexT.type="NEQ";
        else retreat(),lexT.type="NOT";
    }
    else if(chr=='&'){
        catToken();
        getChar();
        if(chr=='&') catToken(),lexT.type="AND";
        else return 1;
    }
    else if(chr=='|'){
        catToken();
        getChar();
        if(chr=='|') catToken(),lexT.type="OR";
        else return 1;
    }
    else if(chr=='<'){
        catToken();
        getChar();
        if(chr=='=') catToken(),lexT.type="LEQ";
        else retreat(),lexT.type="LSS";
    }
    else if(chr=='>'){
        catToken();
        getChar();
        if(chr=='=') catToken(),lexT.type="GEQ";
        else retreat(),lexT.type="GRE";
    }
    else if(chr=='='){
        catToken();
        getChar();
        if(chr=='=') catToken(),lexT.type="EQL";
        else retreat(),lexT.type="ASSIGN";
    }
    else if(chr=='/'){
        catToken();
        getChar();
        if(chr=='*'){
            while(true){
                do{
                    getChar();
                }while(chr!='*');
                getChar();
                if(chr=='/') break;
                retreat();
            }
            return -1;
        }
        else if(chr=='/'){
            while(chr!='\n') getChar();
            return -1;
        }
        else retreat(),lexT.type="DIV";
    }
    else if(chr=='+') catToken(),lexT.type="PLUS";
    else if(chr=='-') catToken(),lexT.type="MINU";
    else if(chr=='*') catToken(),lexT.type="MULT";
    else if(chr=='%') catToken(),lexT.type="MOD";
    else if(chr==';') catToken(),lexT.type="SEMICN";
    else if(chr==',') catToken(),lexT.type="COMMA";
    else if(chr=='(') catToken(),lexT.type="LPARENT";
    else if(chr==')') catToken(),lexT.type="RPARENT";
    else if(chr=='[') catToken(),lexT.type="LBRACK";
    else if(chr==']') catToken(),lexT.type="RBRACK";
    else if(chr=='{') catToken(),lexT.type="LBRACE";
    else if(chr=='}') catToken(),lexT.type="RBRACE";
    else return 1;
    lexT.src=token;
    return 0;
}
void lexicalAnalysis(){
    int status;
    getFile();
    pos=-1;
    while(true){
        status= getsym();
        if(!status) cout<<lexT.type<<" "<<lexT.src<<endl;
        if(status>0) break;
    }
}