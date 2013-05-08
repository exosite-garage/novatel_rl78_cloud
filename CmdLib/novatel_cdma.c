/*-------------------------------------------------------------------------*
* File:  AtCmdLib.c
*-------------------------------------------------------------------------*
* Description:
*      The GainSpan AT Command Library (AtCmdLib) provides the functions
*      that send AT commands to a GainSpan node and looks for a response.
*      Parse commands are provided to interpret the response data.
*-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
* Includes:
*-------------------------------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h> /* for sprintf(), strstr(), strlen() , strtok() and strcpy()  */
#include <stdlib.h>
#include <ctype.h>
//#include "HostApp.h"
#include "novatel_cdma.h"
//#include <system/console.h>
#include <system/mstimer.h>
#include <system/platform.h>
#include <drv/Glyph/lcd.h>
#include <system/console.h>

/*-------------------------------------------------------------------------*
* Constants:
*-------------------------------------------------------------------------*/
#define ATLIB_RESPONSE_HANDLE_TIMEOUT   100000 /* ms */


/*-------------------------------------------------------------------------*
* Globals:
*-------------------------------------------------------------------------*/
/* Receive buffer to save async and response message from S2w App Node */


/* Flag to indicate whether S2w Node is currently associated or not */
//static uint8_t nodeAssociationFlag = false;
//static uint8_t nodeResetFlag = false; /* Flag to indicate whether S2w Node has rebooted after initialisation  */

// local defines

// connection status bit defines
#define CONN_SGACT      0x01
#define CONN_SD         0x02
#define CONN_CLOUD      0x04
#define CONN_NOCARRIER  0x08
#define CONN_OK         0x10

#define POWER_OFF_PIN   5 // In port 8
#define MODEM_PHON_PIN  5 // In Port 15


// GPRS states
enum MODEM_STATES
{
    MODEM_STATE_AT_CHECK,
    MODEM_STATE_AT_INIT,
    MODEM_STATE_AT_CREG,
    MODEM_STATE_AT_PADS_SET,
    MODEM_STATE_AT_SD,
    MODEM_STATE_AT_SH
};

// others...
#define MODEM_RETRY_INTERVAL        10000   //  AT command retry Interval
#define MODEM_RETRY_MAX              3      //  AT command retry Count Max

// globals
unsigned char modemstate;
volatile unsigned char conn_status = 0;


struct http_packet
{
    char status[20];      // Http status code  ex: 200 OK
    char date[36];        // Date: Fri, 03 Feb 2012 06:17:17 GMT
    char body[255];       //
};


// continously flushes the buffer for mSecs milli seconds
void flushUart0RxBuffer(uint8_t mSecs)
{
    uint8_t dummy;
    MSTimerInit();
    while( MSTimerGet() <= mSecs)
    {
        UART0_ReceiveByte(&dummy);
    }
}

// Gets the Novatel modem in a known reset state
// Does not return until modem is responsive.
void NovatelModemInit()
{
    uint8_t exit = 0;

    // shut echo off
    AtModem_Write((unsigned char *)"ate0\r", 5);

    MSTimerDelay(500);
    // clear ate0 response
    UART0_ResetBuffers();

    //flushUart0RxBuffer(1000);
    // while modem is not responding to 'at'
    while ((AT_REPLY_OK != AtModem_OnCheck()) && (exit != 1))
    {
        // try to get modem out of data mode '+++'
        AtModem_Write("+++\r", 4);

        // Let's also try and power the modem on
        ADPC = 0x09U;   //DEFAULT is all AINx pins are Analog, change 8-15 to
                        // digital
        P15 &= ~(1<<MODEM_PHON_PIN); //SET LOW
        PM15 &= ~(1<<MODEM_PHON_PIN); //SET AS OUTPUT
        P15 |= (1<<MODEM_PHON_PIN);  //SET HIGH
        MSTimerDelay(500); //pulse
        P15 &= ~(1<<MODEM_PHON_PIN); //SET LOW
        PM15 |= (1<<MODEM_PHON_PIN); //SET AS INPUT

        // need to wait ~30 seconds for the disconnect to occurr.
        MSTimerDelay(30000);

        // shut echo off
        AtModem_Write((unsigned char *)"ate0\r", 5);

        MSTimerDelay(1000);

        // clear ate0 response
        UART0_ResetBuffers();

        if(AT_REPLY_OK != AtModem_OnCheck())
        {
            // If still no response,
            // power off modem through gpio

            P8 &= ~(1<<POWER_OFF_PIN); //SET LOW
            PM8 &= ~(1<<POWER_OFF_PIN); //SET AS OUTPUT
            P8 |= (1<<POWER_OFF_PIN);  //SET HIGH
            MSTimerDelay(500); //pulse
            P8 &= ~(1<<POWER_OFF_PIN); //SET LOW


            // wait 1 seconds
            MSTimerDelay(1000);

            // Let's also try and power the modem on
            ADPC = 0x09U;   //DEFAULT is all AINx pins are Analog, change 8-15
                            // to digital
            P15 &= ~(1<<MODEM_PHON_PIN); //SET LOW
            PM15 &= ~(1<<MODEM_PHON_PIN); //SET AS OUTPUT
            P15 |= (1<<MODEM_PHON_PIN);  //SET HIGH
            MSTimerDelay(500); //pulse
            P15 &= ~(1<<MODEM_PHON_PIN); //SET LOW
            PM15 |= (1<<MODEM_PHON_PIN); //SET AS INPUT

            MSTimerDelay(5000);

            // shut echo off
            AtModem_Write((unsigned char *)"ate0\r", 5);


            MSTimerDelay(1000);

            UART0_ResetBuffers();
        }
        else
        {
            // successful reply, exit out
            exit = 1;
        }
        // start over
    }
    // response is valid, modem is in command mode and availble
}



//static struct http_packet http_response;
/*---------------------------------------------------------------------------*
* Routine:  AtModem_Write
*---------------------------------------------------------------------------*
* Description:
*      ATCmdLib callback to write a string of characters to the module.
* Inputs:
*      const uint8_t *txData -- string of bytes
*      uint32_t dataLength -- Number of bytes to transfer
* Outputs:
*      void
*---------------------------------------------------------------------------*/
void AtModem_Write(const void *txData, uint16_t dataLength)
{
    const uint8_t *tx = (uint8_t *)txData;
    while (dataLength--)
    {
        /* Keep trying to send this data until it goes */
        while (!UART0_SendByte(*tx))
        {
        }
        tx++;
    }
}

/*---------------------------------------------------------------------------*
* Routine:  AtModem_Read
*---------------------------------------------------------------------------*
* Description:
*      ATCmdLib callback to read a string of characters from the module.
*      This routine can block if needed until the data arrives.
* Inputs:
*      uint8_t *rxData -- Pointer to a place to store a string of bytes
*      uint16_t dataLength -- Number of bytes to transfer
*      uint8_t blockFlag -- true/non-zero to wait for bytes, else false/zero.
* Outputs:
*      bool -- true if character received, else false
*---------------------------------------------------------------------------*/
bool AtModem_Read(uint8_t *rxData, uint16_t dataLength, uint8_t blockFlag)
{
    bool got_data = false;

    /* Keep getting data if we have a number of bytes to fetch */
    while (dataLength)
    {
        /* Try to get a byte */
        if (UART0_ReceiveByte(rxData))
        {
            /* Got a byte, move up to the next position */
            rxData++;
            dataLength--;
            got_data = true;
        }
        else
        {
            /* Did not get a byte, are we block?  If not, stop here */
            if (!blockFlag)
                break;
        }
    }

    return got_data;
}

/*****************************************************************************
*
*  UART_ReadBufferLine
*
*  \param  buffer: character buffer to tx; len: size of buffer;
*
*  \return failure: -1; success: bytes rx'd
*
*  \brief  Reads characters out of the UART buffer into the caller buffer until
*          a CRLF is found or the buffer size is hit.  Returns number of bytes
*          read into buffer. Populates fullLine with a '1' if a full line was
*          read.
*
*****************************************************************************/
int AtModem_ReadLine(   unsigned char * fullLine,
                        unsigned int checkTime,
                        char *buffer,
                        unsigned int bufSize,
                        char *termStr)
{
    char * lineptr = buffer;
    char * maxptr = buffer + bufSize;
    uint32_t startTime = MSTimerGet();

    while (MSTimerDelta(startTime)< checkTime)
    {
        while (UART0_ReceiveByte((unsigned char *)lineptr))
        {
            if ((lineptr + 1) > maxptr) // too many bytes to read into our line!
            {
                return lineptr - buffer;
            }
            else
                lineptr++;
            if (strnloc(buffer,termStr,lineptr - buffer))
            {
                *fullLine = 1;
                return lineptr - buffer;
            }
        }
    }

    //if we read a partial line, but timed out, we still return how many
    //bytes we read just in case the caller wants to try to assemble the line
    //ConsolePrintf("timedout\r\n");
    *fullLine = 0;
    return lineptr - buffer;
}

/*****************************************************************************
*
*  UART_ReadBufferLine
*
*  \param  buffer: character buffer to tx; len: size of buffer;
*
*  \return failure: -1; success: bytes rx'd
*
*  \brief  Reads characters out of the UART buffer into the caller buffer until
*          the buffer size is hit.  Returns number of bytes
*          read into buffer. Populates fullLine with a '1' if a full line was
*          read.
*
*****************************************************************************/
int AtModem_ReadLineTimeOut(unsigned int checkTime,
                            char *buffer,
                            unsigned int bufSize)
{
    char * lineptr = buffer;
    char * maxptr = buffer + bufSize;
    uint32_t startTime = MSTimerGet();

    while (MSTimerDelta(startTime)< checkTime)
    {
        while (UART0_ReceiveByte((unsigned char *)lineptr))
        {
            if ((lineptr + 1) > maxptr) // too many bytes to read into our line!
            {
                return lineptr - buffer;
            }
            else
                lineptr++;
        }
    }

    //if we read a partial line, but timed out, we still return how many
    //bytes we read just in case the caller wants to try to assemble the line
    //ConsolePrintf("timedout\r\n");
    return lineptr - buffer;
}


// Flush the modems buffer
void AtModem_FlushBuffer()
{
    char linebuffer[200];
    unsigned char fullLine = 1;
    unsigned char line_length = 0;

    unsigned long startTime = MSTimerGet();

    AtModem_Write((unsigned char *)"\r", 1);
    while (fullLine)
    {
        line_length = AtModem_ReadLine( &fullLine,
                                        2000,
                                        linebuffer + line_length,
                                        sizeof(linebuffer) - line_length,
                                        "\r\n");
    }
}

/*****************************************************************************
*
*  AtModem_GetMeid
*
*  \param  buffer: character buffer to for MEID;
*
*  \return failure: 0; success: 1
*
*  \brief  Reads characters out of the UART buffer into the caller buffer until
*          a CRLF is found or the buffer size is hit.  Returns number of bytes
*          read into buffer. Populates fullLine with a '1' if a full line was
*          read.
*
*****************************************************************************/
uint8_t Novatel_GetMeid(char *buffer)
{
    char linebuffer[200];
    unsigned char fullLine = 1;
    unsigned char line_length = 0;
    unsigned char total_bytes = 0;
    uint8_t returnVal = 0;
    unsigned long startTime = MSTimerGet();

    AtModem_Write((unsigned char *)"AT+CGSN\r", 8);        //at+cgsn

    /*************************************************
       45317471582021439:2159004912
       0xa1000013c3e73f:0x80afccf0
    *************************************************/

    while (fullLine)
    {
        line_length = AtModem_ReadLine( &fullLine,
                                        2000, linebuffer + line_length,
                                        sizeof(linebuffer) - line_length,
                                        "\r\n");
        total_bytes += line_length;
    }


    // parsing this response assumes that the meid starts after the third
    // occurance of \n, begins with the ascii char 'x', and is terminated with
    // a ':' char
    uint8_t meidFound = 0;
    uint8_t delimeterCnt = 0;

    for (int i = 0; (i < total_bytes) & (!meidFound); i++)
    {
        // Find third occurance of \n
        if  (linebuffer[i] == '\n')
        {
            delimeterCnt++;
        }

        // MEID starts at third occurance
        if (delimeterCnt == 3)
        {
            meidFound = 1;

            uint8_t meidCopied = 0;

            // advance i past '\n' and 'x'
            i += 2;
            // copy string until ':' found
            for (int j = 0; (j < (total_bytes - i)) & (!meidCopied); j++)
            {

                buffer[j] = linebuffer[i + j];

                // check if next char is ':'
                if (linebuffer[i + j + 1] == ':')
                {
                    meidCopied = 1;

                    // add null terminator
                    buffer[j + 1] = '\0';
                    returnVal = 1;
                }
            }
        }
    }

    /*************************************************/

    return returnVal;
}

/*****************************************************************************
*
* AT_response_check
*
* Checks for the expected response in our ring buffer.  This is a destructive
* read from the ring buffer - once read, the data is gone.
*
*****************************************************************************/
int AtModem_response_check(unsigned int checkTime, char *needle, char *termStr)
{
    char linebuffer[100];
    char * lineptr = linebuffer;
    unsigned char fullLine = 0;
    int result = -1;
    unsigned char line_length = 0;
    unsigned long startTime = MSTimerGet();

    //WriteUILine(LCD_BLANK_LINE, LCD_AT_LINE);
    // a '0' in checkTime means read a long time
    if (!checkTime)
        checkTime = 0xFFFF;

    // keep checking until we either get the response we want or until
    // checkTime is elapsed
    while ((AT_REPLY_OK != result) && ((MSTimerGet() - startTime) < checkTime))
    {
        line_length = AtModem_ReadLine(&fullLine, checkTime, lineptr, sizeof(linebuffer) - (lineptr - linebuffer), termStr);

        //check if we read a full line...
        if (fullLine)
        {
            line_length += lineptr - linebuffer;
            if (strnloc(linebuffer,needle,line_length))
            {
                result = AT_REPLY_OK;
            }
            else //if we didn't find the response, but have a line...
            {
                if (strnloc(linebuffer,"ERROR",line_length)) // check for errors
                {
                    result = AT_REPLY_ERROR;
                }
                else
                {
                    if (strnloc(linebuffer,"OK",line_length)) // check for ?? some edge case???
                    {
                        result = AT_REPLY_SGACT_NOT_SET;
                    }
                }
            }
            //since we did not get AT_REPLY_OK yet, we continue to read the next line
            lineptr = linebuffer;
            line_length = 0;
        }
        else
        if ((lineptr + line_length - linebuffer) <= sizeof(linebuffer))
            lineptr += line_length;                                                           // move lineptr in case we need to read again to get a full line
        else
            break;  //we can't read any more
    }

    //linebuffer[line_length] = 0;
    //WriteUILine(linebuffer, LCD_AT_LINE);

    return result;
}

// if modem is activated returns 1, else 0
// TODO: This needs to be more robust.  assuming everything that isn't
// the checked response is not activated, may not always be true.
uint8_t Novatel_isModemActivate()
{
    AtModem_Write((unsigned char *)"at$OTASP?\r", 10);
    if(AtModem_response_check(1000, "$OTASP: 1, ", "\r\n")==AT_REPLY_OK)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}
/*****************************************************************************
*
*  GPRS_Init
*
*  \param  none
*
*  \return -1:Failure, other: Success
*
*  \brief  Initializes GPRS communications.  Ran at startup.
*
*****************************************************************************/
int AtModem_Init(void)
{
    unsigned char response = AT_REPLY_OK;
    unsigned char retryCount = 0;
    modemstate = MODEM_STATE_AT_CHECK;
    char errorMsg[12] = {'\0'};
    //char LCDString[30];

    DisplayLCD(LCD_LINE8, "Modem Init");

    // disable loopback on modem
    AtModem_Write((unsigned char *)"ate\r", 4);
    // eat up responses
    AtModem_response_check(1000, "ate", "\r");
    AtModem_response_check(1000, "OK", "\r\n");

    ConsolePrintf("Init Novatel Modem Interface\r\n");

    while(modemstate < MODEM_STATE_AT_SD)
    {
        if (response != AT_REPLY_OK)
        {
            sprintf(errorMsg, "C-Error: %d", response);
            DisplayLCD(LCD_LINE7, errorMsg);
            while(1)
                ;
        }
        switch (modemstate)
        {
        case MODEM_STATE_AT_CHECK:
            ConsolePrintf("Modem Check\r\n");
            DisplayLCD(LCD_LINE7, "MODEM: CHECK");
            response = AtModem_OnCheck();
            break;
        case MODEM_STATE_AT_INIT:
            ConsolePrintf("Modem Init Params\r\n");
            DisplayLCD(LCD_LINE7, "MODEM: INIT ");
            response = modem_SetParams();
            break;
        case MODEM_STATE_AT_CREG:
            //sprintf((char *)LCDString, "TEMP: %.1fF", gTemp_F);
            //DisplayLCD(LCD_LINE4, (const uint8_t *)LCDString);
            ConsolePrintf("Check Network Registration\r\n");
            DisplayLCD(LCD_LINE7, "MODEM: REG ?");
            response = AtModem_NetworkCheck();
            if(response ==AT_REPLY_OK)
                DisplayLCD(LCD_LINE7, "MODEM: REG 1");
            else
                DisplayLCD(LCD_LINE7, "MODEM: REG 0");
            break;
        case MODEM_STATE_AT_PADS_SET:
            ConsolePrintf("Set PADS Context\r\n");
            response = send_AT_PADS();
            break;
        }

        if (response == AT_REPLY_OK)
        {
            //WriteUILine("AT Response: OK", LCD_DEBUG_LINE);
            ConsolePrintf("AT Response: OK\r\n");
            if (modemstate == MODEM_STATE_AT_PADS_SET)
                modemstate = MODEM_STATE_AT_SD;
            else
                modemstate++;
        }
        else //error == AT_REPLY_ERROR  or AT_REPLY_TIMED_OUT, retry
        {
            if (modemstate == MODEM_STATE_AT_CREG)
            {
                //WriteUILine("AT Response: No Network Yet", LCD_DEBUG_LINE);
                ConsolePrintf("AT Response: No Network Yet\r\n");
            }
            else
            {
                //WriteUILine("AT Response: ERROR", LCD_DEBUG_LINE);
                ConsolePrintf("AT Response: ERROR\r\n");
            }
            if(retryCount < MODEM_RETRY_MAX)
            {
                retryCount++;
            }
            else
            {
                retryCount = 0;
                modemstate = MODEM_STATE_AT_CHECK; //start over
                AtModem_SocketClose(0);
                //WriteUILine("AT: Init Start Over", LCD_DEBUG_LINE);
                ConsolePrintf("AT: Init Start Over\r\n");
            }
            MSTimerDelay(MODEM_RETRY_INTERVAL);
        }
    }
    //WriteUILine("MODEM Initialized", LCD_DEBUG_LINE);
    ConsolePrintf("MODEM Initialized\r\n");
    return 1;
}




/*****************************************************************************
*
*  AtModem_OnCheck
*
*  \param  none
*
*  \return -1:Failure, other: Success
*
*  \brief  Check if GPRS interface is active...
*
*****************************************************************************/
int AtModem_OnCheck(void)
{
    AtModem_Write((unsigned char *)"at\r", 3);
    return AtModem_response_check(1000, "OK", "\r\n");
}

/*****************************************************************************
*
*  modem_SetParams
*
*  \param  none
*
*  \return -1:Failure, other: Success
*
*  \brief  Check if GPRS interface is active...
*
*****************************************************************************/
int modem_SetParams(void)
{
    return 1;
}

/*****************************************************************************
*
   //! AtModem_NetworkCheck
   //!
   //! \param  none
   //!
   //! \return -1:Failure, other: Success
   //!
   //! \brief  Request status on context activation request.  Telit specific.
*
*****************************************************************************/
int AtModem_NetworkCheck(void)
{
    AtModem_Write((unsigned char *)"AT$CREG\r", 8);
    return AtModem_response_check(5000, "$CREG: 1", "\r\n\r\nOK\r\n");
}


/*****************************************************************************
*
* send_AT_SGACT
*
* Activate context set in Send_AT_CGDCONT.  Telit specific.
*
*****************************************************************************/
int send_AT_PADS(void)
{
    AtModem_Write((unsigned char *)"at$hostif=2\r", 12);
    AtModem_response_check(1000, "OK", "\r\n");
    AtModem_Write((unsigned char *)"at$paddisc=0\r", 13);
    AtModem_response_check(1000, "OK", "\r\n");
    /* TODO:  Correct error handling */
    return AT_REPLY_OK;

}

int ATModem_CellActivate(void)
{
    // disable loopback on modem
    AtModem_Write((unsigned char *)"ate\r", 4);
    // eat up responses
    AtModem_response_check(1000, "ate", "\r");
    AtModem_response_check(1000, "OK", "\r\n");
    
    
    AtModem_Write((unsigned char *)"at$OTASP?\r", 10);
    if(AtModem_response_check(1000, "$OTASP: 1, 0", "\r\n")==AT_REPLY_OK)
    {
        DisplayLCD(LCD_LINE1, "Device      ");
        DisplayLCD(LCD_LINE2, "Appears     ");
        DisplayLCD(LCD_LINE3, "Activated.  ");
        DisplayLCD(LCD_LINE4, "            ");
        DisplayLCD(LCD_LINE5, "Trying to   ");
        DisplayLCD(LCD_LINE6, "activate    ");
        DisplayLCD(LCD_LINE7, "again       ");
        MSTimerDelay(4000);
    }
    DisplayLCD(LCD_LINE1, "Sending     ");
    DisplayLCD(LCD_LINE2, "activation  ");
    DisplayLCD(LCD_LINE3, "cmd to VZW  ");
    DisplayLCD(LCD_LINE4, "network.    ");
    DisplayLCD(LCD_LINE5, "");
    DisplayLCD(LCD_LINE6, "");
    DisplayLCD(LCD_LINE7, "");
    AtModem_Write((unsigned char *)"at+cdv*22899\r", 13);
    MSTimerDelay(4000);
    
    DisplayLCD(LCD_LINE1, "Activation  ");
    DisplayLCD(LCD_LINE2, "cmd sent.   ");
    DisplayLCD(LCD_LINE3, "Awaiting    ");
    DisplayLCD(LCD_LINE4, "Response    ");
    
    uint8_t progressInd = 0;
    while(1)
    {
        if(AtModem_response_check(1000, "$OTASP: 8", "\r\n")==AT_REPLY_OK)
        {
            DisplayLCD(LCD_LINE1, "Network     ");
            DisplayLCD(LCD_LINE2, "Activation  ");
            DisplayLCD(LCD_LINE3, "Successful. ");
            DisplayLCD(LCD_LINE4, "            ");
            DisplayLCD(LCD_LINE5, "Restart     ");
            DisplayLCD(LCD_LINE6, "the RL78G14 ");
            DisplayLCD(LCD_LINE7, "dev board   ");
            
            while(1)
            {
            }
        }
        else
        {
            // toggle the '.' to show user that we're doing something.
            if (progressInd)
            {
                DisplayLCD(LCD_LINE3, "Awaiting    ");
                DisplayLCD(LCD_LINE4, "Response.   ");
            }
            else
            {
                DisplayLCD(LCD_LINE3, "Awaiting    ");
                DisplayLCD(LCD_LINE4, "Response    ");
            }
            // toggle indicator
            progressInd ^= 1;
        }
        MSTimerDelay(2000);
    }
}


/*****************************************************************************
*
* GPRS_SocketOpen
*
* Open a connection with a remote host.  Telit specific.
*
*****************************************************************************/
int32_t AtModem_SocketOpen(char * ip, uint8_t port, uint8_t * cid)
{
    static unsigned char socketFailures = 0;
    //TODO: review hard coded IP addresses -> perhaps swap for replaceable
    //ip via "ip" API call

    /* 30 is big enough for any IP */
    char location[30];
    uint8_t len;

    len = sprintf(location, "atdt\"%s\",%hhu\r", ip, port);

    AtModem_Write(location,len);
    int32_t result = AtModem_response_check(25000, "CONNECT", "\r\n");

    if (result == AT_REPLY_OK)
    {
        socketFailures = 0;
        ConsolePrintf("Socket Open\r\n");
        DisplayLCD(LCD_LINE7, "SOCKET: OPEN");
        *cid = 1; // we only have one connection
    }
    else
    {
        if (result == AT_REPLY_ERROR)
        {
            ConsolePrintf("Socket Error\r\n");
            DisplayLCD(LCD_LINE7, "SOCKET: ERR");
            AtModem_SocketClose(0);
        }
        else
        {
            ConsolePrintf("Socket No Response\r\n");
            DisplayLCD(LCD_LINE7, "SOCKET: DOWN");
        }
        result = -1;
        if (socketFailures++ > 5)
        {
            AtModem_Init();
            socketFailures = 0;
        }
    }
    return result;
}


/*****************************************************************************
*
* GPRS_ExitDataMode
*
* Exite data mode...
*
*****************************************************************************/
void AtModem_ExitDataMode(void)
{
    AtModem_Write((unsigned char *)"+++",3);
    if (AT_REPLY_OK != AtModem_response_check(10000, "NO CARRIER", "\r\n"))
        HAL_BusyWait(1000);
}

/*****************************************************************************
*
* GPRS_SocketClose
*
* Close a connection with a remote host.  Telit specific.
*
*****************************************************************************/
int AtModem_SocketClose(uint8_t cid)
{

    AtModem_Write((unsigned char *)"+++",3);
    DisplayLCD(LCD_LINE7, "SOCKET: OFF");
    return AtModem_response_check(15000, "NO CARRIER", "\r\n");
}

/*****************************************************************************
*
* GPRS_Check_NoCarrier
*
* Check for connection to time out.
*
*****************************************************************************/
int AtModem_CheckNoCarrier(void)
{
    return AtModem_response_check(10000, "NO CARRIER", "\r\n");
}

/* UTILITIES */

/*****************************************************************************
*
* exstrnloc
*
* \param  none
*
* \return none
*
* \brief  Returns position of needle location in haystack.  NOTE: we use limit
*         because haystack may not be null terminated...
*
*****************************************************************************/
char *strnloc(const char * haystack, const char * needle, unsigned char limit)
{
    unsigned char retval = 0;
    unsigned char liveone = 0;
    char * needletest = (char *)needle;

    for (retval = 0; retval < limit; retval++)
    {
        if (*needletest == *haystack++)
            liveone = 1;
        else
        {
            needletest = (char *)needle;
            liveone = 0;
        }

        if (liveone)
        {
            needletest++;
            if (0 == *needletest)
                break;
        }
    }
    if (retval == limit)
        return 0;

    haystack -= strlen(needle);

    return (char *)(haystack);
}


/*****************************************************************************
*
* Data_SocketSend
*
* Send data to the socket
*
*****************************************************************************/
int Modem_Socket_Send(uint8_t sock, char * buffer, long length) {
    AtModem_Write(buffer, length);
    return 0;
}


/*-------------------------------------------------------------------------*
* End of File:  AtCmdLib.c
*-------------------------------------------------------------------------*/

