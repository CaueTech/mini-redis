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
    		(*fdmax)--;
    	}
    }
}

int set(command_block cmd, linked_node ** pointer_collection, min_heap * mheap){
    unsigned int hash = hash_function(cmd.key);
    linked_node * ll_node = pointer_collection[hash];

    // CASO 1: array está vazio naquela posição, campo limpo
    if(ll_node == NULL){
        dict_node * new_entry = malloc(sizeof(dict_node));
        if(new_entry == NULL) return 0;

        strncpy(new_entry->key, cmd.key, sizeof(new_entry->key) - 1);
        new_entry->key[sizeof(new_entry->key) - 1] = '\0'; 
        
        strncpy(new_entry->value, cmd.value, sizeof(new_entry->value) - 1);
        new_entry->value[sizeof(new_entry->value) - 1] = '\0';

        if(cmd.has_ttl){
            new_entry->expires_at = cmd.expires_at;
        }
        else{
            new_entry->expires_at = -1;
            new_entry->i_mheap = -1;
        }

        linked_node * new_ll_node = malloc(sizeof(linked_node));
        if(new_ll_node != NULL){
            if(cmd.has_ttl) {
                int push_status = heap_push(mheap, new_entry);
                if(!push_status) {
                    fprintf(stderr, "ERROR <HeapPush>: inserting in Min Heap failed. Wait until some keys expire.\n");
                    free(new_entry);
                    free(new_ll_node); // Importante liberar o linked_node que já havia sido alocado!
                    return 0;
                }
            }
            
            new_ll_node->dict = new_entry;
            new_ll_node->next = NULL;
            pointer_collection[hash] = new_ll_node;
        }
        else{
            free(new_entry);
            return 0; 
        }

        return 1;
    }

    // CASO 2: array já tem nós naquela posição, precisa olhar a galera
    else{
        while(ll_node != NULL){
            dict_node * dict_entry = ll_node->dict;

            // Busca achar uma duplicata da chave para substituir o valor captado
            if(strcmp(dict_entry->key, cmd.key) == 0){
                
                // Se a chave já tinha um TTL anterior na heap, removemos a versão antiga primeiro
                if(dict_entry->expires_at != -1 && dict_entry->i_mheap >= 0) {
                    heap_sniper(mheap, dict_entry->i_mheap);
                }

                if(cmd.has_ttl){
                    int push_status = heap_push(mheap, dict_entry);
                    if(!push_status) {
                        fprintf(stderr, "ERROR <HeapPush>: inserting in Min Heap failed. Wait until some keys expire.\n");
                        // Aborta a atualização se não pode garantir o TTL
                        return 0;
                    }
                    dict_entry->expires_at = cmd.expires_at;
                }
                else{
                    dict_entry->expires_at = -1;
                    dict_entry->i_mheap = -1;
                }

                // Só atualiza o valor se passou pela verificação do TTL com sucesso
                strncpy(dict_entry->value, cmd.value, sizeof(dict_entry->value) - 1);
                dict_entry->value[sizeof(dict_entry->value) - 1] = '\0';

                return 1;
            }

            // No caso de não haver duplicatas, cria um novo nó com aquela key e valor no fim da lista
            if(ll_node->next == NULL){
                dict_node * new_entry = malloc(sizeof(dict_node));
                if(new_entry == NULL) return 0; 
                
                strncpy(new_entry->key, cmd.key, sizeof(new_entry->key) - 1);
                new_entry->key[sizeof(new_entry->key) - 1] = '\0';
                
                strncpy(new_entry->value, cmd.value, sizeof(new_entry->value) - 1);
                new_entry->value[sizeof(new_entry->value) - 1] = '\0';

                if(cmd.has_ttl){
                    new_entry->expires_at = cmd.expires_at;
                }
                else{
                    new_entry->expires_at = -1;
                    new_entry->i_mheap = -1;
                }

                linked_node * new_ll_node = malloc(sizeof(linked_node));

                if(new_ll_node != NULL){
                    // Fail-Fast: Tenta colocar na Heap ANTES de conectar na lista
                    if(cmd.has_ttl){
                        int push_status = heap_push(mheap, new_entry);
                        if(!push_status) {
                            fprintf(stderr, "ERROR <HeapPush>: inserting in Min Heap failed. Wait until some keys expire.\n");
                            free(new_entry);
                            free(new_ll_node);
                            return 0;
                        }
                    }

                    new_ll_node->dict = new_entry;
                    new_ll_node->next = NULL;
                    ll_node->next = new_ll_node; // Agora sim, conectamos!
                }
                else{
                    free(new_entry);
                    return 0; 
                }

                return 1;
            }

            ll_node = ll_node->next;
        }
    }
    
    return 0;
}

dict_node * get(command_block cmd, linked_node ** pointer_collection){
    unsigned int hash = hash_function(cmd.key);
    linked_node * ll_node = pointer_collection[hash];
    
    // Varre a lista encadeada daquela gaveta do hash
    while(ll_node != NULL){
        // Só extraímos o dicionário agora que temos CERTEZA que ll_node existe
        dict_node * dict_entry = ll_node->dict;
        
        // Essa verificação é meio exagerada mas deixo só por precaução
        if (dict_entry != NULL) {
            if(!strcmp(cmd.key, dict_entry->key)){
                return dict_entry;           
            }
        }
        
        ll_node = ll_node->next;
    }
    
    return NULL;
}

int del(command_block cmd, min_heap * mheap, linked_node ** pointer_collection){
    
    dict_node * dict_entry = find_dict_node(cmd.key, pointer_collection);
    
    // Se a chave sequer existe no banco, morre aqui antes de tentar apagar qualquer coisa
    if (dict_entry == NULL) {
        return 0;
    }

    int target_heap_idx = dict_entry->i_mheap;

    // Remove o wrapper da lista encadeada na Hash Table
    int del_status = del_ll_node(cmd.key, pointer_collection);

    if (del_status) {
        if (target_heap_idx >= 0) {
            heap_sniper(mheap, target_heap_idx);
        }
        
        free(dict_entry);
        return 1;
    }

    return 0;
}