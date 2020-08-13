//this is ex9
#include "parser_gps.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define GGA_TIME_IDX 0
#define GGA_LAT_IDX 1
#define GGA_LAT_DIR_IDX 2
#define GGA_LONG_IDX 3
#define GGA_LONG_DIR_IDX 4
#define GGA_ALT_IDX 8
#define GGA_VALID_FIX_IDX 5
#define GGA_NUM_SATS_IDX 6
#define GSA_HDOP_IDX 10

char delim[] = ",";
int32_t latitude;
int32_t longitude;
int32_t altitude;
uint8_t valid_fix;
uint8_t hdop;

int is_not_valid(char valid, char* title)
{

    if (strcmp(title, GGA_TITLE) == 0)
    {
        if (valid != '1' && valid != '2' && valid != '3' && valid != '4' && valid != '5' && valid != '6' &&
            valid != '7' && valid != '8')
            return 1;
    }
    else if (strcmp(title, GSA_TITLE) == 0)
    {
        if (valid != '2' && valid != '3')
            return 1;
    }
    return 0;
}

void utc_parse(char* gps_utc_inpt, char* loc)
{
    loc[0] = gps_utc_inpt[0];
    loc[1] = gps_utc_inpt[1];
    loc[2] = ':';
    loc[3] = gps_utc_inpt[2];
    loc[4] = gps_utc_inpt[3];
    loc[5] = ':';
    loc[6] = gps_utc_inpt[4];
    loc[7] = gps_utc_inpt[5];
    loc[8] = 0;
}

int geo_coor_parse(char* geo_coor_inpt, char dir)
{
    size_t len = strlen(geo_coor_inpt);
    size_t len_min = 7;
    size_t len_deg = len - len_min;
    char deg_buf[len_deg + 1];
    char min_buf[8];
    char* end;
    for (int i = 0; i < len; ++i)
    {
        if (i < len_deg)
            deg_buf[i] = geo_coor_inpt[i];
        else
            min_buf[i - len_deg] = geo_coor_inpt[i];
    }
    deg_buf[len_deg] = 0;
    min_buf[len_min] = 0;

    double deg_freq = strtod(min_buf,&end) / 60;
    double deg = (strtod(deg_buf,&end) + deg_freq) * 1E7;

    if (dir == 'S' || dir == 'W')
    {
        deg = deg * (-1);
    }
    return (int) deg;
}

int parse_sentence(GPS_LOCATION_INFO *location, char *gps_output, char *title,
                   int min_sentence_len, int max_sentence_len, int validation_idx)
{
    char* str = gps_output;
    char valid = 'V';
    char* coor_to_parse;
    char *ptr_end;
    char buf[MAX_ENMA_LINE_LENGTH + 1];

    while (is_not_valid(valid, title) && str != NULL)
    {
        str = strstr(str, title);
        if (str == NULL)
            return -1;
        str++;
        if ( strlen(str) < min_sentence_len || strstr(str, "\n") == NULL)
            return -1;
        for (int i = 0; i < max_sentence_len; ++i)
        {
            char current = str[i];
            buf[i] = current;
            if (current == '\n')
            {
                buf[i + 1] = 0;
                break;
            }
        }
        int i = 0;
        char *ptr = strtok(buf, delim);
        while (ptr != NULL){
            ptr = strtok(NULL, delim);
            if (i == validation_idx)
                valid = *ptr;
            if (strcmp(title, GGA_TITLE) == 0)
            {
                if (i == GGA_TIME_IDX)
                {
                    utc_parse(ptr, location->fixtime);
                }
                if (i == GGA_LAT_IDX)
                    coor_to_parse = ptr;
                else if (i == GGA_LAT_DIR_IDX)
                    latitude = geo_coor_parse(coor_to_parse, (char) ptr);
                else if (i == GGA_LONG_IDX)
                    coor_to_parse = ptr;
                else if (i == GGA_LONG_DIR_IDX)
                    longitude = geo_coor_parse(coor_to_parse, (char) ptr);
                if (i == GGA_NUM_SATS_IDX)
                {
                    if (!ptr)
                        return -1;
                    location->num_sats = (uint8_t) strtod(ptr, &ptr_end); // todo BUG: crashes when gps unlocked
                }
                else if (i == GGA_ALT_IDX)
                    altitude = (int) (strtod(ptr, &ptr_end)) * 100;
                else if (i == GGA_VALID_FIX_IDX)
                    valid_fix = 1;
            }
            else if (strcmp(title, GSA_TITLE) == 0)
            {
                if (i == GSA_HDOP_IDX)
                {
                    if (ptr != NULL)
                    {
                        hdop = (uint8_t) (strtod(ptr, &ptr_end) * 5);
                        location->hdop = hdop;
                    }
                    else
                        return -1;
                }
            }
            ++i;
        }
        if (strcmp(title, GSA_TITLE) == 0 && is_not_valid(valid, title) == 0)
        {
            location->latitude = latitude;
            location->longitude = longitude;
            location->altitude = altitude;
            location->valid_fix = valid_fix;
        }
    }
    if (is_not_valid(valid, title) == 0)
    {
        return 0;
    }
    else
    {
        location->valid_fix = 0;
        return -1;
    }
}
