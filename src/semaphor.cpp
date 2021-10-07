#include "semaphor.h"
#include "ksem.h"
#include "semlist.h"

Semaphore::Semaphore(int init)
{
	LOCK
	this->myImpl=new KernelSem(init);
	UNLOCK
}
Semaphore::~Semaphore()
{
	LOCK
	delete myImpl;
	UNLOCK
}
int Semaphore::wait(Time maxTimeToWait)
{

	this->myImpl->kWait(maxTimeToWait);
	return myImpl->ret;
}
void Semaphore::signal()
{
	LOCK
	myImpl->kSignal();
	UNLOCK;
}
int Semaphore::val() const{
	return myImpl->val;
}
