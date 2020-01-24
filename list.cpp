#include "list.h"
#include "const.h"

List::List() : head(nullptr), tail(nullptr) {}

List::~List() {
	ListElement *e = head;

	while (head) {
		e = head;
		head = head->next;

		delete e;
	}

	tail = nullptr;
}

List::ListElement* List::toEnd(void* data) {
	tail = (!tail ? head : tail->next) = new ListElement(data, tail, nullptr);

	return tail;
}

List::ListElement* List::toBeginning(void* data) {
	head = (!head ? tail : head->prev) = new ListElement(data, nullptr, head);

	return head;
}

List::ListElement* List::insertBefore(void* data, ListElement* elem) {
	ListElement* _new = new ListElement(data, elem->prev, elem);

	if (elem->prev) elem->prev->next = _new;
	else head = _new;

	elem->prev = _new;

	return _new;
}

List::ListElement* List::insertAfter(void* data, ListElement* elem) {
	ListElement* _new = new ListElement(data, elem, elem->next);

	if (elem->next) elem->next->prev = _new;
	else tail = _new;

	elem->next = _new;

	return _new;
}

int List::empty() const {
	return head == nullptr ? true : false;
}

List::ListElement* List::first() {
	return head;
}

List::ListElement* List::last() {
	return tail;
}

void* List::removeFirst() {
	return remove(head);
}

void* List::removeLast() {
	return remove(tail);
}

void* List::remove(ListElement* elem) {
	if (!elem) return nullptr;
	
	if (elem->prev) elem->prev->next = elem->next;
	if (elem->next) elem->next->prev = elem->prev;
	if (elem == head) head = elem->next;
	if (elem == tail) tail = elem->prev;

	void* ret = elem->data;
	delete elem;

	return ret;
}
