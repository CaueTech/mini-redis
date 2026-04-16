#include <stdlib.h>
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

hash_entry ** initialize_entry(){
    hash_entry ** pointer_collection = malloc(sizeof(hash_entry) * TABLE_SIZE);
    return pointer_collection;

    /*
        Usamos alocação dinâmica porque depois que a função retorna, a memória - se tivesse em Stack, iria
        ser liberada, então retornariamos um ponteiro que aponta para lixo.
    */
};