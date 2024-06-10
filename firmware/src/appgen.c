/*******************************************************************************
  MPLAB Harmony Application Source File
  
  Company:
    Microchip Technology Inc.
  
  File Name:
    appgen.c

  Summary:
    This file contains the source code for the MPLAB Harmony application.

  Description:
    This file contains the source code for the MPLAB Harmony application.  It 
    implements the logic of the application's state machine and it may call 
    API routines of other MPLAB Harmony modules in the system, such as drivers,
    system services, and middleware.  However, it does not call any of the
    system interfaces (such as the "Initialize" and "Tasks" functions) of any of
    the modules in the system or make any assumptions about when those functions
    are called.  That is the responsibility of the configuration-specific system
    files.
 *******************************************************************************/

// DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2013-2014 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
 *******************************************************************************/
// DOM-IGNORE-END


// *****************************************************************************
// *****************************************************************************
// Section: Included Files 
// *****************************************************************************
// *****************************************************************************

#include "appgen.h"
#include "Mc32DriverLcd.h"
#include <stdint.h>
#include <stdbool.h>
#include "DefMenuGen.h"
#include "MenuGen.h"
#include "bsp.h"
#include "Generateur.h"
#include "Mc32gestSpiDac.h"
#include "GesPec12.h"
#include "Mc32SpiUtil.h"
#include "system_config/pic32mx_eth_sk2/framework/driver/tmr/drv_tmr_static.h"
#include "Mc32gestI2cSeeprom.h"

// *****************************************************************************
// *****************************************************************************
// Section: Global Data Definitions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Application Data

  Summary:
    Holds application data

  Description:
    This structure holds the application's data.

  Remarks:
    This structure should be initialized by the APP_Initialize function.
    
    Application strings and buffers are be defined outside this structure.
*/

APPGEN_DATA appgenData;
S_ParamGen LocalParamGen;
S_ParamGen RemoteParamGen;
IPV4_ADDR  ipAddr;
S_Flag FLAG;


//flag permettant d'initialiser l'ecran
bool Mode_Remote ;
bool EtatIP;
uint8_t flag_tour = 1 ;
bool Local;
bool stateIP;
// *****************************************************************************
// *****************************************************************************
// Section: Application Callback Functions
// *****************************************************************************
// *****************************************************************************

/* TODO:  Add any necessary callback functions.
*/

// *****************************************************************************
// *****************************************************************************
// Section: Application Local Functions
// *****************************************************************************
// *****************************************************************************


/* TODO:  Add any necessary local functions.
*/


// *****************************************************************************
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void APPGEN_Initialize ( void )

  Remarks:
    See prototype in appgen.h.
 */

void APPGEN_Initialize ( void )
{
    /* Place the App state machine in its initial state. */
    appgenData.state = APPGEN_STATE_INIT;

    
    /* TODO: Initialize your application's state machine and other
     * parameters.
     */
}


/******************************************************************************
  Function:
    void APPGEN_Tasks ( void )

  Remarks:
    See prototype in appgen.h.
 */

void APPGEN_Tasks ( void )
{

    /* Check the application's current state. */
    switch ( appgenData.state )
    {
        /* Application's initial state. */
        case APPGEN_STATE_INIT:
        {
            //initaliser l'affichage
            lcd_init();
            lcd_gotoxy(1,1);
            printf_lcd("Tp5 IpGen");
            lcd_gotoxy(1,2);
            printf_lcd("Caroline Mieville");
            lcd_gotoxy(1,3);
            printf_lcd("Joao Marques");
            lcd_bl_on();
            
            //initialiser l'I2C
            I2C_InitMCP79411();
            // Init SPI DAC            
            SPI_InitLTC2604();              
            // Initialisation du generateur
            GENSIG_Initialize(&LocalParamGen);
            //realiser le signal
            GENSIG_UpdateSignal(&LocalParamGen);
            //mettre à jour la période
            GENSIG_UpdatePeriode(&LocalParamGen);
            
            //Init Pec12Init
            Pec12Init();
            
           
            
            /* initialisation des timers */
            DRV_TMR0_Start();
            DRV_TMR1_Start();  
            
            //Synchroniser les paramètres
            RemoteParamGen = LocalParamGen;
             
            //Modifier l'etat app
            APPGEN_UpdateState(APPGEN_STATE_WAIT); 
            break;
        }

        case APPGEN_STATE_SERVICE_TASKS:
        {
            //toggle la led2
            LED2_W = !LED2_R;
         //Afficher le nouvel affichage lors du premier tours
            if (flag_tour == 1)
            {
                // Initialisation du menu
                MENU_Initialize(&LocalParamGen);
                //remettre le flag à 0 car s'initailise une seul fois
                flag_tour = 0;
            }
            //execution menu
            if(FlagIP_State())
            {
                stateIP = DisplayIP();
                if(stateIP)
                {
                    FlagIP_Val(false);
                    MENU_Initialize(&LocalParamGen);
                }
            }else
            {
                if (Remotemode_State())
                {
                    //mettre le paramètre à 0
                    Local = 0;       
                    //obtenir la chaine de caratere
                    //GetMessage(appData.readBuffer, &RemoteParamGen);
                    //executer menu execute
                    MENU_Execute(&RemoteParamGen, Local);
                    //SendMessage((int8_t *)appData.readBuffer,(int8_t *)appData.readBuffer, Flag_Save() );
                }
                else
                {   
                    //mettre le paramètre à 1
                    Local = 1;
                    //executer menu execute
                    MENU_Execute(&LocalParamGen, Local);
                }
            }
            //le prochaine état est: attente
            APPGEN_UpdateState(APPGEN_STATE_WAIT);
            //toogle la LED2
            LED2_W = !LED2_R;
            break;
        }
        case APPGEN_STATE_WAIT:
        {
        
            break;
        }

        /* TODO: implement your application state machine.*/
        

        /* The default state should never be executed. */
        default:
        {
            /* TODO: Handle error in application's state machine. */
            break;
        }
    }
}
void APPGEN_UpdateState(APPGEN_STATES newState)
{
    appgenData.state = newState;
}
//Afficher l'adresse ip
bool DisplayIP(void)
{
    bool etat;
    uint16_t NCYCLE = 499;
    static uint16_t ATTENDRE = 0;
        
    if(ATTENDRE != NCYCLE )
    {  
        //afficher l'adresse IP
        lcd_bl_on();
        lcd_gotoxy(7,2);
        printf_lcd("Adr. IP");
        lcd_gotoxy(3,3);
        printf_lcd("%03d.%03d.%03d.%03d", ipAddr.v[0], ipAddr.v[1], ipAddr.v[2], ipAddr.v[3]);
        APPGEN_UpdateState(APPGEN_STATE_SERVICE_TASKS);
        ATTENDRE ++;
        etat=false;
    }
    else
    {
        Clear_LCD();
        etat=true;
    }   
    return (etat);//Clear LCD
}

/*Flag pour savoir si on souhaite save ou non en remote*/
bool  FlagSave_State(void)
{
    return (FLAG.SAVE);
}
/*Flag pour savoir si on souhaite save ou non en remote*/
void FlagSave_Val(bool Val)
{
    FLAG.SAVE = Val;
}
/*Flag pour savoir si on souhaite activer le flag pout refresh LCD ou non*/
void FlagRefreshLCD_Val(bool Val)
{
    FLAG.REFRESH = Val;
}
/*Fonction pour connaitre la valeur de ce flag*/
bool  FlagRefreshLCD_State (void)
{
    return (FLAG.REFRESH);
}
/*Fonction pour connaitre la valeur de ce flag*/
bool  FlagRemote_State(void)
{
    return (FLAG.REMOTE );
}
/*Fonction pour mettre à jour la valeur du flag*/
void FlagRemote_Val(bool Val)
{
    FLAG.REMOTE = Val;
}
/*Fonction pour mettre à jour la valeur du flag*/
 void Remotemode_Val(bool Val)
 {
    Mode_Remote = Val;
 }
/*Fonction pour connaitre la valeur de ce flag (mode remote ou non)*/
 bool Remotemode_State(void)
 {
    return (Mode_Remote);
 }
/*Fonction pour mettre à jour la valeur du flag*/
 void FlagIP_Val(bool val)
 {
    EtatIP = val;
 }
 /*Flag pour connaitre l'etat de l'IP*/
 bool FlagIP_State(void)
 {
    return EtatIP;
 }

/*******************************************************************************
 End of File
 */
