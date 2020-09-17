#include "serial_io_gps.h"
#include <stdio.h>
#include "em_device.h"
#include "em_cmu.h"
#include "em_chip.h"
#include "em_emu.h"
#include "em_leuart.h"
#include "bspconfig.h"
#include "retargetserial.h"

#ifndef GPS_SERIAL_IO_H
#define GPS_SERIAL_IO_H

volatile uint32_t msTicks; /* counts 1ms timeTicks */

bool SerialInit(char* port, uint32_t baud)
{
	/* Enable peripheral clocks */
	  CMU_ClockEnable(cmuClock_HFPER, true);
	  /* Configure GPIO pins */
	  CMU_ClockEnable(cmuClock_GPIO, true);
	  /* To avoid false start, configure output as high */
	  GPIO_PinModeSet(RETARGET_TXPORT, RETARGET_TXPIN, gpioModePushPull, 1);
	  GPIO_PinModeSet(RETARGET_RXPORT, RETARGET_RXPIN, gpioModeInput, 0);

	  LEUART_Init_TypeDef init = LEUART_INIT_DEFAULT;

	  /* Enable CORE LE clock in order to access LE modules */
	  CMU_ClockEnable(cmuClock_CORELE, true);

	  /* Select LFXO for LEUARTs (and wait for it to stabilize) */
	  CMU_ClockSelectSet(cmuClock_LFB, cmuSelect_LFXO);
	  CMU_ClockEnable(cmuClock_LEUART0, true);

	  /* Do not prescale clock */
	  CMU_ClockDivSet(cmuClock_LEUART0, cmuClkDiv_1);

	  /* Configure LEUART */
	  init.enable = leuartDisable;

	  LEUART_Init(LEUART0, &init);
	  LEUART_BaudrateSet(LEUART0, 0, baud);

	  /* Enable pins at default location */
	  LEUART0->ROUTELOC0 = (LEUART0->ROUTELOC0 & ~(_LEUART_ROUTELOC0_TXLOC_MASK
	                                               | _LEUART_ROUTELOC0_RXLOC_MASK))
	                       | (RETARGET_TX_LOCATION << _LEUART_ROUTELOC0_TXLOC_SHIFT)
	                       | (RETARGET_RX_LOCATION << _LEUART_ROUTELOC0_RXLOC_SHIFT);

	  LEUART0->ROUTEPEN  = USART_ROUTEPEN_RXPEN | USART_ROUTEPEN_TXPEN;

	  /* Set RXDMAWU to wake up the DMA controller in EM2 */
	  LEUART_RxDmaInEM2Enable(LEUART0, true);

	  /* Finally enable it */
	  LEUART_Enable(LEUART0, leuartEnable);
	  return 1;
}

bool SerialSend(const uint8_t *buf, uint16_t len)
{
	return 1;
}

uint16_t SerialRecv(uint8_t *buf, uint16_t maxlen, uint32_t timeout_ms)
{
	int i = 0;
	uint32_t curTicks = msTicks;
	uint32_t timeout_ticks = timeout_ms * 8;
	while (i < maxlen - 1)
		{

			if (LEUART0->STATUS & LEUART_STATUS_RXDATAV)
			{
				buf[i] = (uint8_t)LEUART0->RXDATA;
				++i;
			}
			if (msTicks - curTicks >= timeout_ticks)
			{
				return 0;
			}
		}
	buf[maxlen - 1] = 0;
	return i;
}

void SerialFlushInputBuff (void)
{

}

void SerialDisable()
{
	LEUART_Reset(LEUART0);
}

#endif //GPS_SERIAL_IO_H
