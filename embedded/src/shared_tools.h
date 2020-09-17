#include <stdio.h>

volatile uint32_t msTicks; /* counts 1ms timeTicks */

void delay(float number_of_seconds);
void flushLocalBuffer(unsigned char *buf, int maxlen);
