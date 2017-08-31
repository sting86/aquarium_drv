/*
 * fifo.c
 *
 *  Created on: 28 sie 2017
 *      Author: Pawe³
 */

#include "framework/typedefs.h"
#include "framework/error.h"
#include "framework/fifo.h"

Error Fifo_Initialize (struct fifo *fifo, uint8_t buffer[], uint8_t size)
{

	if (fifo == NULL) return ERROR_INVALID_PARAMETER;
	if (size == 0) return ERROR_INVALID_OPERATION;

    fifo->buffer = buffer;
    fifo->size = size;
    fifo->read = fifo->write = 0;

    return NO_ERROR;
};

uint8_t Fifo_DataLength (struct fifo *fifo)
{
    // return length of valid data in fifo
    return (fifo->write - fifo->read) & (fifo->size - 1);
};


Error Fifo_Pop (struct fifo *fifo, unsigned char data)
{
    // fifo full : error
    if ((fifo->write+1) % fifo->size == fifo->read)
    {
        return ERROR_INSUFFICIENT_BUFFER;
    }
    // write data & increment write pointer
    fifo->buffer[fifo->write] = data;
    fifo->write = (fifo->write + 1) % fifo->size;
    return NO_ERROR;
};


Error Fifo_Pull (struct fifo *fifo, unsigned char *data)
{
    // fifo empty : error
    if (fifo->write == fifo->read)
    {
        return ERROR_EMPTY_BUFFER;
    }
    // read data & increment read pointer
    *data = fifo->buffer[fifo->read];
    fifo->read = (fifo->read + 1) % (fifo->size);
    return NO_ERROR;
};
