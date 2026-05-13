#pragma once

#include <Arduino.h>
#include "app_config.h"
#include "buffer_store.h"
#include "data_types.h"

class CommsSim {
public:
  void begin() {
    pinMode(AppConfig::BLE_LED_PIN, OUTPUT);
    digitalWrite(AppConfig::BLE_LED_PIN, LOW);
    Serial.println("[BLE] Servicio simulado listo: Animal Wellness Collar");
  }

  bool isConnected(const WellnessSample &sample) const {
    return sample.bleConnected;
  }

  void setBleLed(bool connected) const {
    digitalWrite(AppConfig::BLE_LED_PIN, connected ? HIGH : LOW);
  }

  void sendSample(const WellnessSample &sample, const WellnessAnalysis &analysis, size_t pendingRecords) const {
    Serial.print("[BLE] temp=");
    Serial.print(sample.temperatureC, 1);
    Serial.print("C activity=");
    Serial.print(sample.activityRms, 2);
    Serial.print(" battery=");
    Serial.print(sample.batteryPercent);
    Serial.print("% alerts=0x");
    Serial.print(analysis.alerts, HEX);
    Serial.print(" mode=");
    Serial.println(sample.deviceMode);

    printPayload(sample, analysis, pendingRecords, "ble_notification");
  }

  void syncPending(BufferStore &buffer) const {
    Serial.print("[BLE] Registros sincronizados desde buffer: ");
    Serial.println(buffer.count());
    buffer.clear();
  }

  void printPayload(const WellnessSample &sample,
                    const WellnessAnalysis &analysis,
                    size_t pendingRecords,
                    const char *transport) const {
    Serial.println("[PAYLOAD] Archivo JSON enviado a la app para analisis:");
    Serial.println("{");
    Serial.println("  \"petId\": \"luna\",");
    Serial.print("  \"timestampMs\": ");
    Serial.print(sample.timestampMs);
    Serial.println(",");
    Serial.print("  \"transport\": \"");
    Serial.print(transport);
    Serial.println("\",");
    Serial.print("  \"temperatureC\": ");
    Serial.print(sample.temperatureC, 2);
    Serial.println(",");
    Serial.print("  \"activityRms\": ");
    Serial.print(sample.activityRms, 3);
    Serial.println(",");
    Serial.print("  \"inactiveHours\": ");
    Serial.print(sample.inactiveHours, 2);
    Serial.println(",");
    Serial.print("  \"continuousActivityMinutes\": ");
    Serial.print(sample.continuousActivityMinutes);
    Serial.println(",");
    Serial.print("  \"batteryPercent\": ");
    Serial.print(sample.batteryPercent);
    Serial.println(",");
    Serial.print("  \"bleConnected\": ");
    Serial.print(sample.bleConnected ? "true" : "false");
    Serial.println(",");
    Serial.print("  \"bleDisconnectedSeconds\": ");
    Serial.print(sample.bleDisconnectedSeconds);
    Serial.println(",");
    Serial.print("  \"bufferRecords\": ");
    Serial.print(pendingRecords);
    Serial.println(",");
    Serial.print("  \"gnssActive\": ");
    Serial.print(sample.gnssActive ? "true" : "false");
    Serial.println(",");
    Serial.print("  \"location\": ");
    if (sample.gnssActive) {
      Serial.println("{");
      Serial.print("    \"lat\": ");
      Serial.print(sample.latitude, 6);
      Serial.println(",");
      Serial.print("    \"lng\": ");
      Serial.print(sample.longitude, 6);
      Serial.println();
      Serial.print("  }");
    } else {
      Serial.print("null");
    }
    Serial.println(",");
    Serial.print("  \"deviceMode\": \"");
    Serial.print(modeToString(sample.deviceMode));
    Serial.println("\",");
    Serial.print("  \"activityClass\": \"");
    Serial.print(activityToString(analysis.activity));
    Serial.println("\",");
    Serial.print("  \"alertFlags\": ");
    Serial.print(analysis.alerts);
    Serial.println(",");
    Serial.print("  \"alerts\": ");
    printAlertsArray(analysis.alerts);
    Serial.println();
    Serial.println("}");
  }

private:
  void printAlertsArray(uint16_t alerts) const {
    Serial.print("[");
    bool first = true;
    printAlertItem(alerts, ALERT_INACTIVITY, "INACTIVIDAD", first);
    printAlertItem(alerts, ALERT_HIGH_ACTIVITY, "ALTA_ACTIVIDAD", first);
    printAlertItem(alerts, ALERT_ACTIVITY_DELTA, "CAMBIO_ACTIVIDAD", first);
    printAlertItem(alerts, ALERT_THERMAL_EVENT, "EVENTO_TERMICO", first);
    printAlertItem(alerts, ALERT_BLE_LOSS, "PERDIDA_BLE", first);
    printAlertItem(alerts, ALERT_LOW_BATTERY, "BATERIA_BAJA", first);
    printAlertItem(alerts, ALERT_CRITICAL, "EVENTO_CRITICO", first);
    Serial.print("]");
  }

  void printAlertItem(uint16_t alerts, uint16_t flag, const char *name, bool &first) const {
    if ((alerts & flag) == 0) {
      return;
    }
    if (!first) {
      Serial.print(", ");
    }
    Serial.print("\"");
    Serial.print(name);
    Serial.print("\"");
    first = false;
  }

  const char *modeToString(DeviceMode mode) const {
    switch (mode) {
      case MODE_INIT: return "INIT";
      case MODE_DEEP_SLEEP: return "DEEP_SLEEP";
      case MODE_PERIODIC_MONITORING: return "MONITOREO_PERIODICO";
      case MODE_PROCESSING: return "PROCESAMIENTO";
      case MODE_BUFFERING: return "BUFFER_LOCAL";
      case MODE_BLE_SYNC: return "BLE_SYNC";
      case MODE_CRITICAL_EVENT: return "EVENTO_CRITICO";
      case MODE_GNSS_ACTIVE: return "GNSS_ACTIVO";
      default: return "DESCONOCIDO";
    }
  }

  const char *activityToString(ActivityClass activity) const {
    switch (activity) {
      case ACTIVITY_REST: return "REPOSO";
      case ACTIVITY_LIGHT: return "ACTIVIDAD_LIGERA";
      case ACTIVITY_NORMAL: return "ACTIVIDAD_NORMAL";
      case ACTIVITY_HIGH: return "ALTA_ACTIVIDAD";
      default: return "DESCONOCIDA";
    }
  }
};
