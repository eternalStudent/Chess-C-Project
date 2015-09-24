#include "LinkedList.h"
#include "Iterator.h"
#include <stdio.h>

/*
 * Initializes an iterator structure.
 *
 * @params: (iterator) - a pointer to the iterator to be initialized
 */
void Iterator_init(Iterator* iterator, LinkedList* list){
	iterator->first = list->first;
	iterator->current = NULL;
}

/* 
 * @return: the next element in the structure over which the iterator is currently working
 */
void* Iterator_next(Iterator* iterator){
	if (iterator->current == NULL){
		iterator->current = iterator->first;
	}
	else{
		iterator->current = iterator->current->next;
	}
	return iterator->current->data;
}

/* 
 * @return: 0 (false) if the given iterator has reached the end of the list, 1 (true) otherwise
 */
int Iterator_hasNext(Iterator* iterator){
	if (iterator->current == NULL){
		if (iterator->first == NULL){
			return 0;
		}
		return 1;
	}
	return (iterator->current->next != NULL);
}