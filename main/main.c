#include "structs.h"
#include "server.c"
#include "hash_entry.c"

int main(){
	int fdmax;
	fd_set master;

	int setup_status = setup(&fdmax);
	if(setup_status){
		hash_entry ** pointer_collection = initialize_entry();
		startup(pointer_collection, server, &master, fdmax);		
	}
	else{
		return 1;
	}

	return 0;
}