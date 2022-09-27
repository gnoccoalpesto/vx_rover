/*
 * simulation.h
 *
 *  Created on: May 14, 2018
 *  Last modified on: May 20, 2018
 *      Author: Paolo Torroni. All rights reserved.
 */

#ifndef SIMULATION_H_
#define SIMULATION_H_

#include "vxWorks.h"
#include "taskLib.h"
#include "semLib.h"
#include "sysLib.h"
#include "wdLib.h"
#include "metascheduler.h"
#include "resources.h"
#include "applications.h"

extern int Instance[], Phase[], Period[], JobReleaseCounter[], NotFinished[], ResourceCount[], CurrentPriority[][N_TASKS];
extern RAPName_t RAPName[];
extern TASK_ID MetaschedulerId, WorkerId[];

#define DEFAULT_TOTAL_SIMULATION_TICKS 250

/* Simulation functions */
void StartSimulation();
void BatchModeSimulation(int sec);
void WdHandler(int ticks);
void CleanUp(void);

/* Output functions */
void myLogMsg(char* format_string, 
		_Vx_usr_arg_t arg1, _Vx_usr_arg_t arg2, _Vx_usr_arg_t arg3, 
		_Vx_usr_arg_t arg4, _Vx_usr_arg_t arg5, _Vx_usr_arg_t arg6);
void ShowJobNumber(int task);

/* Auxiliary functions */
int Hyperperiod();


#endif /* SIMULATION_H_ */
