#ifndef SERVER_H
#define SERVER_H

#include "structs.h"      // your structs: server_info, linked_node, command_block
#include <sys/socket.h>   // fd_set
#include <sys/types.h>    // socket(), bind(), accept()
#include <sys/select.h>   // select(), fd_set
#include <netinet/in.h>   // struct sockaddr_in
#include <arpa/inet.h>    // (optional, but good practice for networking)
#include <time.h>
#include <stdio.h>        // perror
#include <stddef.h>       // NULL
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>       // close(), read(), write()

// Conjunto de protótipos de funções

server_info * create_listening_socket();
server_info * setup(int * fdmax, fd_set * master, linked_node ** pointer_collection, min_heap * mheap);
server_info * create_listening_socket();

void startup(linked_node ** pointer_collection, min_heap * mheap, server_info * server, fd_set * master, int * fdmax);
void close_connection(int fd, int * fdmax, fd_set *master);
void free_linked_list(linked_node * node);
void free_pointer_collection(linked_node ** pointer_collection);
void send_response(int fd, const char* response);

linked_node ** initialize_collection();

min_heap * initialize_mheap();

dict_node * heap_peek(min_heap *heap);
dict_node * get(command_block cmd, linked_node ** pointer_collection);
dict_node * find_dict_node (char * key, linked_node ** pointer_collection);

command_block parsing_command(char * buffer, char reading_type);

unsigned int hash_function(char *str);

int heap_push(min_heap * mheap, dict_node * new_node);
int receive_data(int fd, char * buffer);
int accept_connection(int fd, struct sockaddr_in *);
int bind_server(int fd, struct sockaddr_in addr);
int set(command_block cmd, linked_node ** pointer_collection, min_heap * mheap);
int add_aof(command_block cmd);
int load_aof(linked_node ** pointer_collection, min_heap * mheap);
int del_ll_node(char * key, linked_node ** pointer_collection);
int del(command_block cmd, min_heap * mheap, linked_node ** pointer_collection);
int heap_sniper(min_heap * mheap, int idx_to_remove);
int heap_pop(min_heap * heap);

#endif