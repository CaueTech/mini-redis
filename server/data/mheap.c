#include "../../headers/structs.h"
#include "../../headers/server.h"

/*
	As manipulações da min-heap se baseam em operações de índices, por isso definimos às constantes acima.
*/


// Troca a posição de índices na min-heap
void heap_swap(min_heap * mheap, int i, int j){
    dict_node * temp = mheap->array[i];

    mheap->array[i] = mheap->array[j];
    mheap->array[j] = temp;

    // Atualizando os índices da min-heap nos nós

    mheap->array[i]->i_mheap = i;
    mheap->array[j]->i_mheap = j;

}

min_heap * initialize_mheap() {
    min_heap * mheap = calloc(1, sizeof(min_heap));
    if (mheap == NULL) return NULL;

    mheap->array = calloc(HEAP_SIZE, sizeof(dict_node*));
    if (mheap->array == NULL) {
        free(mheap); // Evita memory leak se a segunda alocação falhar
        return NULL;
    }

    mheap->size = 0;
    mheap->capacity = HEAP_SIZE;

    return mheap;
}

void sift_up(min_heap *mheap, int index) {
    while (index > 0) {
        int parent = HEAP_PARENT(index);

        if (mheap->array[index]->expires_at < mheap->array[parent]->expires_at) {
            heap_swap(mheap, index, parent);
            index = parent;
        } else {
            break;
        }
    }
}

void sift_down(min_heap *mheap, int index) {
    while (1) {
        int left = HEAP_LEFT(index);
        int right = HEAP_RIGHT(index);
        int smallest = index;

        // Verifica se o filho esquerdo existe e se expira antes do atual "menor"
        if (left < mheap->size && mheap->array[left]->expires_at < mheap->array[smallest]->expires_at) {
            smallest = left;
        }
        // Verifica se o filho direito existe e se expira antes do atual "menor"
        if (right < mheap->size && mheap->array[right]->expires_at < mheap->array[smallest]->expires_at) {
            smallest = right;
        }

        if (smallest != index) {
            heap_swap(mheap, index, smallest);
            index = smallest; // Continua descendo a partir da nova posição
        } else {
            break;
        }
    }
}

// Insere elementos na min-heap respeitando a ordenação dos nós pelo expire_at
int heap_push(min_heap * mheap, dict_node * new_node){
    //printf("DEBUG [MHeap] | Size - %d\nCapacity - %d\n", mheap->size, mheap->capacity);
    if (mheap->size >= mheap->capacity) return 0; // Heap cheia

    int index = mheap->size;
    mheap->array[index] = new_node;
    new_node->i_mheap = index;
    mheap->size++;

    // Reutiliza a lógica de subida isolada
    sift_up(mheap, index); 

    return 1;
}

int heap_pop(min_heap * mheap) {
    if (mheap->size == 0) return 0;

    mheap->size--;
    
    if (mheap->size > 0) {
        // CORRIGIDO: alterado de heap->size para mheap->size
        mheap->array[0] = mheap->array[mheap->size]; 
        mheap->array[0]->i_mheap = 0;
        
        // Reutiliza a lógica de descida isolada a partir do topo (0)
        sift_down(mheap, 0); 
    }

    return 1;
}

// Esta função deleta um elemento da min-heap direto pelo índice
int heap_sniper(min_heap * mheap, int idx_to_remove) {
    if (idx_to_remove < 0 || idx_to_remove >= mheap->size) return 0;

    mheap->size--; 
    
    // Se o elemento removido não era o último do array, precisamos cobrir o buraco e reordenar
    if (idx_to_remove < mheap->size) {
        mheap->array[idx_to_remove] = mheap->array[mheap->size];
        mheap->array[idx_to_remove]->i_mheap = idx_to_remove;

        // O elemento veio do fim do array para o meio da árvore. Para reorganizar, usamos:
        sift_up(mheap, idx_to_remove);
        sift_down(mheap, idx_to_remove);
    }

    return 1;
}

dict_node * heap_peek(min_heap * mheap) {
    if (mheap->size == 0) return NULL;
    return mheap->array[0]; // Retorna quem vai morrer primeiro, sem remover da árvore
}