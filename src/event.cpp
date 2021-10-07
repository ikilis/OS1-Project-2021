#include "event.h"
#include "pcb.h"
#include "kevent.h"

Event::Event(IVTNo ivtNo){
	LOCK
	myImpl=new KernelEv(ivtNo,this);
	UNLOCK
}
Event::~Event(){
	LOCK
	delete myImpl;
	UNLOCK
}

void Event::signal(){
	LOCK
	myImpl->signal();
	UNLOCK
}
void Event::wait(){
	myImpl->wait();
}
