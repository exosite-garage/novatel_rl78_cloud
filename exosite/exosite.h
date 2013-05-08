/*****************************************************************************
*
*  exosite.h - Exosite library interface header
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

#ifndef EXOSITE_H
#define EXOSITE_H

#include <stdint.h>

// defines
typedef enum
{
    IF_WIFI,
    IF_ENET,
    IF_FILE,
    IF_HDD,
    IF_I2C,
    IF_GPRS,
    IF_NOVATEL,
    IF_NONE
}UUIDInterfaceTypes;
#define EXOSITE_DEMO_UPDATE_INTERVAL            4000 // ms
#define CIK_LENGTH                              40

typedef enum
{
    CONNECTION_ERROR,       /*!< Error in connecting to Exosite */
    VALID_CIK,              /*!< CIK is valid  */
    DEVICE_NOT_ENABLED,     /*!< Device has not been enabled */
    R_W_ERROR               /*!< R/W error */
}EXOSITE_DEVICE_ACTIVATION_STATE;

// functions for export
int Exosite_Write(char * pbuf, unsigned char bufsize);
int Exosite_Read(char * palias, char * pbuf, unsigned char buflen);
void Exosite_Init(const char *vendor, const char *model, const unsigned char if_nbr);
void Exosite_Cloud_Setup(void);
int Exosite_CheckCIK(void);
void Exosite_SetCIK(char * pCIK);
void Exosite_GetCIK(char * pCIK);
int Exosite_Cloud_Status(void);
EXOSITE_DEVICE_ACTIVATION_STATE activate_device(void);
#endif



