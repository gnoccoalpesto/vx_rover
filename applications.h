/*
 * applications.h
 *
 *  Created on: 		23 Sep, 2022
 *  Last modified on: 	27 Sep, 2022
 *      Author: Francesco Cerri. All rights reserved.
 */

#ifndef APPLICATIONS_H_
#define APPLICATIONS_H_

#include "resources.h"
#include "dummyTask.h"

extern int Overruns[], ResponseTime[];

/* --> Change this to select application <-- */
#define APPLICATION_ROVER

/* Baseline for worker tasks */
#define TOP_PRIORITY 110

/* Application parameters */


#ifdef APPLICATION_ROVER
	#define N_TASKS 4
#define TASK_PERIODS {16,30,40,60}
#define TASK_PHASES {20,0,0,0}
	#define TASK_NORMAL_PRIOS {TOP_PRIORITY,TOP_PRIORITY+1,TOP_PRIORITY+2,TOP_PRIORITY+3}
	#define N_SEMAPHORES 2
	#define CEILINGS { TOP_PRIORITY+1, TOP_PRIORITY+2 }
	#define APPLICATION applicationRover
	#define CLEAN_UP_STRING_1 "Simulation ended. Response times: %d, %d, %d, %d" 
	#define CLEAN_UP_PARAMS_1 (_Vx_usr_arg_t) ResponseTime[0], (_Vx_usr_arg_t) ResponseTime[1], (_Vx_usr_arg_t) ResponseTime[2], (_Vx_usr_arg_t) ResponseTime[3],0,0
	#define CLEAN_UP_STRING_2 "Overruns: %d, %d, %d, %d.\n" 
	#define CLEAN_UP_PARAMS_2 (_Vx_usr_arg_t) Overruns[0],  (_Vx_usr_arg_t) Overruns[1], (_Vx_usr_arg_t) Overruns[2], (_Vx_usr_arg_t) Overruns[3],0,0

	/*TASK SET*/
	#define MONITOR_TASK 0
	#define CAMERA_TASK 1
	#define COLLISION_TASK 2
	#define PLANNING_TASK 3
	
	/*RESOURCES*/
	#define CAMERA_RESOURCE 0
	#define OBJECTS_RESOURCE 1

#endif

/* Defined applications */
void APPLICATION(int task);


#endif /* APPLICATIONS_H_ */
