#ifndef _PCBLIST_H
#define _PCBLIST_H

#include "thread.h"

class PCB;

class PCBList{
	struct Elem{
		PCB *data;
		Elem *next;
		Elem(PCB* pcb){
			data=pcb;
			next=0;
		}
	};
	Elem *head;
	Elem *tail;

public:
	int broj;
	PCBList();
	~PCBList();
	void insert(PCB*);
	void deleteID(ID);
	void notifyAll();
	PCB* get(ID);

	void waitAll();
	int isEmpty(){
		return head==0;
	}

	PCB* first(){
		PCB* c=head->data;
		head=head->next;
		return c;
	}
};

extern volatile PCBList Lista;

#endif
