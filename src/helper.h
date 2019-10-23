#ifndef HELPER_H
#define HELPER_H

#ifndef STRING
#define STRING
#include <string>
#endif

bool has_suffix(const std::string &str, const std::string &suffix);

// Função para limpar a tela
void clrscr();

bool isNumber(std::string s);


#endif