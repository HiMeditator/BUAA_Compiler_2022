#include "global.h"
#include "analysis.h"
using namespace std;

static string file,token;
static char chr;
static int pos,line;
static LexEntry lexT;
vector<LexEntry>lexTable;

void getFile(){
    string buf;
    while(getline(fin,buf)) {
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
static void retreat(){
    chr = file[--pos];
}
string reserved(){
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
static int getSym(){
    clearToken();
    getChar();
    while(isspace(chr)) {
        if(chr=='\n') line++;
        getChar();
    }
    if(isalpha(chr)||chr=='_'){
        while(isalpha(chr)|| isdigit(chr)||chr=='_'){
            catToken();
            getChar();
        }
        retreat();
        lexT.type= reserved();
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
                    if(chr=='\n') line++;
                }while(chr!='*');
                getChar();
                if(chr=='/') break;
                retreat();
            }
            return -1;
        }
        else if(chr=='/'){
            while(chr!='\n') getChar();
            line++;
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
    lexT.line=line;
    return 0;
}
void lexicalAnalysis(){
    int status;
    getFile();
    pos=-1;
    line=1;
    while(true){
        status = getSym();
        if(!status) {
            lexTable.push_back(lexT);
            //cout<<lexT.line<<"\t"<<lexT.src<<"\t\t"<<lexT.type<<endl;
        }
        if(status>0) break; 
    }
}