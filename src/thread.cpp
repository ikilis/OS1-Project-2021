#include "thread.h"
#include "pcb.h"
#include "SCHEDULE.h"
#include "semaphor.h"

Thread::Thread(StackSize stackSize, Time timeSlice){
	LOCK
	myPCB=new PCB(stackSize,timeSlice,this);
	UNLOCK
}
Thread::~Thread(){
	LOCK
	delete myPCB;
	UNLOCK
}
void Thread::start(){
	LOCK
	myPCB->myStatus=RUNNABLE;
	Scheduler::put(myPCB);
	UNLOCK
}
void Thread::waitToComplete(){
	if(!this)return;
	LOCK
	if((PCB*)Running==this){UNLOCK return;}			//can't wait myself
	if(this->myPCB->myStatus==FINISHED){UNLOCK return;}	//don't wait for one who is already finished
	if(this->myPCB->myStatus==IDLE){UNLOCK return;}
	this->myPCB->waitingMe.insert((PCB*)Running);
	Running->myStatus=BLOCKED;
	UNLOCK
	dispatch();
}
ID Thread::getId(){return myPCB->myId;}
ID Thread::getRunningId(){
	return Running->myId;
}
Thread* Thread::getThreadById(ID id){
	LOCK
	Thread * tmp=Lista.get(id)->myThread;
	UNLOCK
	return tmp;
}

Thread* Thread::clone()const{
	return new Thread();
}


Thread* childThread=0;
ID Thread::fork(){
	LOCK
	childThread=new Thread();
	if(!childThread){
		UNLOCK
		return -1;
	}
	Running->numberOfChildren++;
	ID childId=childThread->myPCB->myId;

	((PCB*)Running)->myChildren.insert(childThread->myPCB);

	PCB::fork();


	if(childId<0){
		return-1;
	}
	if(Running->myId!=childId){
		Running->myChildren.insert(childThread->myPCB);
	}

	UNLOCK
	if(Running->myId!=childId)return childId;
	return 0;

}

void Thread::exit(){
	LOCK
	if(Running->myStatus==FINISHED)return;

	PCB::finish();
	UNLOCK
	dispatch();
}

void Thread::waitForForkChildren(){

	if(Running->numberOfChildren)
		((PCB*)Running)->myChildren.waitAll();

}

void dispatch(){
	asm{
		pushf
		cli
	}
	context_switch_demand=1;
	timer();
	asm popf
}






