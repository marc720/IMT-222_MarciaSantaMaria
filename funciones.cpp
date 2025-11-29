#include "funciones.h"

// -------- Variables globales --------
mic_state_t estadoMic = MIC_INIT;
piezo_state_t estadoPiezo = PIEZO_INIT;
buz_state_t estadoBuzzer = BUZ_OFF;

TickType_t tiempoSilencio = 0;
TickType_t tPiezo = 0;

// -------- FSM MICRÓFONO --------
void fsmMic(float amplitud) {
    TickType_t ahora = xTaskGetTickCount();

    switch (estadoMic) {
        case MIC_INIT: estadoMic = MIC_ESCUCHA; break;
        case MIC_ESCUCHA:
            if (amplitud > UMBRAL_MIC) estadoMic = MIC_RESPIRA;
            else { tiempoSilencio = ahora; estadoMic = MIC_SILENCIO; }
            break;
        case MIC_RESPIRA:
            if (amplitud <= UMBRAL_MIC) { tiempoSilencio = ahora; estadoMic = MIC_SILENCIO; }
            break;
        case MIC_SILENCIO:
            if (amplitud > UMBRAL_MIC) estadoMic = MIC_RESPIRA;
            else if (ahora - tiempoSilencio > TIEMPO_APNEA) estadoMic = MIC_APNEA;
            break;
        case MIC_APNEA:
            if (amplitud > UMBRAL_MIC) estadoMic = MIC_RESPIRA;
            break;
    }
}

// -------- FSM PIEZO --------
void fsmPiezo(int valor) {
    TickType_t ahora = xTaskGetTickCount();

    switch (estadoPiezo) {
        case PIEZO_INIT: estadoPiezo = PIEZO_CALMO; break;
        case PIEZO_CALMO:
            if (valor > UMBRAL_PIEZO) estadoPiezo = PIEZO_MOV;
            else { tPiezo = ahora; estadoPiezo = PIEZO_SIN_MOV; }
            break;
        case PIEZO_MOV:
            if (valor <= UMBRAL_PIEZO) { tPiezo = ahora; estadoPiezo = PIEZO_SIN_MOV; }
            break;
        case PIEZO_SIN_MOV:
            if (valor > UMBRAL_PIEZO) estadoPiezo = PIEZO_MOV;
            else if (ahora - tPiezo > T_APNEA_PIEZO) estadoPiezo = PIEZO_APNEA;
            break;
        case PIEZO_APNEA:
            if (valor > UMBRAL_PIEZO) estadoPiezo = PIEZO_MOV;
            break;
    }
}

// -------- FSM BUZZER --------
void fsmBuzzer(bool apneaDetectada) {
    switch (estadoBuzzer) {
        case BUZ_OFF:
            if (apneaDetectada) estadoBuzzer = BUZ_ON;
            digitalWrite(BUZZER_PIN, LOW);
            break;
        case BUZ_ON:
            digitalWrite(BUZZER_PIN, HIGH);
            if (!apneaDetectada) estadoBuzzer = BUZ_OFF;
            break;
        case BUZ_PULSO:
            digitalWrite(BUZZER_PIN, (xTaskGetTickCount() / 250) % 2);
            if (!apneaDetectada) estadoBuzzer = BUZ_OFF;
            break;
    }
}

// -------- Lectura simulada del micrófono --------
float leerMicrofono() {
    // Aquí debes colocar tu código I2S para INMP441
    // Por ejemplo, retornar RMS normalizado
    return analogRead(34) / 4095.0f; // ejemplo simplificado
}
