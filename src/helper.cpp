#include "helper.h"

bool has_suffix(const std::string &str, const std::string &suffix) {
    return str.size() >= suffix.size() &&
           str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}

// Função para limpar a tela
void clrscr() { system("@cls||clear"); }

// Checar se é número
bool isNumber(std::string s) {
    for (int i = 0; i < s.length(); i++)
        if (isdigit(s[i]) == false) return false;

    return true;
}