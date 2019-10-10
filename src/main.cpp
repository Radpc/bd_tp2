#include <pthread.h>  //Pthreads
#include <unistd.h>
#include <algorithm>  // std::count
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#define SEC 1000000               // Valor de um segundo
#define TAXA_ATUALIZACAO SEC / 2  // O fps

#define HELP_FILE "help"
#define MODE_INSERT 1

using namespace std;

// Global Variables
double progress = 0;
int lines = 0;
int total_lines = 0;

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

// Função para limpar a tela
void clrscr() { system("@cls||clear"); }

void *t_progress(void *lpParam) {
    while (progress <= 1.0) {
        int barWidth = 70;

        std::cout << "[";
        int pos = barWidth * progress;
        for (int i = 0; i < barWidth; ++i) {
            if (i < pos)
                std::cout << "=";
            else if (i == pos)
                std::cout << ">";
            else
                std::cout << " ";
        }
        std::cout << "] " << int(progress * 100.0) << " %\r";
        std::cout.flush();

        progress = lines / total_lines;
        usleep(SEC);
    }
    std::cout << std::endl;
}

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

int insert_csv(string csv_name) {}

int main(int argc, char *argv[]) {
    vector<string> cmdLineArgs(argv, argv + argc);
    vector<string> insertFiles;
    pthread_t aux;

    // Limpar tela
    clrscr();

    // Iniciar modo
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

    for (auto i : insertFiles) {
        ifstream my_file(i);
        string line;
        lines = 0;
        if (my_file.is_open()) {
            // Iniciar barra com progress
            pthread_create(&aux, NULL, t_progress, NULL);

            while (getline(my_file, line)) lines++;
            cout << lines << endl;
        }
    }
}