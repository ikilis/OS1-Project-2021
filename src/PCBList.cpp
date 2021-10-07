#include "PCBList.h"
#include "pcb.h"
#include "SCHEDULE.h"


PCBList::PCBList(){head=0;tail=0;broj=0;}
PCBList::~PCBList(){
	LOCK
	while(head){
		broj--;
		Elem *tmp=head;
		head=head->next;
		delete tmp;
	}
	UNLOCK
}
void PCBList::insert(PCB*pcb){
	LOCK
	this->broj++;
	if(head){
		tail->next=new Elem(pcb);
		tail=tail->next;
	}
	else{
		head=new Elem(pcb);
		tail=head;
	}
	tail->next=0;
	UNLOCK
}
void PCBList::deleteID(ID id){
	LOCK
	Elem *tmp=head;
	Elem*prev=0;
	while(tmp){
		if(tmp->data->myId==id)	break;
		prev=tmp;
		tmp=tmp->next;
	}
	if(!tmp){
		UNLOCK
		return;
	}
	broj--;
	if(prev){
		prev->next=tmp->next;
		if(tmp==tail)tail=prev;
		delete tmp;
	}
	else{
		head=tmp->next;
		if(tmp==tail) tail=0;	// ako je ujedno i jedini element
		delete tmp;
	}

	UNLOCK
}
PCB* PCBList::get(ID id){

	LOCK
	Elem *tmp=head;
	while(tmp){
		if(tmp->data->myId==id){
	
			break;
		}
		tmp=tmp->next;
	}
	if(tmp){
		UNLOCK
		return tmp->data;
	}
	UNLOCK
	return 0;

}
void PCBList::notifyAll(){
	LOCK
	Elem *tmp=head;
	while(tmp){
		tmp->data->myStatus=RUNNABLE;
		Scheduler::put(tmp->data);
		tmp=tmp->next;
	}
	UNLOCK

}
void PCBList::waitAll(){

	Elem *tmp=head;
	while(tmp){
		tmp->data->myThread->waitToComplete();
		tmp=tmp->next;
	}
}


