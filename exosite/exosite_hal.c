/*****************************************************************************
*
*  exosite_hal.c - Exosite hardware & environmenat adapation layer.
*  Copyright (C) 2012 Exosite LLC
*
*  Redistribution and use in source and binary forms, with or without
*  modification, are permitted provided that the following conditions
*  are met:
*
*    Redistributions of source code must retain the above copyright
*    notice, this list of conditions and the following disclaimer.
*
*    Redistributions in binary form must reproduce the above copyright
*    notice, this list of conditions and the following disclaimer in the
*    documentation and/or other materials provided with the
*    distribution.
*
*    Neither the name of Texas Instruments Incorporated nor the names of
*    its contributors may be used to endorse or promote products derived
*    from this software without specific prior written permission.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
*  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
*  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
*  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
*  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
*  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
*  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
*  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
*  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
*  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*****************************************************************************/
#include "exosite.h"
#include "exosite_hal.h"
#include "exosite_meta.h"
#include <string.h>
#include <inc/common.h>
#include <CmdLib/AtCmdLib.h>
#include <drv/Glyph/lcd.h>
#include <system/mstimer.h>
#include <init/hwsetup.h>
#include <system/eeprom.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include <HostApp.h>
#include <system/platform.h>

#include <drv/Glyph/lcd.h>
#include <system/mstimer.h>
#include <system/console.h>
#include <apps/apps.h>
#include "CmdLib/novatel_cdma.h"

// local variables
#define GS_RECV_OFFSET 1
static uint8_t cid = 0xff;
char exometa[META_SIZE];
static int exo_recv_index = -1; // first socket buffer byte was GainSpan command byte

// local functions

// externs
extern void DisplayLCD(uint8_t, const uint8_t *);
extern char *itoa(int n, char *s, int b);

// global variables
#define EXOMETA_ADDR 177

UUIDInterfaceTypes ifaceType = IF_NONE;

void exoHAL_SetIface(UUIDInterfaceTypes type)
{
    ifaceType = type;
}

/*****************************************************************************
*
*  exoHAL_ReadHWMAC
*
*  \param  Interface Number (1 - WiFi), buffer to return hexadecimal MAC
*
*  \return 0 if failure; len of UUID if success;
*
*  \brief  Reads the MAC address from the hardware
*
*****************************************************************************/
int exoHAL_ReadUUID(unsigned char if_nbr, unsigned char * UUID_buf)
{
    int retval = 0;
    int check_count = 0;
    unsigned char idBuf[20];


    switch (if_nbr)
    {
    case IF_GPRS:
        break;
    case IF_ENET:
        break;
    case IF_WIFI:
        ATLIBGS_MSG_ID_E rxMsgId;

        // Read MAC address from gainspan
        rxMsgId = AtLibGs_GetMAC((char *)idBuf);
        if (rxMsgId == ATLIBGS_MSG_ID_OK)
            AtLibGs_ParseGetMacResponse((char *)idBuf);

        while (idBuf[check_count] && (check_count++ < 12))
            ;
        if (check_count > 12 || check_count < 11)
            return retval;

        memcpy(UUID_buf, idBuf, 12); // mac address length : 12
        UUID_buf[12] = 0;
        retval = strlen((char *)UUID_buf);
        break;
    case IF_NOVATEL:
        // Read MEID from Novatel
        if (Novatel_GetMeid((char *)idBuf))
        {
            memcpy(UUID_buf, idBuf, 14); // MEID length : 14
            // add null terminator
            UUID_buf[14] = '\0';
            retval = strlen((char *)UUID_buf);
        }
        else
        {
            // Get MEID failed
            retval = 0;
        }
        break;
    default:
        break;
    }

    return retval;
}


/*****************************************************************************
*
* exoHAL_EnableNVMeta
*
*  \param  None
*
*  \return None
*
*  \brief  Enables meta non-volatile memory, if any
*
*****************************************************************************/
void exoHAL_EnableMeta(void)
{
    return;
}


/*****************************************************************************
*
*  exoHAL_EraseNVMeta
*
*  \param  None
*
*  \return None
*
*  \brief  Wipes out meta information - replaces with 0's.
*
*****************************************************************************/
void exoHAL_EraseMeta(void)
{
    EEPROM_Erase(EXOMETA_ADDR,128);

    return;
}


/*****************************************************************************
*
*  exoHAL_WriteMetaItem
*
*  \param  buffer - string buffer containing info to write to meta; len -
*          size of string in bytes; offset - offset from base of meta
*          location to store the item.
*
*  \return None
*
*  \brief  Stores information to the NV meta structure.
*
*****************************************************************************/
void exoHAL_WriteMetaItem(unsigned char * buffer, unsigned char len, int offset)
{
    EEPROM_Write(EXOMETA_ADDR+offset,(uint8_t *)buffer,len);

    return;
}


/*****************************************************************************
*
*  exoHAL_ReadMetaItem
*
*  \param  buffer - buffer we can read meta info into; len - size of the
*          buffer (max 256 bytes); offset - offset from base of meta to begin
*          reading from;
*
*  \return None
*
*  \brief  Read information from the NV meta structure.
*
*****************************************************************************/
void exoHAL_ReadMetaItem(unsigned char * buffer, unsigned char len, int offset)
{
    EEPROM_Seq_Read(EXOMETA_ADDR+offset,(uint8_t *)buffer, len);

    return;
}


/*****************************************************************************
*
*  exoHAL_SocketClose
*
*  \param  socket - socket handle
*
*  \return None
*
*  \brief  The function closes a socket
*
*****************************************************************************/
void exoHAL_SocketClose(long socket)
{
    if(socket == (long)cid)
    {
        if (ifaceType == IF_NOVATEL)
        {
            AtModem_SocketClose(cid);
            cid = 0xff;
            // 1 second delay required after disconnect
            MSTimerDelay(1000);
            /* open novatel socket */
        }
        else
        {
            AtLibGs_Close(cid);
            cid = 0xff;
            exo_recv_index = -1;
        }
    }
    return;
}


/*****************************************************************************
*
*  exoHAL_SocketOpenTCP
*
*  \param  None
*
*  \return -1: failure; Other: socket handle
*
*  \brief  The function opens a TCP socket
*
*****************************************************************************/
long exoHAL_SocketOpenTCP()
{

    unsigned char server[META_SERVER_SIZE];
    char serverip[20];

    if (cid != 0xff)
        return -1;

    exosite_meta_read(server, META_SERVER_SIZE, META_SERVER);

    // convert to string IP
    sprintf(  serverip,
              "%hhu.%hhu.%hhu.%hhu",
              server[0],
              server[1],
              server[2],
              server[3]);

    if (ifaceType == IF_NOVATEL)
    {
        if(AtModem_SocketOpen(serverip, server[5], &cid) == -1)
        {
            exoHAL_HandleError(EXO_ERROR_CONNECT);
            return -1;
        }
        else
        {
            // delay required after socket opened and before writing to it
            MSTimerDelay(500);
        }
    }
    else
    {
        AtLibGs_TCPClientStart(serverip, server[5], &cid);
    }

    return (long)cid;
}


/*****************************************************************************
*
*  exoHAL_ServerConnect
*
*  \param  None
*
*  \return socket - socket handle
*
*  \brief  The function opens a TCP socket
*
*****************************************************************************/
long exoHAL_ServerConnect(long sock)
{
    //TODO - use DNS or check m2.exosite.com/ip to check for updates
    if( sock == (long)cid)
        return (long)cid;
    else
        return -1;
}

/*****************************************************************************
*
*  exoHAL_SocketSend
*
*  \param  socket - socket handle; buffer - string buffer containing info to
*          send; len - size of string in bytes;
*
*  \return Number of bytes sent
*
*  \brief  Sends data out the network interface
*
*****************************************************************************/
unsigned char exoHAL_SocketSend(long socket, char * buffer, unsigned char len)
{
    App_PrepareIncomingData();
    if(socket == (long)cid)
    {
        if (ifaceType == IF_NOVATEL)
        {
            Modem_Socket_Send(cid, buffer, len);
        }
        else
        {
            AtLibGs_SendTCPData(cid, (char *)buffer, len);
        }
    }
    else
        len = 0;

    return len;
}


/*****************************************************************************
*
*  exoHAL_SocketRecv
*
*  \param  socket - socket handle; buffer - string buffer to put info we
*          receive; len - size of buffer in bytes;
*
*  \return Number of bytes received
*
*  \brief  Sends data out the network interface
*
*****************************************************************************/
unsigned char exoHAL_SocketRecv(long socket, char * buffer, unsigned char len)
{
    if (socket == (long)cid)
    {
        int rec_len = 0, rxbufsize = 0;
        ATLIBGS_MSG_ID_E rxMsgId = ATLIBGS_MSG_ID_NONE;

        if (exo_recv_index == -1)
        {
            if (ifaceType == IF_NOVATEL)
            {
                // read incoming data for 10 seconds,
                rxMsgId = (ATLIBGS_MSG_ID_E)AtModem_ReadLineTimeOut( 10000, buffer, len);
                // novatel doesn't need the rest of this stuff
                return rxMsgId;
            }
            else
            {
                rxMsgId = AtLibGs_ReceiveDataHandle(3000);
            }
            if (ATLIBGS_MSG_ID_DATA_RX != rxMsgId || G_receivedCount <= GS_RECV_OFFSET)
                return 0;
            exo_recv_index = GS_RECV_OFFSET;
        }
        rxbufsize = G_receivedCount - exo_recv_index;

        rec_len = len <= rxbufsize ? len : rxbufsize;
        memcpy(buffer, &G_received[exo_recv_index], rec_len);
        exo_recv_index += rec_len;

        if(exo_recv_index == G_receivedCount)
        {
            exo_recv_index = -1;
        }

        return rec_len;
    }

    return 0;
}


/*****************************************************************************
*
*  exoHAL_HandleError
*
*  \param  code - error code;
*
*  \return None
*
*  \brief  Handles errors in platform-specific way
*
*****************************************************************************/
void exoHAL_HandleError(unsigned char code)
{
    switch (code)
    {
    case EXO_ERROR_WRITE:
        DisplayLCD(LCD_LINE6, " *Exosite* ");
        DisplayLCD(LCD_LINE7, " Write err ");
        DisplayLCD(LCD_LINE8, "           ");
        break;
    case EXO_ERROR_CONNECT:
        DisplayLCD(LCD_LINE6, " *Exosite* ");
        DisplayLCD(LCD_LINE7, "Connect err");
        DisplayLCD(LCD_LINE8, "           ");
        break;
    default:
        DisplayLCD(LCD_LINE6, " *Exosite* ");
        DisplayLCD(LCD_LINE7, " Unknow err");
        DisplayLCD(LCD_LINE8, "           ");
        break;
    }

    return;
}


/*****************************************************************************
*
*  exoHAL_ShowUIMessage
*
*  \param  code - UI code for message to display;
*
*  \return None
*
*  \brief  Displays message in a platform specific way
*
*****************************************************************************/
void exoHAL_ShowUIMessage(unsigned char code)
{
    switch (code)
    {
    case EXO_SERVER_CONNECTED:
        DisplayLCD(LCD_LINE6, "  Exosite  ");
        DisplayLCD(LCD_LINE7, " Connected ");
        break;
    case EXO_CLIENT_RW:
        DisplayLCD(LCD_LINE6, "  Exosite  ");
        DisplayLCD(LCD_LINE7, " Read/Write");
        break;
    default:
        break;
    }

    return;
}

// displays an error message, limited to 12 chars
void exoHAL_ShowErrorMessage(char * errMsg, uint8_t length)
{
    DisplayLCD(LCD_LINE7, "Error:");
    DisplayLCD(LCD_LINE8, errMsg);
    return;
}


/*****************************************************************************
*
*  exoHAL_MSDelay
*
*  \param  delay - milliseconds to delay
*
*  \return None
*
*  \brief  Delays for specified milliseconds
*
*****************************************************************************/
void exoHAL_MSDelay(unsigned short delay)
{
    MSTimerDelay(delay);

    return;
}


// perform modem initialization in here
void exoHAL_initModem()
{
    while( AtModem_Init() == -1)
    {
      // reset the modem
      P8 &= ~(1<<POWER_OFF_PIN); //SET LOW
      PM8 &= ~(1<<POWER_OFF_PIN); //SET AS OUTPUT
      P8 |= (1<<POWER_OFF_PIN);  //SET HIGH
      MSTimerDelay(500); //pulse
      P8 &= ~(1<<POWER_OFF_PIN); //SET LOW


      // pulse the phone pin as well
      ADPC = 0x09U;   //DEFAULT is all AINx pins are Analog, change 8-15
                      // to digital
      P15 &= ~(1<<MODEM_PHON_PIN); //SET LOW
      PM15 &= ~(1<<MODEM_PHON_PIN); //SET AS OUTPUT
      P15 |= (1<<MODEM_PHON_PIN);  //SET HIGH
      MSTimerDelay(500); //pulse
      P15 &= ~(1<<MODEM_PHON_PIN); //SET LOW
      PM15 |= (1<<MODEM_PHON_PIN); //SET AS INPUT

      // wait for modem to power up
      DisplayLCD(LCD_LINE1, "Waiting for ");
      DisplayLCD(LCD_LINE2, "  Modem to  ");
      DisplayLCD(LCD_LINE3, " Initialize ");
      DisplayLCD(LCD_LINE4, "      3     ");
      MSTimerDelay(1000);
      DisplayLCD(LCD_LINE4, "      2     ");
      MSTimerDelay(1000);
      DisplayLCD(LCD_LINE4, "      1     ");
      MSTimerDelay(1000);
      DisplayLCD(LCD_LINE4, "");
    }
    
    // if modem isn't activated, tell user.
    if(!Novatel_isModemActivate())
    {
        exoHAL_ShowErrorMessage("Not Activatd",sizeof("Not Activatd"));
        while(1);
        
    }

    return;
}


