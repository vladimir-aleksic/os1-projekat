#include "kernel.h"
#include "thread.h"
#include "kersem.h"
#include "queue.h"
#include "list.h"
#include "schedule.h"

static Semaphore threadListMutex(1);

Thread *const Kernel::iddleThread = new Iddle;
Thread *const Kernel::mainThread = new Thread;
List* Kernel::allThreads = new List;
List* Kernel::timeBlock = new List;

bool inSemaphoreTick = false;


void Kernel::init() {
	mainThread->myPCB->setStatus(READY);
	PCB::running = mainThread->myPCB;

	Timer::init();
}


void Kernel::restore() {
	Timer::restore();

	delete mainThread;
	delete allThreads;
	delete timeBlock;
	delete iddleThread;

	for (int i = 0; i < IVTCNT; i++) {
		if (IVTEntry::entries[i]) {
			IVTEntry::restoreEntry(i);
			delete IVTEntry::entries[i];
		}
	}
	__iunlock__;
}


Thread* Kernel::getThreadById(ID id) {
	Thread* retVal = nullptr;

	threadListMutex.wait(0);

	for (List::ListElement* el = allThreads->first(); el; el = el->next) {
		Thread* t = (Thread*)el->data;
		
		if (t->getId() == id) {
			retVal = t;
			break;
		}

	}
	
	threadListMutex.signal();
	return retVal;
}


void Kernel::threadCreated(Thread* thread) {
	threadListMutex.wait(0);
	thread->myPCB->listElement = allThreads->toEnd(thread);
	threadListMutex.signal();
}


void Kernel::threadDestroyed(Thread* thread) {
	threadListMutex.wait(0);
	allThreads->remove(thread->myPCB->listElement);
	threadListMutex.signal();
}


// Ubaci nit u uredjenu listu svih niti koje su blokirane na maxTimeToWait>0
void Kernel::registerTimeBlock(SemElem* newElement) {
	List::ListElement *le = timeBlock->first();
	List::ListElement *&nwe = newElement->waitingElement;
	Time& nwc = newElement->waitingCounter;

	if (!le) nwe = timeBlock->toEnd(newElement);

	while (le && !nwe) {
		Time& cwc = ((SemElem*)le->data)->waitingCounter;

		if (nwc <= cwc) {
			nwe = timeBlock->insertBefore(newElement, le);
			cwc -= nwc;
		} else {
			nwe = le->next?0: timeBlock->insertAfter(newElement, le);
			nwc -= cwc;
		}

		le = le->next;
	}
}


// Izbaci nit iz liste svih niti koje su blokirane na maxTimeToWait>0
// jer je nit odblokirana preko signal metode
void Kernel::unregisterTimeBlock(List::ListElement* le) {
	Time wc = ((SemElem*)le->data)->waitingCounter;

	if (wc != 0 && le->next)
		((SemElem*)le->next->data)->waitingCounter += wc;

	timeBlock->remove(le);
}


// Odblokiraj niti koju su blokirane maxTimeToWait; metodu poziva tajmer
void Kernel::semaphoreTick() {
	if (inSemaphoreTick || timeBlock->empty()) return;
	inSemaphoreTick = true;

	bool oldFlag = lockFlag;
	lockFlag = true;

	__iunlock__;

	((SemElem*)timeBlock->first()->data)->waitingCounter--;

	while (((SemElem*)timeBlock->first()->data)->waitingCounter == 0) {
		SemElem* we = (SemElem*)timeBlock->removeFirst();

		Scheduler::put(we->waitingPCB);
		we->waitingPCB->setStatus(READY);
		we->retVal = 0;
		we->kersem->val++;
		we->kersem->blocked->remove(we->defaultElement);
	}
	
	__ilock__;
	inSemaphoreTick = false;
	lockFlag = oldFlag;
}


PCB* Kernel::iddlePCB() {
	return iddleThread->myPCB;
}


void Iddle::run() {
	while (true);
}