/*****************************************************************************
*
*  App_Exosite.c - Exosite application for Custom Demo.
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
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <drv/Glyph/lcd.h>
#include <system/mstimer.h>
#include "led.h"
#include "NVSettings.h"
#include <sensors/Temperature.h>
#include <sensors/Potentiometer.h>
#include <exosite/exosite_hal.h>
#include <exosite/exosite_meta.h>
#include <exosite/exosite.h>
#include <inc/common.h>

// Globals:
NVSettings_t GNV_Setting;

// local defines
#define WRITE_INTERVAL 5
#define EXO_BUFFER_SIZE 200
char exo_buffer[EXO_BUFFER_SIZE];
char ping = 0;
int16_t G_adc_int[2] = { 0, 0 };
char G_temp_int[2] = { 0, 0 };

// external defines

// external funsions
extern ATLIBGS_MSG_ID_E WIFI_init(int16_t showMessage);
extern ATLIBGS_MSG_ID_E WIFI_Associate(void);





/*****************************************************************************
*
*  TemperatureReading
*
*  \param  None
*
*  \return None
*
*  \brief  Take a reading of a temperature and show it on the LCD display
*
*****************************************************************************/
void TemperatureReading(void)
{
    char lcd_buffer[20];

    // Temperature sensor reading
    int16_t temp;
    temp = Temperature_Get()>>3;
    // Get the temperature and show it on the LCD
    G_temp_int[0] = (int16_t)temp / 16 - 2;
    G_temp_int[1] = (int16_t)((temp & 0x000F) * 10) / 16;

    /* Display the contents of lcd_buffer onto the debug LCD */
    sprintf((char *)lcd_buffer, "TEMP: %d.%d C", G_temp_int[0], G_temp_int[1]);
    DisplayLCD(LCD_LINE3, (const uint8_t *)lcd_buffer);
}


/*****************************************************************************
*
*  PotentiometerReading
*
*  \param  None
*
*  \return None
*
*  \brief  Take a reading of the potentiometer and show it on the LCD display
*
*****************************************************************************/
void PotentiometerReading(void)
{
    char lcd_buffer[20];

    // Temperature sensor reading
    int32_t percent;
    percent = Potentiometer_Get();
    G_adc_int[0] = (int16_t)(percent / 10);
    G_adc_int[1] = (int16_t)(percent % 10);

    sprintf((char *)lcd_buffer, " POT: %d.%d ", G_adc_int[0], G_adc_int[1]);
    /* Display the contents of lcd_buffer onto the debug LCD */
    DisplayLCD(LCD_LINE4, (const uint8_t *)lcd_buffer);
}


/*****************************************************************************
*
*  RSSIReading
*
*  \param  None
*
*  \return None
*
*  \brief  Take a reading of the RSSI level with the WiFi and show it on
*         the LCD display
*
*****************************************************************************/
void RSSIReading(void)
{
    int16_t rssi;
    char line[20];
    int rssiFound = 0;

    if (AtLibGs_IsNodeAssociated())
    {
        if (AtLibGs_GetRssi() == ATLIBGS_MSG_ID_OK)
        {
            if (AtLibGs_ParseRssiResponse(&rssi))
            {
                sprintf(line, "RSSI: %d", rssi);
                DisplayLCD(LCD_LINE5, (const uint8_t *)line);
                rssiFound = 1;
            }
        }
    }

    if (!rssiFound)
    {
        DisplayLCD(LCD_LINE5, "RSSI: ----");
    }
}


/*****************************************************************************
*
*  UpdateReadings
*
*  \param  None
*
*  \return None
*
*  \brief  Takes a reading of temperature and potentiometer and show
*          on the LCD
*
*****************************************************************************/
void UpdateReadings(void)
{
    TemperatureReading();
    PotentiometerReading();
    DisplayLCD(LCD_LINE7, "");
    DisplayLCD(LCD_LINE8, "");
}


/*****************************************************************************
*
*  ReportReadings
*
*  \param  None
*
*  \return None
*
*  \brief  report the sensor and customization values to Exosite cloud
*
*****************************************************************************/
void ReportReadings(void)
{
    static char content[256];

#ifdef HOST_APP_TCP_DEBUG
    if (updateError)
    {
        sprintf(content, "temp=%d.%d&adc1=%d.%d&ping=%d&ect=%d\r\n",
                G_temp_int[0],G_temp_int[1], G_adc_int[0], G_adc_int[1],
                ping,parsererror);
        updateError = 0;
    }
    else
    {
        sprintf(content, "temp=%d.%d&adc1=%d.%d&ping=%d\r\n",
                G_temp_int[0],G_temp_int[1], G_adc_int[0], G_adc_int[1],
                ping);
    }
#else
    sprintf(content, "temp=%d.%d&adc1=%d.%d&ping=%d\r\n",
            G_temp_int[0],G_temp_int[1], G_adc_int[0], G_adc_int[1],
            ping);
#endif
    ping++;
    if (ping >= 100)
        ping = 0;
    Exosite_Write(content, strlen(content));

    return;
}


/*****************************************************************************
*
*  checkWiFiConnected
*
*  \param  None
*
*  \return TRUE if connected, FALSE if not
*
*  \brief  Checks to see that WiFi is still connected.  If not associated
*          with an AP for 5 consecutive retries, it will reset the board.
*
*****************************************************************************/
unsigned char checkWiFiConnected(void)
{
    if(!AtLibGs_IsNodeAssociated())
    {
        WIFI_Associate();
        Exosite_Init("renesas", "rl78g14", IF_WIFI);
    }
    RSSIReading();

    return AtLibGs_IsNodeAssociated();
}


/*****************************************************************************
*
*  App_Exosite
*
*  \param  None
*
*  \return None
*
*  \brief  Take a reading of temperature and potentiometer and send to the
*          Exosite Cloud using a TCP connection.
*
*****************************************************************************/
void App_Exosite(void)
{
    MSTimerDelay(1000);
    NVSettingsLoad(&GNV_Setting);
    char * pbuf = exo_buffer;
    int loop_time = 1000;
    unsigned char loopCount = 0;

    // tell HAL iface we're using
    exoHAL_SetIface(IF_NOVATEL);
    
    
    DisplayLCD(LCD_LINE4, "Checking w/");
    DisplayLCD(LCD_LINE5, " Exosite   ");
    
    Exosite_Init("renesas", "YRDKRL78G14CDMA", IF_NOVATEL);
    
    DisplayLCD(LCD_LINE3, "Starting    ");
    DisplayLCD(LCD_LINE4, "Application ");
    DisplayLCD(LCD_LINE5, "");
    
    while (1)
    {
        if (Exosite_Read("led_ctrl", pbuf, EXO_BUFFER_SIZE))
        {
            if (!strncmp(pbuf, "0", 1))
                led_all_off();
            else
            if (!strncmp(pbuf, "1", 1))
                led_all_on();
        }
        // POST the Sensor and templature values
        UpdateReadings();
        //if (loopCount++ >= WRITE_INTERVAL)
        //{
            ReportReadings();
        //    loopCount = 0;
        //}

        // TODO - make this a sleep instead of busy wait
        //MSTimerDelay(loop_time); //delay before looping again
    }
}



