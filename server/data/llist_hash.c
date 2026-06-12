#include "../../headers/structs.h"
#include "../../headers/server.h"

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

linked_node ** initialize_collection() {
    // Aloca um array de PONTEIROS (linked_node*) e zera todos eles
    linked_node ** pointer_collection = calloc(TABLE_SIZE, sizeof(linked_node*));
    
    return pointer_collection;
}

void free_linked_list(linked_node * node) {
    linked_node * current = node;

    while (current != NULL) {
        linked_node * next = current->next;
        free(current);
        current = next;
    }
}

void free_pointer_collection(linked_node ** pointer_collection){
    for(int i = 0; i < TABLE_SIZE; i++){
        linked_node * node = pointer_collection[i];
        if(node != NULL) free_linked_list(node);
    }
}

dict_node * find_dict_node (char * key, linked_node ** pointer_collection){
    unsigned int hash = hash_function(key);
    linked_node * current = pointer_collection[hash];

    while(current != NULL){
        // BLINDAGEM: Garante que o dicionário interno existe antes de ler a chave
        if (current->dict != NULL) {
            
            if(!strcmp(current->dict->key, key)){
                return current->dict;
            }
        }
        current = current->next;
    }

    return NULL; // Retorna NULL com total segurança se não encontrar
}

// Essa função é importante no processo de limpeza das chaves expiradas
int del_ll_node(char * key, linked_node ** pointer_collection){
    unsigned int hash = hash_function(key);
    linked_node * current = pointer_collection[hash];
    linked_node * prev = NULL;

    while (current != NULL) {
        
        // BLINDAGEM: Só tenta ler a chave se o ponteiro dict for válido
        if (current->dict != NULL) {
            
            if (strcmp(current->dict->key, key) == 0) {

                // Caso 1: O nó a ser deletado é o primeiro da lista
                if (prev == NULL) {
                    pointer_collection[hash] = current->next;
                } 
                // Caso 2: O nó está no meio ou fim
                else {
                    prev->next = current->next;
                }
                
                free(current);
                return 1;
            }
        }
        
        prev = current;
        current = current->next;
    }
    
    return 0; // Chave não encontrada
}