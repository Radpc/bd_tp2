#ifndef HASHING_H
#define HASHING_H

int insertHash(std::string folder, std::string line, int mod1, int mod2);

std::string findHash(std::string folder, int id, int mod1, int mod2,
                     bool reduced = false);

#endif