#include <dos.h>
#include "const.h"
#include "timer.h"
#include "schedule.h"
#include "pcb.h"
#include "thread.h"
#include "kernel.h"

static REG tsp;
static REG tss;
static REG tbp;
static PCB *newPCB;

static volatile unsigned int *stackToDelete = nullptr;
static volatile unsigned int timeQuant = 2;
static volatile bool indefiniteTime = false;
volatile bool lockFlag = false;
volatile bool interruptRequested = false;

interruptRoutine Timer::oldTimer = nullptr;
bool Timer::routineSet = false;



void interrupt timer(...) {
    
    if (!interruptRequested) {
        asm int 60h
        tick();
        Kernel::semaphoreTick();
        timeQuant--;
    }

    if ((timeQuant <= 0 && !indefiniteTime) || interruptRequested) {
        
        if (!lockFlag) {
            interruptRequested = false;
            indefiniteTime = false;
            stackToDelete = nullptr;

            // Provera da li je nit obrisana sa delete ili SIGKILL
            if (PCB::running->getStatus() != DELETED) {
                asm {
   	                mov tsp, sp
  	                mov tss, ss
   	                mov tbp, bp
  	            }

  	            PCB::running->context->sp = tsp;
  	            PCB::running->context->ss = tss;
  	            PCB::running->context->bp = tbp;
            } else stackToDelete = PCB::running->stack;


            // Ako je nit zavrsila, obradi signale odmah zbog signala 1
           	if (PCB::running->getStatus() == READY)
           		Scheduler::put(PCB::running);    
           	else if (PCB::running->getStatus() == FINISHED)
           		PCB::running->processSignals();


           	newPCB = Scheduler::get();


            if (newPCB == nullptr)
            	newPCB = Kernel::iddlePCB();


            tsp = newPCB->context->sp;
            tss = newPCB->context->ss;
            tbp = newPCB->context->bp;

            timeQuant = newPCB->context->time;
            indefiniteTime = (newPCB->context->time == 0 ? true : false);

            asm {
                mov sp, tsp
                mov ss, tss
                mov bp, tbp
            }

            delete stackToDelete;
            
            PCB::running = newPCB;
            PCB::running->processSignals();

        }
        else interruptRequested = true;

    }
}


void Timer::init() {
    if (routineSet) return;

    __ilock__

	oldTimer = getvect(MAIN_TIMER_ENTRY);

    setvect(AUX_TIMER_ENTRY,  oldTimer);
    setvect(MAIN_TIMER_ENTRY, timer);

    routineSet = true;
    __iunlock__
}


void Timer::restore() {
    if (!routineSet) return;

    __ilock__

    setvect(MAIN_TIMER_ENTRY, oldTimer);
    routineSet = false;

    __iunlock__
}
