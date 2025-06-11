/*******************************************************************************
  MPLAB Harmony Application Source File
  
  Company:
    Microchip Technology Inc.
  
  File Name:
    app.c

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

#include "app.h"
#include "framework/driver/adc/drv_adc_static.h"

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

APP_DATA appData;

// *****************************************************************************
// *****************************************************************************
// Section: Application Callback Functions
// *****************************************************************************
// *****************************************************************************


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
    void APP_Initialize ( void )

  Remarks:
    See prototype in app.h.
 */

void APP_Initialize(void) {
    /* Place the App state machine in its initial state. */
    appData.state = APP_STATE_INIT;
    uint8_t i = 0;
    appData.pid.Kp = 1.0f;
    appData.pid.Ki = 0.0f;
    appData.pid.Kd = 0.0f;
    appData.pid.previous_error = 0.0f;
    appData.pid.integral = 0.0f;
    appData.consigne_tension = 2000;
    for (i = 0; i < SLIDING_WINDOW_SIZE; i++) appData.tension_window[i] = 0;
    appData.window_index = 0;
    appData.window_filled = 0;


    /* TODO: Initialize your application's state machine and other
     * parameters.
     */
}

/******************************************************************************
  Function:
    void APP_Tasks ( void )

  Remarks:
    See prototype in app.h.
 */
#define DEBUG

void APP_Tasks(void) {
    /* Check the application's current state. */
    switch (appData.state) {
            /* Application's initial state. */
        case APP_STATE_INIT:
        {

            BRIDGE_ENABLEOn();
            ADJ_OUT_ENABLEOn();

            App_Init_Periph();
            appData.state = APP_STATE_SERVICE_TASKS;

            break;
        }

        case APP_STATE_SERVICE_TASKS:
        {
            LED2_Toggle();
#ifdef DEBUG

            DRV_OC0_PulseWidthSet(400); // Appliquer la nouvelle valeur sur OC2

#endif 
#ifndef DEBUG
            uint32_t sum = 0;
            uint8_t i = 0;
            // Calcul de la moyenne glissante

            for (i = 0; i < appData.window_filled; i++) {
                sum += appData.tension_window[i];
            }
            float tension_moyenne = (float) sum / appData.window_filled;
            // Calcul PID sur la tension
            float pid_out = pid_compute(&appData.pid, (float) appData.consigne_tension, tension_moyenne);
            // Limiter la sortie PID pour le PWM (0-100%)
            if (pid_out < 0) pid_out = 0;
            if (pid_out > 100) pid_out = 100;
            DRV_OC0_PulseWidthSet((uint16_t) pid_out); // Appliquer la nouvelle valeur sur OC2
#endif
            break;
        }



            /* The default state should never be executed. */
        default:
        {
            /* TODO: Handle error in application's state machine. */
            break;
        }
    }
}





// Paramètres PID et consigne configurables
// static PID_t pid = { .Kp = 1.0f, .Ki = 0.0f, .Kd = 0.0f, .previous_error = 0.0f, .integral = 0.0f };
// static uint16_t consigne_tension = 2000; // Peut être modifié dynamiquement

/**
 * @brief Permet de configurer dynamiquement la consigne de tension.
 * @param consigne Nouvelle consigne de tension
 */
void Set_Consigne_Tension(uint16_t consigne) {
    appData.consigne_tension = consigne;
}

/**
 * @brief Permet de configurer dynamiquement les paramètres PID.
 * @param kp Gain proportionnel
 * @param ki Gain intégral
 * @param kd Gain dérivé
 */
void Set_PID_Params(float kp, float ki, float kd) {
    appData.pid.Kp = kp;
    appData.pid.Ki = ki;
    appData.pid.Kd = kd;
    appData.pid.integral = 0.0f;
    appData.pid.previous_error = 0.0f;
}

/**
 * @brief Calcule la moyenne des échantillons ADC
 * @param samples Tableau d'échantillons
 * @param count Nombre d'échantillons
 * @return Moyenne
 */
static uint16_t adc_average(uint16_t* samples, uint8_t count) {
    uint32_t sum = 0;
    uint8_t i = 0;
    for (i = 0; i < count; i++) {
        sum += samples[i];
    }
    return (uint16_t) (sum / count);
}

/**
 * @brief Calcule la sortie PID
 * @param pid Structure PID
 * @param setpoint Consigne
 * @param measured Mesure
 * @return Sortie PID
 */
static float pid_compute(PID_t* pid, float setpoint, float measured) {
    float error = setpoint - measured;
    pid->integral += error;
    float derivative = error - pid->previous_error;
    float output = pid->Kp * error + pid->Ki * pid->integral + pid->Kd * derivative;
    pid->previous_error = error;
    return output;
}

/**
 * @brief Callback du timer 1 pour la régulation PID de l'alimentation.
 *
 * @details
 * Lit les échantillons ADC (courant et tension), effectue une moyenne glissante sur 10 mesures de tension,
 * calcule l'erreur de tension, applique le PID et ajuste le PWM sur OC2.
 *
 * @param Aucun paramètre.
 * @return Aucun retour.
 *
 * @pre Le module ADC doit être initialisé et en cours d'acquisition.
 * @post Le PWM OC2 est ajusté selon la régulation PID.
 */
void timer1calback() {
    static uint16_t adc_samples[ADC_SAMPLE_COUNT];
    uint8_t i = 0;
    uint8_t CadenceTask = 0;
    if (DRV_ADC_SamplesAvailable()) {
        for (i = 0; i < ADC_SAMPLE_COUNT; i++) {
            adc_samples[i] = DRV_ADC_SamplesRead(i);
        }
        appData.tension_window[appData.window_index] = adc_samples[1];
        appData.window_index = (appData.window_index + 1) % SLIDING_WINDOW_SIZE;
        if (appData.window_filled < SLIDING_WINDOW_SIZE) appData.window_filled++;
    }
    if (CadenceTask >= 200) {
        CadenceTask = 0;
        appData.state = APP_STATE_SERVICE_TASKS;
    }
    CadenceTask++;
}

/**
 * @brief Initialise les périphériques utilisés par l'application.
 *
 * @details
 * Cette fonction initialise l'ADC et démarre les timers nécessaires au fonctionnement de l'application.
 * Elle doit être appelée après l'initialisation du système, typiquement depuis la fonction SYS_Initialize.
 *
 * @param Aucun paramètre.
 * @return Aucun retour.
 *
 * @pre Le système doit être initialisé avant d'appeler cette fonction.
 * @post Les périphériques ADC et timers sont prêts à être utilisés.
 */
void App_Init_Periph(void) {
    /* Initialize the ADC */
    DRV_ADC_Open();
    DRV_ADC_Start();

    DRV_OC0_Start();
    /* Start the Timers */
    DRV_TMR0_Start();
    DRV_TMR1_Start();
    DRV_TMR2_Start();

}


/*******************************************************************************
 End of File
 */
