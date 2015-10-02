#include "LinkedList.h"
#include <stdlib.h>
#include <string.h>

/* 
 * Creates a new LinkedList structure.
 *
 * @params: freeFunc - the function that will free the data
 * @return: NULL if any allocation errors occurred, the list otherwise
 */
LinkedList* LinkedList_new(void(*freeFunc)(void*)){
	LinkedList* list;
	list = (LinkedList*)calloc(1, sizeof(LinkedList));
	if (!list){
		return NULL;
	}
	list->length = 0;
	list->first = NULL;
	list->last = NULL;
	list->freeFunc = freeFunc;
	return list;
}

/* 
 * Creates a new ListNode structure, containing a pointer to a new element in a list.
 *
 * @params: data - a pointer to the element
 * @return: NULL if any allocation errors occurred, the node otherwise
 */
static ListNode* ListNode_new(void* data){
	ListNode* node;
	node = (ListNode*)calloc(1, sizeof(ListNode));
	if (!node){
		return NULL;
	}
	node->data = data;
	return node;
}

/* 
 * Appends a new element to the end of the list.
 * 
 * @params: data - a pointer to the element
 * @return: -1 if any allocation errors occurred, 0 otherwise
 */
int LinkedList_add(LinkedList* list, void* data){
	ListNode* node = ListNode_new(data);
	if (node == NULL){
		return -1;
	}
	if (list->length == 0){
		list->first = node;
	}
	else{
		list->last->next = node;
	}
	list->length++;
	list->last = node;
	return 0;
}

void LinkedList_concatenate(LinkedList* this, LinkedList* other){
	if (other->length == 0){
		free(other);
		return;
	}
	if (this->length == 0){
		this->first = other->first;
	}
	else{
		this->last->next = other->first;
	}
	this->length += other->length;
	this->last = other->last;
	free(other);
}

/* 
 * @return: the number of elements in the list
 */
int LinkedList_length(LinkedList* list){
	return list->length;
}

/*
 * @return: the first element in the list
 */
void* LinkedList_first(LinkedList* list){
	return list->first->data;                                                                      
}

/*
 * @return: the second element in the list
 */
void* LinkedList_second(LinkedList* list){
	if (list->first->next){
		return list->first->next->data;
	}
	else{
		return NULL;
	}	
}

void LinkedList_freeNodesOnly(LinkedList* list){
	ListNode* node = list->first;
	while (node != NULL){
		ListNode* next = node->next;
		list->freeFunc(node->data);
		free(node);
		node = next;
	}
	list->length = 0;
	list->first = NULL;
	list->last = NULL;
}

/* 
 * Frees the list from memory.
 */
void LinkedList_free(LinkedList* list){
	ListNode* node = list->first;
	while (node != NULL){
		ListNode* next = node->next;
		list->freeFunc(node->data);
		free(node);
		node = next;
	}
	free(list);
}

/* 
 * Frees the list from memory, all but for one specified element.
 */
void LinkedList_freeAllButOne(LinkedList* list, void* data){
	ListNode* node = list->first;
	while (node != NULL){
		ListNode* next = node->next;
		if (data != node->data){
			list->freeFunc(node->data);
		}	
		free(node);
		node = next;
	}
	free(list);
}

void LinkedList_removeAll(LinkedList* list){
	ListNode* node = list->first;
	while (node != NULL){
		ListNode* next = node->next;
		free(node);
		node = next;
	}
	list->length = 0;
	list->first = NULL;
	list->last = NULL;
}