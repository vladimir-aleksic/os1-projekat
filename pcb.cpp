#include <dos.h>
#include "const.h"
#include "thread.h"
#include "pcb.h"
#include "queue.h"
#include "schedule.h"
#include "kernel.h"

bool PCB::globallyBlockedSignals[SIGCNT] = { false };
bool processingSignals = false;
ID PCB::uidGenerator = 0;
PCB* PCB::running = nullptr;


PCB::PCB(Thread *mt, StackSize sz, Time ts):
	myThread(mt),
	stackSize(sz),
	parent(PCB::running),
	context(new PCBContext),
	uid(uidGenerator++)
{
	stack = new unsigned int[sz];
	blockedThreads = new Queue;

	REG PSW = 0x200;
	REG IP = FP_OFF(&PCB::wrapper);
	REG CS = FP_SEG(&PCB::wrapper);

	stack[sz-1] = PSW;
	stack[sz-2] = CS;
	stack[sz-3] = IP;

	context->sp = FP_OFF(stack + sz - 12);
	context->bp = FP_OFF(stack + sz - 12);
	context->ss = FP_SEG(stack + sz - 12);
	context->time = ts;

	signalHandlers = new List*[SIGCNT];
	receivedSignals = new List;
	blockedSignals = new bool[SIGCNT];


	for (int i = 0; i < SIGCNT; i++) {
		signalHandlers[i] = new List;
		blockedSignals[i] = parent ? parent->blockedSignals[i] : false;
		
		if (!parent) continue;

		for (List::ListElement* le = parent->signalHandlers[i]->first(); le; le = le->next) {
			// Handler za kill registruje metoda Thread::Thread()
			if ((SignalHandler)le->data == &PCB::kill) continue;
			
			signalHandlers[i]->toEnd(le->data);
		}
	}

	setStatus(INITIALIZED);
}


PCB::~PCB() {
	if (getStatus() != DELETED){
		unblockBlockedThreads();
		releaseResources();
		setStatus(DELETED);
	}
}


void PCB::wrapper() {
	PCB::running->myThread->run();

	if (PCB::running->parent)
		PCB::running->parent->myThread->signal(1);

	PCB::running->myThread->signal(2);

	__lock__
	PCB::running->setStatus(FINISHED);
	PCB::running->unblockBlockedThreads();
	
	dispatch();
}


void PCB::blockCallingThread() {
	__ilock__;
	blockedThreads->put(PCB::running);
	PCB::running->setStatus(BLOCKED);
	dispatch();
}


void PCB::unblockBlockedThreads() {
	PCB* temp = (PCB*)blockedThreads->get();
	while (temp) {
		temp->setStatus(READY);
		Scheduler::put(temp);
		temp = (PCB*)blockedThreads->get();
	}
}


void PCB::processSignals() {
	if (receivedSignals->empty()) return;

	processingSignals = true;
	lockFlag = true;
	__iunlock__;

	List::ListElement* le = receivedSignals->first(), *next;

	while (le) {
		unsigned signal = (unsigned)le->data;
		next = le->next;

		if (PCB::globallyBlockedSignals[signal] == false && blockedSignals[signal] == false) {
			receivedSignals->remove(le);

			if (!(signal == 0 && getStatus() == FINISHED)) {
				for (List::ListElement* e = signalHandlers[signal]->first(); e; e = e->next) {
					((SignalHandler)e->data)();
				}
			}
		}

		le = next;
	}

	__ilock__;
	processingSignals = false;
	lockFlag = false;
}


void PCB::kill() {
	__ilock__;
	processingSignals = false;
	lockFlag = false;

	PCB::running->setStatus(DELETED);
	PCB::running->unblockBlockedThreads();
	PCB::running->releaseResources();

	dispatch();
}


void PCB::releaseResources() {
	delete blockedThreads;
	delete receivedSignals;

	for (int i = 0; i < SIGCNT; i++)
		delete signalHandlers[i];

	delete[] signalHandlers;
	delete[] blockedSignals;
	delete context;

	context = nullptr;
	signalHandlers = nullptr;
	receivedSignals = nullptr;
	blockedThreads = nullptr;
}


PCBStatus PCB::getStatus() const { return status; }
ID PCB::getId() const {	return uid; }
void PCB::setStatus(PCBStatus s) { status = s; }
