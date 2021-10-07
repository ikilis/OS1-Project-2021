#ifndef _KEVENT_H_
#define _KEVENT_H_

#include "event.h"
class PCB;


class KernelEv{
public:
	int val;
	IVTNo ivtNo;
	PCB*owner;
	Event *myEvent;
	short blocked;
	static KernelEv *events[256];

	int isBlocked;


	KernelEv(IVTNo,Event*);
	~KernelEv();

	void signal();
	void wait();
};

#endif
