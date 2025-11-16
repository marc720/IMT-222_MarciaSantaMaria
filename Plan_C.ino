/*
  Programa: Secuencia de LEDs - Versión Parametrizada
  Objetivo: Encender una tira de LEDs de forma secuencial,
            evitando números mágicos y permitiendo ajustar parámetros
            fácilmente sin modificar la lógica principal.
  Autor: [Tu Nombre]
  Fecha: [Colocar fecha]
*/

// Parámetros configurables
const int NUM_LEDS = 15;        // Número total de LEDs
const int LED_PINS[NUM_LEDS] = {2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, A0, A1, A2}; 
const unsigned long INTERVALO = 500;  // Intervalo en milisegundos (0.5 segundos)

void setup() {
  // Configurar cada pin como salida
  for (int i = 0; i < NUM_LEDS; i++) {
    pinMode(LED_PINS[i], OUTPUT);
  }
}

void loop() {
  // Encender los LEDs en secuencia con retardo
  for (int i = 0; i < NUM_LEDS; i++) {
    digitalWrite(LED_PINS[i], HIGH); // Encender LED
    delay(INTERVALO);                // Esperar intervalo definido
    digitalWrite(LED_PINS[i], LOW);  // Apagar LED
  }
}
