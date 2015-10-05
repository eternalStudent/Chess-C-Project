#ifndef LINKEDLIST_H
#define LINKEDLIST_H

typedef struct ListNode{
	void* data;
	struct ListNode* next;
} ListNode;

typedef struct LinkedList{
	struct ListNode* first;
	struct ListNode* last;
	int length;
	void (*freeFunc)(void*);
} LinkedList;

LinkedList* LinkedList_new(void(*freeFunc)(void*));

int LinkedList_add(LinkedList* list, void* data);

void LinkedList_concatenate(LinkedList* this, LinkedList* other);

void LinkedList_concatenateAndFree(LinkedList* this, LinkedList* other);

int LinkedList_addAll(LinkedList* this, LinkedList* other);

int LinkedList_length(LinkedList* list);

void* LinkedList_first(LinkedList* list);

void* LinkedList_last(LinkedList* list);

void* LinkedList_second(LinkedList* list);

void LinkedList_removeAllAndFree(LinkedList* list);

void LinkedList_free(LinkedList* list);

void LinkedList_freeAllButOne(LinkedList* list, void* data);

void LinkedList_removeAll(LinkedList* list);

#endif