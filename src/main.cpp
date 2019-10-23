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
#include <pthread.h>  //Pthreads
#include <unistd.h>   //Time for debugging
#include <ctime>      //Time
#include <iostream>   //Files
#include <regex>      //Get csv fields

#ifndef FSTREAM
#define FSTREAM
#include <fstream>
#endif

#ifndef VECTOR
#define VECTOR
#include <vector>
#endif

#ifndef REDUCE_TO
#define REDUCE_TO 60
#endif

// Custom modules
#include "bplus.h"
#include "hashing.h"
#include "helper.h"

// Options define's
#define SEC 1000000                // Valor de um segundo
#define TAXA_ATUALIZACAO SEC / 10  // O fps

// // SIZES
// #define TITLE_SIZE 300
// #define WRITERS_SIZE 150
// #define DATE_SIZE 30
// #define SNIPPET_SIZE 1024

// HASHES
#define FIRST_HASH 127
#define SECOND_HASH 131

// Config define's
#define MODE_INSERT 1  // Codigo do modo de inserir
#define MODE_FIND 2
#define MODE_NORMAL 3

// Retirar a necessidade de escrever std
using namespace std;

/// Global Variables
// Quantidade de linhas lidas já
int lines = 0;

// regex r_field("(.*?)(\";|;$)");

// Progresso mostrando
bool progress = true;

//##################################################\\============================================================================
//                                                  \\============================================================================
//                      CLASSES
//                                                  \\============================================================================
//                                                  \\============================================================================
//##################################################\\============================================================================
/*
// Struct

// MUITO DEVAGAR!!!!!

typedef struct csv_struct {
    int id;
    int year;
    int citations;
    char title[300];
    char writers[150];
    char date_time[30];
    char snippet[1024];

} csv_struct;

csv_struct create_csv_struct(string line) {
    csv_struct s;
    int char_counter = 0;
    int attr_counter = 0;
    string aux = line;

    while (regex_search(line, m, r_field)) {
        switch (attr_counter) {
            // ID
            case 0:
                s.id = stoi(m[0].str().substr(1, m[0].str().size() - 2));
                break;

            // TITLE
            case 1:
                strcpy(s.title, m[0].str().c_str());
                break;

            // YEAR
            case 2:
                s.year = stoi(m[0].str().substr(1, m[0].str().size() - 2));
                break;

            // WRITERS
            case 3:
                strcpy(s.writers, m[0].str().c_str());
                break;

            // CITATIONS
            case 4:
                s.citations = stoi(m[0].str().substr(1, m[0].str().size() - 2));
                break;
            // DATE
            case 5:
                strcpy(s.date_time, m[0].str().c_str());
                break;
        }
        attr_counter++;
        char_counter += m[0].str().size();
        line = m.suffix();
    }
    strcpy(s.snippet, aux.substr(char_counter + 1).c_str());
    return s;
}
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
    cout << "usage: db [-h] [-r] [-t] [-i \"/path/to/file.csv\"] [-f "
            "\"/path/to/folder.db/\" [ID's]] [-n \"/path/to/file.csv\" [ID's]]"
         << endl
         << endl;
    cout << " -h, --help                    Show this help" << endl;
    cout << " -r, --reduced                 Reduced mode" << endl;
    cout << " -t, --timed                   Timed mode" << endl;
    cout << " -i, --insert [FILES]          Insert Files" << endl;
    cout << " -f, --find [/folder.db] [id]  Find (hash mode)" << endl;
    cout << " -n, --normal [file.csv] [id]  Find (Sequential mode)" << endl;
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

// Busca sequencial para comparação
string normalSearch(string csv, int id, bool reduced = false) {
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
    bool reduced = false;
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
                insertHash(folder, line, FIRST_HASH, SECOND_HASH);
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
            cout << findHash(findPlace, find, FIRST_HASH, SECOND_HASH, reduced)
                 << endl;
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
            cout << normalSearch(normalPlace, normalFind, reduced) << endl;
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