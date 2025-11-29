#ifndef FUNCIONES_H
#define FUNCIONES_H

#include <Arduino.h>

// -------- Pines --------
#define PIEZO_PIN 34
#define BUZZER_PIN 26

// -------- Constantes FSM Micrófono --------
const float UMBRAL_MIC = 0.1f;
const TickType_t TIEMPO_APNEA = 15000 / portTICK_PERIOD_MS; // 15s

// -------- Constantes FSM Piezo --------
const int UMBRAL_PIEZO = 200;
const TickType_t T_APNEA_PIEZO = 15000 / portTICK_PERIOD_MS; // 15s

// -------- Tipos de FSM --------
typedef enum { MIC_INIT, MIC_ESCUCHA, MIC_RESPIRA, MIC_SILENCIO, MIC_APNEA } mic_state_t;
typedef enum { PIEZO_INIT, PIEZO_CALMO, PIEZO_MOV, PIEZO_SIN_MOV, PIEZO_APNEA } piezo_state_t;
typedef enum { BUZ_OFF, BUZ_ON, BUZ_PULSO } buz_state_t;

// -------- Variables globales FSM --------
extern mic_state_t estadoMic;
extern piezo_state_t estadoPiezo;
extern buz_state_t estadoBuzzer;

extern TickType_t tiempoSilencio;
extern TickType_t tPiezo;

// -------- Funciones FSM --------
void fsmMic(float amplitud);
void fsmPiezo(int valor);
void fsmBuzzer(bool apnea);

// -------- Lecturas --------
float leerMicrofono();

#endif
