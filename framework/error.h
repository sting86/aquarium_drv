/*
 * error.h
 *
 *  Created on: 29 lis 2014
 *      Author: Pawe³
 */

#ifndef ERROR_H_
#define ERROR_H_

#include <stdio.h>
#include <stdlib.h>

typedef enum {
	NO_ERROR,

	ERROR, //unspecify error
	ERROR_INVALID_PARAMETER,
	ERROR_INDEX_OUT_OF_RANGE,

	ERROR_INSUFFICIENT_RESOURCES, //e.g. no more memory to allocate (malloc)
	ERROR_INSUFFICIENT_BUFFER, //no more space in buffer
	ERROR_BUFFER_NULL,

	ERROR_COUNT
} Error;

#define SHOW_ERROR() printf("ERR: File: %s, line: %d\n", __FILE__, __LINE__)

#define CHECK_PARAMETR(expresion) if ( (!(expresion)) && ret == NO_ERROR ) {\
		ret = ERROR_INVALID_PARAMETER;\
		SHOW_ERROR();\
	}

#define CALL( function ) ret = function; \
	if (ret != NO_ERROR) SHOW_ERROR();

#endif /* ERROR_H_ */
