#include "ksem.h"
#include "semaphor.h"
#include "semlist.h"

KernelSem::KernelSem(int v):blocked(this)
{
	LOCK
	this->val=v;
	if(v)	this->ret=1;
	else	this->ret=0;
	SemaphoreList.insert(this);
	UNLOCK
}
KernelSem::~KernelSem()
{
	LOCK
	SemaphoreList.remove(this);
	UNLOCK
}
void KernelSem::kWait(Time maxTimeToWait){
	LOCK

	if(--val<0){
		Running->myStatus=BLOCKED;

		blocked.insert((PCB*)Running,maxTimeToWait);
		ret=0;

		UNLOCK
		dispatch();
	}
	else{
		ret=1;						
		UNLOCK
	}
}
void KernelSem::kSignal()
{

	if(val++<0){
		blocked.unblock();

	}

}

