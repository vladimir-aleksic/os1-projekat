#include "const.h"
#include "timer.h"
#include "thread.h"
#include "pcb.h"
#include "schedule.h"
#include "kernel.h"
#include "list.h"
#include "semaphor.h"

void Thread::start() {
	if (myPCB->getStatus() != INITIALIZED)
		return;

	Kernel::threadCreated(this);
	myPCB->setStatus(READY);

	__lock__
    Scheduler::put(myPCB);
	__unlock__
}


void Thread::waitToComplete() {
    // Cekanje na samu sebe
    if (PCB::running == this->myPCB)
    	return;

    // Cekanje na iddle nit
    if (myPCB == Kernel::iddlePCB())
        return;

    // Cekanje na zavrsenu nit
    if (myPCB->getStatus() == FINISHED || myPCB->getStatus() == DELETED || myPCB->getStatus() == INITIALIZED)
    	return;

    // Cekanje na main
    if (myPCB == Kernel::mainThread->myPCB)
    	return;

    myPCB->blockCallingThread();
}


Thread::~Thread() {
    waitToComplete();
	Kernel::threadDestroyed(this);
	
    __lock__
    delete myPCB;
    __unlock__

	myPCB = nullptr;
}


Thread::Thread(StackSize stackSize, Time timeSlice) {
	__lock__
    myPCB = new PCB(this, stackSize, timeSlice);
	registerHandler(0, &PCB::kill);
	__unlock__
}


ID Thread::getId() {
    return myPCB->getId();
}


ID Thread::getRunningId() {
    return PCB::running->getId();
}


Thread* Thread::getThreadById(ID id) {
    return Kernel::getThreadById(id);
}


void dispatch() {
	__ilock__;
	lockFlag = false;
	interruptRequested = true;
	timer();
	__iunlock__;
}


void Thread::signal(SignalId signal) {
	__lock__
	if (!myPCB->signalHandlers[signal]->empty())
		myPCB->receivedSignals->toEnd((void*)signal);
	__unlock__
}


void Thread::registerHandler(SignalId signal, SignalHandler handler) {
	List *handlersList;
	
	__lock__;
	handlersList = myPCB->signalHandlers[signal];

	// Handler za PCB::kill je uvek na poslednjem mestu
	if (signal == 0 && !handlersList->empty()) {
		handlersList->insertBefore(handler, handlersList->last());
	}
	else {
		handlersList->toEnd(handler);
	}
	__unlock__;
}


void Thread::unregisterAllHandlers(SignalId id) {
	__lock__
	delete myPCB->signalHandlers[id];

	myPCB->signalHandlers[id] = new List;
	if (id == 0) registerHandler(0, &PCB::kill);
	__unlock__
}


void Thread::swap(SignalId id, SignalHandler hand1, SignalHandler hand2) {
	// Handler za PCB::kill je uvek na poslednjem mestu
	if (id == 0 && (hand1 == &PCB::kill || hand2 == &PCB::kill)) return;

	List::ListElement *e1 = nullptr, *e2 = nullptr;
	__lock__;

	for (List::ListElement* e = myPCB->signalHandlers[id]->first(); e; e = e->next) {
		if ((SignalHandler)e->data == hand1) e1 = e;
		if ((SignalHandler)e->data == hand2) e2 = e;
	}

	if (e1 != nullptr && e2 != nullptr) {
		SignalHandler temp = (SignalHandler)e1->data;
		e1->data = e2->data;
		e2->data = temp;
	}

	__unlock__;
}


void Thread::blockSignal(SignalId signal) {
	if (signal == 0) return;
	
	myPCB->blockedSignals[signal] = true;
}


void Thread::blockSignalGlobally(SignalId signal) {
	if (signal == 0) return;

	PCB::globallyBlockedSignals[signal] = true;
}


void Thread::unblockSignal(SignalId signal) {
	myPCB->blockedSignals[signal] = false;
}


void Thread::unblockSignalGlobally(SignalId signal) {
	PCB::globallyBlockedSignals[signal] = false;
}
