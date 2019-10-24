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
    int pai = -1;
    int firstOfPack = -1;
} node;

node getNode(FILE *f, int ponteiro) {
    node nd;
    fseek(f, sizeof(int), SEEK_SET);
    fseek(f, ponteiro * sizeof(struct node), SEEK_CUR);
    fread(&nd, sizeof(struct node), 1, f);
    return nd;
}

bool writeRoot(FILE *f, int ponteiro) {
    fseek(f, 0, SEEK_SET);
    fwrite(&ponteiro, sizeof(int), 1, f);
    return true;
}

int getRoot(FILE *f) {
    int r;
    fseek(f, 0, SEEK_SET);
    fread(&r, sizeof(int), 1, f);
    return r;
}

int sizeOfPack(FILE *f, int first) {
    int size = 1;
    node nd = getNode(f, first);
    while (nd.ponteiro_proximo != -1) {
        nd = getNode(f, nd.ponteiro_proximo);
        size++;
    }
    return size;
}

bool writeNode(FILE *f, node nd) {
    fseek(f, 0, SEEK_END);
    fwrite(&nd, sizeof(struct node), 1, f);
    return true;
}

bool changeEsquerda(FILE *f, int ponteiro, int new_esquerda) {
    node k = getNode(f, ponteiro);
    k.ponteiro_esquerda = new_esquerda;
    fseek(f, sizeof(int), SEEK_SET);
    fseek(f, ponteiro * sizeof(struct node), SEEK_CUR);
    fwrite(&k, sizeof(struct node), 1, f);
    return true;
}

bool changeDireita(FILE *f, int ponteiro, int new_direita) {
    node k = getNode(f, ponteiro);
    k.ponteiro_direita = new_direita;
    fseek(f, sizeof(int), SEEK_SET);
    fseek(f, ponteiro * sizeof(struct node), SEEK_CUR);
    fwrite(&k, sizeof(struct node), 1, f);
    return true;
}

bool changeProximo(FILE *f, int ponteiro, int new_proximo) {
    node k = getNode(f, ponteiro);
    k.ponteiro_proximo = new_proximo;
    fseek(f, sizeof(int), SEEK_SET);
    fseek(f, ponteiro * sizeof(struct node), SEEK_CUR);
    fwrite(&k, sizeof(struct node), 1, f);
    return true;
}

bool changePai(FILE *f, int ponteiro, int new_pai) {
    node k = getNode(f, ponteiro);
    k.pai = new_pai;
    fseek(f, sizeof(int), SEEK_SET);
    fseek(f, ponteiro * sizeof(struct node), SEEK_CUR);
    fwrite(&k, sizeof(struct node), 1, f);
    return true;
}

bool changeFirstOfPack(FILE *f, int ponteiro, int new_first) {
    node k = getNode(f, ponteiro);
    k.firstOfPack = new_first;
    fseek(f, sizeof(int), SEEK_SET);
    fseek(f, ponteiro * sizeof(struct node), SEEK_CUR);
    fwrite(&k, sizeof(struct node), 1, f);
    return true;
}

bool subirMeio(FILE *f, int firstOfPack, int ponteiro_atual) {
    int p = ponteiro_atual;
    node n = getNode(f, firstOfPack);

    for (int i = 0; i < MAX_SIZE / 2; i++) {
        n = getNode(f, n.ponteiro_proximo);
    }

    while (sizeOfPack(f, n.firstOfPack) > MAX_SIZE) {
        if (n.pai == -1) {
            // Eles sao o topo

            node copy = n;
            copy.ponteiro_direita = n.ponteiro;
            copy.ponteiro_esquerda = n.firstOfPack;
            copy.ponteiro_proximo = -1;
            copy.ponteiro = p;
            p++;

            int aux = n.ponteiro;
            while (getNode(f, aux).ponteiro_proximo != -1) {
                changePai(f, aux, p);
                aux = getNode(f, aux).ponteiro_proximo;
            }

            aux = n.ponteiro;
            while (getNode(f, aux).firstOfPack == n.firstOfPack) {
                changeFirstOfPack(f, aux, n.ponteiro);
                aux = getNode(f, aux).ponteiro_proximo;
            }

            copy.firstOfPack = copy.ponteiro;

            writeNode(f, copy);
            writeRoot(f, copy.ponteiro);
            n = copy;

        } else {
            // Eles nao sao o topo
            node copy = n;
            node pai = getNode(f, n.pai);
            int aux = n.ponteiro;
            while (getNode(f, aux).firstOfPack == n.firstOfPack) {
                changeFirstOfPack(f, aux, n.ponteiro);
                aux = getNode(f, aux).ponteiro_proximo;
            }

            copy.pai = pai.pai;
            copy.ponteiro_direita = n.ponteiro;
            copy.ponteiro_esquerda = pai.ponteiro_direita;
            copy.ponteiro = p;
            copy.ponteiro_proximo = pai.ponteiro_proximo;
            changeProximo(f, pai.ponteiro, copy.ponteiro);
            p++;

            if (getNode(f, pai.firstOfPack).id > copy.id) {
                // Tem que mudar o first of pack de cima
                node aux2 = getNode(f, pai.firstOfPack);
                int antigo = aux2.firstOfPack;

                while (aux2.firstOfPack == antigo) {
                    changeFirstOfPack(f, aux2.ponteiro, copy.ponteiro);
                    aux2 = getNode(f, aux2.ponteiro_proximo);
                }
                copy.firstOfPack = copy.ponteiro;
                if (copy.pai == -1) {
                    writeRoot(f, copy.ponteiro);
                }

            } else {
                copy.firstOfPack = pai.firstOfPack;
            }

            writeNode(f, copy);

            n = copy;
        }
    }
}

bool addNode(FILE *f, node nd) {
    node search = getNode(f, 0);
    int firstOfPack = search.id;
    int numberOfNodes = 1;
    while (true) {
        if (nd.id > search.id) {
            // IR PARA DIREITA

            // Ir pro proximo
            if (search.ponteiro_proximo != -1 and
                getNode(f, search.ponteiro_proximo).id < nd.id) {
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
                            nd.firstOfPack = firstOfPack;
                            changeProximo(f, search.ponteiro, nd.ponteiro);
                            writeNode(f, nd);
                        } else {
                            nd.ponteiro_proximo = search.ponteiro_proximo;
                            nd.firstOfPack = firstOfPack;
                            changeProximo(f, search.ponteiro, nd.ponteiro);
                            writeNode(f, nd);
                        }
                    } else {
                        nd.ponteiro_proximo = search.ponteiro_proximo;
                        nd.firstOfPack = firstOfPack;
                        changeProximo(f, search.ponteiro, nd.ponteiro);
                        writeNode(f, nd);
                        subirMeio(f, firstOfPack, nd.ponteiro + 1);
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
                if (sizeOfPack(f, firstOfPack) <= MAX_SIZE) {
                    if(getNode(f,firstOfPack).id > nd.id){
                        int aux = firstOfPack;
                        node aux2 = getNode(f,firstOfPack);
                        while(aux2.firstOfPack == aux){
                            changeFirstOfPack(f,aux2.ponteiro,nd.id);
                            aux2 = getNode(f,aux2.ponteiro_proximo);
                        }
                        nd.firstOfPack = nd.ponteiro;
                        
                    }
                }

                break;
            }
        }
    }
    return true;
}

int main(int argc, char const *argv[]) {
    FILE *f;

    f = fopen("Teste.txt", "wb");
    fclose(f);

    f = fopen("Teste.txt", "rb+");

    node nd;

    writeRoot(f, 0);

    nd = {0, 1, 60};
    writeNode(f, nd);

    nd = {1, 2, 70};
    addNode(f, nd);

    nd = {2, 3, 70};
    addNode(f, nd);

    nd = {3, 6, 70};
    addNode(f, nd);

    nd = {4, 5, 80};
    addNode(f, nd);

    fclose(f);

    f = fopen("Teste.txt", "r");
    nd = getNode(f, 0);
    while (nd.ponteiro_proximo != -1) {
        cout << "[" << nd.id << "]  ";
        nd = getNode(f, nd.ponteiro_proximo);
    }
    cout << "[" << nd.id << "]  " << endl;
    cout << getRoot(f) << endl;

    return 0;
}
