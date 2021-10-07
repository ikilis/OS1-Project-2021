#ifndef _KSEM_H_
#define _KSEM_H_

#include "semaphor.h"
#include "wl.h"

class KernelSem{

public:
	WList blocked;
	long  val;
	int ret;

	KernelSem(int);
	~KernelSem();
	void kWait(Time);
	void kSignal();
};
#endif
