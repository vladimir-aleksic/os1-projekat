#ifndef __KERSEM_H__
#define __KERSEM_H__

class KernelSem;
#include "const.h"
#include "queue.h"
#include "thread.h"


struct SemElem {
    SemElem(PCB* p, Time t, KernelSem* ks):
        defaultElement(nullptr),
        waitingElement(nullptr),
        kersem(ks),
        retVal(1),
        waitingPCB(p),
        waitingCounter(t) {}

    PCB* waitingPCB;
    Time waitingCounter;
    KernelSem* kersem;
    List::ListElement* defaultElement;
    List::ListElement* waitingElement;
    int retVal;
};


class KernelSem {
public:
    KernelSem(int);
    ~KernelSem();

    int wait(Time maxTimeToWait);
    int signal(int n);

    int value() const;

private:
    friend class Kernel;
    int val;
    Queue* blocked;
};

#endif
