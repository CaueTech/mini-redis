#include <stdlib.h>      // malloc, free
#include <string.h>      // memset, strlen, strncpy, strcmp
#include <unistd.h>      // close
#include <sys/socket.h>  // socket, bind, accept, recv, send
#include <netinet/in.h>  // sockaddr_in, htons, INADDR_ANY
#include <arpa/inet.h>   // (optional, but good practice for networking)
#include <sys/select.h>  // fd_set, FD_CLR, FD_ISSET

#include "../headers/structs.h"
#include "../headers/server.h"

server_info* create_listening_socket() {
    server_info* server = malloc(sizeof(server_info));

    server->ls_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(server->ls_socket_fd == -1){
    	return NULL;
    	// SO falhou em definir o file descriptor
    }

    server->server_address.sin_family = AF_INET;
    server->server_address.sin_addr.s_addr = INADDR_ANY;
    server->server_address.sin_port = htons(8080);

    return server;
}

int bind_server(int ls_socket_fd, struct sockaddr_in address){
	int opt = 1;
	setsockopt(ls_socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
	
	/*
		Essas primeiras linhas configuram o SO para impedir que ele bloqueie o servidor
		de usar a mesma porta previamente usada em caso de reinício do servidor.
	*/


    return bind(ls_socket_fd, (struct sockaddr*)&address, sizeof(address));
}

int accept_connection(int server_fd, struct sockaddr_in* client_addr) {
    socklen_t addrlen = sizeof(*client_addr);
    return accept(server_fd, (struct sockaddr*)client_addr, &addrlen);
}

int receive_data(int fd, char* buffer) {
    memset(buffer, 0, BUFFER_SIZE);
    return recv(fd, buffer, BUFFER_SIZE - 1, 0); // BUFFER_SIZE - 1 para sobrar espaço para o caractere de término '\0'
}

void send_response(int fd, const char* response) {
    send(fd, response, strlen(response), 0);
}

void close_connection(int fd, int * fdmax, fd_set* master) {
    close(fd);
    FD_CLR(fd, master);

    if(fd == *fdmax){
    	while(*fdmax > 0 && !FD_ISSET(*fdmax, master)){
    		fdmax--;
    	}
    }
}

int set(command_block cmd, hash_entry ** pointer_collection){
	unsigned int hash = hash_function(cmd.key);
	hash_entry * dict_entry = pointer_collection[hash];

	// CASO 1: array está vazio naquela posição, campo limpo
	if(dict_entry == NULL){
		hash_entry * new_entry = malloc(sizeof(hash_entry));
		if(new_entry == NULL) return 0; // malloc() returna NULL se falha alocação

		strncpy(new_entry->key, cmd.key, sizeof(new_entry->key));
		new_entry->key[sizeof(new_entry->key) - 1] = '\0';
		
		strncpy(new_entry->value, cmd.value, sizeof(new_entry->value));
		new_entry->value[sizeof(new_entry->value) - 1] = '\0';
		new_entry->next = NULL;

		pointer_collection[hash] = new_entry;
		return 1;
	}

	// CASO 2: array já tem nós naquela posição, precisa varrer a galera
	else{
		while(dict_entry != NULL){

			// Busca achar uma duplicata da chave para subsituir o valor captado
			if(strcmp(dict_entry->key, cmd.key) == 0){
				strncpy(dict_entry->value, cmd.value, sizeof(dict_entry->value));
				dict_entry->key[sizeof(dict_entry->key) - 1] = '\0';
				return 1;
			}

			// No caso de não haver duplicatas, cria um novo nó com aquela key e valor.
			if(dict_entry->next == NULL){
				hash_entry * new_entry = malloc(sizeof(hash_entry));
				if(new_entry == NULL) return 0; // malloc() returna NULL se falha alocação
				
				strncpy(new_entry->key, cmd.key, sizeof(dict_entry->key));
				new_entry->key[sizeof(new_entry->key) - 1] = '\0';
				
				strncpy(new_entry->value, cmd.value, sizeof(dict_entry->value));
				new_entry->value[sizeof(new_entry->value) - 1] = '\0';
				new_entry->next = NULL;

				dict_entry->next = new_entry;
				return 1;
			}

			dict_entry = dict_entry->next;
		}
		return 0;
	}
}

hash_entry * get(command_block cmd, hash_entry ** pointer_collection){
	unsigned int hash = hash_function(cmd.key);
	hash_entry * dict_entry = pointer_collection[hash];
	while(dict_entry != NULL){
		
		if(!strcmp(cmd.key, dict_entry->key)){
			return dict_entry;			
		}
		dict_entry = dict_entry->next;
	}
	return NULL;
}
