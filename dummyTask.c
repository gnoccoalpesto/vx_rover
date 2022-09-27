/*
 * dummyTask.c
 *
 *  Created on: April 15, 2018
 *  Last modified on: April 15, 2018
 *      Author: Paolo Torroni. All rights reserved.
 */

#include "dummyTask.h"

void all_done(void) {}

void get_busy(int n) {
	int i, r=LOWER_LIMIT, going=UP;
	for(i=0;i<n;i++) {
		if(going==UP) {
			if(r<UPPER_LIMIT)
				r++;
			else
				going=DOWN;
		}
		else {
			if(r>LOWER_LIMIT)
				r--;
			else
				going=UP;
		}
	}
	all_done();
}
