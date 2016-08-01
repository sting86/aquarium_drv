/*
 * typedefs.h
 *
 *  Created on: 29 lis 2014
 *      Author: Pawe�
 */

#ifndef TYPEDEFS_H_
#define TYPEDEFS_H_

#define false 0
#define true !false

#ifndef NULL
  #define NULL ((void*) 0)
#endif

#define MIN(x, y) ((x<y) ? x : y)
#define MAX(x, y) ((x>y) ? x : y)

typedef unsigned char _u8;

typedef _u8 bool;

#endif /* TYPEDEFS_H_ */
