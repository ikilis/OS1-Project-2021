#ifndef _PCB_H
#define _PCB_H


#include <DOS.H>
#include <STDIO.H>
#include <STDARG.H>

//thread safe print function
void syncPrintf(const char *format, ...);

void interrupt timer(...);
void inic();
void restore();

class Semaphore;

#include "thread.h"
#include "PCBList.h"

extern volatile int lockFlag;
extern volatile int context_switch_demand;

#define LOCK lockFlag++;
#define UNLOCK lockFlag --;\
	if(context_switch_demand && lockFlag==0) dispatch();

typedef enum{ INIT,IDLE,RUNNABLE,BLOCKED,FINISHED}Status;

class PCB{

public:
	static int _ID;
	ID myId;
	Status myStatus;
	Thread* myThread;

	unsigned ss,sp,bp;

	unsigned *stack;
	StackSize myStackSize;
	Time myTimeSlice;

	PCBList waitingMe;

	PCB(StackSize,Time,Thread*);
	~PCB();

	static void wrapper();
	static void finish();

// usefull for fork
	int numberOfChildren;
	PCBList myChildren;
	PCB* myParent;

	static void interrupt fork();

};

extern volatile PCB* Running;

#endif
