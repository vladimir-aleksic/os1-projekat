#include "queue.h"


List::ListElement* Queue::put(void* data) {
	return toEnd(data);
}

void* Queue::get() {
	return removeFirst();
}
