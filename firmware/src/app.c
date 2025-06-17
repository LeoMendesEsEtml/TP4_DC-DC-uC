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

// Structure principale contenant toutes les données de l'application (état, PID, buffers, etc.)
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
/**
 * @brief Initialise l'application et ses variables.
 *
 * Place la machine d'état de l'application dans son état initial et initialise les paramètres PID, la consigne de tension, et les buffers de mesure.
 *
 * @return Aucun retour.
 */
// Best values yet
//    appData.pid.Kp = 0.0008f;
//    appData.pid.Ki = 0.00008f;
//    appData.pid.Kd = 0.0f;

void APP_Initialize(void) {
    /* Place la machine d'état dans l'état initial */
    appData.state = APP_STATE_INIT;
    uint8_t i = 0;
    // Initialisation des gains PID (Kp, Ki, Kd)
    appData.pid.Kp = 2.0f;
    appData.pid.Ki = 0.010f;
    appData.pid.Kd = 0.001f;
    appData.pid.previous_error = 0.0f;
    appData.pid.integral = 0.0f;
    // Consigne de tension de sortie en mV (ici 5V)
    appData.consigne_tension = 5000;
    // Initialisation de la fenêtre glissante pour la moyenne de tension
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
/**
 * @brief Machine d'état principale de l'application.
 *
 * Gère les différents états de l'application, dont l'initialisation, la gestion des tâches de service, et la régulation PID.
 *
 * @return Aucun retour.
 */
//#define DEBUG

void APP_Tasks(void) {
    /* Machine d'état principale de l'application */
    switch (appData.state) {
            /* Application's initial state. */
        case APP_STATE_INIT:
        {

            // Active les drivers de puissance et initialise les périphériques
            BRIDGE_ENABLEOn();
            ADJ_OUT_ENABLEOn();
            App_Init_Periph();
            // Valeur initiale du PWM (OC)
            DRV_OC0_PulseWidthSet(400);
            // Passe à l'état de service
            appData.state = APP_STATE_SERVICE_TASKS;

            break;
        }

        case APP_STATE_SERVICE_TASKS:
        {
            // Clignote la LED pour indiquer que l'application tourne
            LED2_Toggle();
#ifdef DEBUG
            // En mode debug, force une valeur de PWM
            DRV_OC0_PulseWidthSet(827); // Appliquer la nouvelle valeur sur OC2

#endif 
#ifndef DEBUG
            uint32_t sum = 0;
            uint8_t i = 0;
            // Calcul de la moyenne glissante de la tension mesurée
            for (i = 0; i < SLIDING_WINDOW_SIZE; i++) {
                sum += appData.tension_window[i];
            }
            appData.tension_moyenne = (float) sum / SLIDING_WINDOW_SIZE;
            // Calcul de la sortie PID à partir de la consigne et de la tension mesurée
            appData.pid_out = pid_compute(&appData.pid, (float) appData.consigne_tension, appData.tension_moyenne);
            // Limite la sortie PID à la plage autorisée (0 à MAV_TENSION_6V_MV)
            if (appData.pid_out < 0) appData.pid_out = 0;
            if (appData.pid_out > MAV_TENSION_6V_MV) appData.pid_out = MAV_TENSION_6V_MV;
            // Application d'une correction feedforward pour linéariser la commande PWM
            float OC_final = OC_FEEDFORWARD_A * appData.pid_out + OC_FEEDFORWARD_B;
            // Limite la commande PWM à la plage autorisée
            if (OC_final < 0) OC_final = 0;
            if (OC_final > OC_MAX_FOR_6VOLTS) OC_final = OC_MAX_FOR_6VOLTS;
            // Applique la nouvelle valeur PWM (OC) au driver
            DRV_OC0_PulseWidthSet(OC_final);
            // Sauvegarde la moyenne précédente du courant (pour d'autres traitements éventuels)
            appData.previous_moyenne = appData.courant_moyenne;
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
 * @brief Modifie dynamiquement la consigne de tension de sortie.
 *
 * @param consigne Nouvelle consigne de tension en mV.
 * @return Aucun retour.
 */
void Set_Consigne_Tension(uint16_t consigne) {
    // Modifie la consigne de tension de sortie (en mV)
    appData.consigne_tension = consigne;
}

/**
 * @brief Modifie dynamiquement les paramètres du régulateur PID.
 *
 * @param kp Gain proportionnel.
 * @param ki Gain intégral.
 * @param kd Gain dérivé.
 * @return Aucun retour.
 */
void Set_PID_Params(float kp, float ki, float kd) {
    // Modifie dynamiquement les gains du PID et réinitialise l'intégrale et l'erreur précédente
    appData.pid.Kp = kp;
    appData.pid.Ki = ki;
    appData.pid.Kd = kd;
    appData.pid.integral = 0.0f;
    appData.pid.previous_error = 0.0f;
}

/**
 * @brief Calcule la sortie du régulateur PID.
 *
 * @param pid Pointeur vers la structure PID à utiliser.
 * @param setpoint Consigne à atteindre (mV).
 * @param measured Valeur mesurée (mV).
 * @return Sortie du PID (mV).
 */
float pid_compute(PID_t* pid, float setpoint, float measured) {
    // Calcul de l'erreur entre la consigne et la mesure
    float error = setpoint - measured;
    // Intégration de l'erreur (pour l'action intégrale)
    pid->integral += error;
    // Calcul de la dérivée de l'erreur
    float derived = error - pid->previous_error;
    // Limite l'intégrale pour éviter le windup
    if (pid->integral > PID_INTEGRAL_MAX) pid->integral = PID_INTEGRAL_MAX;
    if (pid->integral < PID_INTEGRAL_MIN) pid->integral = PID_INTEGRAL_MIN;
    // Calcul de la sortie PID
    float output = pid->Kp * error + pid->Ki * pid->integral + pid->Kd * derived;
    // Sauvegarde l'erreur pour la prochaine itération
    pid->previous_error = error;
    // Retourne la sortie PID (en mV)
    return output;
}

/**
 * @brief Callback du timer 1 pour la régulation PID de l'alimentation.
 *
 * Lit les échantillons ADC (courant et tension), effectue une moyenne glissante sur les mesures de tension,
 * calcule l'erreur de tension, applique le PID et ajuste le PWM sur OC2.
 *
 * @return Aucun retour.
 *
 * @pre Le module ADC doit être initialisé et en cours d'acquisition.
 * @post Le PWM OC2 est ajusté selon la régulation PID.
 */
void timer1calback() {
    // Callback appelé périodiquement par le timer 1 pour gérer la régulation
    static uint16_t adc_samples[ADC_SAMPLE_COUNT];
    uint8_t i = 0;
    static uint8_t CadenceTask = 0;
    // Vérifie si de nouveaux échantillons ADC sont disponibles
    if (DRV_ADC_SamplesAvailable()) {
        // Récupère les échantillons ADC (courant et tension)
        for (i = 0; i < ADC_SAMPLE_COUNT; i++) {
            adc_samples[i] = DRV_ADC_SamplesRead(i);
        }
        // Convertit la valeur ADC de la tension de sortie et la stocke dans la fenêtre glissante
        appData.tension_window[appData.window_index] = (uint16_t)( (3300.0f * (float)adc_samples[1] * 2.0f) / 1023.0f );
        appData.window_index = (appData.window_index + 1) % SLIDING_WINDOW_SIZE;
        // Convertit la valeur ADC du courant de sortie et la stocke dans la fenêtre glissante
        appData.courant_window[appData.courant_window_index] = (uint16_t) ((((3300.0f * (float) adc_samples[0]) / 1023.0f) / 48.0f) / 0.01f);
        appData.courant_window_index = (appData.courant_window_index + 1) % SLIDING_WINDOW_SIZE;
    }
    // Toutes les 200 itérations, déclenche la mise à jour de l'état principal
    if (CadenceTask >= 200) {
        CadenceTask = 0;
        appData.state = APP_STATE_SERVICE_TASKS;
    }
    CadenceTask++;
}

/**
 * @brief Initialise les périphériques utilisés par l'application.
 *
 * Initialise l'ADC, démarre les timers et configure les modules nécessaires au fonctionnement de l'application.
 *
 * @return Aucun retour.
 *
 * @pre Le système doit être initialisé avant d'appeler cette fonction.
 * @post Les périphériques ADC et timers sont prêts à être utilisés.
 */
void App_Init_Periph(void) {
    // Initialise l'ADC
    DRV_ADC_Open();
    DRV_ADC_Start();
    // Démarre le module de sortie PWM (OC)
    DRV_OC0_Start();
    // Démarre les timers nécessaires à l'application
    DRV_TMR0_Start();
    DRV_TMR1_Start();
    DRV_TMR2_Start();

}


/*******************************************************************************
 End of File
 */
