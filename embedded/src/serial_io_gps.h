//this is ex9
#include <stdbool.h>
#include <stdint.h>

#ifndef SRC_GPS_SERIAL_IO_H_
#define SRC_GPS_SERIAL_IO_H_

bool SerialInit(char* port, uint32_t baud);
bool SerialSend(const uint8_t *buf, uint16_t len);
uint16_t SerialRecv(uint8_t *buf, uint16_t maxlen, uint32_t timeout_ms);
void SerialFlushInputBuff (void);
void SerialDisable();

#endif /* SRC_GPS_SERIAL_IO_H_ */
