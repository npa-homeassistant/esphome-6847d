#pragma once

#include "esphome/core/component.h"
#include "esphome/components/i2c/i2c.h"
#include "esphome/components/sensor/sensor.h"

namespace esphome {
namespace 6847d {

class Sensor6847D : public PollingComponent,
                   public sensor::Sensor,
                   public i2c::I2CDevice {
 public:
  void setup() override;
  void update() override;

 protected:
  bool read_pressure(float *pressure_kpa);
};

}  // namespace 6847d
}  // namespace esphome
