#ifndef __QUEUE_H__
#define __QUEUE_H__

#include "list.h"

class Queue : public List {
public:
	ListElement* put(void*);
	void* get();
};

#endif
