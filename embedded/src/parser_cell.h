//this is ex9

#ifndef EMBEDDED_EX3_PARSER_H
#define EMBEDDED_EX3_PARSER_H

#include <stdbool.h>
#include "cellular.h"
//#include "gps.h"
//
//#define GSA_TITLE "$GNGSA"
//#define GGA_TITLE "$GPGGA"
//#define RMC_TITLE "$GNRMC"
//#define RMC_ACTIVATION_IDX 1
//#define MIN_RMC_LENGTH 70


//#define MAX_ENMA_LINE_LENGTH 90
//#define MIN_GGA_LENGTH 70
//#define MIN_GSA_LENGTH 52
//#define GGA_ACTIVATION_IDX 5
//#define GSA_ACTIVATION_IDX 1
//
//int parse_sentence(GPS_LOCATION_INFO *location, char *gps_output, char *title,
//                   int min_sentence_len, int max_sentence_len, int validation_idx);
//int parse_rmc_sentence(GPS_LOCATION_INFO *location, char *gps_output, char* date, char* time);

bool parse_reg_status(char* modem_output, int* status);
bool parse_sig_quality(char* modem_output, int* csq);
bool extract_single_operator_info(OPERATOR_INFO* operator, int op_idx, char* buf);
//
//double deg2rad(double);
//double rad2deg(double);
//double distance(double lat1, double lon1, double lat2, double lon2, char unit);

void get_cell_str(OPERATOR_INFO* operator, char* buf);
//void get_gps_str(GPS_LOCATION_INFO *location, char* buf);
//unsigned int get_epoch(char* date, char* time, char* buf);





#endif //EMBEDDED_EX3_PARSER_H
