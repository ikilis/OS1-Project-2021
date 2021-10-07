#include "thread.h"
#include "pcb.h"
#include <iostream.h>
#include <DOS.h>
#include "SCHEDULE.h"
#include "user.h"
#include "semaphor.h"
#include "wl.h"
#include "ksem.h"
#include "semlist.h"

void syncPrintf(const char *format, ...)	//thread safe print function
{
	int res;
	va_list args;
	asm{
		pushf
		cli
	}
	va_start(args, format);
	res = vprintf(format, args);
	va_end(args);
	asm popf

}


volatile int lockFlag=0;
volatile int context_switch_demand=0;
volatile PCB* Running=0;
volatile PCBList Lista;
volatile int counter=0;
volatile unsigned int tbp,tss,tsp;
Semlist SemaphoreList;

extern void tick();

typedef void interrupt(*pInterrupt)(...);
pInterrupt oldTimer;

class Idle : public Thread {
public:
	Idle() : Thread(256, 1) {
		myPCB->myStatus=IDLE;
		pcb=myPCB;
	}

	void run() {
		while (1);
	}
	PCB *pcb;

};
Idle *idle=0;


void interrupt timer(...){


	if(!context_switch_demand){
		tick();	
		SemaphoreList.unblockTime();			//updating waiting time in semaphore lists

		counter--;
		oldTimer();
	}

	if(counter==0 || context_switch_demand==1){//promena konteksta

		if(lockFlag==0){

			context_switch_demand=0;

			asm {
					mov tsp, sp
					mov tss, ss
					mov tbp, bp
			}

			Running->ss = tss;
			Running->sp = tsp;
			Running->bp = tbp;



			if(Running->myStatus==RUNNABLE && Running!=idle->pcb )
					Scheduler::put((PCB*)Running);


			Running=Scheduler::get();

			if(Running==0 ){
				Running=idle->pcb;
			}
			tsp = Running->sp;
			tss = Running->ss;
			tbp = Running->bp;

			asm{
					mov sp, tsp
					mov ss, tss
					mov bp, tbp
			}

			counter=Running->myTimeSlice;
		}
		else						
			context_switch_demand=1;		//if we were interupted in critical section we need to remmember it

	}


}


//preparing ivt with our "new" interrupt routine
void inic() {

	asm{
		pushf
		cli
	}
	oldTimer = getvect(8);
	setvect(8, timer);
	setvect(60,oldTimer);
	asm popf
}

void restore() {

	setvect(8, oldTimer);
	delete idle;

}

extern int userMain(int argc, char* argv[]);

int value;

class Main : public Thread {
	int argc;
	char **argv;
public:

	Main(int argc, char **argv) : Thread(defaultStackSize, defaultTimeSlice), argc(argc), argv(argv) {}

	void run() { value = userMain(argc, argv); }

	~Main() { waitToComplete();  }
};


int main(int argc, char *argv[]) {

	inic();

	idle= new Idle();
	((Thread*)idle)->start();

	Running = new PCB(0 ,0 , 0);


	Main *mainThread = new Main(argc, argv);
	mainThread->start();

	delete mainThread;


	restore();
	return value;
}

