#include "bplus.h"

#define MAX_SIZE 8

#include <stdio.h>
#include <iostream>

using namespace std;

typedef struct node {
    int id = 0;
    long int seek = 0;
    long int ponteiro_esquerda = 0;
    long int ponteiro_direita = 50;
    long int ponteiro_next = 50;
} node;

node getNode(FILE *f, int id) {
    node nd;
    fseek(f, id * sizeof(struct node), 0);
    fread(&nd, sizeof(struct node), 1, f);
    return nd;
}

bool addNode(FILE *f, node nd) {
    node search = getNode(f, 0);

    while () {
        if (nd.id > search.id) {
            
        }
    }
}

int main(int argc, char const *argv[]) {
    FILE *f;
    f = fopen("Teste.txt", "wb");

    node n;
    n.id = 10;
    n.seek = 20;
    n.ponteiro_direita = 500;
    n.ponteiro_esquerda = 500;

    fwrite(&n, sizeof(struct node), 1, f);

    fclose(f);
    f = fopen("Teste.txt", "rb");

    node k = getNode(f, 0);
    cout << k.ponteiro_direita << endl;

    return 0;
}
