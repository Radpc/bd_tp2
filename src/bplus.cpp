#include "bplus.h"
#ifndef VECTOR
#define VECTOR
#include <vector>
#endif

#define MAX_SIZE 8

class node {
   public:
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
    nodepack* packRaiz;
};