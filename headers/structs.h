#ifndef STRUCTS_H
#define STRUCTS_H

#include <netinet/in.h>

#define KEY_SIZE 50
#define VALUE_SIZE 500
#define TABLE_SIZE 1024

typedef struct {
    struct sockaddr_in server_address;
    int ls_socket_fd;
} server_info;

typedef struct {
    char command[5];
    char key[KEY_SIZE];
    char value[VALUE_SIZE];
} command_block;


typedef struct hash_entry {
    char key[KEY_SIZE];
    char value[VALUE_SIZE];
    struct hash_entry* next; // Para tratar colisões (lista encadeada)
} hash_entry;

hash_entry * initialize_entry(){
    hash_entry * pointer_collection[TABLE_SIZE];
    return pointer_collection;
};

/*
    Temos um array de ponteiros para as entries (nós), nós que por sua vez são dicionário (key -> value), 
    no caso de haver uma colisão, o nó aponta para um outro cujo "índice" deu igual, se tornando uma lista encadeada nesse 
    caso.
*/

#endif