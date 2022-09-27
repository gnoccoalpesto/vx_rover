/*
 * simulation.c
 *
 *  Created on: May 14, 2018
 *  Last modified on: 23 Sep 2022 (Francesco Cerri)
 *      Author: Paolo Torroni. All rights reserved.
 */

#include "simulation.h"
#include <stdio.h>

/* Resource information */
SEM_ID Semaphore[N_SEMAPHORES];
int Ceiling[N_SEMAPHORES]=CEILINGS;

/* Semaphore for batch-mode simulation */
SEM_ID BatchSem;

/* Simulation parameters*/
int ResourceAccessProtocol = NPP;
int TotalSimulationTicks; /* total number of simulation ticks; defaults to hyperperiod */
WDOG_ID WdId; /* watch dog for terminating simulation after SimulationCounter ticks */
TASK_ID CleanUpId; /* clean up task upon simulation termination */
int Verbose;
int BatchMode;
int Phased;
/*AUX CLOCK ARTIFICIALLY LOWERED TO AVOID HOST INTERFERENCE
 * 	behaves consistently with this rate*/
int AuxClkRate = 20;

/* Simulation functions */
void TaskShell(int task) {
	FOREVER {
		taskSuspend(0);
		Instance[task]++;
		APPLICATION(task);
		ShowJobNumber(task);
	}
}

void StartSimulation(int sec) {
	static int WorkerCounter = 1;
	int semaphore, SemOptions=SEM_Q_PRIORITY;
	int task;
	char task_name[15][N_TASKS];
	
	/* initialize simulation parameters */
	for(task=0;task<N_TASKS;task++) {
		NotFinished[task]=FALSE;
		Overruns[task]=0;
		Instance[task]=0;
		ResponseTime[task]=0;
		JobReleaseCounter[task]=1+(Phased?Phase[task]:0);
	}

	/* initialize semaphores */
	if(ResourceAccessProtocol==PIP){
		SemOptions|=SEM_INVERSION_SAFE;
	}

	for(semaphore=0;semaphore<N_SEMAPHORES;semaphore++)
		Semaphore[semaphore]=semMCreate(SemOptions);
	for(task=0;task<N_TASKS;task++) {
		ResourceCount[task]=0;
		CurrentPriority[0][task]=TOP_PRIORITY+task;
	}

	/* set up watch-dog for termination management */
	WdId = wdCreate();
	if(!BatchMode) {
		if(!sec)
			TotalSimulationTicks=Hyperperiod()+Phase[0]+1;
		else
			TotalSimulationTicks = sec*sysAuxClkRateGet();
	}
	wdStart(WdId, TotalSimulationTicks, (FUNCPTR) WdHandler, TotalSimulationTicks);
	
	/* create tasks */
	MetaschedulerId = taskSpawn("tMetascheduler", 101, 0, 4000, (FUNCPTR) MetaschedulerShell, 0,0,0,0,0,0,0,0,0,0);
	for(task=0;task<N_TASKS;task++) {
		sprintf(task_name[task],"tWork-%d",WorkerCounter++);
		WorkerId[task] = taskSpawn(task_name[task], TOP_PRIORITY+task, 0, 4000, (FUNCPTR) TaskShell, (_Vx_usr_arg_t) task,0,0,0,0,0,0,0,0,0);
	}
	CleanUpId = taskSpawn("tCleanUp", 100, 0, 4000, (FUNCPTR) CleanUp, 0,0,0,0,0,0,0,0,0,0);
	
	/* start simulation */
	sysAuxClkEnable();
	sysAuxClkRateSet(AuxClkRate);
	myLogMsg("sysAuxClk rate is %d\n", sysAuxClkRateGet(), 0, 0, 0, 0, 0 );
	sysAuxClkConnect((FUNCPTR) AuxClkHandler, 0); 
	myLogMsg("sysAuxClkConnect()\n", 0, 0, 0, 0, 0, 0 );
}

void BatchModeSimulation(int sec) {
	int RAP;
	int VerboseState=Verbose;
	Verbose=FALSE;
	BatchMode=TRUE;
	if(!sec)
		TotalSimulationTicks=Hyperperiod()+Phase[0]+1;
	else
		TotalSimulationTicks = sec*sysAuxClkRateGet();
	BatchSem=semBCreate(SEM_Q_PRIORITY,SEM_FULL);
	if(!sec)
		logMsg( "Starting hyperperiod batch simulations\n", 0, 0, 0, 0, 0, 0 );
	else if(sec%60)
		logMsg( "Starting %d-sec batch simulations\n", sec, 0, 0, 0, 0, 0 );
	else
		logMsg( "Starting %d-min batch simulations\n", sec/60, 0, 0, 0, 0, 0 );
	for(RAP=0;RAP<5;RAP++) {
		semTake(BatchSem,WAIT_FOREVER);
		taskDelay(Hyperperiod());
		ResourceAccessProtocol=RAP;
		logMsg( "%d: %s\n", RAP, (_Vx_usr_arg_t)RAPName[RAP], 0, 0, 0, 0);
		StartSimulation(0);
	}
	semTake(BatchSem,WAIT_FOREVER);
	semDelete(BatchSem);
	BatchMode=FALSE;
	Verbose=VerboseState;
}

void WdHandler(int ticks) {
	taskResume(CleanUpId);
	myLogMsg("WdHandler() fired after %d system clock ticks (%d ticks/sec)\n", 
			ticks, sysClkRateGet(), 0, 0, 0, 0 );
}

void CleanUp() {
	int task, semaphore;
	taskSuspend(0);
	sysAuxClkDisable(); /* turn off system clock interrupts */
	myLogMsg("Terminating simulation\n", 0, 0, 0, 0, 0, 0 );
	taskDelete(MetaschedulerId);
	for(task=0;task<N_TASKS;task++)
		taskDelete(WorkerId[task]);
	for(semaphore=0;semaphore<N_SEMAPHORES;semaphore++)
		semDelete(Semaphore[semaphore]);/*
	logMsg(CLEAN_UP_STRING_1, CLEAN_UP_PARAMS_1);
	logMsg(CLEAN_UP_STRING_2, CLEAN_UP_PARAMS_2);*/
	printf("Simulation ended. Response times: %d, %d, %d, %d; Overruns: %d, %d, %d, %d.\n ",
			ResponseTime[0],ResponseTime[1],ResponseTime[2],ResponseTime[3],
			Overruns[0], Overruns[1],Overruns[2],Overruns[3]);
	if(BatchMode)
		semGive(BatchSem);
}

/* Output functions */
void myLogMsg(char* s, 
		_Vx_usr_arg_t a1, _Vx_usr_arg_t a2, _Vx_usr_arg_t a3, 
		_Vx_usr_arg_t a4, _Vx_usr_arg_t a5, _Vx_usr_arg_t a6 ) {
	if(Verbose)
		logMsg(s, a1, a2, a3, a4, a5, a5);
}

void ShowJobNumber(int task) {
	myLogMsg("%s: %d\n", (_Vx_usr_arg_t) taskName(0), Instance[task], 0, 0, 0, 0 );
}

/* Auxiliary functions */
int GCD(int a, int b) {
	while(a!=b) {
		if(a>b)
			a-=b;
		else
			b-=a;
	}
	return a;
}

int Hyperperiod() {
	int i, hyperperiod=Period[0];
	if(N_TASKS<1)
		return 1;
	for(i=1;i<N_TASKS;i++)
		hyperperiod *= Period[i]/GCD(hyperperiod,Period[i]);
	return hyperperiod;
}
