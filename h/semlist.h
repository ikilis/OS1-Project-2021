#ifndef _SEMLIST_H
#define _SEMLIST_H
#include "pcb.h"

class KernelSem;		

class Semlist{

	struct Clan{
		KernelSem *data;
		Clan *next;
		Clan(KernelSem* ks):data(ks),next(0){}
	};
	Clan *head;

public:
	Semlist(){head=0;}
	~Semlist();
	void insert(KernelSem*);
	void remove(KernelSem*);
	void unblockTime();		

};

extern Semlist SemaphoreList;

#endif
