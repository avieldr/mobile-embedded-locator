#include <stdbool.h>
#include <stdio.h>
#include "em_device.h"
#include "em_cmu.h"
#include "em_chip.h"
#include "em_emu.h"
#include "bsp.h"
#include "em_leuart.h"
#include "em_ldma.h"
#include "bspconfig.h"
#include "retargetserial.h"
#include "display.h"
#include "textdisplay.h"
#include "retargettextdisplay.h"
#include "gps.h"
#include "cellular.h"
#include "shared_tools.h"

#define MAXLEN_PAYLOAD 2000
#define PAYLOAD_BUF_LEN 2001
#define MAXLEN_RESPONSE 2000
#define RESPONSE_BUF_LEN 2001
#define MAXLEN_TOKEN 300
#define TOKEN_BUF_LEN 301

#define SIGNIN_FORMAT "POST /%s HTTP/1.1\r\n" \
                      "accept: application/json, text/plain, */*\r\n" \
                      "Host: %s\r\n" \
                      "Content-Type: application/json\r\n" \
                      "User-Agent: GemaltoModem\r\n" \
                      "Cache-Control: no-cache\r\n" \
                      "Content-Length: %d\r\n" \
                      "Connection: keep-alive\r\n" \
                      "\r\n" \
                      "{\"email\": \"%s\", \"password\": \"%s\"}" \
                      "\r\n\r\n"

#define TRACK_POST_FORMAT "POST /%s HTTP/1.1\r\n" \
                      "accept: application/json, text/plain, */*\r\n" \
                      "authorization: Bearer %s\r\n" \
                      "Host: %s\r\n" \
                      "Content-Type: application/json\r\n" \
                      "User-Agent: GemaltoModem\r\n" \
                      "Cache-Control: no-cache\r\n" \
                      "Content-Length: %d\r\n" \
                      "Connection: keep-alive\r\n" \
                      "\r\n" \
                      "{" \
                      "\"name\": \"bulbul\"," \
                      "\"locations\": {\"coords\": {\"latitude\": \"%s\", \"longitude\": \"%s\"}}," \
                      "\"_id\": \"345\"," \
                      "\"createdAt\": \"%d\"" \
                      "}" \
                      "\r\n\r\n"

#define TRACK_GET_FORMAT "GET /%s HTTP/1.1\r\n" \
                      "accept: application/json, text/plain, */*\r\n" \
                      "authorization: Bearer %s\r\n" \
                      "Host: %s\r\n" \
                      "Content-Type: application/json\r\n" \
                      "User-Agent: GemaltoModem\r\n" \
                      "Cache-Control: no-cache\r\n" \
                      "Content-Length: %d\r\n" \
                      "Connection: keep-alive\r\n" \
                      "\r\n" \
                      "{\"email\": \"\", \"password\": \"\"}" \
                      "\r\n\r\n"

OPERATOR_INFO opList[10];


/***************************************************************************//**
 * @brief SysTick_Handler
 * Interrupt Service Routine for system tick counter
 ******************************************************************************/
void SysTick_Handler(void)
{
  msTicks++;       /* increment counter necessary in Delay()*/
}


void setupLeuart(void)
{
  /* Enable peripheral clocks */
  CMU_ClockEnable(cmuClock_HFPER, true);
  /* Configure GPIO pins */
  CMU_ClockEnable(cmuClock_GPIO, true);
  /* To avoid false start, configure output as high */
  GPIO_PinModeSet(RETARGET_TXPORT, RETARGET_TXPIN, gpioModePushPull, 1);
  GPIO_PinModeSet(RETARGET_RXPORT, RETARGET_RXPIN, gpioModeInput, 0);

  LEUART_Init_TypeDef init = LEUART_INIT_DEFAULT;

  /* Enable CORE LE clock in order to access LE modules */
  CMU_ClockEnable(cmuClock_CORELE, true);

  /* Select LFXO for LEUARTs (and wait for it to stabilize) */
  CMU_ClockSelectSet(cmuClock_LFB, cmuSelect_LFXO);
  CMU_ClockEnable(cmuClock_LEUART0, true);

  /* Do not prescale clock */
  CMU_ClockDivSet(cmuClock_LEUART0, cmuClkDiv_1);

  /* Configure LEUART */
  init.enable = leuartDisable;

  LEUART_Init(LEUART0, &init);

  /* Enable pins at default location */
  LEUART0->ROUTELOC0 = (LEUART0->ROUTELOC0 & ~(_LEUART_ROUTELOC0_TXLOC_MASK
                                               | _LEUART_ROUTELOC0_RXLOC_MASK))
                       | (RETARGET_TX_LOCATION << _LEUART_ROUTELOC0_TXLOC_SHIFT)
                       | (RETARGET_RX_LOCATION << _LEUART_ROUTELOC0_RXLOC_SHIFT);

  LEUART0->ROUTEPEN  = USART_ROUTEPEN_RXPEN | USART_ROUTEPEN_TXPEN;

  /* Set RXDMAWU to wake up the DMA controller in EM2 */
  LEUART_RxDmaInEM2Enable(LEUART0, true);

  /* Finally enable it */
  LEUART_Enable(LEUART0, leuartEnable);
}

bool getGPSLocation(GPS_LOCATION_INFO* location){
    int gps_recievd = 0;

    gps_recievd = GPSGetFixInformation(location);
    if (gps_recievd)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool parseToken(char* inputBuf, char* token, int tokenMaxlen){
    int firstTokenCharIdx = 9;
    char* str = strstr(inputBuf, "\"token\":");
    int len = strlen(str);
    if (!str) { return false; }

    char currChar;
    int i = firstTokenCharIdx;
    while (i < len - 1 && i - firstTokenCharIdx < tokenMaxlen) {
        currChar = str[i];
        if (currChar == '\"') {
            if (str[i + 1] == '}') {
                token[i - firstTokenCharIdx] = 0;
                return true;
            }
            return false;
        }
        token[i - firstTokenCharIdx] = currChar;
        i++;
    }
    return false;
}

bool signin(char *payload, char *response, char* token, char* host, int contentLength,
            char* email, char* pass, char* route)
{
    int len = contentLength + (int) strlen(pass) + (int) strlen(email);
    sprintf(payload, SIGNIN_FORMAT, route, host, len, email, pass);
    if (CellularSendHTTPPOSTRequest(payload, MAXLEN_PAYLOAD, response, MAXLEN_RESPONSE)) {
        printf("response: %s\n", response);
        if (parseToken(response, token, MAXLEN_TOKEN))
        {
            printf("%s %s\n", "parsed token: ", token);
            return true;
        }
        else
        {
            printf("%s\n", "Failed to parse the token");
        }
    }
    else
    {
        printf("%s \n", "signin http msg failed!!");
    }
    return false;
}

bool postTrack(char *payload, char *response, char* token, char* host, int contentLength, char* route, int lat, int lon)
{
    char latStr[12];
    char lonStr[12];
    sprintf(latStr, "%d", lat);
	sprintf(lonStr, "%d", lon);

	int latLen = (int) strlen(latStr);
	int lonLen = (int) strlen(lonStr);

	int len = contentLength + latLen + lonLen;

    sprintf(payload, TRACK_POST_FORMAT, route, token, host, len, latStr, lonStr, time(NULL));
    return  (CellularSendHTTPPOSTRequest(payload, MAXLEN_PAYLOAD, response, MAXLEN_RESPONSE) != -1);
}

bool getTracks(char *payload, char *response, char* token, char* host, int contentLength, char* route)
{
    sprintf(payload, TRACK_GET_FORMAT, route, token, host, contentLength);
    return  (CellularSendHTTPPOSTRequest(payload, MAXLEN_PAYLOAD, response, MAXLEN_RESPONSE) != -1);
}

/***************************************************************************//**
 * @brief  Main function
 ******************************************************************************/
int main(void)
{
  EMU_DCDCInit_TypeDef dcdcInit = EMU_DCDCINIT_STK_DEFAULT;
  CMU_HFXOInit_TypeDef hfxoInit = CMU_HFXOINIT_STK_DEFAULT;

  /* Chip errata */
  CHIP_Init();

  /* Init DCDC regulator and HFXO with kit specific parameters */
  EMU_DCDCInit(&dcdcInit);
  CMU_HFXOInit(&hfxoInit);

  /* Switch HFCLK to HFXO and disable HFRCO */
  CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFXO);
  CMU_OscillatorEnable(cmuOsc_HFRCO, false, false);

  /* Initialize LEUART */
  setupLeuart();

  /* Setup SysTick Timer for 1 msec interrupts  */
  if (SysTick_Config(CMU_ClockFreqGet(cmuClock_CORE) / 1000)) {
    while (1) ;
  }

  /* Initialize the display module. */
  DISPLAY_Init();

  /* Retarget stdio to a text display. */
  if (RETARGET_TextDisplayInit() != TEXTDISPLAY_EMSTATUS_OK) {
    while (1) ;
  }

  GPS_LOCATION_INFO location;

  int iter = 0;

  const OPERATOR_INFO op_info = {"", 0, ""};
  for (int i = 0; i < 10; ++i)
  {
      opList[i] = op_info;
  }
  int csq;
  int numOpsFound;
  int status;
  char ccidNum[20];
  bool connected = false;
  bool profileConnection = false;

  char* route;
  char* pass = "YOUR_PASSWORD_HERE";
  char* email = "YOUR_EMAIL_HERE";
  char token[TOKEN_BUF_LEN];
  char* host = "NGROK_SESSION_ADRESS_HERE";
  char* ip = "INTERNET_SERVICE_PROFILE_IP_HERE";

  char response_buf[RESPONSE_BUF_LEN];
  flushLocalBuffer(&token, MAXLEN_TOKEN);

  CellularInit(NULL);
  GPSInit(NULL);

  SetEchoMode(0);

	 
  while (!profileConnection){
	  profileConnection = CellularSetupInternetConnectionProfile(20) && CellularSetupInternetServiceSetupProfile(ip, 80, 200);
  }

  char payload[PAYLOAD_BUF_LEN];
  while (!strlen(token)) 
  {
  signin(payload, response_buf, token, host, 29, email, pass, "signin"); // todo handle failure
  }

  while(1)
  {
    ++iter;
    printf("%s%d:\n", "Iteration ",iter);
    GPS_LOCATION_INFO location;
	  
    if (getGPSLocation(&location) && strlen(token))
    {
      printf("%s %d,%d\n", "gps:", location.latitude, location.longitude);
      bool ret_val = postTrack(payload, response_buf, token, host, 110, "tracks", location.latitude, location.longitude);
    }
    else
    {
	  if (!strlen(token)) printf("%s\n", "Invalid token");
	  else printf("%s\n", "Failed to retrieve location");
	bool ret_val = postTrack(payload, response_buf, token, host, 116, "tracks", 317497050, 351877183);
    }
  }
}
