//this is ex9
#include <stdint.h>
#include "cellular.h"
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include "parser_cell.h"
#include "serial_io_cell.h"


#define BAUD 115200
#define BUF_LEN 384
#define MAX_INIT_TRIES 20
#define MAX_CHECK_MODEM_TRIES 5
#define NUM_OF_RETRIES_SETUP_CONNECTION_PROFILE 5
#define NUM_OF_RETRIES_SETUP_SERVICE_PROFILE 5

#define CHECK_MODEM_RECV_TIMEOUT 1000
#define SIGNAL_QUALITY_RECV_TIMEOUT 500
#define REG_STATUS_RECV_TIMEOUT 1000
#define GET_OPERATORS_FIRST_RECV_TIMEOUT 38000
#define GET_OPERATORS_EXTRA_RECV_TIMEOUT 500
#define URL_MAX_LEN 100


char buf[BUF_LEN + 1];






bool get_single_op_info(char* input_buf, char* op_buff, char** str_tmp)
{
    int curr_op_buff_idx = 0;
    bool fill_mode = 0;
    bool curr_filled = 0;

    while (!curr_filled)
    {
        char curr = (*str_tmp)[0];
        (*str_tmp)++;
        if (!curr)
        {
            unsigned int read_bytes = 0;
            read_bytes = SerialRecvCellular(input_buf, BUF_LEN - 1, GET_OPERATORS_EXTRA_RECV_TIMEOUT);
            if (!read_bytes)
            {
                return 0;
            }
            else
            {
                (*str_tmp) = buf;
                continue;
            }
        }
        if (curr == '(')
        {
            fill_mode = 1;
        }
        if (fill_mode)
        {
            op_buff[curr_op_buff_idx] = curr;
            ++curr_op_buff_idx;
        }
        if (curr == ')')
        {
            fill_mode = 0;
            curr_filled = 1;
        }
    }
    return 1;
}

void CellularInit(char *port)
{
    printf("%s\n", "-->Initializing Cellular Connection...");
    bool ret_val = 0;
    for (int i = 0; i < MAX_INIT_TRIES; ++i)
    {
        printf("%s\n", "-->Trying to open serial comm");
        ret_val = SerialInitCellular(port, BAUD);

        if (ret_val)
        {
            printf("%s\n", "---->Initialized successfully");
            return;
        }

        if (i == MAX_INIT_TRIES - 1)
        {
            return;
        }
        printf("%s%d%c%d%s\n", "Retry num. ", i + 1, '/', MAX_INIT_TRIES ," in 10 seconds...");
        delay(10);
    }
    printf("%s\n", "Max num. of trues reached --> Serial init failed.");
}

void SetEchoMode(bool on)
{
    if (on)
    {
        printf("%s\n", "-->Setting echo mode ON");
        SerialSendCellular("ATE1\r\n", 7);
    }
    else
    {
        printf("%s\n", "-->Setting echo mode OFF");
        SerialSendCellular("ATE0\r\n", 7);
    }

}


void CellularDisable()
{
    printf("%s\n", "-->Disabling Cellular Connection...");
    SerialDisableCellular();
    printf("%s\n", "---->Cellular Connection disabled. Bye Bye!");

}
void ResetAtCommand()
{
//    SerialSend("A", 4);
    SerialFlushInputBuff();
}

bool CellularCheckModem(void)
{
    printf("%s\n", "-->Checking connection to modem");

    bool send_ret_val = 0;
    unsigned int read_bytes = 0;

    char* valid_command = "AT\r\n";


    for (int i = 0; i < MAX_CHECK_MODEM_TRIES + 1; ++i)
    {
        if (i > 0) {
            printf("%s\n", "---->Did not received AT answer\n---->CONNECTIVITY QUERY FAILED!!");
            printf("%s%d%c%d%s\n", "Retry num. ", i, '/', MAX_CHECK_MODEM_TRIES ," in 3 seconds...");
            delay(3);
        }
        send_ret_val = SerialSendCellular(valid_command, 4);
        if (!send_ret_val)
        {
            continue;
        }

//    flushLocalBuffer(buf, BUF_LEN);
        read_bytes = SerialRecvCellular(buf, BUF_LEN - 1, CHECK_MODEM_RECV_TIMEOUT);
        if (!read_bytes)
        {

            continue;
        }

        char* ok_verifier = "OK";
        if (strstr(buf, ok_verifier))
        {
            printf("%s\n\n", "---->Modem is connected!");
            return 1;
        }
        else
        {

            continue;
        }
    }
    printf("%s\n\n", "---->Modem is off!");
    return 0;

}


bool CellularGetRegistrationStatus(int *status)
{
    ResetAtCommand();

    printf("%s\n", "-->Trying to get registration status");

    bool send_ret_val = 0;
    unsigned int read_bytes = 0;

    char* creg_command = "AT+CREG?\r\n";
    send_ret_val = SerialSend(creg_command, 11);

    if (!send_ret_val)
    {
        printf("%s\n", "---->Did not succeed sending CREG command\n---->REGISTRATION FAILED!!");
        return 0;
    }
//    flushLocalBuffer(buf, BUF_LEN);
    read_bytes = SerialRecvCellular(buf, BUF_LEN - 1, REG_STATUS_RECV_TIMEOUT);

    if (!read_bytes)
    {
        printf("%s\n", "---->Did not received CREG answer\n-->REGISTRATION FAILED!!");
        return 0;
    }
    else
    {
        if (parse_reg_status(buf, status))
        {
            printf("\n%s%d\n", "---->Registration status: ", *status);
            return 1;
        }
        return 0;
    }
}


bool CellularGetSignalQuality(int *csq)
{
    printf("%s\n", "-->Trying to get signal quality");


    char* csq_command = "AT+CSQ\r\n";
    unsigned int read_bytes = 0;
    bool send_ret_val = 0;

    send_ret_val = SerialSend(csq_command, 8);

    if (!send_ret_val)
    {
        printf("%s\n", "---->Did not succeed sending CSQ command\n---->SIGNAL STRENGTH QUERY FAILED!!");
        return 0;
    }

//    flushLocalBuffer(buf, BUF_LEN);
    read_bytes = SerialRecvCellular(buf, BUF_LEN - 1, SIGNAL_QUALITY_RECV_TIMEOUT);

    if (!read_bytes)
    {
        printf("%s\n", "---->Did not received CSQ answer\n---->SIGNAL STRENGTH QUERY FAILED!!");
        return 0;
    }

    else
    {
        if (parse_sig_quality(buf, csq))
        {
            printf("%s%d\n\n", "---->Signal_quality: ", *csq);

            return 1;
        }
        return 0;
    }
}


bool CellularSetOperator(int mode, char *operatorName)
{
    char cops_command[32] = {'A','T','+','C','O','P','S','='};
    unsigned int timeout_ms;
    unsigned int maxlen = 0;

    if (mode == 0)
    {
        printf("%s\n", "-->Trying to set operator: Auto connect");

        maxlen = 12;
        timeout_ms = 8000;
        strcat(cops_command, "0\r\n");
    }
    else if (mode == 1)
    {
        printf("%s%s\n", "-->Trying to set operator: ", operatorName);

        maxlen = 32;
        timeout_ms = 10000;
        strcat(cops_command, "1,2,\"");
        strcat(cops_command, operatorName);
        strcat(cops_command, "\"");
        strcat(cops_command, "\r\n");
    }
    else if (mode == 2)
    {
        printf("%s\n", "-->Trying to set operator: Disconnect operator");

        maxlen = 12;
        timeout_ms = 12000;
        strcat(cops_command, "2\r\n");
    }

    bool send_ret_val = 0;
    unsigned int read_bytes = 0;

    send_ret_val = SerialSend(cops_command, maxlen);


    if (!send_ret_val)
    {
        printf("%s\n", "---->Did not succeed sending CSQ command\n---->SIGNAL STRENGTH QUERY FAILED!!");
        return 0;
    }

//    flushLocalBuffer(buf, BUF_LEN);
    read_bytes = SerialRecvCellular(buf, BUF_LEN - 1, timeout_ms);

    if (!read_bytes)
    {
        printf("%s\n", "---->Did not received COPS answer---->QUERY FAILED!!");
        return 0;
    }
    else
    {
        char* ok_verifier = "OK";
        if (strstr(buf, ok_verifier))
        {
            printf("%s\n\n", "---->Operator was set successfully!");
            return 1;
        }
        printf("%s\n\n", "---->Failed to set the operator!");
        return 0;
    }
}


bool CellularGetOperators(OPERATOR_INFO *opList, int maxops, int *numOpsFound)
{
    ResetAtCommand();

    printf("%s\n", "-->Trying to get cellular operators");
    char* cops_command = "AT+COPS=?\r\n";
    unsigned int read_bytes = 0;
    bool send_ret_val = 0;
    send_ret_val = SerialSendCellular(cops_command, 11);

    if (!send_ret_val)
    {
        printf("%s\n", "---->Did not succeed sending COPS command\n---->CELLULAR OPERATORS QUERY FAILED!!");
        return 0;
    }

    read_bytes = SerialRecvCellular(buf, BUF_LEN - 1, GET_OPERATORS_FIRST_RECV_TIMEOUT);
    if (!read_bytes)
    {
        printf("%s\n", "---->Did not received COPS answer\n---->CELLULAR OPERATORS QUERY FAILED!!");
        return 0;
    }

    char curr_op_buff[BUF_LEN];
    char* str_tmp = buf;
    int ops_found = 0;
    while (ops_found < maxops)
    {
        flushLocalBuffer(curr_op_buff, BUF_LEN);

        if (get_single_op_info(buf, curr_op_buff, &str_tmp))
        {
            if (extract_single_operator_info(opList, ops_found, curr_op_buff))
            {
                ops_found++;
            }
        }
        else
        {
            break;
        }
    }
    *numOpsFound = ops_found;
    if (ops_found)
    {
        printf("%s%d\n\n", "---->Operators retrieved successfully, numOpFound: ", ops_found);
    }
    else
    {
        printf("%s\n\n", "---->Failed to retrieve operators");
    }
    return 1;
}

bool CellularGetICCID(char* iccid, int maxlen)
{
    ResetAtCommand();

    printf("%s\n", "-->Trying to get sim number");

    bool send_ret_val = 0;
    unsigned int read_bytes = 0;

    char* ccid_command = "AT+CCID\r\n";
    send_ret_val = SerialSend(ccid_command, 11);

    if (!send_ret_val)
    {
        printf("%s\n", "---->Did not succeed sending CCID command\n");
        return 0;
    }

    read_bytes = SerialRecvCellular(buf, maxlen - 1, REG_STATUS_RECV_TIMEOUT);  // todo set timeout

    if (!read_bytes)
    {
        printf("%s\n", "---->Did not received CCID answer\n");
        return 0;
    }

    char* ok_verifier = "+CCID: ";
    char *ptr = strstr(buf, ok_verifier);
    if (ptr && strlen(ptr) >= 26)
    {
        ptr += 7;
        strncpy(iccid, ptr, 19);
        iccid[19] = 0;
        printf("%s%s\n\n", "---->CCID received: ", iccid);
        return 1;
    }
    else
    {
        printf("%s\n\n", "---->CCID response is corrupted.");
        return 0;
    }
}

int CellularGetLastError(char *errmsg, int errmsg_max_len)
{
    return 1;
}


bool CellularGetCops() {
    ResetAtCommand();

    printf("%s\n", "-->");
    char *cops_command = "AT+COPS?\r\n";
    unsigned int read_bytes = 0;
    bool send_ret_val = 0;
    send_ret_val = SerialSend(cops_command, 11);

    if (!send_ret_val) {
        printf("%s\n", "---->Did not succeed sending COPS command\n---->CELLULAR OPERATORS QUERY FAILED!!");
        return 0;
    }

    read_bytes = SerialRecvCellular(buf, BUF_LEN - 1, REG_STATUS_RECV_TIMEOUT);
    if (!read_bytes) {
        printf("%s\n", "---->Did not received COPS answer\n---->CELLULAR OPERATORS QUERY FAILED!!");
        return 0;
    }
    return 1;
}
bool CellularSetupInternetConnectionProfile(int inact_time_sec)
{
    char *conTypeStr = "AT^SICS=0,conType,\"GPRS0\"\r\n";
    char *apnStr = "AT^SICS=0,apn,\"postm2m.lu\"\r\n";
    char *inactToStr = "AT^SICS=0,inactTO,\"";
    char *suffix = "\"\r\n";

    char snum[5];
    char strBuf[80];
    char response_buf[131];

    for (int i = 0; i < NUM_OF_RETRIES_SETUP_CONNECTION_PROFILE; ++i)
    {
        if (i > 0) {
            printf("%s\n", "Setup failed - Retries in 1 sec.");
            delay(1);

        }
        printf("%s\n", "-->Setting up Internet connection profile");
        strcpy(strBuf, conTypeStr);
        if (!send_ok_response_command(response_buf, strBuf, 31, CHECK_MODEM_RECV_TIMEOUT)) {
            continue;
        }

        strcpy(strBuf, apnStr);
        if (!send_ok_response_command(response_buf, strBuf, 31, CHECK_MODEM_RECV_TIMEOUT)) {
            continue;
        }

        sprintf(snum, "%d", inact_time_sec);
        strcpy(strBuf, inactToStr);
        strcat(strBuf, snum);
        strcat(strBuf, suffix);
        if (!send_ok_response_command(response_buf, strBuf, 29, CHECK_MODEM_RECV_TIMEOUT)) {
            continue;
        }
        printf("%s\n", "-->Internet connection profile was set successfully");
        return 1;
    }

    printf("%s\n", "-->Failed to setup internet connection profile.");
    return 0;

}


bool CellularSetupInternetServiceSetupProfile(char *IP, int port, int keepintvl_sec)
{

    char local_buf[URL_MAX_LEN +101];
    char response_buf[131];
    int  response_max_len = 100;
    char snum[7];


    for (int i = 0; i < NUM_OF_RETRIES_SETUP_SERVICE_PROFILE; ++i)
    {
        if (i > 0) {
            printf("%s\n", "Setup failed - Retries in 1 sec.");
            send_ok_response_command(response_buf, "AT^SISC=1\r\n", response_max_len, 1000);
            delay(1);
        }
        printf("%s\n", "-->Setting up Internet service setup profile");
        if (!send_ok_response_command(response_buf, "AT^SISS=1,\"SrvType\",\"Socket\"\r\n", 30, 1000))
        {
            continue;
        }
//        printf("%s", "SENT: AT^SISS=1,\"SrvType\",\"Socket\"\r\n");

        if (!send_ok_response_command(response_buf, "AT^SISS=1,\"conId\",\"0\"\r\n",
                response_max_len, 1000))
        {
            continue;
        }
//        printf("%s", "SENT: AT^SISS=1,\"conId\",\"0\"\r\n");

        flushLocalBuffer(local_buf, URL_MAX_LEN +100);
        strcat(local_buf, "AT^SISS=1,\"address\",\"socktcp://");
        strcat(local_buf, IP);
        strcat(local_buf, ":");
        sprintf(snum, "%d", port);
        strcat(local_buf, snum);
        strcat(local_buf, ";etx;timer=");
        sprintf(snum, "%d", keepintvl_sec);
        strcat(local_buf, snum);
        strcat(local_buf, "\"\r\n");

        if (!send_ok_response_command(response_buf, local_buf, response_max_len, 4000)) {
            continue;
        }
        return 1;
    }

    printf("%s\n", "-->Failed to setup internet service setup profile.");
    return 0;

}

int CellularSendHTTPPOSTRequest(char *payload, int payload_len, char *response, int response_max_len)
{
    char response_buf[1001];
    flushLocalBuffer(response_buf, 1000);
    flushLocalBuffer(response, response_max_len);
    send_ok_response_command(response_buf, "AT^SISC=1\r\n", response_max_len, 1000);


    printf("%s\n", "-->Sending http request");

    SerialSendCellular("AT^SISO=1\r\n", 14);
//    if (!send_ok_response_command(response_buf, "AT^SISO=1\r\n", 14, 2000)) {
//    	return -1;
//	}

    if (!awaitReceviediResponse(response_buf, "^SISW: 1,1",6000)){
    	send_ok_response_command(response_buf, "AT^SISC=1\r\n", response_max_len, 1000);
        return -1;
    }
    delay(2);
    SerialSendCellular("AT^SIST=1\r\n", 14);
    if (!awaitReceviediResponse(response_buf, "CONNECT", 6000)) { return -1;}
    SerialSendCellular(payload, payload_len);
    int rec_bytes = SerialRecvCellular(response, 1000, 6000);
    delay(2);
    SerialSend("+++", 4);
    delay(2);
    send_ok_response_command(response_buf, "AT^SISC=1\r\n", response_max_len, 1000);
//    awaitReceviediResponse(response_buf, "NO CARRIER", 30000);
//    printf("%s%s\n", "rec: ", response_buf);
//    strcpy(response, response_buf);
//    if (!send_ok_response_command(response_buf, "AT^SISC=1\r\n", response_max_len, 1000)) { return -1;}

    if (rec_bytes) { return rec_bytes; }
    return -1;
}



bool send_ok_response_command(char* response_buf, char* command, int maxlen, unsigned int timeout_ms)
{
    bool send_ret_val = 0;
    unsigned int read_bytes = 0;
    send_ret_val = SerialSendCellular(command, maxlen);
    if (!send_ret_val)
    {
        return 0;
    }

    flushLocalBuffer(response_buf, 130);

    read_bytes = SerialRecvCellular(response_buf, 130, timeout_ms);
    if (!read_bytes)
    {
        return 0;
    }

    char* ok_verifier = "OK";
    if (strstr(response_buf, ok_verifier))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

int awaitReceviediResponse(char* response_buf, char* response_verifier, unsigned int max_timeout_ms)
{
    flushLocalBuffer(response_buf, 130);
    unsigned int timeout_ms = 1000;
    int max_iter = max_timeout_ms / timeout_ms;
    int read_bytes = 0;

    char buf[131];

    for (int i = 0; i < max_iter; ++i) {



        read_bytes += SerialRecvCellular(buf, 130, 1000);
        strcat(response_buf, buf);
        if (strstr(response_buf, response_verifier))
        {
            return read_bytes;
        }
    }

    return 0;
}



