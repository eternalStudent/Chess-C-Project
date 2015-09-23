#ifndef ITERATOR_H
#define ITERATOR_H
typedef struct Iterator{
	struct ListNode* first;
	struct ListNode* current;
} Iterator;

void Iterator_init(struct Iterator* iterator, struct LinkedList* list);
	
void* Iterator_next(struct Iterator* iterator);

int Iterator_hasNext(struct Iterator* iterator);
#endif