#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>        // close(), read(), write()
#include <sys/types.h>    // basic system data types
#include <sys/socket.h>   // socket(), bind(), accept()
#include <sys/select.h>   // select(), fd_set

#include "../headers/structs.h"
#include "../headers/server.h"

#define BUFFER_SIZE 1024

/* -------- SHUTDOWN -------- */

int server_shutdown(server_info * server, hash_entry ** pointer_collection){
	free_pointer_collection(pointer_collection);
	close(server->ls_socket_fd);
	free(server);
	return 1;
}

/* -------- SETUP -------- */


server_info * setup(int * fdmax, fd_set * master){
	server_info * server = create_listening_socket();
	if(server == NULL) return server; // SO falhou na criação do listening socket

	int bind_status = bind_server(server->ls_socket_fd, server->server_address);

	if(bind_status == -1) perror("ERROR <Binding>\n"); 
	else{ 
		printf("MESSAGE <Binding>: server succesfully bound to port 8080\n");
		FD_ZERO(master); // Limpa memória lixo do conjunto mestre de FDs
		FD_SET(server->ls_socket_fd, master);
		*fdmax = server->ls_socket_fd;

		int listen_status = listen(server->ls_socket_fd, 3); 

		if(listen_status == -1){
			perror("ERROR <Listening>: failed to setup listening socket\n");
		}

		/* 
			A partir de listen() o server começa a esperar conexões, 
			usei o número 3 como limite de conexões que podem ficar na "fila". 
			Então se o server_fd puder cuidar de 10 conexões ao mesmo tempo, 
			ele deixará mais 3 na fila de espera, e qualquer request acima disso será recusada. 
		*/

		return server; // Deu tudo certo
	} 
}

/* -------- STARTUP -------- */

void startup(hash_entry ** pointer_collection, server_info* server, fd_set * master, int * fdmax){
	
	while(1){
		fd_set read_fds = *master;
		
		int nfds = *fdmax + 1;
		select(nfds, &read_fds, NULL, NULL, NULL);
		
		for(int fd = 0; fd <= *fdmax; fd++){
			
			// Se há uma conexão esperando
			if(FD_ISSET(fd, &read_fds)){ 

				// CASO 1: aceitando conexão
				if(fd == server->ls_socket_fd){
					
					struct sockaddr_in client_addr;
					socklen_t addrlen = sizeof(client_addr);
					
					int new_fd = accept_connection(fd, &client_addr);
					
					// Supondo retorno válido (-1 seria um erro do accept())
					if(new_fd != -1){ 
						FD_SET(new_fd, master);
						if(new_fd > *fdmax) *fdmax = new_fd;
					}
					else{
						perror("ERROR <AcceptError>\n");
						break;
					}
					
				}

				// CASO 2: há dados sendo transmitidos
				else{
					
					char buffer[BUFFER_SIZE];
					memset(buffer, 0, BUFFER_SIZE); // Limpa o buffer e o preenche com caracteres de término (zeros)
					
					int bytes = receive_data(fd, buffer);
					
					// CASO 2.1: sem dados para serem tratados, conexão será fechada
					if(bytes == 0){
						printf("MESSAGE <ReceiveData>: user shutted conection down\n");
						close_connection(fd, fdmax, master);
						FD_CLR(fd, master);

						// Reconfigurando o fdmax
						while (*fdmax > 0 && !FD_ISSET(*fdmax, master)) {
						    *fdmax--;
						}
					}
					
					// CASO 2.2: nesse caso, há dados há serem tratados no recebimento
					else if(bytes > 0){ 
						
						command_block cmd = parsing_command(buffer);
						
						if(!strcmp(cmd.command, "SET")){
							int set_status = set(cmd, pointer_collection);
						}
						else if(!strcmp(cmd.command, "GET")){
							hash_entry *entry = get(cmd, pointer_collection);

							if (entry != NULL) {
							    char *value = entry->value;
							    printf("%s\n", value);
							} else {
							    printf("MESSAGE <ReceiveData>: no valid data read from key\n");
							}
						}
						else{
							printf("MESSAGE <ReceiveData>: no valid command read\n");
						}

						send_response(fd, "+OK\r\n"); // Status de OK enviado para o cliente
					
					}

					// CASO 2.3: erro ao ler dados, conexão será fechada
					else{
						perror("ERROR <ReceiveData>: closing socket\n");
						close_connection(fd, fdmax, master);
						FD_CLR(fd, master);
					}
				}
			}
		}
	}
	
	if (!server_shutdown(server, pointer_collection))printf("MESSAGE <Serverserver_ShutDown>: server shutted down\n");
	else{
		perror("ERROR <Serverserver_ShutDown>: memory was not properly freed\n");
	}
}