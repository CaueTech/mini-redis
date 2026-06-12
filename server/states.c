#include "../headers/structs.h"
#include "../headers/server.h"

/* -------- SHUTDOWN -------- */

int server_shutdown(server_info * server, linked_node ** pointer_collection){
	free_pointer_collection(pointer_collection);
	close(server->ls_socket_fd);
	free(server);
	return 1;
}

/* -------- SETUP -------- */


server_info * setup(int * fdmax, fd_set * master, linked_node ** pointer_collection, min_heap * mheap){
	server_info * server = create_listening_socket();
	if(server == NULL) return NULL;

	int bind_status = bind_server(server->ls_socket_fd, server->server_address);

	if(bind_status == -1) {
		perror("SystemError: failed binding server\n");
		return NULL;
	}

	else{ 
		printf("MESSAGE <Binding>: server succesfully bound to port 8080\n");
		FD_ZERO(master); // Limpa memória lixo do conjunto mestre de FDs
		FD_SET(server->ls_socket_fd, master);
		*fdmax = server->ls_socket_fd;

		int load_aof_status = load_aof(pointer_collection, mheap);

		if(load_aof_status){
			printf("MESSAGE <LoadLog>: log.txt succesfully read and saved into memory\n");
		}
		else{
			fprintf(stderr, "ERROR <LoadLog>: failed loading - partially or completely - log.txt\n");
		}

		int listen_status = listen(server->ls_socket_fd, 3); 

		if(listen_status == -1){
			perror("SystemError: failed to setup listening socket\n");
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

void startup(linked_node ** pointer_collection, min_heap * mheap, server_info * server, fd_set * master, int * fdmax){
	
	while(1){
		fd_set read_fds = *master;
		
		int nfds = *fdmax + 1;

		// 1. Configuração de timeout e select

		struct timeval timeout;
		timeout.tv_sec = 0;
		timeout.tv_usec = 10000;

		// select() retorna o número de FD's ativos
		int activity = select(nfds, &read_fds, NULL, NULL, &timeout);


		// 2. Leitura de FD's

		if(activity > 0){
		
			for(int fd = 0; fd <= *fdmax; fd++){
				
				// Se há uma conexão esperando
				if(FD_ISSET(fd, &read_fds)){ 

					// CASO 1: aceitando conexão
					if(fd == server->ls_socket_fd){
						
						struct sockaddr_in client_addr;
						
						int new_fd = accept_connection(fd, &client_addr);
						
						// Supondo retorno válido (-1 seria um erro do accept())
						if(new_fd != -1){ 
							FD_SET(new_fd, master);
							if(new_fd > *fdmax) *fdmax = new_fd;
						}
						else{
							perror("SystemError: accept_connection() failed\n");
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
							    (*fdmax)--;
							}
						}
						
						// CASO 2.2: nesse caso, há dados há serem tratados no recebimento
						else if(bytes > 0){ 
							
							command_block cmd = parsing_command(buffer, 'C');
							
							if(!strcmp(cmd.command, "SET")){
								int set_status = set(cmd, pointer_collection, mheap);
								if(set_status){
									printf("MESSAGE <SetData>: data succesfully saved into memory\n");

									int log_status = add_aof(cmd);

									if(log_status){
								    	printf("MESSAGE <LogAppending>: data succesfully added to log.txt\n");
									}
									else{
										fprintf(stderr, "ERROR <LogAppending>: could not load or create log.txt after command\n");
									}
								}
								else{
									fprintf(stderr, "ERROR <SetData>: failed to save data into memory\n");
								}
							}

							else if(!strcmp(cmd.command, "GET")){
								dict_node
							 * entry = get(cmd, pointer_collection);

								if (entry != NULL) {
								    char * value = entry->value;
								    printf("%s\n", value);
								} else {
								    printf("MESSAGE <ReceiveData>: no valid data read from key\n");
								}
							}

							else if(!strcmp(cmd.command, "DEL")){
								int del_status = del(cmd, mheap, pointer_collection);

								if(del_status){
									printf("MESSAGE <DeleteKey>: data succesfully deleted\n");

									int log_status = add_aof(cmd);

									if(log_status){
								    	printf("MESSAGE <ReadLog>: command succesfully added to log.txt\n");
									}
									else{
										fprintf(stderr, "ERROR <ReadLog>: could not load or create log.txt after command\n");
									}
								}
								else{
									printf("MESSAGE <DeleteKey>: nothing happnened. It is likely that the key does not exist yet\n");
								}
							}

							else{
								printf("MESSAGE <ReceiveData>: no valid command read\n");
							}

							send_response(fd, "+OK\r\n"); // Status de OK enviado para o cliente
						
						}

						// CASO 2.3: erro ao ler dados, conexão será fechada
						else{
							fprintf(stderr, "ERROR <ReceiveData>: closing socket\n");
							close_connection(fd, fdmax, master);
							FD_CLR(fd, master);
						}
					}
				}
			}
		}

		// 3. Verificação da MIN-HEAP

		dict_node * top = heap_peek(mheap);

		int clean_keys = 0;

		long long cur_timestamp = (long long) time(NULL);

		// Enquanto houver chaves com TTL expirada na Heap
		while(top != NULL && top->expires_at != -1 && cur_timestamp >= top->expires_at){
			if(del_ll_node(top->key, pointer_collection)){
				heap_pop(mheap);
				printf("MESSAGE <RemovedTTLKey>: expired key '%s' being removed...\n", top->key);
				free(top);
				clean_keys++;

				top = heap_peek(mheap);
				printf("MESSAGE <RemovedTTLKey>: key removed succesfully\n");
			}
			else{
				fprintf(stderr, "ERROR <RemovedTTLKey>: failed removing expired key\n");
			}

			// Evita loop muito grande, limitando 10 chaves por ciclo de exclusão
			if(clean_keys >= 10){
				break;
			}
		}
	}
	
	if (!server_shutdown(server, pointer_collection)) printf("MESSAGE <Server_ShutDown>: server shutted down\n");
	else{
		fprintf(stderr, "ERROR <Server_ShutDown>: memory was not properly freed\n");
	}
}