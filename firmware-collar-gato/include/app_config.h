#pragma once

#include <Arduino.h>

namespace AppConfig {
  static constexpr uint32_t SERIAL_BAUD = 115200;

  static constexpr uint8_t IMU_BUTTON_PIN = 13;
  static constexpr uint8_t BLE_LOSS_BUTTON_PIN = 12;
  static constexpr uint8_t TEMP_EVENT_BUTTON_PIN = 14;
  static constexpr uint8_t INACTIVITY_BUTTON_PIN = 27;
  static constexpr uint8_t LOW_BATTERY_BUTTON_PIN = 26;
  static constexpr uint8_t GNSS_LED_PIN = 25;
  static constexpr uint8_t BLE_LED_PIN = 33;
  static constexpr uint8_t ACTIVITY_LED_PIN = 32;
  static constexpr uint8_t I2C_SDA_PIN = 21;
  static constexpr uint8_t I2C_SCL_PIN = 22;
  static constexpr uint8_t TEMP_SENSOR_PIN = 34;
  static constexpr uint8_t BATTERY_SENSOR_PIN = 35;

  static constexpr uint32_t LOOP_INTERVAL_MS = 100;
  static constexpr uint32_t FIRST_SAMPLE_DELAY_MS = 1000;
  static constexpr uint32_t PERIODIC_SAMPLE_MS = 5000;
  static constexpr uint32_t PERIODIC_WAKEUP_SECONDS = 60;

  static constexpr uint16_t BLE_LOSS_ALERT_SECONDS = 30;
  static constexpr float INACTIVITY_ALERT_HOURS = 1.0f;
  static constexpr uint16_t HIGH_ACTIVITY_MINUTES = 10;
  static constexpr float ABRUPT_ACTIVITY_DELTA_PERCENT = 30.0f;
  static constexpr float THERMAL_DELTA_C = 2.0f;
  static constexpr uint8_t LOW_BATTERY_PERCENT = 20;
  static constexpr bool GNSS_ON_CRITICAL_EVENT = true;
}
