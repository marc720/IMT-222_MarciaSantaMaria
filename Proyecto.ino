#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>
#include <driver/i2s.h>

// =================== PINES ===================
#define I2S_SCK   14
#define I2S_WS    15
#define I2S_SD    32
#define I2S_PORT  I2S_NUM_0

#define PIEZO_PIN 34

// =================== CONFIG ===================
#define SAMPLE_RATE   16000
#define BLOCK_SIZE    256

const float LP_ALPHA_MIC   = 0.3f;
const float LP_ALPHA_PIEZO = 0.2f;
const float BASELINE_ALPHA = 0.001f;
const float THRESH_MARGIN  = 15.0f;

const float MIC_WEIGHT     = 0.5f;
const float PIEZO_WEIGHT   = 0.5f;

const unsigned long MIN_BREATH_MS = 700;
const unsigned long APNEA_MS      = 15000;
const unsigned long RPM_WINDOW_MS = 60000;

// =========== MAC DEL MONITOR ==============
uint8_t monitorMac[] = { 0xAA, 0xBB, 0xCC, 0x11, 0x22, 0x33 };

// ================= BUFFERS =================
int32_t i2sBuffer[BLOCK_SIZE];

// ============ ESTADO GLOBAL ================
float micSmooth   = 0.0f;
float piezoSmooth = 0.0f;
float baseline    = 0.0f;
float threshold   = 30.0f;

bool inBreath  = false;
bool inApnea   = false;
bool anyBreath = false;

unsigned long lastBreath       = 0;
unsigned long apneaStart       = 0;
unsigned long lastRpmCheckTime = 0;

uint8_t breathsInWindow = 0;
uint8_t currentRpm      = 0;

// ============ PAQUETE =============
typedef struct {
  float fusedSignal;
  uint8_t rpm;
  bool apnea;
  uint32_t msSinceLastBreath;
} RespPacket;

RespPacket txData;

// ================= I2S CONFIG =================
void i2s_install() {
  const i2s_config_t cfg = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
    .sample_rate = SAMPLE_RATE,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_32BIT,
    .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
    .communication_format = I2S_COMM_FORMAT_I2S,
    .intr_alloc_flags = 0,
    .dma_buf_count = 4,
    .dma_buf_len = BLOCK_SIZE,
    .use_apll = false,
    .tx_desc_auto_clear = false,
    .fixed_mclk = 0
  };
  i2s_driver_install(I2S_PORT, &cfg, 0, NULL);
}

void i2s_setpins() {
  const i2s_pin_config_t pin_cfg = {
    .bck_io_num = I2S_SCK,
    .ws_io_num = I2S_WS,
    .data_out_num = I2S_PIN_NO_CHANGE,
    .data_in_num = I2S_SD
  };
  i2s_set_pin(I2S_PORT, &pin_cfg);
}

float computeRmsMic() {
  size_t bytesRead = 0;
  i2s_read(I2S_PORT, (void*)i2sBuffer, sizeof(i2sBuffer), &bytesRead, portMAX_DELAY);
  int samples = bytesRead / sizeof(int32_t);
  if (samples <= 0) return 0.0f;

  double acc = 0;
  for (int i = 0; i < samples; i++) {
    int32_t v = i2sBuffer[i] >> 14;
    acc += (double)v * (double)v;
  }
  return (float)sqrt(acc / (double)samples);
}

// ====================================================
// ===============  TASK 1: MIC + PIEZO  ===============
// ====================================================
void TaskMicPiezo(void *pv) {
  for (;;) {
    unsigned long now = millis();

    // --- MIC ---
    float micRms = computeRmsMic();
    micSmooth = LP_ALPHA_MIC * micRms + (1 - LP_ALPHA_MIC) * micSmooth;

    // --- PIEZO ---
    int piezoRaw = analogRead(PIEZO_PIN);
    piezoSmooth = LP_ALPHA_PIEZO * piezoRaw + (1 - LP_ALPHA_PIEZO) * piezoSmooth;
    float piezoNorm = piezoSmooth / 4095.0f * 150.0f;

    // --- FUSIÓN ---
    float fused = MIC_WEIGHT * micSmooth + PIEZO_WEIGHT * piezoNorm;

    // --- BASELINE + UMBRAL DINÁMICO ---
    if (fused < threshold) {
      baseline  = (1 - BASELINE_ALPHA) * baseline + BASELINE_ALPHA * fused;
      threshold = baseline + THRESH_MARGIN;
    }

    // --- DETECCIÓN DE RESPIRACIÓN ---
    bool aboveThresh  = (fused > threshold);
    bool belowRelease = (fused < threshold * 0.7f);

    if (aboveThresh && !inBreath && (now - lastBreath > MIN_BREATH_MS)) {
      inBreath = true;
      lastBreath = now;
      anyBreath = true;
      breathsInWindow++;

      Serial.println("🫁 Respiración detectada");

      if (inApnea) {
        inApnea = false;
        Serial.println("Fin de apnea.");
      }
    }
    else if (belowRelease && inBreath) {
      inBreath = false;
    }

    // --- DETECCIÓN DE APNEA ---
    if (anyBreath && !inApnea && (now - lastBreath > APNEA_MS)) {
      inApnea = true;
      apneaStart = now;
      Serial.println("⚠️ APNEA DETECTADA");
    }

    // Guardar para enviar
    txData.fusedSignal = fused;
    txData.apnea = inApnea;

    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}

// ====================================================
// ===============  TASK 2: RPM  =======================
// ====================================================
void TaskRpm(void *pv) {
  for (;;) {
    unsigned long now = millis();

    if (now - lastRpmCheckTime >= RPM_WINDOW_MS) {
      currentRpm = breathsInWindow;
      breathsInWindow = 0;
      lastRpmCheckTime = now;

      Serial.print("RPM: ");
      Serial.println(currentRpm);
    }

    txData.rpm = currentRpm;
    txData.msSinceLastBreath = millis() - lastBreath;

    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}

// ====================================================
// ===============  TASK 3: ESP-NOW  ===================
// ====================================================
void TaskSendNow(void *pv) {
  for (;;) {
    esp_now_send(monitorMac, (uint8_t*)&txData, sizeof(txData));
    vTaskDelay(200 / portTICK_PERIOD_MS);
  }
}

// =================== SETUP ===================
void setup() {
  Serial.begin(115200);

  // ADC
  analogReadResolution(12);

  // I2S
  i2s_install();
  i2s_setpins();
  i2s_start(I2S_PORT);

  // ESP-NOW
  WiFi.mode(WIFI_STA);
  esp_now_init();
  esp_now_peer_info_t p = {};
  memcpy(p.peer_addr, monitorMac, 6);
  esp_now_add_peer(&p);

  unsigned long now = millis();
  lastBreath = now;
  lastRpmCheckTime = now;

  Serial.println("SENSOR con FreeRTOS iniciado.");

  // ===== CREAR TAREAS =====
  xTaskCreate(TaskMicPiezo, "MicPiezo", 5000, NULL, 2, NULL);
  xTaskCreate(TaskRpm,      "RPM",      3000, NULL, 1, NULL);
  xTaskCreate(TaskSendNow,  "SendNow",  3000, NULL, 1, NULL);
}

void loop() {
  // Vacío: FreeRTOS toma el control total
}

