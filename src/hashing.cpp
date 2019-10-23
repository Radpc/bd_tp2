#ifndef STRING
#include <string>
#endif

#ifndef REGEX
#define REGEX
#include <regex>
#endif

#ifndef FSTREAM
#define FSTREAM
#include <fstream>
#endif

#ifndef REDUCE_TO
#define REDUCE_TO 60
#endif

std::regex r_id("[[:digit:]]+");

int insertHash(std::string folder, std::string line, int mod1, int mod2) {
    std::smatch m;
    if (regex_search(line, m, r_id)) {
        std::fstream file;
        int id = stoi(m[0]);
        int subfolder = id % mod1;
        int bucket = id % mod2;

        file.open(folder + "/hashing/" + std::to_string(subfolder) + "/" +
                      std::to_string(bucket) + ".b",
                  std::ios_base::app);

        if (!file.is_open()) {
            std::string cmd = "mkdir -p " + folder + "/hashing/" +
                              std::to_string(subfolder) + " && touch " +
                              folder + "/hashing/" + std::to_string(subfolder) +
                              "/" + std::to_string(bucket) + ".b";
            char cmdd[cmd.size() + 1];
            strcpy(cmdd, cmd.c_str());
            system(cmdd);
            file.open(folder + "/hashing/" + std::to_string(subfolder) + "/" +
                          std::to_string(bucket) + ".b",
                      std::ios_base::app);
        }
        file << line << std::endl;
        file.close();

    } else {
        return -1;
    }
}

std::string findHash(std::string folder, int id, int mod1, int mod2,
                     bool reduced = false) {
    int subfolder = id % mod1;
    int bucket = id % mod2;
    std::string line;

    std::string idd = "\"" + std::to_string(id) + "\"";

    std::ifstream file(folder + "/hashing/" + std::to_string(subfolder) + "/" +
                       std::to_string(bucket) + ".b");
    file.seekg(0, std::ios::beg);

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
        return "Could not find " + std::to_string(id) + ".";
        file.close();
    } else {
        file.close();
        return "File could not be open! - " + folder;
    }
}
