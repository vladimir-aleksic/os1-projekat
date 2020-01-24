#ifndef __CONST_H__
#define __CONST_H__

typedef unsigned int bool;
typedef volatile unsigned REG;
typedef void interrupt (*interruptRoutine)(...);

#define nullptr 0
#define true 1
#define false 0
#define MAIN_TIMER_ENTRY 0x08
#define AUX_TIMER_ENTRY 0x60
#define SIGCNT 16
#define IVTCNT 256
#define MAX_INT -1U>>1

#define __ilock__ asm cli
#define __iunlock__ asm sti
#define __lock__ lockFlag = true;
#define __unlock__ lockFlag = false; if (interruptRequested) dispatch();

extern volatile bool lockFlag;
extern volatile bool interruptRequested;

#endif
