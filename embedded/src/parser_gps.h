#ifndef GPS_PARSER_H
#define GPS_PARSER_H

#include "gps.h"

#define GSA_TITLE "$GNGSA"
#define GGA_TITLE "$GPGGA"

#define MAX_ENMA_LINE_LENGTH 90
#define MIN_GGA_LENGTH 70
#define MIN_GSA_LENGTH 52
#define GGA_ACTIVATION_IDX 5
#define GSA_ACTIVATION_IDX 1

int parse_sentence(GPS_LOCATION_INFO *location, char *gps_output, char *title,
                   int min_sentence_len, int max_sentence_len, int validation_idx);

#endif //GPS_PARSER_H
