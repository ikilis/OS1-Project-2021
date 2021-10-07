#ifndef _THREAD_H
#define _THREAD_H

typedef unsigned long StackSize;
const StackSize defaultStackSize=4096;
typedef unsigned int Time;
const Time defaultTimeSlice = 2;
typedef int ID;

class PCB;

class Thread{
public:
	void start();
	void waitToComplete();
	virtual ~Thread();
	ID getId();
	static ID getRunningId();
	static Thread* getThreadById(ID id);

	friend class Idle;

	static ID fork();
	static void exit();
	static void waitForForkChildren();
	virtual Thread* clone()const;

protected:
	friend class PCB;
	Thread(StackSize stackSize=defaultStackSize, Time timeSlice=defaultTimeSlice);
	virtual void run(){}

private://ugodi ovo posle
	PCB *myPCB;
};
void dispatch();
#endif

