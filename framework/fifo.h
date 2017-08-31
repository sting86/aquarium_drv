/*
 * fifo.h
 *
 *  Created on: 28 sie 2017
 *      Author: Pawe³
 */

#ifndef FRAMEWORK_FIFO_H_
#define FRAMEWORK_FIFO_H_

struct fifo {
    uint8_t size;           /* size of buffer in bytes */
    uint8_t read;           /* read pointer */
    uint8_t write;          /* write pointer */
    uint8_t *buffer;       /* fifo ring buffer */
};

Error Fifo_Initialize (struct fifo *fifo, uint8_t buffer[], uint8_t size);
uint8_t Fifo_DataLength (struct fifo *fifo);

Error Fifo_Pop (struct fifo *fifo, unsigned char data);
Error Fifo_Pull (struct fifo *fifo, unsigned char *data);

#endif /* FRAMEWORK_FIFO_H_ */
