//this is ex9
#include <stdio.h>
#include "gps.h"

#include "parser_gps.h"
#include "serial_io_gps.h"

#define MAX_TRIES 10
#define BAUD 9600
#define BUF_LEN 720


void GPSInit()
{
    char *pcCommPort = "COM6";
    bool ret_val = 0;
    int iter_count = 0;
    while (!ret_val)
    {
        ++iter_count;
        ret_val = SerialInit(pcCommPort, BAUD);
        if (ret_val)
        {
            return;
        }
        else
        {
            printf("%s\n", "Failed to init s_port, try again in 3 seconds..");
//            if (iter_count < MAX_TRIES)
//            {
//                printf("%s\n", "Failed to init s_port, try again in 3 seconds..");
//            }
//            else
//            {
//                printf("%s", "Failed to init s_port after 10 tries, Rebooting...");
////			TODO reboot!!
//            }
        }
        delay(3);
    }
}

void GPSDisable()
{
    SerialDisable();
}

uint32_t GPSGetReadRaw(char *buf, uint32_t maxlen)
{
    uint32_t timeout_ms = 500;
    uint16_t bytes_read;
    bytes_read = SerialRecv(buf, maxlen, timeout_ms);
    if (!bytes_read)
        printf("%s\n", "Failed to read from s_port");
    return bytes_read;
}
void flush(char* buf, int len)
{
	for (int j = 0; j < len - 1; ++j)
	{
		buf[j] = 0;
	}
}
bool GPSGetFixInformation(GPS_LOCATION_INFO *location)
{

    char row_buf[BUF_LEN];
    uint32_t maxlen = BUF_LEN - 1;

    int gga_ret_val = -1;
    int gsa_ret_val = -1;
    int num_of_serial_reads = 0;

    while (gga_ret_val == -1 || gsa_ret_val == -1)
    {
        uint32_t bytes_read = 0;
        while (bytes_read == 0)
        {
//        	flush(row_buf, BUF_LEN - 1);
            bytes_read = GPSGetReadRaw(row_buf, maxlen);
            ++num_of_serial_reads;
            if (num_of_serial_reads >= MAX_TRIES)
                return 0;
        }
        if (gga_ret_val == -1)
        {
        	printf("%s\n", "GPSGetFixInformation");
            gga_ret_val = parse_sentence(location, row_buf, GGA_TITLE, MIN_GGA_LENGTH, MAX_ENMA_LINE_LENGTH, GGA_ACTIVATION_IDX);
        }
        if (gsa_ret_val == -1)
        {
            gsa_ret_val = parse_sentence(location, row_buf, GSA_TITLE, MIN_GSA_LENGTH, MAX_ENMA_LINE_LENGTH, GSA_ACTIVATION_IDX);
        }
    }
    return 1;
}
