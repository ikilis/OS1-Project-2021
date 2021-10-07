#ifndef _event_h_
#define _event_h_

typedef unsigned char IVTNo;

class KernelEv;				//spoznaje klasu KernelEv

class Event {
public:
 Event (IVTNo ivtNo);
 ~Event ();
 void wait ();

protected:
 friend class KernelEv;
 void signal(); // can call KernelEv

private:
 KernelEv* myImpl;
};


typedef void interrupt(*pInterrupt)(...);

class IVTEntry {
public:

	IVTEntry(IVTNo ivtNo, pInterrupt newRoutine);
	~IVTEntry();

	void signal();
	void callold();

	static IVTEntry* IVT[256];
	static IVTEntry* getEntry(IVTNo i);

private:
	pInterrupt oldRoutine;
	IVTNo ivtNo;
};

#define PREPAREENTRY(i,callOld)\
	void interrupt inter##i(...);\
	IVTEntry e##i = IVTEntry(i,inter##i);\
	void interrupt inter##i(...){\
		e##i.signal();\
		if (callOld == 1)\
			e##i.callold();\
	}

#endif
