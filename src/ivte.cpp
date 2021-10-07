#include "event.h"
#include "dos.h"
#include "PCB.h"
#include "kevent.h"

IVTEntry* IVTEntry::IVT[256]={0,};

IVTEntry::IVTEntry(IVTNo iNo,pInterrupt newRoutine){
	asm{
		pushf
		cli
	}
	this->ivtNo=iNo;
	if(ivtNo>=0 && ivtNo<256){
		if(IVT[iNo]==0){
			IVT[iNo]=this;

			oldRoutine=getvect(iNo);
			setvect(ivtNo,newRoutine);
		}
	}
	asm popf
}
IVTEntry::~IVTEntry(){
	asm{
		pushf
		cli
	}

	setvect(this->ivtNo,oldRoutine);

	if(PCB::_ID>3)
			callold();

	IVT[this->ivtNo]=0;


	asm popf
}
void::IVTEntry::callold(){
	oldRoutine();
}

void IVTEntry::signal(){

	if(KernelEv::events[this->ivtNo]!=0)
			KernelEv::events[this->ivtNo]->signal();

}









