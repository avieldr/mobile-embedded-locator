
#include "serial_io_cell.h"
#include <stdio.h>
#include "em_device.h"
#include "em_cmu.h"
#include "em_chip.h"
#include "em_emu.h"
#include "em_leuart.h"
#include "bspconfig.h"
#include "retargetserial.h"
#include "shared_tools.h"

#ifndef GPS_SERIAL_IO_H
#define GPS_SERIAL_IO_H

bool SerialInitCellular(char* port, unsigned int baud)
{
	/* Enable peripheral clocks */
	  CMU_ClockEnable(cmuClock_HFPER, true);
	  /* Configure GPIO pins */
	  CMU_ClockEnable(cmuClock_GPIO, true);
	  /* Select LFXO for LEUARTs (and wait for it to stabilize) */
	  CMU_ClockEnable(cmuClock_USART2, true);

	  USART_InitAsync_TypeDef initAsync = USART_INITASYNC_DEFAULT;
	  initAsync.enable = usartDisable;
	  USART_InitAsync(USART2, &initAsync);
	  USART_BaudrateAsyncSet(USART2, 0, 115200, USART_CTRL_OVS_X16);

	  /* To avoid false start, configure output as high */
	  GPIO_PinModeSet(gpioPortA, 6, gpioModePushPull, 1);
	  GPIO_PinModeSet(gpioPortA, 7, gpioModeInput, 0);

	  /* Enable CORE LE clock in order to access LE modules */
	  CMU_ClockEnable(cmuClock_CORELE, true);

	  /* Enable I/O and set location */
	  USART2->ROUTEPEN |= USART_ROUTEPEN_RXPEN | USART_ROUTEPEN_TXPEN;
	  USART2->ROUTELOC0 = (USART2->ROUTELOC0 &
	                       ~(_USART_ROUTELOC0_TXLOC_MASK
	                         | _USART_ROUTELOC0_RXLOC_MASK ))
	                       | (_USART_ROUTELOC0_TXLOC_LOC1 << _USART_ROUTELOC0_TXLOC_SHIFT)
	                       | (_USART_ROUTELOC0_RXLOC_LOC1 << _USART_ROUTELOC0_RXLOC_SHIFT);
	  USART2->ROUTEPEN  = USART_ROUTEPEN_RXPEN | USART_ROUTEPEN_TXPEN;

	  USART_Enable(USART2, usartEnable);
	  return 1;
}

bool SerialSendCellular(unsigned char *buf, unsigned int size)
{

	printf("Sending: \n%s\n", buf);

	int i = 0;
	while (i < size - 1)
	{
		if (!buf[i])
			return 1;
		USART_Tx(USART2, buf[i]);
		i++;
	}
	return 1;
}

unsigned int SerialRecvCellular(unsigned char *buf, unsigned int maxlen, unsigned int timeout_ms)
{
	flushLocalBuffer(buf, maxlen);
	int i = 0;
	uint32_t curTicks = msTicks;
	uint32_t timeout_ticks = timeout_ms; // TODO convert!!
	while (i < maxlen - 1)
		{

			if (USART2->STATUS & USART_STATUS_RXDATAV)
			{
				char c = (uint8_t)USART2->RXDATA;
				buf[i] = c;
				++i;
			}
			if (msTicks - curTicks >= timeout_ticks)
			{
				++i;
				break;
			}
		}
	buf[i] = 0;
	if (i)
	{
		printf("Received: \n%s\n", buf);
	}
	return i;
}

void SerialFlushInputBuffCellular(void)
{

}

void SerialDisableCellular()
{
	USART_Reset(USART2);
}

#endif //GPS_SERIAL_IO_H
