#include "bplus.h"

#define MAX_SIZE 8

class node {
   public:
    node(int idd, int seekGg) {
        id = idd;
        seekG = seekGg;
    }
    int id;
    int seekG;
    node* left;   // always goes down, if NULL then it's leaf
    node* right;  // almost always go right
};

class nodepack {
   public:
    std::vector<node> pack;
    int max_size = MAX_SIZE;
};

class tree {
   public:
    tree() {
        nodepack k;
        *packRaiz = k;
    }

    bool add(int id, int seekg) {
        node n(id, seekg);
        return true;
    }

    nodepack* packRaiz;
};