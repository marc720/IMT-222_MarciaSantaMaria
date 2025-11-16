#include <Arduino.h>
#include "funciones.h"

SensorMPU mpu;

// Pines de los LEDs
#define LED_POS 25   // LED para valores positivos
#define LED_NEG 26   // LED para valores negativos

void setup() {
  Serial.begin(115200);
  mpu.begin();

  pinMode(LED_POS, OUTPUT);
  pinMode(LED_NEG, OUTPUT);

  Serial.println("MPU6050 inicializado correctamente con control de LEDs.");
}

void loop() {
  float ax, ay, az, gx, gy, gz;
  mpu.leerDatos(ax, ay, az, gx, gy, gz);

  // Mostrar valores en el monitor serial
  Serial.print("Aceleración [g]: ");
  Serial.print(ax); Serial.print(", ");
  Serial.print(ay); Serial.print(", ");
  Serial.print(az);

  Serial.print(" | Giroscopio [°/s]: ");
  Serial.print(gx); Serial.print(", ");
  Serial.print(gy); Serial.print(", ");
  Serial.println(gz);

  // --- LÓGICA DE LOS LEDs ---
  if (ax > 0) {
    digitalWrite(LED_POS, HIGH);
    digitalWrite(LED_NEG, LOW);
  } else {
    digitalWrite(LED_POS, LOW);
    digitalWrite(LED_NEG, HIGH);
  }

  delay(300);
}
