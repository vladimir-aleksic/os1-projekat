#ifndef __LIST_H__
#define __LIST_H__


class List {
public:
	List();
	virtual ~List();

	struct ListElement {
		ListElement(void* d, ListElement* p, ListElement* n) : data(d), next(n), prev(p) {}
		void* data;
		ListElement* next;
		ListElement* prev;
	};

	ListElement* toEnd(void*);
	ListElement* toBeginning(void*);
	ListElement* insertAfter(void*, ListElement*);
	ListElement* insertBefore(void*, ListElement*);
	ListElement* first();
	ListElement* last();

	void* removeFirst();
	void* removeLast();
	void* remove(ListElement*);

	int empty() const;

private:
	ListElement *head, *tail;
};

#endif
