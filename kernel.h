#ifndef __KERNEL_H__
#define __KERNEL_H__

#include "const.h"
#include "pcb.h"
#include "timer.h"
#include "thread.h"
#include "semaphor.h"
#include "kersem.h"
#include "list.h"
#include "event.h"
#include "ivtentry.h"


class Kernel {
public:
	static PCB* iddlePCB();
	static Thread* getThreadById(ID id);

	static void init();
	static void restore();

	static void threadCreated(Thread*);
	static void threadDestroyed(Thread*);

	static void registerTimeBlock(SemElem*);
	static void unregisterTimeBlock(List::ListElement*);
	static void semaphoreTick();

	static Thread *const iddleThread;
	static Thread *const mainThread;

private:
	static List* allThreads;
	static List* timeBlock;

};



class Iddle : public Thread {
public:
	Iddle() : Thread(64, 1) {}
protected:
	virtual void run();
};

#endif
