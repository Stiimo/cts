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
    std::string a;
    try {
        std::getline(out, a);
        out.close();
    } catch (...) {
        out.close();
        std::cerr << "Wrong format\n";
        return PE;
    }
    if (a != "YES" && a != "NO") {
        std::cerr << "Answer must be `YES` or `NO`, but your answer is `" + a + "`\n";
        return PE;
    }

    std::ifstream ans(answer.getValue());
    std::string b;
    std::getline(ans, b);
    ans.close();

    if (a == b) {
        std::cerr << "OK\n";
        return OK;
    } else {
        std::cerr << "Wrong answer\n";
        return WA;
    }
};
