#include <fstream>
#include <iostream>
#include <vector>

#define HELP_FILE "help"
#define MODE_INSERT 1

using namespace std;

class csv_line {
   public:
    csv_line(int f_id, string f_title, int f_year, string f_writers,
             int f_citations, string f_date_time, string f_snippet) {
        id = f_id;
        title = f_title;
        year = f_year;
        writers = f_writers;
        citations = f_citations;
        date_time = f_date_time;
        snippet = f_snippet;
    }

    string description() {
        return "[" + to_string(id) + "] - " + title + " - " + to_string(year);
    }

    int id;
    string title;
    int year;
    string writers;
    int citations;
    string date_time;
    string snippet;
};

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

bool has_suffix(const string &str, const string &suffix) {
    return str.size() >= suffix.size() &&
           str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}

int main(int argc, char *argv[]) {
    vector<string> cmdLineArgs(argv, argv + argc);
    vector<string> insertFiles;
    int mode = -1;

    for (auto &arg : cmdLineArgs) {
        if (arg == "--help" || arg == "-help") {
            readHelp();
            return 0;
        }
        if (has_suffix(arg, ".csv")) {
            insertFiles.push_back(arg);
        } else if (arg == "-i" || arg == "--insert") {
            mode = MODE_INSERT;
        }
    }
}