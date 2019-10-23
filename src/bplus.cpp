#include "bplus.h"

#define MAX_SIZE 8

#include <stdio.h>
#include <iostream>

using namespace std;

typedef struct node {
    int ponteiro = 0;
    int id = 0;
    long int seek = -1;
    long int ponteiro_esquerda = -1;
    long int ponteiro_direita = -1;
    long int ponteiro_proximo = -1;
    int pai = 0;
} node;

node getNode(FILE *f, int ponteiro) {
    node nd;
    fseek(f, ponteiro * sizeof(struct node), SEEK_SET);
    fread(&nd, sizeof(struct node), 1, f);
    return nd;
}

int sizeOfPack(FILE *f, int first) {
    int size = 1;
    node nd = getNode(f, first);
    while (nd.ponteiro_proximo != -1) {
        nd = getNode(f, nd.ponteiro_proximo);
        size++;
    }
}

bool writeNode(FILE *f, node nd) {
    fseek(f,0,SEEK_END);
    fwrite(&nd, sizeof(struct node), 1, f);
    return true;
}

bool changeEsquerda(FILE *f, int ponteiro, int new_esquerda) {
    node k = getNode(f, ponteiro);
    k.ponteiro_esquerda = new_esquerda;
    fseek(f, ponteiro * sizeof(struct node), SEEK_SET);
    fwrite(&k, sizeof(struct node), 1, f);
}

bool changeDireita(FILE *f, int ponteiro, int new_direita) {
    node k = getNode(f, ponteiro);
    k.ponteiro_direita = new_direita;
    fseek(f, ponteiro * sizeof(struct node), SEEK_SET);
    fwrite(&k, sizeof(struct node), 1, f);
}

bool changeProximo(FILE *f, int ponteiro, int new_proximo) {
    node k = getNode(f, ponteiro);
    k.ponteiro_proximo = new_proximo;
    fseek(f, ponteiro * sizeof(struct node), SEEK_SET);
    fwrite(&k, sizeof(struct node), 1, f);
}

bool addNode(FILE *f, node nd) {
    node search = getNode(f, 0);
    int firstOfPack = search.id;
    int numberOfNodes = 1;
    while (true) {
        if (nd.id > search.id) {
            
            // IR PARA DIREITA

            // Ir pro proximo
            if (search.ponteiro_proximo != -1 and getNode(f,search.ponteiro_proximo).id < nd.id) {
                search = getNode(f, search.ponteiro_proximo);
                numberOfNodes++;
            } else {
                // DESCER DIREITA
                if (search.ponteiro_direita != -1) {
                    search = getNode(f, search.ponteiro_direita);
                    firstOfPack = search.id;
                    numberOfNodes = 1;
                } else {
                    // @CRIAR AQUI NA DIREITA

                    // Pode criar vizinho deste leaf?
                    if (sizeOfPack(f, firstOfPack) <= MAX_SIZE) {
                        if (search.ponteiro_proximo == -1) {
                            changeProximo(f, search.ponteiro, nd.ponteiro);
                            writeNode(f, nd);
                        } else {
                            nd.ponteiro_proximo = search.ponteiro_proximo;
                            changeProximo(f, search.ponteiro, nd.ponteiro);
                            writeNode(f, nd);
                        }
                    } else {
                        cout << "epa" << endl;
                        // Cacete
                    }
                    break;
                }
            }
        } else {
            // DESCER ESQUERDA
            if (search.ponteiro_esquerda != -1) {
                search = getNode(f, search.ponteiro_esquerda);
                firstOfPack = search.ponteiro_esquerda;
                numberOfNodes = 1;
            } else {
                // @CRIAR AQUI NA ESQUERDA
                break;
            }
        }
    }

}

int main(int argc, char const *argv[]) {
    FILE *f;

    f = fopen("Teste.txt", "wb");
    fclose(f);

    f = fopen("Teste.txt", "rb+");

    node nd;

    nd = {0, 1, 60};
    writeNode(f, nd);

    nd = {1, 2, 70};
    addNode(f, nd);

    nd = {2, 3, 70};
    addNode(f, nd);

    nd = {3, 6, 70};
    addNode(f, nd);

    nd = {4, 5, 80};
    addNode(f,nd);

    fclose(f);

    f = fopen("Teste.txt", "r");
    nd = getNode(f,0);
    while(nd.ponteiro_proximo != -1){
        cout << "[" << nd.id << "]  ";
        nd = getNode(f,nd.ponteiro_proximo);
    }
    cout << "[" << nd.id << "]  " << endl;

    return 0;
}
