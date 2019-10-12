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
#include <fstream>    //Files
#include <iostream>   //Files
#include <vector>     //Vector
#include <bits/stdc++.h> 

// Custom modules
#include "helper.h"

// Options define's
#define SEC 1000000                // Valor de um segundo
#define TAXA_ATUALIZACAO SEC / 10  // O fps

// Config define's
#define MODE_INSERT 1  // Codigo do modo de inserir

// Retirar a necessidade de escrever std
using namespace std;

/// Global Variables
// Quantidade de linhas lidas já
int lines = 0;

// Progresso mostrando
bool progress = true;

//##################################################\\============================================================================
//                                                  \\============================================================================
//                      CLASSES
//                                                  \\============================================================================
//                                                  \\============================================================================
//##################################################\\============================================================================

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

//##################################################\\============================================================================
//                                                  \\============================================================================
//                      FUNÇÕES
//                                                  \\============================================================================
//                                                  \\============================================================================
//##################################################\\============================================================================

// Thread para demonstrar o progresso
void *t_progress(void *lpParam) {
    int load = 0;
    progress = true;
    char l = ' ';
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
}

int readHelp() {
    cout << " -[DB - HELP]-" << endl << endl;
    cout << " -i, --insert [FILES]          Insert Files" << endl;
    cout << " -h, --help                    Show this help" << endl;
}

int insert_csv(string csv_name) {
    string folder;
    if (csv_name.find("/")) {
        folder = csv_name.substr(0, csv_name.size() - 4) + ".db";
    } else {
        folder = ".";
    }
    string cmd = "mkdir -p " + folder;
    char  cmdd[cmd.size() + 1];
    strcpy(cmdd,cmd.c_str());
    system(cmdd);
    cout << cmdd << endl;
}

//##################################################\\============================================================================
//                                                  \\============================================================================
//                        MAIN
//                                                  \\============================================================================
//                                                  \\============================================================================
//##################################################\\============================================================================

int main(int argc, char *argv[]) {
    vector<string> cmdLineArgs(argv, argv + argc);
    vector<string> insertFiles;
    pthread_t aux;

    string folder = argv[0];
    // folder = folder.substr(0,folder.size()-)

    cout << argv[0] << endl;

    // Iniciar modo
    int mode = -1;

    // Tratar os argumentos
    for (auto &arg : cmdLineArgs) {
        if (arg == "--help" || arg == "-h") {
            readHelp();
            return 0;
        }
        if (has_suffix(arg, ".csv")) {
            insertFiles.push_back(arg);
        } else if (arg == "-i" || arg == "--insert") {
            mode = MODE_INSERT;
        }
    }

    // Limpar tela
    clrscr();

    // Tratar as criações de bancos
    for (auto i : insertFiles) {
        cout << "Opening " << i << endl;
        ifstream my_file(i);
        string line;
        lines = 0;
        if (my_file.is_open()) {
            cout << "File open!" << endl;
            cout << "Reading..." << endl;
            insert_csv(i);
            // ofstream myfile;
            // myfile.open("example.bin", ios::out | ios::app | ios::binary);
            pthread_create(&aux, NULL, t_progress, NULL);
            while (getline(my_file, line)) {  // Iniciar barra com progress
                // myfile << line << endl;
                lines++;
            }
            progress = false;
            cout << "DONE - [" << lines << " linhas]" << endl;
            cout.flush();

        } else {
            cout << "Some error ocurred..." << endl;
            return -1;
        }
    }
}