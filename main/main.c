#include "../headers/server.h"

int main(){
	int fdmax;
	fd_set master;

	linked_node ** pointer_collection = initialize_collection();

	if(pointer_collection == NULL) {
        perror("SystemError: could not initialize pointer_collection\n");
        exit(EXIT_FAILURE);
    }
    
	min_heap * mheap = initialize_mheap();

	if(mheap == NULL){
		perror("SystemError: could not initialize min-heap\n");
        exit(EXIT_FAILURE);
	}

	server_info * server = setup(&fdmax, &master, pointer_collection, mheap);

	if(server != NULL){
		startup(pointer_collection, mheap, server, &master, &fdmax);		
	}
	else{
		perror("SystemError: setup failed\n");
		exit(EXIT_FAILURE);
	}

	return 0;
}