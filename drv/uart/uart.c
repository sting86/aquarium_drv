/*
 * uart.c
 *
 *  Created on: 19 sie 2017
 *      Author: Pawe³
 */

#include "uart.h"
#include "config.h"
#include "avr/io.h"
#include "avr/interrupt.h"
#include <string.h>

#include "drv/LCD/HD44780.h"
#include "framework/fifo.h"

struct usartPort {
	bool isOpened;//reserve a port
	bool isConnected;//config if not connected, send/read if connected.
	uint16_t ubrr; //=((Fosc)/(8*bound rate))-1
	uint8_t bits; //for now support from 5 up to 8 bits
	enum UARTParity parity;
	enum UARTStopBits stopBits;
	enum UARTHwFlowCtrl flowControl;

	struct fifo *txBuffer;
	struct fifo *rxBuffer;
	bool isTransmiting;
};

struct fifo txFifo;
struct fifo rxFifo;
uint8_t txBuffer[TX_BUFF_SIZE];
uint8_t rxBuffer[RX_BUFF_SIZE];

volatile struct usartPort devUsart = {0};//make it an array if more usarts available, add a handling of this case.

ISR(USART_RXC_vect, ISR_BLOCK) {
	if (Fifo_Pop(devUsart.rxBuffer, UDR) == NO_ERROR) {
		//TODO: callback
		char d;
		Fifo_Pull(devUsart.rxBuffer,(uint8_t*) &d);
		LCD_WriteData(d);

	} else {
		//TODO: error handling
	}
}

ISR(USART_TXC_vect, ISR_NOBLOCK) {
	uint8_t data;
	if (Fifo_Pull(devUsart.txBuffer, &data) == NO_ERROR) {
		LCD_WriteData(data);

		UDR = data;
	} else {
		devUsart.isTransmiting = false;
	}
}


Error UART_Initialize() {
	Error ret = NO_ERROR;

	devUsart.rxBuffer = &rxFifo;
	devUsart.txBuffer = &txFifo;
	ret = Fifo_Initialize(devUsart.rxBuffer, rxBuffer, RX_BUFF_SIZE);
	ret |= Fifo_Initialize(devUsart.txBuffer, txBuffer, TX_BUFF_SIZE);

	return ret;
}

Error UART_Open(drvUart **drv) {
	Error ret = NO_ERROR;
	if (devUsart.isOpened) {
		ret = ERROR_INVALID_OPERATION;
	} else if (drv == NULL) {
		ret = ERROR_INVALID_PARAMETER;
	} else {
		UCSRA = 0;
		UCSRB = 0;
		UCSRC = 0;
		UCSRA |= 1<<U2X;
		devUsart.isOpened = true;
		*drv = (drvUart*) &devUsart;
	}
	return ret;
}

Error UART_Close(drvUart *drv) {
	Error ret = NO_ERROR;

	if (drv == NULL) return ERROR_INVALID_PARAMETER;

	if (drv->isOpened) {
		devUsart.isOpened = false;
	}
	return ret;
}

Error UART_Connect(drvUart *drv) {
	Error ret = NO_ERROR;

	if (drv == NULL) return ERROR_INVALID_PARAMETER;
	if (!drv->isOpened) return ERROR_INVALID_PARAMETER;

	if (drv->isConnected) {
		ret = ERROR_INVALID_OPERATION;
	} else {
		/* Enable receiver and transmitter */
		/* Enable interrupt for receive and transmit */
		UCSRB |= ((1<<RXEN)|(1<<TXEN)|(1<<RXCIE)|(1<<TXCIE));
		drv->isConnected = true;
	}
	return ret;
}

Error UART_Disconnect(drvUart *drv) {
	Error ret = NO_ERROR;

	if (drv == NULL) return ERROR_INVALID_PARAMETER;
	if (!drv->isOpened) return ERROR_INVALID_PARAMETER;

	if (drv->isConnected) {
		/* Disable receiver and transmitter */
		/* Disable interrupt for receive and transmit */
		UCSRB &= ~((1<<RXEN)|(1<<TXEN)|(1<<RXCIE)|(1<<TXCIE));
		drv->isConnected = false;
	}
	return ret;
}

Error UART_ConfigBound(drvUart *drv, uint32_t boundRate) {
	Error ret = NO_ERROR;

	if (drv == NULL) return ERROR_INVALID_PARAMETER;
	if (!drv->isOpened) return ERROR_INVALID_PARAMETER;
	if (drv->isConnected) return ERROR_INVALID_OPERATION;

	drv->ubrr = (uint16_t)((F_CPU/(8*boundRate))-1);

	UBRRL = (drv->ubrr) & 0xFF;
	UBRRH = (drv->ubrr) >> 8;

	return ret;
}

Error UART_ConfigParity(drvUart *drv, enum UARTParity parity) {
	Error ret = NO_ERROR;

	if (drv == NULL) return ERROR_INVALID_PARAMETER;
	if (!drv->isOpened) return ERROR_INVALID_PARAMETER;
	if (drv->isConnected) return ERROR_INVALID_OPERATION;

	switch (parity) {
		case UART_PARITY_NONE:
			UCSRC &= ~(1<<UPM0 | 1<<UPM1);
			break;

		case UART_PARITY_ODD:
			UCSRC |= (1<<UPM0 | 1<<UPM1);
			break;

		case UART_PARITY_EVEN:
			UCSRC &= ~(1<<UPM0);
			UCSRC |= (1<<UPM1);
			break;

		default:
			ret = ERROR_INVALID_PARAMETER;
	}

	if (ret != NO_ERROR) {
		drv->parity = parity;
	}

	return ret;
}

Error UART_ConfigStopBits(drvUart *drv, enum UARTStopBits sb) {
	Error ret = NO_ERROR;

	if (drv == NULL) return ERROR_INVALID_PARAMETER;
	if (!drv->isOpened) return ERROR_INVALID_PARAMETER;
	if (drv->isConnected) return ERROR_INVALID_OPERATION;

	switch (sb) {
		case UART_SB_ONE:
			UCSRC &= ~(1<<USBS);
			break;

		case UART_SB_TWO:
			UCSRC |= (1<<USBS);
			break;

		default:
			ret = ERROR_INVALID_PARAMETER;
	}

	if (ret != NO_ERROR) {
		drv->stopBits = sb;
	}

	return ret;
}

Error UART_ConfigCtrlFlow(drvUart *drv, enum UARTHwFlowCtrl flowCrtl) {
	Error ret = ERROR_NOT_IMPLEMENTED;

	if (drv == NULL) return ERROR_INVALID_PARAMETER;
	if (!drv->isOpened) return ERROR_INVALID_PARAMETER;
	if (drv->isConnected) return ERROR_INVALID_OPERATION;

	//TODO:fill this

	return ret;
}

Error UART_ConfigDataBits(drvUart *drv, uint8_t dataBitsNumber) {
	Error ret = ERROR_NOT_IMPLEMENTED;

	if (drv == NULL) return ERROR_INVALID_PARAMETER;
	if (!drv->isOpened) return ERROR_INVALID_PARAMETER;
	if (drv->isConnected) return ERROR_INVALID_OPERATION;
	if (dataBitsNumber > 8 || dataBitsNumber < 5) return ERROR_INVALID_PARAMETER;

	drv->bits = dataBitsNumber;

	UCSRB &= ~(1<<UCSZ2);
	UCSRC &= ~(1<<UCSZ1 | 1<<UCSZ0);//this is also 5 bits configuration

	switch (dataBitsNumber) {
		case 6:
			UCSRC |= (1<<UCSZ0);
			break;

		case 7:
			UCSRC |= (1<<UCSZ1);
			break;

		case 8:
			UCSRC |= (1<<UCSZ1 | 1<<UCSZ0);
			break;

		default:
			ret = ERROR_INDEX_OUT_OF_RANGE;//never should be here.
	}

	return ret;
}


Error UART_WriteText(drvUart *drv, char const *buffer);
Error UART_WriteText_P(drvUart *drv, char *buffer);
Error UART_ReadText(drvUart *drv, char *buffer, uint8_t *size /*in-out*/);

Error UART_WriteData(drvUart *drv, uint8_t *buffer, uint8_t size) {
	Error ret = NO_ERROR;
	uint8_t i;

	if (drv == NULL) return ERROR_INVALID_PARAMETER;
	if (buffer == NULL) return ERROR_INVALID_PARAMETER;
	if (!drv->isOpened) return ERROR_INVALID_PARAMETER;
	if (!drv->isConnected) return ERROR_INVALID_OPERATION;

	for (i=0; i<size; ++i) {
		ret = Fifo_Pop(drv->txBuffer, buffer[i]);
		if (ret != NO_ERROR) {
			break;
		}
	}

	if (!drv->isTransmiting) { //start transmition
		uint8_t data;
		Fifo_Pull(drv->txBuffer, &data);
		LCD_WriteData(data);
		drv->isTransmiting = true;
		UDR = data;
	}
	return ret;
}

Error UART_WriteData_P(drvUart *drv, uint8_t const *buffer, uint8_t size);
Error UART_ReadData(drvUart *drv, uint8_t *buffer, uint8_t *size /*in-out*/) {
	Error ret = NO_ERROR;

	if (drv == NULL) return ERROR_INVALID_PARAMETER;
	if (buffer == NULL) return ERROR_INVALID_PARAMETER;
	if (size == NULL) return ERROR_INVALID_PARAMETER;
	if (!drv->isOpened) return ERROR_INVALID_PARAMETER;
	if (!drv->isConnected) return ERROR_INVALID_OPERATION;


	return ret;
}
