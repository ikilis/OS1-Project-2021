#include "wl.h"
#include "pcb.h"
#include "SCHEDULE.h"
#include "ksem.h"

WList::WList(KernelSem *cao){
	LOCK
	head=0;
	tail=0;
	owner=cao;
	UNLOCK
}
WList::~WList(){
	LOCK
	while(head){
		Elem *tmp=head;
		head=head->next;
		delete tmp;
	}
	head=tail=0;
	UNLOCK
}
void WList::unblock(){

	LOCK
	if(!head){
		UNLOCK
		return;
	}
	PCB *t=head->data;
	Elem *tmp=head;
	head=head->next;
	if(!head)tail=0;

	if ( tmp->next && tmp->next->forever == 0)
		tmp->next->time += tmp->time;


	t->myStatus=RUNNABLE;
	Scheduler::put(t);
	this->owner->ret=1;

	delete tmp;
	UNLOCK

}
void WList::unblockTime(){
	LOCK;

	if (!head || head->forever) {
		UNLOCK
		return;
	}
	head->time--;
	if(head->time){
		UNLOCK
		return;
	}
	while (head && head->time == 0 && head->forever==0) {
		Elem* tmp = head;
		head = head->next;

		Scheduler::put(tmp->data);
	}
		this->owner->ret=0;
		if(this->owner->val<0)
			this->owner->val++;
		delete tmp;
	}
	if (!head)tail = 0;
	UNLOCK
}
void WList::insert(PCB*pcb,Time t){
	LOCK
	if(!head){
		if (t){
			head = new Elem(pcb, t);
		}
		else head = new Elem(pcb, t,1);
		tail=head;
	}
	else{
		if (t == 0) {
			tail->next = new Elem(pcb, t, 1);
			tail = tail->next;

		}
		else {
			if (!head) {//ubaci na pocetak i kraj
				head = new Elem(pcb, t);
				tail = head;
			}
			else {
				Elem* tmp = head, * prev = 0;
				short ubacen = 0;
				while (tmp) {
					if (tmp->time > t || tmp->forever) {//stigli smo do duzeg ili onog koji ceka zauvek
						ubacen = 1;
						if (prev) {							// ubaci negde na sredinu
							prev->next = new Elem(pcb, t);
							prev->next->next = tmp;
							if (t) {
								if (tmp->time> 0) {
									tmp->time-=t;
								}
							}

						}
						else {								//ubaci na pocetak
							head = new Elem(pcb, t);
							head->next = tmp;
							if (t)
								if (tmp->time > 0)tmp->time-=t;
						}

						break;
					}

					else {
						t -= tmp->time;
						prev = tmp;
						tmp = tmp->next;
					}


				}

				if (!ubacen) {
					tail->next = new Elem(pcb, t);
					tail = tail->next;
				}
			}
		}
	}
	UNLOCK
}

void WList::deleteID(ID id){

	LOCK
	Elem *tmp=head;
	Elem*prev=0;

	while(tmp){
		if(tmp->data->myId==id)	break;
		prev=tmp;
		tmp=tmp->next;
	}
	if ( tmp->next && tmp->next->forever == 0)
		tmp->next->time += tmp->time;

	if(!tmp){
		UNLOCK
		return;
	}

	if(this->owner->val<0)	this->owner->val++;	
	tmp->data->myStatus=FINISHED;

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

