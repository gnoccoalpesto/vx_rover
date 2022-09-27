/*
 * metascheduler.h
 *
 *  Created on: May 14, 2018
 *  Last modified on: May 20, 2018
 *      Author: Paolo Torroni. All rights reserved.
 */

#ifndef METASCHEDULER_H_
#define METASCHEDULER_H_

#include "applications.h"
#include "simulation.h"

/* Meta-scheduler function declarations */
void AuxClkHandler(void);
void MetaschedulerShell();
void Metascheduler();
void TaskShell(int task);

#endif /* METASCHEDULER_H_ */
