#include "global.h"
#include "analysis.h"
#include "generate.h"

std::ifstream fin;
std::ofstream fso;
std::ofstream feo;
std::ofstream fmo;
std::ofstream fmips;

int main() {
    fin.open("testfile.txt");
    fso.open("output.txt");
    //fso.open("CON");
    feo.open("error.txt");
    //feo.open("CON");
    fmo.open("middle.txt");
    //fmo.open("CON");
    fmips.open("mips.txt");

    lexicalAnalysis();
    //syntacticAnalysis();
    generateMiddle();
    generateMIPS();
    return 0;
}
