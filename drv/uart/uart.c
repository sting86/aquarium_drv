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

	void (*onDataReceived) (drvUart *dev);
	void (*onEndLineReceived) (drvUart *dev);
	void (*onDataTransmited) (drvUart *dev);

	bool CR_received;

	bool isTransmiting;
};

struct fifo txFifo;
struct fifo rxFifo;
uint8_t txBuffer[TX_BUFF_SIZE];
uint8_t rxBuffer[RX_BUFF_SIZE];

volatile struct usartPort devUsart = {0};//make it an array if more usarts available, add a handling of this case.

ISR(USART_RXC_vect, ISR_BLOCK) {
	char d = UDR;
	if (Fifo_Pop(devUsart.rxBuffer, d) == NO_ERROR) {
		//TODO: callback

		//Fifo_Pull(devUsart.rxBuffer,(uint8_t*) &d);

		if (devUsart.onDataReceived) {
			devUsart.onDataReceived((drvUart *) &devUsart);
		}

		if (d == 13) {//TODO: support if EOL is CR, LF or CR+LF
			devUsart.CR_received = true;
		} else if (devUsart.CR_received) {
			if (d == 10) {
				if (devUsart.onEndLineReceived) {
					devUsart.onEndLineReceived((drvUart *) &devUsart);
				}
			}
			devUsart.CR_received = false;
		}

	} else {
		//TODO: error handling
	}
}

ISR(USART_TXC_vect, ISR_NOBLOCK) {
	uint8_t data;
	if (Fifo_Pull(devUsart.txBuffer, &data) == NO_ERROR) {
	//	LCD_WriteData(data);

		UDR = data;
	} else {
		devUsart.isTransmiting = false;

		if (devUsart.onDataTransmited) {
			devUsart.onDataTransmited((drvUart *) &devUsart);
		}
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

Error UART_Open(drvUart **drv, struct uartInitParams *params) {
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

		if (params != NULL) {
			if (params->boundRate > 0) {
				ret = UART_ConfigBound(*drv, params->boundRate);
			}

			if (params->dataBits > 0) {
				ret |= UART_ConfigDataBits(*drv, params->dataBits);
			}

			if (params->handShake != UART_HFC_NULL) {
				ret |= UART_ConfigCtrlFlow(*drv, params->handShake);
			}

			if (params->parity != UART_PARITY_NULL) {
				ret |= UART_ConfigParity(*drv, params->parity);
			}

			if (params->stopBits != UART_SB_NULL) {
				ret |= UART_ConfigStopBits(*drv, params->stopBits);
			}

			(*drv)->onDataReceived    = params->onDataReceived;
			(*drv)->onDataTransmited  = params->onDataTransmited;
			(*drv)->onEndLineReceived = params->onEndLineReceived;
		}
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
	//uint8_t i;

	if (drv == NULL) return ERROR_INVALID_PARAMETER;
	if (buffer == NULL) return ERROR_INVALID_PARAMETER;
	if (!drv->isOpened) return ERROR_INVALID_PARAMETER;
	if (!drv->isConnected) return ERROR_INVALID_OPERATION;

	while (*buffer) {
		ret = Fifo_Pop(drv->txBuffer, *buffer++);
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
	return NO_ERROR;
}

Error UART_ReadLine(drvUart *drv, char *buffer, uint8_t *size /*in-out*/) {
	Error ret = NO_ERROR;
	char d;
	bool stop = false;
	bool CR_read = false;
	uint8_t bytesRead = 0;

	if (drv == NULL) return ERROR_INVALID_PARAMETER;
	if (buffer == NULL) return ERROR_INVALID_PARAMETER;
	if (size == NULL) return ERROR_INVALID_PARAMETER;
	if (!drv->isOpened) return ERROR_INVALID_PARAMETER;
	if (!drv->isConnected) return ERROR_INVALID_OPERATION;

	ret = Fifo_Pull(drv->rxBuffer, (uint8_t*) &d);
	if (ret == NO_ERROR) ++bytesRead;

	while (d != 0 && !stop && ret == NO_ERROR && bytesRead < *size-1) {
		if (CR_read)
		{
			if (d == 10) {
				bytesRead -= 2;
				break;
			} else {
				buffer[bytesRead-2] = 13;
				buffer[bytesRead-1] = d;
			}
			CR_read = false;
		} else {
			buffer[bytesRead-1] = d;
		}

		if (d == 13) {
			CR_read = true;
		}

		ret = Fifo_Pull(drv->rxBuffer, (uint8_t*) &d);
		if (ret == NO_ERROR) ++bytesRead;
	}

	if (bytesRead == *size) {
		ret |= ERROR_INSUFFICIENT_BUFFER;
	} else {
		*size = bytesRead;
	}

	if (*size != 0) {
		buffer[*size] = 0;
	} else {
		buffer[0] = 0;
	}

	return ret;
}
