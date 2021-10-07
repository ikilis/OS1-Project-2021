#include "pcb.h"
#include <DOS.h>
#include <string.h>
#include "Semaphor.h"


int PCB::_ID=0;

PCB::PCB(StackSize sz, Time t, Thread *thr){
	this->myId=_ID++;
	this->myStackSize=sz;
	this->myTimeSlice=t;
	this->myThread=thr;

	this->numberOfChildren=0;
	this->myParent=0;

	if(sz>0 && sz<65536){
		this->myStackSize=sz/sizeof(unsigned);
		this->stack=new unsigned[myStackSize];
		stack[myStackSize-1]=0x200;

		stack[myStackSize-2]=FP_SEG(&(PCB::wrapper));
		stack[myStackSize-3]=FP_OFF(&(PCB::wrapper));

		//pocetni BP
		stack[myStackSize-12]=0;

		this->ss=FP_SEG(stack + myStackSize - 12);
		this->sp = FP_OFF(stack + myStackSize - 12);

		this->bp=this->sp;
	}
	else stack=0;
	this->myStatus=INIT;
}

PCB::~PCB(){
	LOCK
	delete [] stack;
	UNLOCK
}
void PCB::wrapper(){
//	syncPrintf("U omotacu sam od niti id=%d \n",Running->myId);

	Running->myThread->run();
	finish();
}

void PCB::finish(){
	LOCK

	//oslobodi iz waitToComplete()
	(Running->waitingMe).notifyAll();
	Running->myStatus=FINISHED;

	UNLOCK
	dispatch();
}

extern Thread * childThread;
static unsigned tbp, tss, childOff, parOff, parBP, childBP;

void interrupt PCB::fork() {

	delete[] childThread->myPCB->stack; // Da bi se napravio stack prave velicine, a ne default velicine
	childThread->myPCB->myStackSize = Running->myStackSize;
	childThread->myPCB->stack = new unsigned[Running->myStackSize];

	childThread->myPCB->myTimeSlice = Running->myTimeSlice;

	childThread->myPCB->myParent =(PCB*) Running;



	memcpy(childThread->myPCB->stack, Running->stack, Running->myStackSize * sizeof(unsigned));

	asm {
		mov tbp, bp
		mov tss, ss
	}

	childThread->myPCB->ss = FP_SEG(childThread->myPCB->stack);
	parOff = FP_OFF(Running->stack);
	childOff = FP_OFF(childThread->myPCB->stack);
	childThread->myPCB->bp = childThread->myPCB->sp = tbp - parOff + childOff;

	parOff = tbp;
	childOff = childThread->myPCB->bp;

	while(1) {

		parBP = *((unsigned *) MK_FP(tss, parOff));
		childBP = *((unsigned *) MK_FP(childThread->myPCB->ss, childOff));

		if( parBP == 0 )	break;

		childBP = parBP - parOff + childOff;

		parOff = parBP;
		childOff= childBP;
	}childBP=0;

	childThread->start();
}





