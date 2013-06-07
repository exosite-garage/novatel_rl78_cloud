/*-------------------------------------------------------------------------*
* File:  main.c
*-------------------------------------------------------------------------*
* Description:
*     Sets up one of the hardware and drivers and then calls one of the
*     tests or demos.
*-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
* Includes:
*-------------------------------------------------------------------------*/
#include <system/platform.h>
#include <HostApp.h>
#include <init/hwsetup.h>
#include <drv\Glyph\lcd.h>
#include <system\mstimer.h>
#include <system/Switch.h>
#include <sensors\Potentiometer.h>
#include <sensors\Temperature.h>
//#include <Tests\Tests.h>
#include <system\console.h>
#include <drv\UART0.h>
#include <drv\UART2.h>
#include <Sensors\LightSensor.h>
#include <drv\SPI.h>
#include <CmdLib\GainSpan_SPI.h>
#include <CmdLib\novatel_cdma.h>
#include <Apps/NVSettings.h>
#include <Apps/Apps.h>
#include "stdio.h"
#include "string.h"
#include "led.h"
#include <exosite/exosite.h> //Include the exosite.h for Exosite's CIK read function
#include <exosite/exosite_meta.h> //Include the exosite_meta.h for Exosite's CIK read function

extern void LEDFlash(uint32_t timeout);
extern void led_task(void);
extern void DisplayLCD(uint8_t, const uint8_t *);
extern int16_t *Accelerometer_Get(void);
extern void Accelerometer_Init(void);
extern int16_t gAccData[3];
/*-------------------------------------------------------------------------*
* Macros:
*-------------------------------------------------------------------------*/
/* Set option bytes */
#pragma location = "OPTBYTE"
__root const uint8_t opbyte0 = 0xEFU;
#pragma location = "OPTBYTE"
__root const uint8_t opbyte1 = 0xFFU;
#pragma location = "OPTBYTE"
__root const uint8_t opbyte2 = 0xF8U;
#pragma location = "OPTBYTE"
__root const uint8_t opbyte3 = 0x04U;

/* Set security ID */
#pragma location = "SECUID"
__root const uint8_t secuid[10] =
{0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U};

/*-------------------------------------------------------------------------*
* Types:
*-------------------------------------------------------------------------*/
/* Application Modes */
typedef enum
{
    RUN_EXOSITE,
    ACTIVATE_MODEM
    
}AppMode_T;

typedef enum
{
    UPDATE_LIGHT,
    UPDATE_TEMPERATURE,
    UPDATE_POTENIOMETER,
    UPDATE_ACCELEROMETER
} APP_STATE_E;



/*---------------------------------------------------------------------------*
* Routine:  main
*---------------------------------------------------------------------------*
* Description:
*      Setup the hardware, setup the peripherals, show the startup banner,
*      wait for the module to power up, run a selected test or app.
* Inputs:
*      void
* Outputs:
*      int -- always 0.
*---------------------------------------------------------------------------*/
uint16_t gAmbientLight;
float gTemp_F;
typedef union
{
    int16_t temp;
    uint8_t T_tempValue[2];
} temp_u;
extern void SPI2_Init();
extern void SPI_Init(uint32_t bitsPerSecond);
extern void App_Exosite(void);
//- old renesas's function extern int16_t GetUserCIK(void);
extern NVSettings_t GNV_Setting;
extern void App_WebProvisioning(void);
extern void App_OverTheAirProgrammingPushMetheod(void);
int main(void)
{
    AppMode_T AppMode;

    HardwareSetup();
    MSTimerInit();
    
    /************************initializa LCD module********************************/
    SPI2_Init();
    InitialiseLCD();
    led_init();
    
    /* Default app mode */
    AppMode = RUN_EXOSITE;

    /* Determine if SW1 & SW3 is pressed at power up to enter nvm erase mode */
    if (Switch1IsPressed() && Switch3IsPressed())
    {
        exosite_meta_defaults();
        DisplayLCD(LCD_LINE1, "*NVM ERASED*");
        DisplayLCD(LCD_LINE2, "Reboot      ");
        DisplayLCD(LCD_LINE3, "  Device    ");
        while(1)
        {
            // wait here
        }
    }
    else if(Switch1IsPressed())
    {
        AppMode = ACTIVATE_MODEM;
    }
    
    DisplayLCD(LCD_LINE1, "Initializing");
    DisplayLCD(LCD_LINE2, "  Novatel   ");
    DisplayLCD(LCD_LINE3, "   Modem    ");
        
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
    // Start UART0 for Novatel modem
    UART0_Start(NOVATEL_UART_BAUD_RATE);

   

    /* If the CIK is exist, auto into the Exosite mode */
    NVSettingsLoad(&GNV_Setting);


    if(AppMode == RUN_EXOSITE)
    {
        LCDDisplayLogo();
        LCDSelectFont(FONT_SMALL);
        DisplayLCD(LCD_LINE3, "RL78G14 RDK    V2.0");
        DisplayLCD(LCD_LINE4, "   Cellular        ");
        DisplayLCD(LCD_LINE5, "     demos by:     ");
        DisplayLCD(LCD_LINE6, "Novatel            ");
        DisplayLCD(LCD_LINE7, "Exosite            ");
        MSTimerDelay(3500);
        ClearLCD();
        DisplayLCD(LCD_LINE1, "Demo Modes:        ");
        DisplayLCD(LCD_LINE2, "-RST no key:       ");
        DisplayLCD(LCD_LINE3, "   ExoSite App     ");
        DisplayLCD(LCD_LINE4, "-RST + SW1 & SW3:  ");
        DisplayLCD(LCD_LINE5, "   Reset NVM       ");
        DisplayLCD(LCD_LINE6, "-RST + SW1:        ");
        DisplayLCD(LCD_LINE7, "   Cell Activate   ");
        MSTimerDelay(3000);
        ClearLCD();

        LCDSelectFont(FONT_LARGE);
        DisplayLCD(LCD_LINE1, "Exosite DEMO");
        
        Temperature_Init();
        Potentiometer_Init();
        
        App_Exosite();
    }
    else if (AppMode == ACTIVATE_MODEM)
    {
        ATModem_CellActivate();
    }

   

    return 0;
}
/*-------------------------------------------------------------------------*
* End of File:  main.c
*-------------------------------------------------------------------------*/



