#include "kersem.h"
#include "const.h"
#include "semaphor.h"
#include "kernel.h"
#include "schedule.h"
#include "pcb.h"


KernelSem::KernelSem(int init) : val(init), blocked(new Queue) {}


KernelSem::~KernelSem() {
	signal(MAX_INT);

	delete blocked;
	blocked = nullptr;
}

int KernelSem::wait(Time maxTimeToWait) {
	__ilock__;

	SemElem elem(PCB::running, maxTimeToWait, this);

	if (val-- <= 0) {

		elem.defaultElement = blocked->put(&elem);
		PCB::running->setStatus(BLOCKED);

		if (maxTimeToWait > 0)
			Kernel::registerTimeBlock(&elem);

		dispatch();
	}

	__iunlock__;
	return elem.retVal;
}

int KernelSem::signal(int n) {
	if (n < 0)
		return n;

	int nn = n;
	int unblocked = 0;
	SemElem *we = nullptr;

	if (n == 0) n = 1;

	__ilock__;
	val += n;

	while (n-- > 0) {
		we = (SemElem*)blocked->get();
		if (!we) break;

		we->waitingPCB->setStatus(READY);
		Scheduler::put(we->waitingPCB);

		unblocked++;
	}

	__iunlock__;
	return (nn == 0) ? 0 : unblocked;


}


int KernelSem::value() const {
	return val;
}
