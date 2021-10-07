#include "semlist.h"
#include "schedule.h"
#include "ksem.h"
#include "wl.h"

Semlist::~Semlist()
{
	LOCK
	while(head){
		Clan *tmp=head;
		head=head->next;
		delete tmp;
		syncPrintf("Brisem semafore \n");
	}
	UNLOCK
}
void Semlist::insert(KernelSem* ks)
{
	LOCK
	Clan *tmp=new Clan(ks);

	tmp->next=head;
	head=tmp;
	UNLOCK`
}
void Semlist::remove(KernelSem* ks)
{
	LOCK
	Clan * last = 0;

	Clan *i=head;
	while(i){
		if( i->data == ks ) {

			if( !last )
				head = head->next;
			else last->next = i->next;

			delete i;

			UNLOCK
			return ;
		}

		last = i;
		i=i->next;
	}
	UNLOCK
}
void Semlist::unblockTime(){
	LOCK
	Clan *tmp=head;
	while(tmp){
		tmp->data->blocked.unblockTime();		
		tmp=tmp->next;
	}
	UNLOCK
}



