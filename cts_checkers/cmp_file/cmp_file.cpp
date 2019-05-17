#include <iostream>
#include <fstream>
#include <tclap/CmdLine.h>

enum {
    OK = 0,
    WA = 1,
    PE = 2
};

int main(int argc, char **argv) {
    TCLAP::CmdLine cmd("CTS cmp_int checker");
    TCLAP::UnlabeledValueArg<std::string> output("output", "path to output file", true, "", "output.txt", cmd);
    TCLAP::UnlabeledValueArg<std::string> answer("answer", "path to answer file", true, "", "answer.a", cmd);
    cmd.parse(argc, argv);

    std::ifstream out(output.getValue());
    std::string a = "";
    std::string s;
    std::getline(out, s);
    s.erase(s.find_last_not_of(" \n\r\t")+1);
    while (!out.eof()) {
        a += "\n" + s;
        std::getline(out, s);
        s.erase(s.find_last_not_of(" \n\r\t")+1);
    }
    a += "\n" + s;
    a.erase(a.find_last_not_of(" \n\r\t")+1);
    if (a.empty()) {
        return PE;
    }

    std::ifstream ans(answer.getValue());
    std::string b = "";
    std::getline(out, s);
    s.erase(s.find_last_not_of(" \n\r\t")+1);
    while (!out.eof()) {
        b += "\n" + s;
        std::getline(out, s);
        s.erase(s.find_last_not_of(" \n\r\t")+1);
    }
    b += "\n" + s;
    b.erase(b.find_last_not_of(" \n\r\t")+1);

    if (a == b) {
        return OK;
    } else {
        return WA;
    }
};
