#ifndef _WL_H
#define _WL_H

#include "pcb.h"

class KernelSem;
class WList{
	struct Elem{
		PCB*data;
		Time time;
		Elem*next;
		short forever;

		Elem(PCB* pcb, unsigned t, short f = 0) {
			data=pcb;time=t;
			next=0;
			forever = f;
		}
	};
	Elem* head,*tail;


public:
	KernelSem *owner;

	WList(KernelSem*);
	~WList();
	void unblock();
	void unblockTime();
	void insert(PCB*,Time);
	void deleteID(ID);
	int isEmpty(){return head==0;}
};
#endif
