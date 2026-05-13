#pragma once

#include <Arduino.h>

enum DeviceMode {
  MODE_INIT,
  MODE_DEEP_SLEEP,
  MODE_PERIODIC_MONITORING,
  MODE_PROCESSING,
  MODE_BUFFERING,
  MODE_BLE_SYNC,
  MODE_CRITICAL_EVENT,
  MODE_GNSS_ACTIVE
};

enum ActivityClass {
  ACTIVITY_REST,
  ACTIVITY_LIGHT,
  ACTIVITY_NORMAL,
  ACTIVITY_HIGH
};

enum AlertFlags {
  ALERT_NONE = 0,
  ALERT_INACTIVITY = 1 << 0,
  ALERT_HIGH_ACTIVITY = 1 << 1,
  ALERT_ACTIVITY_DELTA = 1 << 2,
  ALERT_THERMAL_EVENT = 1 << 3,
  ALERT_BLE_LOSS = 1 << 4,
  ALERT_LOW_BATTERY = 1 << 5,
  ALERT_CRITICAL = 1 << 6
};

enum EventOverrideFlags {
  EVENT_NONE = 0,
  EVENT_IMU_ACTIVITY = 1 << 0,
  EVENT_BLE_LOSS = 1 << 1,
  EVENT_TEMP_HIGH = 1 << 2,
  EVENT_INACTIVITY = 1 << 3,
  EVENT_LOW_BATTERY = 1 << 4
};

struct WellnessSample {
  unsigned long timestampMs;
  float temperatureC;
  float activityRms;
  float baselineTemperatureC;
  float baselineActivityRms;
  float inactiveHours;
  uint16_t continuousActivityMinutes;
  uint8_t batteryPercent;
  bool bleConnected;
  uint16_t bleDisconnectedSeconds;
  bool gnssActive;
  double latitude;
  double longitude;
  DeviceMode deviceMode;
};

struct WellnessAnalysis {
  ActivityClass activity;
  uint16_t alerts;
  bool criticalEvent;
};

struct BufferedRecord {
  WellnessSample sample;
  WellnessAnalysis analysis;
};
