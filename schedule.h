#ifndef __SCHEDULE_H__
#define __SCHEDULE_H__

class PCB;

class Scheduler {
public:
    static void put(PCB *);
    static PCB* get();
};

#endif
