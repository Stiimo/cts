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
    std::vector<int> a;
    try {
        int c;
        out >> c;
        while (!out.eof()) {
            a.push_back(c);
            out >> c;
        }
        out.close();
    } catch (...) {
        out.close();
        return PE;
    }
    if (a.empty()) {
        return PE;
    }

    std::ifstream ans(answer.getValue());
    std::vector<int> b;
    int c;
    ans >> c;
    while (!out.eof()) {
        b.push_back(c);
        ans >> c;
    }
    ans.close();

    if (a.size() != b.size()) {
        return WA;
    }
    for (size_t i = 0; i < a.size(); ++i) {
        if (a[i] != b[i]) {
        } else {
            return WA;
        }
    }
    return OK;
};
