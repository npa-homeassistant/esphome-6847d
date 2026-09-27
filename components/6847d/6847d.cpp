#include "6847d.h"

namespace esphome {
namespace xgzp6847d {

static const char *const TAG = "6847d";

static const uint8_t CMD_REGISTER = 0x30;
static const uint8_t CMD_MEASURE = 0x0A;

static const uint8_t PRESSURE_MSB = 0x06;

static const float PRESSURE_K = 64.0f;

void Sensor6847D::setup() {
  ESP_LOGCONFIG(TAG, "Setting up XGZP6847D...");
}

void Sensor6847D::update() {
  float pressure_kpa;

  if (this->read_pressure(&pressure_kpa)) {
    this->publish_state(pressure_kpa);
  } else {
    ESP_LOGW(TAG, "Failed to read pressure");
    this->status_set_warning();
  }
}

bool Sensor6847D::read_pressure(float *pressure_kpa) {
  // Start a combined pressure/temperature conversion.
  uint8_t command = CMD_MEASURE;

  if (!this->write_register(CMD_REGISTER, &command, 1)) {
    ESP_LOGW(TAG, "Failed to start conversion");
    return false;
  }

  // Allow the sensor to complete the conversion.
  delay(20);

  // Read the three pressure bytes.
  uint8_t data[3];

  if (!this->read_register(PRESSURE_MSB, data, 3)) {
    ESP_LOGW(TAG, "Failed to read pressure registers");
    return false;
  }

  // Assemble the 24-bit value.
  uint32_t raw =
      (static_cast<uint32_t>(data[0]) << 16) |
      (static_cast<uint32_t>(data[1]) << 8) |
      static_cast<uint32_t>(data[2]);

  // Convert 24-bit two's-complement to signed int32.
  int32_t signed_raw;

  if (raw & 0x800000) {
    signed_raw = static_cast<int32_t>(raw) - 0x1000000;
  } else {
    signed_raw = static_cast<int32_t>(raw);
  }

  // K = 64 for the -100 to +100 kPa version.
  //
  // Result from the sensor is in Pa.
  const float pressure_pa =
      static_cast<float>(signed_raw) / PRESSURE_K;

  *pressure_kpa = pressure_pa / 1000.0f;

  ESP_LOGD(
      TAG,
      "Raw=%ld Pressure=%.3f kPa",
      static_cast<long>(signed_raw),
      *pressure_kpa
  );

  this->status_clear_warning();
  return true;
}

}  // namespace xgzp6847d
}  // namespace esphome
