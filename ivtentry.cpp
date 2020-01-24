#include <dos.h>
#include "ivtentry.h"
#include "const.h"
#include "kernelev.h"

IVTEntry* IVTEntry::entries[IVTCNT] = { nullptr };


IVTEntry::IVTEntry(int IVTNum, interruptRoutine ir, bool co) : IVTEntryNumber(IVTNum), newRoutine(ir), myEvent(nullptr), callOld(co) {
	oldRoutine = getvect(IVTNum);
	entries[IVTNum] = this;
}


void IVTEntry::initEntry(int num, KernelEv* kv) {
	if (!entries[num]) return;

	__ilock__;
	entries[num]->myEvent = kv;
	setvect(num, entries[num]->newRoutine);
	__iunlock__;
}


void IVTEntry::restoreEntry(int num) {
	if (!entries[num]) return;

	__ilock__;
	entries[num]->myEvent = nullptr;
	setvect(num, entries[num]->oldRoutine);
	__iunlock__;
}


void IVTEntry::signal() {
	if (!myEvent) return;
	
	if (callOld)
		(*oldRoutine)();
		
	myEvent->signal();
}
