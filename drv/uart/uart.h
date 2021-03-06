/*
 * uart.h
 *
 *  Created on: 19 sie 2017
 *      Author: Pawe�
 */

#ifndef DRV_UART_UART_H_
#define DRV_UART_UART_H_


#include "framework/typedefs.h"
#include "framework/error.h"

enum UARTParity {
	UART_PARITY_NONE,
	UART_PARITY_ODD,
	UART_PARITY_EVEN,
//	UART_PARITY_MARK,
//	UART_PARITY_SPACE
	UART_PARITY_NULL = 0xFF
};

enum UARTStopBits {
	UART_SB_ONE,
	//UART_SB_ONE_AND_HALF,
	UART_SB_TWO,
	UART_SB_NULL = 0xFF
};

enum UARTHwFlowCtrl {
	UART_HFC_NONE,
	UART_HFC_DTR_DSR,
	UART_HFC_RTS_CTS,
	UART_HFC_RS485,
	UART_HFC_NULL = 0xFF
};

typedef struct usartPort drvUart;

struct uartInitParams {
	enum UARTParity parity;
	enum UARTStopBits stopBits;
	enum UARTHwFlowCtrl handShake;
	uint32_t boundRate;
	uint8_t  dataBits;
	//cbf
	void (*onDataReceived) (drvUart *dev);
	void (*onEndLineReceived) (drvUart *dev);
	void (*onDataTransmited) (drvUart *dev);
};

Error UART_Initialize();
Error UART_Open(drvUart **drv, struct uartInitParams *params);
Error UART_Close(drvUart *drv);
Error UART_Connect(drvUart *drv);
Error UART_Disconnect(drvUart *drv);

Error UART_ConfigBound(drvUart *drv, uint32_t boundRate);
Error UART_ConfigParity(drvUart *drv, enum UARTParity);
Error UART_ConfigStopBits(drvUart *drv, enum UARTStopBits);
Error UART_ConfigCtrlFlow(drvUart *drv, enum UARTHwFlowCtrl);
Error UART_ConfigDataBits(drvUart *drv, uint8_t dataBitsNumber);

Error UART_WriteText(drvUart *drv, char const *buffer);
Error UART_WriteText_P(drvUart *drv, char *buffer);
Error UART_ReadText(drvUart *drv, char *buffer, uint8_t *size /*in-out*/);
Error UART_ReadLine(drvUart *drv, char *buffer, uint8_t *size /*in-out*/);

Error UART_WriteData(drvUart *drv, uint8_t *buffer, uint8_t size);
Error UART_WriteData_P(drvUart *drv, uint8_t const *buffer, uint8_t size);
Error UART_ReadData(drvUart *drv, uint8_t *buffer, uint8_t *size /*in-out*/);



#endif /* DRV_UART_UART_H_ */
