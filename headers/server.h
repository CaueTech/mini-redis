#ifndef SERVER_H
#define SERVER_H

#include "structs.h"      // your structs: server_info, hash_entry, command_block
#include <sys/socket.h>   // fd_set
#include <netinet/in.h>   // struct sockaddr_in

// Conjunto de protótipos de funções

server_info * create_listening_socket();
server_info *setup(int * fdmax, fd_set *master);
server_info* create_listening_socket();

void startup(hash_entry **, server_info *, fd_set *, int * fdmax);
void close_connection(int fd, int *fdmax, fd_set *master);
void free_linked_list(hash_entry *node);
void free_pointer_collection(hash_entry ** pointer_collection);
void send_response(int fd, const char* response);

hash_entry **initialize_entry();
hash_entry * get(command_block cmd, hash_entry ** pointer_collection);

command_block parsing_command(char* buffer);
command_block parsing_command(char *buffer);

unsigned int hash_function(char *str);

int receive_data(int fd, char *buffer);
int accept_connection(int fd, struct sockaddr_in *);
int bind_server(int fd, struct sockaddr_in addr);
int set(command_block cmd, hash_entry ** pointer_collection);

#endif