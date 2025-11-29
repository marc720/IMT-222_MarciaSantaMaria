#include <Arduino.h>
#include "funciones.h"

// -------- Tareas FreeRTOS --------
void TaskMic(void *pv) {
    for(;;) {
        float amp = leerMicrofono();
        fsmMic(amp);
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

void TaskPiezo(void *pv) {
    for(;;) {
        int val = analogRead(PIEZO_PIN);
        fsmPiezo(val);
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

void TaskBuzzer(void *pv) {
    for(;;) {
        bool apnea = (estadoMic == MIC_APNEA) || (estadoPiezo == PIEZO_APNEA);
        fsmBuzzer(apnea);
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

void setup() {
    Serial.begin(115200);
    pinMode(BUZZER_PIN, OUTPUT);

    // Crear tareas FreeRTOS
    xTaskCreate(TaskMic, "Mic", 4000, NULL, 2, NULL);
    xTaskCreate(TaskPiezo, "Piezo", 3000, NULL, 2, NULL);
    xTaskCreate(TaskBuzzer, "Buzzer", 2000, NULL, 1, NULL);
}

void loop() {
    // Loop vacío porque todo está en FreeRTOS
    vTaskDelay(1000 / portTICK_PERIOD_MS);
}
