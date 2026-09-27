#pragma once
#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"

class Sensor6847D : public esphome::Component, public esphome::sensor::Sensor {
 public:
  void setup() override;
  void update() override;

 private:
  esp_err_t read_pressure(int32_t *value);
};

