//this is ex9
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "parser_cell.h"





//#define GGA_TIME_IDX 0
//#define GGA_LAT_IDX 1
//#define GGA_LAT_DIR_IDX 2
//#define GGA_LONG_IDX 3
//#define GGA_LONG_DIR_IDX 4
//#define GGA_ALT_IDX 8
//#define GGA_VALID_FIX_IDX 5
//#define GGA_NUM_SATS_IDX 6
//#define GSA_HDOP_IDX 10
//
//char delim[] = ",";
//int32_t latitude;
//int32_t longitude;
//int32_t altitude;
//uint8_t valid_fix;
//uint8_t hdop;
//
//
//
//int is_not_valid(char valid, char* title)
//{
//
//    if (strcmp(title, GGA_TITLE) == 0)
//    {
//        if (valid != '1' && valid != '2' && valid != '3' && valid != '4' && valid != '5' && valid != '6' &&
//            valid != '7' && valid != '8')
//            return 1;
//    }
//    else if (strcmp(title, GSA_TITLE) == 0)
//    {
//        if (valid != '2' && valid != '3')
//            return 1;
//    }
//    else if (strcmp(title, RMC_TITLE) == 0)
//	{
//		if (valid != 'A')
//			return 1;
//	}
//    return 0;
//}
//
//void utc_parse(char* gps_utc_inpt, char* loc)
//{
//    loc[0] = gps_utc_inpt[0];
//    loc[1] = gps_utc_inpt[1];
//    loc[2] = ':';
//    loc[3] = gps_utc_inpt[2];
//    loc[4] = gps_utc_inpt[3];
//    loc[5] = ':';
//    loc[6] = gps_utc_inpt[4];
//    loc[7] = gps_utc_inpt[5];
//    loc[8] = 0;
//}
//
//int geo_coor_parse(char* geo_coor_inpt, char dir)
//{
//    size_t len = strlen(geo_coor_inpt);
//    size_t len_min = 7;
//    size_t len_deg = len - len_min;
//    char deg_buf[len_deg + 1];
//    char min_buf[8];
//    char* end;
//    for (int i = 0; i < len; ++i)
//    {
//        if (i < len_deg)
//            deg_buf[i] = geo_coor_inpt[i];
//        else
//            min_buf[i - len_deg] = geo_coor_inpt[i];
//    }
//    deg_buf[len_deg] = 0;
//    min_buf[len_min] = 0;
//
//    double deg_freq = strtod(min_buf,&end) / 60;
//    double deg = (strtod(deg_buf,&end) + deg_freq) * 1E7;
//
//    if (dir == 'S' || dir == 'W')
//    {
//        deg = deg * (-1);
//    }
//    return (int) deg;
//}
//
//int parse_sentence(GPS_LOCATION_INFO *location, char *gps_output, char *title,
//                   int min_sentence_len, int max_sentence_len, int validation_idx)
//{
//    char* str = gps_output;
//    char valid = 'V';
//    char* coor_to_parse;
//    char *ptr_end;
//    char buf[MAX_ENMA_LINE_LENGTH + 1];
//
//    while (is_not_valid(valid, title) && str != NULL)
//    {
//        str = strstr(str, title);
//        if (str == NULL)
//            return -1;
//        str++;
//        if ( strlen(str) < min_sentence_len || strstr(str, "\n") == NULL)
//            return -1;
//        for (int i = 0; i < max_sentence_len; ++i)
//        {
//            char current = str[i];
//            buf[i] = current;
//            if (current == '\n')
//            {
//                buf[i + 1] = 0;
//                break;
//            }
//        }
//        int i = 0;
//        char *ptr = strtok(buf, delim);
//        while (ptr != NULL){
//            ptr = strtok(NULL, delim);
//            if (i == validation_idx)
//                valid = *ptr;
//            if (strcmp(title, GGA_TITLE) == 0)
//            {
//                if (i == GGA_TIME_IDX)
//                {
//                    utc_parse(ptr, location->fixtime);
//                }
//                if (i == GGA_LAT_IDX)
//                    coor_to_parse = ptr;
//                else if (i == GGA_LAT_DIR_IDX)
//                    latitude = geo_coor_parse(coor_to_parse, (char) ptr);
//                else if (i == GGA_LONG_IDX)
//                    coor_to_parse = ptr;
//                else if (i == GGA_LONG_DIR_IDX)
//                    longitude = geo_coor_parse(coor_to_parse, (char) ptr);
//                if (i == GGA_NUM_SATS_IDX)
//                {
//                    if (!ptr)
//                        return -1;
//                    location->num_sats = (uint8_t) strtod(ptr, &ptr_end); // todo BUG: crashes when gps unlocked
//                }
//                else if (i == GGA_ALT_IDX)
//                    altitude = (int) (strtod(ptr, &ptr_end)) * 100;
//                else if (i == GGA_VALID_FIX_IDX)
//                    valid_fix = 1;
//            }
//            else if (strcmp(title, GSA_TITLE) == 0)
//            {
//                if (i == GSA_HDOP_IDX)
//                {
//                    if (ptr != NULL)
//                    {
//                        hdop = (uint8_t) (strtod(ptr, &ptr_end) * 5);
//                        location->hdop = hdop;
//                    }
//                    else
//                        return -1;
//                }
//            }
//            ++i;
//        }
//        if (strcmp(title, GSA_TITLE) == 0 && is_not_valid(valid, title) == 0)
//        {
//            location->latitude = latitude;
//            location->longitude = longitude;
//            location->altitude = altitude;
//            location->valid_fix = valid_fix;
//        }
//    }
//    if (is_not_valid(valid, title) == 0)
//    {
//        return 0;
//    }
//    else
//    {
//        location->valid_fix = 0;
//        return -1;
//    }
//}
//
//int parse_rmc_sentence(GPS_LOCATION_INFO *location, char *gps_output, char* date, char* time)
//{
//    char delim[] = ",";
//    char* title = RMC_TITLE;
//    int min_sentence_len = MIN_RMC_LENGTH;
//    int max_sentence_len = MAX_ENMA_LINE_LENGTH;
//    int validation_idx = RMC_ACTIVATION_IDX;
//    char* str = gps_output;
//    char valid = 'V';
//    char *ptr_end;
//    char buf[MAX_ENMA_LINE_LENGTH + 1];
//
//    while (is_not_valid(valid, title) && str != NULL)
//    {
//        str = strstr(str, title);
//        if (str == NULL)
//            return -1;
//        str++;
//        if ( strlen(str) < min_sentence_len || strstr(str, "\n") == NULL)
//            return -1;
//        for (int i = 0; i < max_sentence_len; ++i)
//        {
//            char current = str[i];
//            buf[i] = current;
//            if (current == '\n')
//            {
//                buf[i + 1] = 0;
//                break;
//            }
//        }
//        int i = 0;
//        char *ptr = strtok(buf, delim);
//        while (ptr != NULL)
//        {
//            ptr = strtok(NULL, delim);
//            if (i == validation_idx)
//                valid = *ptr;
////            else if (i == 0)
////            {
////                strcpy(time, ptr);
////            }
//            else if (i == 8)
//            {
//                location->date[0] = ptr[0];
//                location->date[1] = ptr[1];
//                location->date[2] = ptr[2];
//                location->date[3] = ptr[3];
//                location->date[4] = ptr[4];
//                location->date[5] = ptr[5];
//                location->date[6] = 0;
//            }
//            ++i;
//        }
//    }
//    if (is_not_valid(valid, title) == 0)
//    {
//        return 0;
//    }
//    else
//    {
//        return -1;
//    }
//}
//
//unsigned int get_epoch(char* date, char* time, char* buf)
//{
//	printf("%s\n", date);
//	printf("%s\n", time);
//	int months_days_arr[] = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334};
//    unsigned int years, month, days, hours, mins, secs;
//    unsigned int epoch_sec = 0;
//    int sec_in_day = 86400;
//    int days_in_year = 365;
//    years = (date[4] - '0') * 10 + (date[5] - '0') + 30;
//    month = (date[2] - '0') * 10 + (date[3] - '0');
//    days = (date[0] - '0') * 10 + (date[1] - '0');
//    hours = (time[0] - '0') * 10 + (time[1] - '0');
//    mins = (time[3] - '0') * 10 + (time[4] - '0');
//    secs = (time[6] - '0') * 10 + (time[7] - '0');
//    epoch_sec = secs + (mins*60) + (hours * 60 * 60) + days * sec_in_day +
//    months_days_arr[month - 1] * sec_in_day + years * days_in_year * sec_in_day;
//
//    sprintf(buf, "%u", epoch_sec);
//
//    return epoch_sec;
//}


bool parse_reg_status(char* modem_output, int* status)
{
    char* comm_prefix ="+CREG:";
    char *ptr_end;

    if (!modem_output)
    {
        return 0;
    }

    char* str = strstr(modem_output, comm_prefix);

    if (!str)
    {
        return 0;
    }

    str = strstr(str, ",");
    if (!str)
    {
        return 0;
    }

    char c_dig = str[1];
    int i_dig = c_dig - '0';
    if (i_dig < 0 || i_dig > 5)
        return 0;
    *status = i_dig;
    return 1;
}


bool parse_sig_quality(char* modem_output, int* csq) {
    char *comm_prefix = "+CSQ:";

    if (!modem_output) {
        return 0;
    }

    char *str = strstr(modem_output, comm_prefix);

    if (!str) {
        return 0;
    }

    str = strstr(str, " ");
    if (!str) {
        return 0;
    }

    char dig_1 = str[1];
    char dig_2 = str[2];

    int d_dig_1 = dig_1 - '0';

    if (dig_2 == ',')
        *csq = (int) d_dig_1;
    else
    {
        int d_dig_2 = dig_2 - '0';
        int sig_q = 10 * d_dig_1 + d_dig_2;
        if (sig_q < 0 || sig_q > 31)
            return 0;
        *csq = sig_q;
    }
    return 1;
}

bool extract_single_operator_info(OPERATOR_INFO* operator, int op_idx, char* buf)
{
    if (!buf)
    {
        return 0;
    }
    int op_status;
    char* op_name_long;
    char* op_name_short;
    int op_code;
    int op_tech;

    char delim[] = ",";
    char *ptr = strtok(buf, delim);
    if (!ptr)
    {
        return 0;
    }
    if (strlen(ptr) >= 2)
    {
        op_status = ptr[1] - '0';
    }
    else {return 0;}
    ptr = strtok(NULL, delim);
    if (!ptr)
    {
        return 0;
    }
    if (strlen(ptr) >= 3)
    {
        ptr++;
        ptr[strlen(ptr) - 1] = 0;
        op_name_long = ptr;
    }
    else {op_name_long = "";}
    ptr = strtok(NULL, delim);
    if (!ptr)
    {
        return 0;
    }
    if (strlen(ptr) >= 3)
    {
        ptr++;
        ptr[strlen(ptr) - 1] = 0;
//        op_name_short = ptr;
        int ptr_len = strlen(ptr);
        int op_obj_array_len = 20;
        for (int j = 0; j < op_obj_array_len; ++j)
        {
            if (j < ptr_len)
                operator[op_idx].operatorName[j] = ptr[j];
            else
                operator[op_idx].operatorName[j] = 0;
        }
    }
    else
    {
        operator[op_idx].operatorName[0] = '\"';
        operator[op_idx].operatorName[1] = '\"';
        operator[op_idx].operatorName[2] = 0;
    }
    ptr = strtok(NULL, delim);
    if (!ptr)
    {
        return 0;
    }
    if (strlen(ptr) >= 3)
    {
        ptr++;
        ptr[strlen(ptr) - 1] = 0;
        char* endPtr;
        op_code = (int) strtod(ptr, &endPtr);
        operator[op_idx].operatorCode = op_code;
    }
    else {return 0;}
    ptr = strtok(NULL, delim);
    if (!ptr)
    {
        return 0;
    }
    if (strlen(ptr) >= 1)
    {
        op_tech = (int) ptr[0] - '0';
        if (op_tech == 0)
        {
            operator[op_idx].accessTechnology[0] = '2';
            operator[op_idx].accessTechnology[1] = 'G';
            operator[op_idx].accessTechnology[2] = 0;
            operator[op_idx].accessTechnology[3] = 0;
            return 1;
        }
        else if (op_tech == 2)
        {
            operator[op_idx].accessTechnology[0] = '3';
            operator[op_idx].accessTechnology[1] = 'G';
            operator[op_idx].accessTechnology[2] = 0;
            operator[op_idx].accessTechnology[3] = 0;
            return 1;
        }
        else
        {
            return 0;
        }
    }
    return 0;
}


