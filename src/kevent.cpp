#include "kevent.h"
#include "pcb.h"
#include "SCHEDULE.h"

KernelEv* KernelEv::events[256]={0,};

KernelEv::KernelEv(IVTNo num, Event *e){
	this->ivtNo=num;
	this->myEvent=e;
	this->val=0;
	this->blocked=0;					//tells us if it's Thread is actually waiting on binary Semaphor AKA Event
	this->owner=(PCB*)Running;
	if(num<256 && num>=0)	events[num]=this;

}
KernelEv::~KernelEv(){
	events[this->ivtNo]=0;
}
void KernelEv::wait(){
	if(((PCB*)Running)!=this->owner)	return;
	LOCK
	if(val==1){
		val=0;
		UNLOCK
	}
	else{
		this->blocked=1;
		Running->myStatus=BLOCKED;
		UNLOCK
		dispatch();
	}
}
void KernelEv::signal(){
	if(val==1)	return;

	if(blocked){
		owner->myStatus=RUNNABLE;
		Scheduler::put(this->owner);
	}else{
		val=1;
	}
	blocked=0;
}


