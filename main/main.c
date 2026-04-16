#include <stdio.h>     // perror
#include <stddef.h>    // NULL

#include "../headers/server.h"

int main(){
	int fdmax;
	fd_set master;

	server_info * server = setup(&fdmax, &master);
	if(server != NULL){
		hash_entry ** pointer_collection = initialize_entry();
		startup(pointer_collection, server, &master, &fdmax);		
	}
	else{
		perror("ERROR <Setuping>\n");
		return 1;
	}

	return 0;
}