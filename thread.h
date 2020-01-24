#ifndef __THREAD_H__
#define __THREAD_H__

typedef unsigned long StackSize;
typedef unsigned int Time;                  // time, * 55ms
typedef int ID;
typedef void (*SignalHandler)();
typedef unsigned SignalId;

const StackSize defaultStackSize = 4096;
const Time defaultTimeSlice = 2;            // default = 2 * 55ms

class Kernel;
class PCB;
void dispatch();

class Thread {
public:
    void start();
    void waitToComplete();
    virtual ~Thread();

    ID getId();
    static ID getRunningId();
    static Thread* getThreadById(ID id);

    void signal(SignalId signal);
    void registerHandler(SignalId signal, SignalHandler handler);
    void unregisterAllHandlers(SignalId id);
    void swap(SignalId id, SignalHandler hand1, SignalHandler hand2);
    void blockSignal(SignalId signal);
    void unblockSignal(SignalId signal);
    static void blockSignalGlobally(SignalId signal);
    static void unblockSignalGlobally(SignalId signal);

protected:
    Thread (StackSize stackSize = defaultStackSize, Time timeSlice = defaultTimeSlice);
    
    friend class PCB;
    friend class Kernel;

    virtual void run() {}

private:
    PCB* myPCB;

};

#endif
