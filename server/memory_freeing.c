#include <stdlib.h>  // free

#include "../headers/structs.h"

void free_linked_list(hash_entry *node) {
    hash_entry *current = node;

    while (current != NULL) {
        hash_entry *next = current->next;
        free(current);
        current = next;
    }
}

void free_pointer_collection(hash_entry ** pointer_collection){
	for(int i = 0; i < TABLE_SIZE; i++){
		hash_entry * node = pointer_collection[i];
		if(node != NULL) free_linked_list(node);
	}
}