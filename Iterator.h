#ifndef ITERATOR_H
#define ITERATOR_H
typedef struct Iterator{
	struct ListNode* first;
	struct ListNode* current;
} Iterator;

void Iterator_init(Iterator* iterator, LinkedList* list);
	
void* Iterator_next(Iterator* iterator);

int Iterator_hasNext(Iterator* iterator);
#endif