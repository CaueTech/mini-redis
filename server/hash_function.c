#include "../headers/structs.h"

unsigned int hash_function(char *str) {
    unsigned int hash = 5381;
    int c;
    while ((c = *str++)) // Este bloco atribui à c o valor atual do caractere e depois soma o ponteiro para percorrer até o próximo
        hash = ((hash << 5) + hash) + c; 
    return hash % TABLE_SIZE;
}

/*
	A hash_function transforma o valor do nome da variável desejada pelo usuário em um índice para o array de ponteiros.
*/