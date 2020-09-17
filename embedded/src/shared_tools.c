# include "shared_tools.h"

void delay(float number_of_seconds)
{
  uint32_t dlyTicks = number_of_seconds * 667;
  uint32_t curTicks;
  curTicks = msTicks;
  while ((msTicks - curTicks) < dlyTicks) ;
}

void flushLocalBuffer(unsigned char *buf, int maxlen)
{
    for (int i = 0; i < maxlen + 1; ++i)
    {
        buf[i] = '\0';
    }
}
