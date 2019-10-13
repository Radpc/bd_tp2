// TP2 - Banco de dados
// Autores: - Matheus Marques
//          - Helder Medeiros
//          - Vitor Hugo

//##################################################\\============================================================================
//                                                  \\============================================================================
//               INCLUDES & INICIAÇÕES
//                                                  \\============================================================================
//                                                  \\============================================================================
//##################################################\\============================================================================

// Universal modules
#include <bits/stdc++.h>
#include <pthread.h>  //Pthreads
#include <unistd.h>   //Time for debugging
#include <ctime>      //Time
#include <fstream>    //Files
#include <iostream>   //Files
#include <regex>      //Get csv fields
#include <vector>     //Vector

// Custom modules
#include "helper.h"

// Options define's
#define SEC 1000000                // Valor de um segundo
#define TAXA_ATUALIZACAO SEC / 10  // O fps
#define REDUCE_TO 60

// Config define's
#define MODE_INSERT 1  // Codigo do modo de inserir
#define MODE_FIND 2
#define MODE_NORMAL 3

// Retirar a necessidade de escrever std
using namespace std;

/// Global Variables
// Quantidade de linhas lidas já
int lines = 0;
bool reduced = false;

/// Regex
std::smatch m;

// Regex de ID
regex r_id("[[:digit:]]+");

// Progresso mostrando
bool progress = true;

//##################################################\\============================================================================
//                                                  \\============================================================================
//                      CLASSES
//                                                  \\============================================================================
//                                                  \\============================================================================
//##################################################\\============================================================================
/*
// Classe da linha do csv
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
*/
//##################################################\\============================================================================
//                                                  \\============================================================================
//                      FUNÇÕES
//                                                  \\============================================================================
//                                                  \\============================================================================
//##################################################\\============================================================================

// Thread para demonstrar o progresso
void *t_progress(void *lpParam) {
    int load = 0;
    char l = ' ';
    progress = true;
    while (progress) {
        switch (load) {
            case 0:
                l = '|';
                load++;
                break;
            case 1:
                l = '/';
                load++;
                break;
            case 2:
                l = '-';
                load++;
                break;
            case 3:
                l = '\\';
                load = 0;
                break;
        }

        cout << "[";
        cout << l;
        cout << "] " << int(lines) << " lines     \r";
        cout.flush();

        usleep(TAXA_ATUALIZACAO);
    }
    return NULL;
}

int readHelp() {
    cout << " -[DB - HELP]-" << endl << endl;
    cout << " -i, --insert [FILES]          Insert Files" << endl;
    cout << " -h, --help                    Show this help" << endl;
}

bool isNumber(string s) {
    for (int i = 0; i < s.length(); i++)
        if (isdigit(s[i]) == false) return false;

    return true;
}

string create_db_folder(string csv_name) {
    string folder;
    if (csv_name.find("/")) {
        folder = csv_name.substr(0, csv_name.size() - 4) + ".db";
    } else {
        folder = ".";
    }
    string cmd = "mkdir -p " + folder;
    char cmdd[cmd.size() + 1];
    strcpy(cmdd, cmd.c_str());
    system(cmdd);
    return folder;
}

int insertHash(string folder, string line, int mod1, int mod2) {
    if (regex_search(line, m, r_id)) {
        fstream file;
        int id = stoi(m[0]);
        int subfolder = id % mod1;
        int bucket = id % mod2;

        file.open(folder + "/hashing/" + to_string(subfolder) + "/" +
                      to_string(bucket) + ".b",
                  std::ios_base::app);

        if (!file.is_open()) {
            string cmd = "mkdir -p " + folder + "/hashing/" +
                         to_string(subfolder) + " && touch " + folder +
                         "/hashing/" + to_string(subfolder) + "/" +
                         to_string(bucket) + ".b";
            char cmdd[cmd.size() + 1];
            strcpy(cmdd, cmd.c_str());
            system(cmdd);
            file.open(folder + "/hashing/" + to_string(subfolder) + "/" +
                          to_string(bucket) + ".b",
                      std::ios_base::app);
        }
        file << line << endl;
        file.close();

    } else {
        return -1;
    }
}

string findHash(string folder, int id, int mod1, int mod2) {
    int subfolder = id % mod1;
    int bucket = id % mod2;
    string line;

    string idd = "\"" + to_string(id) + "\"";

    ifstream file(folder + "/hashing/" + to_string(subfolder) + "/" +
                  to_string(bucket) + ".b");
    file.seekg(0, ios::beg);

    if (file.is_open()) {
        while (getline(file, line)) {
            if (line.substr(0, idd.size()).compare(idd) == 0) {
                file.close();
                if (reduced and line.size() > REDUCE_TO) {
                    return line.substr(0, REDUCE_TO) + "...";
                } else {
                    return line;
                }
            }
        }
        return "Could not find " + to_string(id) + ".";
        file.close();
    } else {
        file.close();
        return "File could not be open! - " + folder;
    }
}

string normalSearch(string csv, int id) {
    ifstream file(csv);
    string line;
    string idd = "\"" + to_string(id) + "\"";
    file.seekg(0, ios::beg);

    if (file.is_open()) {
        while (getline(file, line)) {
            if (line.substr(0, idd.size()).compare(idd) == 0) {
                file.close();
                if (reduced and line.size() > REDUCE_TO) {
                    return line.substr(0, REDUCE_TO) + "...";
                } else {
                    return line;
                }
            }
        }
        return "Could not find " + to_string(id) + ".";
        file.close();
    } else {
        file.close();
        return "CSV could not be open! - " + csv;
    }
}

//##################################################\\============================================================================
//                                                  \\============================================================================
//                        MAIN
//                                                  \\============================================================================
//                                                  \\============================================================================
//##################################################\\============================================================================

int main(int argc, char *argv[]) {
    // List of files
    vector<string> cmdLineArgs(argv, argv + argc);
    vector<string> insertFiles;

    // Search flags
    int find = -1;
    string findPlace = "";
    int normalFind = -1;
    string normalPlace = "";

    // Thread
    pthread_t progress;

    // Global vars
    string folder;
    string line;

    // Time measurements
    bool timed = false;
    clock_t time_start;
    clock_t time_finish;
    double total_time;

    // Iniciar modo
    int mode = -1;

    // Tratar os argumentos
    for (int i = 1; i < cmdLineArgs.size(); i++) {
        if (cmdLineArgs[i] == "--help" || cmdLineArgs[i] == "-h") {
            readHelp();
            return 0;
        } else if (cmdLineArgs[i] == "--insert" ||
                   cmdLineArgs[i] == "-i") {  // INSERT
            mode = MODE_INSERT;
            i++;
            if (i >= cmdLineArgs.size()) {
                cout << "Argument number error!" << endl;
                return -1;
            }
            if (has_suffix(cmdLineArgs[i], ".csv")) {
                insertFiles.push_back(cmdLineArgs[i]);
            } else {
                cout << "Error, csv file could not be loaded!" << endl;
                return -1;
            }
        } else if (cmdLineArgs[i] == "--find" ||
                   cmdLineArgs[i] == "-f") {  // FIND
            mode = MODE_FIND;
            i++;
            if (i >= cmdLineArgs.size()) {
                cout << "Argument number error!" << endl;
                return -1;
            }
            findPlace = cmdLineArgs[i];
            i++;
            if (i >= cmdLineArgs.size()) {
                cout << "Argument number error!" << endl;
                return -1;
            }

            if (isNumber(cmdLineArgs[i])) {
                find = stoi(cmdLineArgs[i]);
            } else {
                cout << "Error! Insert a valid ID" << endl;
                return -1;
            }

        } else if (cmdLineArgs[i] == "--normal" ||
                   cmdLineArgs[i] == "-n") {  // NORMAL FIND
            mode = MODE_NORMAL;
            i++;
            if (i >= cmdLineArgs.size()) {
                cout << "Argument number error!" << endl;
                return -1;
            }
            normalPlace = cmdLineArgs[i];
            i++;
            if (i >= cmdLineArgs.size()) {
                cout << "Argument number error!" << endl;
                return -1;
            }

            if (isNumber(cmdLineArgs[i])) {
                normalFind = stoi(cmdLineArgs[i]);
            } else {
                cout << "Error! Insert a valid ID" << endl;
                return -1;
            }
        } else if (mode == MODE_INSERT and
                   has_suffix(cmdLineArgs[i], ".csv")) {  // INSERT MULTIPLE
            insertFiles.push_back(cmdLineArgs[i]);
        } else if (cmdLineArgs[i] == "-t" || cmdLineArgs[i] == "--timed") {
            timed = true;
        } else if (cmdLineArgs[i] == "-r" || cmdLineArgs[i] == "--reduced") {
            reduced = true;
        } else {
            cout << "Invalid sintax! Maybe missed an \"-i\" ?" << endl;
            return -1;
        }
    }

    // Limpar tela
    // clrscr();

    // Tratar as criações de bancos
    for (auto i : insertFiles) {
        cout << "Opening " << i << endl;

        ifstream my_file(i);
        lines = 0;

        if (my_file.is_open()) {
            cout << "File open!" << endl << endl;

            folder = create_db_folder(i);

            // Contar linhas do csv
            cout << "Reading..." << endl;
            pthread_create(&progress, NULL, t_progress, NULL);
            while (getline(my_file, line)) lines++;
            progress = false;
            cout << "Reading - DONE - [" << lines << " linhas]" << endl;
            cout << endl;
            cout.flush();
            my_file.clear();
            my_file.seekg(0, ios::beg);
            lines = 0;

            // Realizar o hashing duplo
            cout << "Inserting in hash.." << endl;
            pthread_create(&progress, NULL, t_progress, NULL);
            while (getline(my_file, line)) {
                insertHash(folder, line, 23, 97);
                lines++;
            }
            cout << "Hashing - DONE - [" << lines << " linhas]" << endl;
            cout << endl;
            progress = false;

        } else {
            cout << "Some error ocurred..." << endl;
            return -1;
        }
    }

    // Search hash
    if (findPlace != "") {
        if (find != -1) {
            cout << "[Hash search]" << endl;
            if (timed) time_start = clock();
            cout << findHash(findPlace, find, 23, 97) << endl;
            if (timed) {
                time_finish = clock();
                total_time = double(time_finish - time_start) / CLOCKS_PER_SEC;
                cout << " Time elapsed: " << total_time << ";" << endl;
            }
            cout << "  ------" << endl;
        } else {
            cout << "Specify the path!" << endl;
            return -1;
        }
    }

    // Search normal
    if (normalPlace != "") {
        if (normalFind != -1) {
            cout << "[Normal search]" << endl;
            if (timed) time_start = clock();
            cout << normalSearch(normalPlace, normalFind) << endl;
            if (timed) {
                time_finish = clock();
                total_time = double(time_finish - time_start) / CLOCKS_PER_SEC;
                cout << " Time elapsed: " << total_time << ";" << endl;
            }
            cout << "  ------" << endl;
        } else {
            cout << "Specify the csv!" << endl;
            return -1;
        }
    }
}