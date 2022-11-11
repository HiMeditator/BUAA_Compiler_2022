#include "co_head.h"
#include "analysis.h"

std::ifstream fin;
std::ofstream fso;
std::ofstream feo;
std::ofstream fdo;

int main() {
    fin.open("testfile.txt");
    fso.open("output.txt");
    //fso.open("CON");
    feo.open("error.txt");
    //feo.open("CON");
    //fdo.open("debug.txt");
    fdo.open("CON");
    syntacticAnalysis();
    return 0;
}
