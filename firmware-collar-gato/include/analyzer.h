#pragma once

#include <Arduino.h>
#include <math.h>
#include "app_config.h"
#include "data_types.h"

class Analyzer {
public:
  WellnessAnalysis analyze(const WellnessSample &sample) const {
    WellnessAnalysis analysis {};
    analysis.activity = classifyActivity(sample.activityRms);
    analysis.alerts = ALERT_NONE;
    analysis.criticalEvent = false;

    const float thermalDelta = fabs(sample.temperatureC - sample.baselineTemperatureC);
    if (thermalDelta >= AppConfig::THERMAL_DELTA_C) {
      analysis.alerts |= ALERT_THERMAL_EVENT;
    }

    if (sample.inactiveHours >= AppConfig::INACTIVITY_ALERT_HOURS) {
      analysis.alerts |= ALERT_INACTIVITY;
    }

    if (sample.continuousActivityMinutes >= AppConfig::HIGH_ACTIVITY_MINUTES ||
        analysis.activity == ACTIVITY_HIGH) {
      analysis.alerts |= ALERT_HIGH_ACTIVITY;
    }

    if (sample.baselineActivityRms > 0.0f) {
      const float activityDeltaPercent =
        fabs((sample.activityRms - sample.baselineActivityRms) /
             sample.baselineActivityRms) * 100.0f;

      if (activityDeltaPercent >= AppConfig::ABRUPT_ACTIVITY_DELTA_PERCENT) {
        analysis.alerts |= ALERT_ACTIVITY_DELTA;
      }
    }

    if (!sample.bleConnected &&
        sample.bleDisconnectedSeconds > AppConfig::BLE_LOSS_ALERT_SECONDS) {
      analysis.alerts |= ALERT_BLE_LOSS;
      analysis.criticalEvent = true;
    }

    if (sample.batteryPercent <= AppConfig::LOW_BATTERY_PERCENT) {
      analysis.alerts |= ALERT_LOW_BATTERY;
    }

    if ((analysis.alerts & (ALERT_THERMAL_EVENT | ALERT_INACTIVITY | ALERT_BLE_LOSS)) != 0) {
      analysis.alerts |= ALERT_CRITICAL;
      analysis.criticalEvent = true;
    }

    return analysis;
  }

private:
  ActivityClass classifyActivity(float activityRms) const {
    if (activityRms < 0.08f) return ACTIVITY_REST;
    if (activityRms < 0.30f) return ACTIVITY_LIGHT;
    if (activityRms < 0.80f) return ACTIVITY_NORMAL;
    return ACTIVITY_HIGH;
  }
};
