#include "kernelev.h"
#include "const.h"
#include "pcb.h"
#include "ivtentry.h"
#include "schedule.h"


KernelEv::KernelEv(IVTNo ivtNo) : myPCB(PCB::running), IVTNumber(ivtNo), blocked(false) {
	IVTEntry::initEntry(IVTNumber, this);
}


KernelEv::~KernelEv() {
	if (blocked) {
		myPCB->setStatus(READY);
		Scheduler::put(myPCB);
	}

	IVTEntry::restoreEntry(IVTNumber);
}


void KernelEv::wait() {
	if (PCB::running != myPCB) return;

	__ilock__
	blocked = true;
	myPCB->setStatus(BLOCKED);
	dispatch();
}


// Zove se iz IVTEntry::signal koga zove prekidna rutina; prekidi maskirani
void KernelEv::signal() {
	if (!blocked) return;

	blocked = false;
	myPCB->setStatus(READY);
	Scheduler::put(myPCB);
}
