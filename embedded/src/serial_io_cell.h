//this is ex9
#include <stdbool.h>
#include <stdint.h>

#ifndef SRC_GPS_SERIAL_IO_H_
#define SRC_GPS_SERIAL_IO_H_



bool SerialInitCellular(char* port, unsigned int baud);
bool SerialSendCellular(unsigned char *buf, unsigned int size);
unsigned int SerialRecvCellular(unsigned char *buf, unsigned int maxlen, unsigned int timeout_ms);
void SerialDisableCellular();
void SerialFlushInputBuffCellular (void);


#endif /* SRC_GPS_SERIAL_IO_H_ */
