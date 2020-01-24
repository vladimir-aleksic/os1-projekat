#ifndef __PCB_H__
#define __PCB_H__

class Queue;
#include "const.h"
#include "thread.h"
#include "list.h"

struct PCBContext {
    REG sp;
    REG ss;
    REG bp;
    Time time;
};

enum PCBStatus {
    READY, BLOCKED, FINISHED, INITIALIZED, DELETED
};


class PCB {
public:
    PCB(Thread*, StackSize, Time);
    ~PCB();

    PCBStatus getStatus() const;
    void setStatus(PCBStatus);
    void blockCallingThread();
    void processSignals();
    ID getId() const;

    static void kill();
    static bool globallyBlockedSignals[SIGCNT];
    static PCB* running;

    List* receivedSignals;
    List** signalHandlers;
    bool* blockedSignals;
    PCB *const parent;

private:
    friend class Kernel;
    friend void interrupt timer(...);
    
    static ID uidGenerator;
    static void wrapper();

    void unblockBlockedThreads();
    void releaseResources();

    PCBStatus status;
    StackSize stackSize;
    PCBContext* context;
    unsigned int* stack;
    Queue* blockedThreads;
    Thread *const myThread;
    List::ListElement *listElement;
    ID uid;
};

#endif
