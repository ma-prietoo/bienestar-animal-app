#pragma once

#include <Arduino.h>
#include <Wire.h>
#include "app_config.h"
#include "data_types.h"

class SensorSim {
public:
  void begin() {
    readCount = 0;
    pinMode(AppConfig::IMU_BUTTON_PIN, INPUT_PULLUP);
    pinMode(AppConfig::BLE_LOSS_BUTTON_PIN, INPUT_PULLUP);
    pinMode(AppConfig::TEMP_EVENT_BUTTON_PIN, INPUT_PULLUP);
    pinMode(AppConfig::INACTIVITY_BUTTON_PIN, INPUT_PULLUP);
    pinMode(AppConfig::LOW_BATTERY_BUTTON_PIN, INPUT_PULLUP);
    pinMode(AppConfig::TEMP_SENSOR_PIN, INPUT);
    pinMode(AppConfig::BATTERY_SENSOR_PIN, INPUT);

    Wire.begin(AppConfig::I2C_SDA_PIN, AppConfig::I2C_SCL_PIN);
    Wire.beginTransmission(MPU6050_ADDRESS);
    Wire.write(0x6B);
    Wire.write(0x00);
    imuOnline = Wire.endTransmission() == 0;
  }

  WellnessSample readSample(uint16_t eventOverrides = EVENT_NONE) {
    readCount++;

    WellnessSample sample {};
    sample.timestampMs = millis();
    sample.baselineTemperatureC = 38.3f;
    sample.baselineActivityRms = 0.24f;
    sample.temperatureC = (eventOverrides & EVENT_TEMP_HIGH) ? 40.4f : simulatedTemperature();
    const bool forceActivity = (eventOverrides & EVENT_IMU_ACTIVITY) != 0;
    const bool forceInactivity = (eventOverrides & EVENT_INACTIVITY) != 0;
    sample.activityRms = forceInactivity ? 0.03f : readActivityRms(forceActivity);
    sample.inactiveHours = forceInactivity ? 1.2f : 0.3f;
    sample.continuousActivityMinutes = forceInactivity ? 0 : (forceActivity ? 12 : 3);
    sample.batteryPercent = (eventOverrides & EVENT_LOW_BATTERY) ? 12 : readBattery();
    sample.bleConnected = (eventOverrides & EVENT_BLE_LOSS) ? false : true;
    sample.bleDisconnectedSeconds = sample.bleConnected ? 0 : 58;
    sample.gnssActive = false;
    sample.latitude = 0.0;
    sample.longitude = 0.0;
    sample.deviceMode = MODE_PERIODIC_MONITORING;

    return sample;
  }

private:
  static constexpr uint8_t MPU6050_ADDRESS = 0x68;
  uint32_t readCount = 0;
  bool imuOnline = false;

  float simulatedTemperature() const {
    const int raw = analogRead(AppConfig::TEMP_SENSOR_PIN);
    if (raw > 0) {
      return ntcTemperatureC(raw);
    }

    return 38.2f + static_cast<float>(readCount % 5) * 0.1f;
  }

  float ntcTemperatureC(int analogValue) const {
    const float beta = 3950.0f;
    const float seriesResistor = 10000.0f;
    const float adcMax = 4095.0f;
    const float safeAnalog = constrain(static_cast<float>(analogValue), 1.0f, adcMax - 1.0f);
    const float resistance = seriesResistor / ((adcMax / safeAnalog) - 1.0f);
    const float kelvin =
      1.0f / ((1.0f / 298.15f) + (1.0f / beta) * log(resistance / seriesResistor));
    return kelvin - 273.15f;
  }

  float readActivityRms(bool forceActivityEvent) const {
    if (forceActivityEvent) {
      return 0.95f;
    }

    if (imuOnline) {
      int16_t ax = 0;
      int16_t ay = 0;
      int16_t az = 0;

      Wire.beginTransmission(MPU6050_ADDRESS);
      Wire.write(0x3B);
      if (Wire.endTransmission(false) == 0 && Wire.requestFrom(MPU6050_ADDRESS, (uint8_t)6) == 6) {
        ax = (Wire.read() << 8) | Wire.read();
        ay = (Wire.read() << 8) | Wire.read();
        az = (Wire.read() << 8) | Wire.read();

        const float gx = static_cast<float>(ax) / 16384.0f;
        const float gy = static_cast<float>(ay) / 16384.0f;
        const float gz = static_cast<float>(az) / 16384.0f;
        return constrain(sqrt((gx * gx + gy * gy + gz * gz) / 3.0f), 0.02f, 1.20f);
      }
    }

    return 0.12f + static_cast<float>(readCount % 4) * 0.12f;
  }

  uint8_t readBattery() {
    const int raw = analogRead(AppConfig::BATTERY_SENSOR_PIN);
    if (raw > 0) {
      return static_cast<uint8_t>(constrain(map(raw, 0, 4095, 0, 100), 0, 100));
    }

    const int simulatedBattery = 82 - static_cast<int>(readCount);
    return static_cast<uint8_t>(constrain(simulatedBattery, 15, 100));
  }
};
