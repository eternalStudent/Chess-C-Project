#ifndef ITERATOR_H
#define ITERATOR_H
struct Iterator{
	struct ListNode* first;
	struct ListNode* current;
};

void Iterator_init(struct Iterator* iterator, struct LinkedList* list);
	
void* Iterator_next(struct Iterator* iterator);

int Iterator_hasNext(struct Iterator* iterator);
#endif