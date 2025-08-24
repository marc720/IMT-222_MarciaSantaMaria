/*
  Programa: Secuencia de 15 LEDs
  Objetivo: Encender una tira de 15 LEDs de forma secuencial
            con un retardo fijo de 0,5 segundos entre cada LED.
  Autor: [Tu Nombre]
  Fecha: [Colocar fecha]
*/

int leds[15] = {2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, A0, A1, A2}; 
// Usamos pines digitales 2-13 y los analógicos A0-A2 (también sirven como digitales)

int numLeds = 15; // Cantidad de LEDs

void setup() {
  // Configurar cada pin como salida
  for (int i = 0; i < numLeds; i++) {
    pinMode(leds[i], OUTPUT);
  }
}

void loop() {
  // Encender los LEDs en secuencia con retardo
  for (int i = 0; i < numLeds; i++) {
    digitalWrite(leds[i], HIGH); // Encender LED
    delay(500);                  // Esperar 0,5 segundos
    digitalWrite(leds[i], LOW);  // Apagar LED
  }
}
