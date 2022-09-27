/*
 * applications.c
 *
 *  Created on: 		23 Sep, 2022
 *  Last modified on: 	27 Sep, 2022
 *      Author: Francesco Cerri. All rights reserved.
 */



/*  * TASKS DESCRIPTION
 * 
 * MONITOR NODE (M): checks if all nodes are correctly operating and respawns them if needed
 * -PRIO:	max(very high)
 * -PHASE:	high (8u, must wait all nodes to be working)
 * -C:		low (2 u)
 * -T:		high(
 * -R:		0(ADD): net bandwidth (multi instance resource, shared)
 * 			
 * 	CAMERA NODE(V): source of video stream, after preprocessing
 * 	-PRIO:	high(video source)
 * 	-PHASE:	0u
 * 	-C:		med (6 u, let's imagine gpu helps)
 * 	-T:		med(
 * 	-R:		1: camera (exclusive,out)
 * 			0(ADD)
 * 			
 * 	COLLISION NODE (O): uses video for reconstructing phisical environment
 * 	-PRIO:	med(objects source)
 * 	-PHASE: 0u
 * 	-C:		high(8 u)
 * 	-T:		med(
 * 	-R:		1 (in)
 * 			2: objects (exclusive, out)
 * 			
 * 	PLANNING NODE (P): plans robot path using objects (and camera, maybe not all informatin extracted?)
 * 	-PRIO:	low(final utilizator)
 * 	-PHASE: 0u
 * 	-C:		very high(10 u, search algorithm)
 * 	-T:		low (sporadic?)
 * 	-R:		2 (in)
 * 			0(ADD)
 * 			1(ADD1, in)
 * 			
 * 			
 * 			GAMMA	|	PHIi	Ci		Ti		| t(1)	deltai,1	| t(2)	deltai,2
 * 			
* 			  M		|	 12		2		16		|	0		0		|	0		0
* 			  V		|	 0		6		30		|	.5		5.5		|	0		0
* 			  O		|	 0		10		40		|	.5		9.5		|	3		7
* 			  P		|	 0		16		60		|	0		0		|	.5		7
 * 			
 * 			hyperperiod H=240
 * 			
 * 			harmonic task groups: 16,{30,60},40                     s
 * 			
 * 			ALL TASK PERIODIC
 * 			
 * 			FIXED PRIO: PRIO(M)>PRIO(V)>PRIO(O)>PRIO(P)				
 * 							
 * 					
 * 							
 * 	TASK SET ANALYSIS ------------------------------------
 * 	
 * 	since all periodic and fixed PRIO -> LYU LAYLAND BOUNDS (if not), HYPERBOLIC BOUNDS (if not), RESPONSE TIME ANALYSIS
 * 	
 * 	PUC: sum(Ui)=1/8+1/5+1/4+4/15=.8416
 * 	
 * 	
 * 	RESOURCES CEILINGS
 * 	
 * 	1: 		PRIO(V)
 * 	2: 		PRIO(O)
 * 	
 *  BLOCKING TIMES (wrt RAP)
 *  
 *  computation:
 *  
 *  -npp: max CS time from lower PRIO tasks
 *  -hlp: max CS time for resources with ceilings>=task PRIO, from lower PRIO tasks
 *  -pip: max combination from resource table
 *  -pcp: same as hlp
 *  
			GAMMA	|	NPP		HLP		PIP		PCP
 * 			
* 			  M		|	 9.5	0				0		
* 			  V		|	 9.5	9.5				9.5		
* 			  O		|	 7		7				7		
* 			  P		|	 0		0				0		
* 			  
* 
* LLB FOR ALL TASKS -------------------------------
* 
* computation: sum{1,i-1}(Uj) + Ui+Bi/Ti <= LL(nh)
* 
* w/ nh: harmonic groups, LL(nh)=nh*(2^(1/nh)-1)
* LL(1)=1, LL(2)=.8284, LL(3)=.77976
* 
* (Ui:.125, .2, .25, 0.2667)
* 
* 	NPP
* 		M: .125+9.5/16=.718 <1 					OK
* 		V: .125+ .2+ 9.5/30 = .6416 < .828		OK
* 		O: .125 + .2 + .25 +7/40 = .75 < .779	OK
* 		P:	.125+.2+.25+.2667 = .8416 > .779	KO!!!
* 		
* 	NOTE: since P task not ok and FOREACH RAP BP==0
* 	-> feasibility still doubtful for all protocols
* 	
* 	-> SOLUTION: single harmonic group
* 		
* 		
* UB FOR ALL TASKS
* 
* computation: prod{1,i-1}(Uj+1) * (Ui +Bi/T  +1) <= 2
* 
* 	NPP
* 		M: .125 + .593 + 1 <2					OK
* 		V: 1.125 * (.2+1+ .3166) = 1.7 < 2		OK
* 		O: 
* 		P: 1.125*1.2*1.25 * 1.26666 > 2			KO!!!
* 			  
* RTA FOR ALL TASKS
* 
* computation: 	R_j_0=Cj + Bj + sum{1,j-1}(Ck)
* 				R_j_s=Cj + Bj + sum{1,j-1}(ceiling(R_j_s-1 / Tk)*Ck)
* 				until convergence: R_j_s=R_j_s-1
* 	test: R_j<=Dj
* 	
* 	(2, 6, 10, 16;
* 	 16, 30, 40, 60)
* 
* 	NPP
* 		M
* 		V
* 		O
* 		P:	R_0= 2+6+10+16=34<60
* 			R_1= 44<60
* 			R_2= 54<60
* 			R_3=56 < 60
* 			R_4=R_3								OK
* 			
*	TASK SET IS FEASIBLE UNDER FIXED PRIO, FOR EACH RAP
*	
*	
*	
 * 			
 * ADDITIONAL RESOURCES DESCRIPITON
 * 
 * 0: net badwidth
 * 		every node must use this to contact others node, or to advertise or receive data
 * 		hence, other resources are always nested inside it
 * 		stack:				4
 * 		used by nodes:		M:1(0)
 * 							V:2(raw video data, used only by V) , 2(1)
 * 							O:2(1),2(2)
 * 							P:2(2),2(1)
 */
#include "applications.h"

void applicationRover(int task) {
	switch (task) {
	case MONITOR_TASK:
		get_busy(2*OPSMSEC);
		break;
		
	case CAMERA_TASK:
		get_busy(.5*OPSMSEC);
			EntrySection(CAMERA_RESOURCE,CAMERA_TASK);
			get_busy(5.5*OPSMSEC);
			ExitSection(CAMERA_RESOURCE,CAMERA_TASK);
		break;
		
	case COLLISION_TASK:
		get_busy(.5*OPSMSEC);
			EntrySection(CAMERA_RESOURCE,COLLISION_TASK);
			get_busy(2.5*OPSMSEC);
				EntrySection(OBJECTS_RESOURCE,COLLISION_TASK);
				get_busy(7*OPSMSEC);
				ExitSection(OBJECTS_RESOURCE,COLLISION_TASK);
			ExitSection(CAMERA_RESOURCE,COLLISION_TASK);
		break;
		
	case PLANNING_TASK:
		get_busy(.5*OPSMSEC);
			EntrySection(OBJECTS_RESOURCE,PLANNING_TASK);
			get_busy(7*OPSMSEC);
			ExitSection(OBJECTS_RESOURCE,PLANNING_TASK);
		get_busy(8.5*OPSMSEC);
		break;
	}
}
