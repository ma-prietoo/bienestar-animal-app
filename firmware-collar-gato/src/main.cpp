#include <Arduino.h>
#include <esp_sleep.h>

#include "analyzer.h"
#include "app_config.h"
#include "buffer_store.h"
#include "comms_sim.h"
#include "data_types.h"
#include "sensor_sim.h"

SensorSim sensors;
Analyzer analyzer;
BufferStore bufferStore;
CommsSim comms;
unsigned long lastSampleMs = 0;
bool gnssPowered = false;
bool lastImuButtonState = HIGH;
bool lastBleLossButtonState = HIGH;
bool lastTempEventButtonState = HIGH;
bool lastInactivityButtonState = HIGH;
bool lastLowBatteryButtonState = HIGH;

void configurePins() {
  pinMode(AppConfig::GNSS_LED_PIN, OUTPUT);
  pinMode(AppConfig::ACTIVITY_LED_PIN, OUTPUT);
  digitalWrite(AppConfig::GNSS_LED_PIN, LOW);
  digitalWrite(AppConfig::ACTIVITY_LED_PIN, LOW);
}

void activateGnss(WellnessSample &sample) {
  Serial.println("[GNSS] Encendido por perdida de conexion BLE");
  digitalWrite(AppConfig::GNSS_LED_PIN, HIGH);
  gnssPowered = true;
  delay(250);

  sample.gnssActive = true;
  sample.latitude = 4.7110;
  sample.longitude = -74.0721;
  sample.deviceMode = MODE_GNSS_ACTIVE;

  Serial.print("[GNSS] Ubicacion simulada lat=");
  Serial.print(sample.latitude, 6);
  Serial.print(" lon=");
  Serial.println(sample.longitude, 6);
}

void powerOffGnss() {
  digitalWrite(AppConfig::GNSS_LED_PIN, LOW);
  gnssPowered = false;
  Serial.println("[GNSS] Apagado: BLE disponible");
}

void enterDeepSleep() {
  Serial.print("[POWER] Deep sleep programado por ");
  Serial.print(AppConfig::PERIODIC_WAKEUP_SECONDS);
  Serial.println(" s");

  esp_sleep_enable_timer_wakeup(
    static_cast<uint64_t>(AppConfig::PERIODIC_WAKEUP_SECONDS) * 1000000ULL
  );

#if CONFIG_IDF_TARGET_ESP32C3
  esp_deep_sleep_enable_gpio_wakeup(
    1ULL << AppConfig::IMU_BUTTON_PIN,
    ESP_GPIO_WAKEUP_GPIO_LOW
  );
#endif

#if APP_SIMULATION_MODE
  Serial.println("[POWER] Modo simulacion Wokwi: no se entra a deep sleep");
#else
  Serial.flush();
  esp_deep_sleep_start();
#endif
}

void logForcedEvents(uint16_t eventOverrides) {
  if (eventOverrides & EVENT_IMU_ACTIVITY) {
    Serial.println("[EVENT] IMU Wake: alta actividad detectada");
  }
  if (eventOverrides & EVENT_BLE_LOSS) {
    Serial.println("[EVENT] BLE perdido: posible salida de zona segura");
  }
  if (eventOverrides & EVENT_TEMP_HIGH) {
    Serial.println("[EVENT] Temperatura alta simulada");
  }
  if (eventOverrides & EVENT_INACTIVITY) {
    Serial.println("[EVENT] Inactividad prolongada simulada");
  }
  if (eventOverrides & EVENT_LOW_BATTERY) {
    Serial.println("[EVENT] Bateria baja simulada");
  }
}

void runMonitoringCycle(uint16_t eventOverrides = EVENT_NONE) {
  logForcedEvents(eventOverrides);

  WellnessSample sample = sensors.readSample(eventOverrides);

  if (sample.bleConnected && gnssPowered) {
    powerOffGnss();
  }

  Serial.println();
  Serial.println("[SYSTEM] Ciclo de monitoreo");
  Serial.print("[SENSORS] temp=");
  Serial.print(sample.temperatureC, 1);
  Serial.print("C activity=");
  Serial.print(sample.activityRms, 2);
  Serial.print(" battery=");
  Serial.print(sample.batteryPercent);
  Serial.print("% ble=");
  Serial.println(sample.bleConnected ? "on" : "off");

  sample.deviceMode = MODE_PROCESSING;
  WellnessAnalysis analysis = analyzer.analyze(sample);
  digitalWrite(AppConfig::ACTIVITY_LED_PIN, analysis.activity == ACTIVITY_HIGH ? HIGH : LOW);

  if (analysis.criticalEvent) {
    sample.deviceMode = MODE_CRITICAL_EVENT;
    Serial.print("[ALERT] Evento critico detectado flags=0x");
    Serial.println(analysis.alerts, HEX);

    if (AppConfig::GNSS_ON_CRITICAL_EVENT && (analysis.alerts & ALERT_BLE_LOSS)) {
      activateGnss(sample);
    } else {
      sample.gnssActive = false;
      digitalWrite(AppConfig::GNSS_LED_PIN, LOW);
    }
  }

  comms.setBleLed(sample.bleConnected);

  if (comms.isConnected(sample)) {
    sample.deviceMode = MODE_BLE_SYNC;
    comms.sendSample(sample, analysis, bufferStore.count());
    comms.syncPending(bufferStore);
  } else {
    sample.deviceMode = MODE_BUFFERING;
    bufferStore.push(sample, analysis);
    Serial.print("[BUFFER] BLE desconectado. Pendientes=");
    Serial.println(bufferStore.count());
    comms.printPayload(sample, analysis, bufferStore.count(), "local_buffer");
  }

  if (sample.gnssActive && sample.bleConnected) {
    powerOffGnss();
  }

  enterDeepSleep();
}

void setup() {
  Serial.begin(AppConfig::SERIAL_BAUD);
  delay(600);

  Serial.println("[SYSTEM] Simulacion Wokwi - Firmware collar gato");
  Serial.println("[SYSTEM] Botones: IMU, BLE perdido, temperatura alta, inactividad, bateria baja");
  Serial.println("[SYSTEM] NTC: temperatura. Potenciometro: bateria");
  configurePins();
  sensors.begin();
  bufferStore.begin();
  comms.begin();
  lastSampleMs = millis() - AppConfig::PERIODIC_SAMPLE_MS + AppConfig::FIRST_SAMPLE_DELAY_MS;
}

void loop() {
  const bool imuButtonState = digitalRead(AppConfig::IMU_BUTTON_PIN);
  const bool bleLossButtonState = digitalRead(AppConfig::BLE_LOSS_BUTTON_PIN);
  const bool tempEventButtonState = digitalRead(AppConfig::TEMP_EVENT_BUTTON_PIN);
  const bool inactivityButtonState = digitalRead(AppConfig::INACTIVITY_BUTTON_PIN);
  const bool lowBatteryButtonState = digitalRead(AppConfig::LOW_BATTERY_BUTTON_PIN);

  const bool anyNewPress =
    (lastImuButtonState == HIGH && imuButtonState == LOW) ||
    (lastBleLossButtonState == HIGH && bleLossButtonState == LOW) ||
    (lastTempEventButtonState == HIGH && tempEventButtonState == LOW) ||
    (lastInactivityButtonState == HIGH && inactivityButtonState == LOW) ||
    (lastLowBatteryButtonState == HIGH && lowBatteryButtonState == LOW);

  uint16_t heldEvents = EVENT_NONE;
  if (imuButtonState == LOW) {
    heldEvents |= EVENT_IMU_ACTIVITY;
  }
  if (bleLossButtonState == LOW) {
    heldEvents |= EVENT_BLE_LOSS;
  }
  if (tempEventButtonState == LOW) {
    heldEvents |= EVENT_TEMP_HIGH;
  }
  if (inactivityButtonState == LOW) {
    heldEvents |= EVENT_INACTIVITY;
  }
  if (lowBatteryButtonState == LOW) {
    heldEvents |= EVENT_LOW_BATTERY;
  }

  uint16_t eventOverrides = EVENT_NONE;
  if (lastImuButtonState == HIGH && imuButtonState == LOW) {
    eventOverrides |= EVENT_IMU_ACTIVITY;
  }
  if (lastBleLossButtonState == HIGH && bleLossButtonState == LOW) {
    eventOverrides |= EVENT_BLE_LOSS;
  }
  if (lastTempEventButtonState == HIGH && tempEventButtonState == LOW) {
    eventOverrides |= EVENT_TEMP_HIGH;
  }
  if (lastInactivityButtonState == HIGH && inactivityButtonState == LOW) {
    eventOverrides |= EVENT_INACTIVITY;
  }
  if (lastLowBatteryButtonState == HIGH && lowBatteryButtonState == LOW) {
    eventOverrides |= EVENT_LOW_BATTERY;
  }

  if (anyNewPress) {
    eventOverrides = heldEvents;
  }

  lastImuButtonState = imuButtonState;
  lastBleLossButtonState = bleLossButtonState;
  lastTempEventButtonState = tempEventButtonState;
  lastInactivityButtonState = inactivityButtonState;
  lastLowBatteryButtonState = lowBatteryButtonState;

  const bool anyButtonHeld =
    imuButtonState == LOW;
  digitalWrite(AppConfig::ACTIVITY_LED_PIN, anyButtonHeld ? HIGH : LOW);

  if (anyNewPress) {
    runMonitoringCycle(eventOverrides);
    lastSampleMs = millis();
  } else if (millis() - lastSampleMs >= AppConfig::PERIODIC_SAMPLE_MS) {
    runMonitoringCycle(EVENT_NONE);
    lastSampleMs = millis();
  }

  delay(AppConfig::LOOP_INTERVAL_MS);
}
