//this is ex9
#include <stdbool.h>
#include <time.h>
#include "serial_io_cell.h"




#ifndef EMBEDDED_EX3_CELLULAR_H
#define EMBEDDED_EX3_CELLULAR_H

typedef struct  __OPERATOR_INFO {
    char operatorName[10];
    int operatorCode;
    char accessTechnology[4];
} OPERATOR_INFO;

typedef struct __SIGNAL_INFO {
    int signal_power; // In 2G: dBm. In 3G: RSCP. See ^SMONI responses.
    int EC_n0; // In 3G only. See ^SMONI responses.
    char accessTechnology[4]; // "2G" or "3G"
} SIGNAL_INFO;

void CellularInit(char *port);
void CellularDisable();
bool CellularCheckModem(void);
bool CellularGetRegistrationStatus(int *status);
bool CellularGetSignalQuality(int *csq);
bool CellularSetOperator(int mode, char *operatorName);
bool CellularGetOperators(OPERATOR_INFO *opList, int maxops, int *numOpsFound);
bool CellularGetICCID(char* iccid, int maxlen);


bool CellularSetupInternetConnectionProfile(int inact_time_sec);
bool CellularSetupInternetServiceSetupProfile(char *IP, int port, int keepintvl_sec);

int CellularSendHTTPPOSTRequest(char *payload, int payload_len, char *response, int response_max_len);

int CellularGetLastError(char *errmsg, int errmsg_max_len);


// my extra funcs
bool CellularGetCops();
void SetEchoMode(bool on);
//void delay(int number_of_seconds);
bool send_ok_response_command(char* response_buf, char* command, int maxlen, unsigned int timeout_ms);
void resetFactorySettings();
void reboot();
int awaitReceviediResponse(char* response_buf, char* response_verifier, unsigned int max_timeout_ms);



#endif //EMBEDDED_EX3_CELLULAR_H
