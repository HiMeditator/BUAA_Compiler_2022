#include "global.h"
#include "generate.h"
using namespace std;
int genPntStr(string str,int lev){
    int pre=1,cnt=0;
    MIDCode raw;
    string str_,strName;
    for(int i=1;i<str.size();i++){
        if(str[i]=='%'||i==str.size()-1){
            cnt++;
            str_="\""+str.substr(pre,i-pre)+"\"";
            strName="str_"+ to_string(strCount++);
            pre=i+2;
            i++;
            raw.quickSet("constDecl","str",strName,str_);
            midCode.push_back(raw);
            ST.push_back({strName,strName,'c','s',lev,0});
        }
    }
    return cnt;
}
int getConstVal(string src){
    for(int i=0;i<midCode.size();i++)
        if(midCode[i].type=="constDecl"&&midCode[i].arg2==src)
            return stoi(midCode[i].arg3);
    return 0;
}
string getRename(string name){
    for(int i=ST.size()-1;i>=0;i--)
        if(ST[i].name==name)
            return ST[i].rename;
//    cout<<"find "<<name<<" failed\n";
//    for(int i=ST.size()-1;i>=0;i--){
//        cout<<ST[i].name<<" "<<ST[i].rename<<"\n";
//    }
    return "null";
}