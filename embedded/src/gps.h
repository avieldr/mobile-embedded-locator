#include <stdbool.h>
#include "serial_io_gps.h"

#ifndef GPS_GPS_H
#define GPS_GPS_H

typedef struct __GPS_LOCATION_INFO
{
    int32_t latitude;
    int32_t longitude;
    int32_t altitude;
    uint8_t hdop;
    uint8_t valid_fix;
    uint8_t reserved1;
    uint8_t num_sats;
    char fixtime[10];
} GPS_LOCATION_INFO;


void GPSInit();
void GPSDisable();
uint32_t GPSGetReadRaw(char *buf, uint32_t maxlen);
bool GPSGetFixInformation(GPS_LOCATION_INFO *location);

#endif //GPS_GPS_H
