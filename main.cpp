#include <fstream>
#include <iostream>
#include <vector>

#define HELP_FILE "help"

using namespace std;

int readHelp() {
    ifstream reader;
    string output;

    reader.open(HELP_FILE);

    if (reader.is_open()) {
        while (getline(reader, output)) {
            cout << output << endl;
        }
    }
    reader.close();
    return 0;
}

int main(int argc, char *argv[]) {
    vector<string> cmdLineArgs(argv, argv + argc);

    for (auto &arg : cmdLineArgs) {
        if (arg == "--help" || arg == "-help") {
            readHelp();
            return 0;
        }
    }
}