#include "co_head.h"
#include "analysis.h"
using namespace std;

bool isExpFST(string type,string src){
    return  src=="+"||src=="-"||src=="!"||src=="("||
            type=="INTCON"||type=="IDENFR";
}
bool isLval_Eq(int pos){
    if(lexTable[pos++].type=="IDENFR"){
        int cnt=0;
        if(lexTable[pos].src=="[") {
            cnt++;
            while (++pos) {
                if (lexTable[pos].src == "[") cnt++;
                if (lexTable[pos].src == "]") cnt--;
                if (cnt == 0 && lexTable[pos + 1].src != "[") {
                    pos++;
                    break;
                }

            }
        }
        return lexTable[pos].src=="=";
    }
    return false;
}