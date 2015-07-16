struct ListNode{
	void* data;
	struct ListNode* next;
};

struct LinkedList{
	struct ListNode* first;
	struct ListNode* last;
	int length;
	void (*freeFunc)(void*);
};

struct LinkedList* LinkedList_new(void(*freeFunc)(void*));

int LinkedList_add(struct LinkedList* list, void* data);

void LinkedList_concatenate(struct LinkedList* this, struct LinkedList* other);

int LinkedList_length(struct LinkedList* list);

void* LinkedList_first(struct LinkedList* list);

void LinkedList_free(struct LinkedList* list);

void LinkedList_freeAllButOne(struct LinkedList* list, void* data);