#ifndef __IVTENTRY_H__
#define __IVTENTRY_H__

class KernelEv;
#include "const.h"
#include "pcb.h"

#define PREPAREENTRY(IVTN,CALL_OLD) \
	extern bool inSemaphoreTick; \
	extern bool processingSignals; \
	void interrupt interruptHandler##IVTN(...); \
	IVTEntry* ivt ##IVTN = new IVTEntry(IVTN, interruptHandler##IVTN, CALL_OLD); \
	void interrupt interruptHandler##IVTN(...) { \
		ivt##IVTN->signal(); \
		if (!processingSignals && !inSemaphoreTick) { \
			interruptRequested = true; \
			timer(); \
		} \
	}


class IVTEntry {
public:
	IVTEntry(int, interruptRoutine, bool);

	static void initEntry(int, KernelEv*);
	static void restoreEntry(int);

	void signal();

private:
	friend class Kernel;
	static IVTEntry* entries[IVTCNT];

	interruptRoutine oldRoutine;
	interruptRoutine newRoutine;

	KernelEv* myEvent;
	int IVTEntryNumber;
	bool callOld;
};

#endif
