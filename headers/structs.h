#ifndef STRUCTS_H
#define STRUCTS_H

#include <netinet/in.h>

#define KEY_SIZE 50
#define VALUE_SIZE 500
#define TABLE_SIZE 1024
#define BUFFER_SIZE 1024

#define HEAP_PARENT(i) ((i - 1) / 2)
#define HEAP_LEFT(i)   (2 * i + 1)
#define HEAP_RIGHT(i)  (2 * i + 2)  
#define HEAP_SIZE 15

typedef struct {
    struct sockaddr_in server_address;
    int ls_socket_fd;
} server_info;

typedef struct {
    char command[5];
    char key[KEY_SIZE];
    char value[VALUE_SIZE];
    int has_ttl; // Booleano: 0 ou 1 identifica se a chave tem expiração
    long long ttl_val;
    long long expires_at; // Grava o timestamp em que o comando foi feito para calcular expiração ao carregar log.txt
} command_block;


typedef struct {
    char key[KEY_SIZE];
    char value[VALUE_SIZE];
    long long expires_at;
    int i_mheap; // É o índice do nó na min-heap para expiração TTL, esse campo é necessário para otimizar buscas na min-heap no caso de atualizações da chave e seu TTL.

    /*
        Por convenção os que não tiverem índice (não tem TTL) terão i_mheap = -1 e expires_at = -1.
    */
} dict_node;

typedef struct linked_node{
    dict_node * dict;
    struct linked_node * next;
} linked_node;

/*
    O linked_node é um bloco de memória que armazena o endereço do nó key-value e um endereço para um próximo bloco
    linked_node, para funcionar como uma lista encadeada que trata de colisões das chaves dos nós key-value.
*/

typedef struct{
    dict_node ** array;
    int size;
    int capacity;
} min_heap;

/*
    Essa estrutura é uma Min Heap, uma árvore que organiza os nós com expiração em ordem crescente. Assim como na lista encadeada que trata colisões, a Min Heap tratará dos dict_node que precisam ser ordenados - em algum nível - de acordo com sua chave de expiração. No caso a estrutura possui um array ordenado e atributos de controle.

    Isso é feito pois cada nó de índice i tem uma relação de parentesco com dois nós: esquerdo e direito, dados pelas fórmulas 2i + 1 e 2i + 2, respectivamente. Isso faz com que tudo possa ser gerenciado através dos índices, sem precisar de uma estrutura mais complexa de ponteiros como em uma árvore mais complexa.
*/

#endif