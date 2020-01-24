#ifndef __KERNELEV_H__
#define __KENRELEV_H__

class PCB;
#include "event.h"
#include "const.h"

class KernelEv {
public:
	KernelEv(IVTNo ivtn);
	~KernelEv();

	void wait();
	void signal();

private:
	PCB* myPCB;
	IVTNo IVTNumber;
	bool blocked;
};

#endif
