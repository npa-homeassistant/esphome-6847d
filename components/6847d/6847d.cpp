#include "6847d.h"

namespace esphome {
namespace 6847d {

static const char *const TAG = "6847d";

static const uint8_t CMD_REGISTER = 0x30;
static const uint8_t CMD_MEASURE = 0x0A;

static const uint8_t PRESSURE_MSB = 0x06;
static const uint8_t PRESSURE_CSB = 0x07;
static const uint8_t PRESSURE_LSB = 0x08;

static const float PRESSURE_K = 64.0f;

void Sensor6847D::setup() {
  ESP_LOGCONFIG(TAG, "Setting up XGZP6847D...");
}

void Sensor6847D::update() {
  float pressure_kpa;

  if (this->read_pressure(&pressure_kpa)) {
    publish_state(pressure_kpa);
  } else {
    ESP_LOGW(TAG, "Failed to read pressure");
    status_set_warning();
  }
}

bool Sensor6847D::read_pressure(float *pressure_kpa) {
  // Start combined temperature + pressure conversion.
  uint8_t command = CMD_MEASURE;

  if (!this->write_register(CMD_REGISTER, &command, 1)) {
    ESP_LOGW(TAG, "Failed to start conversion");
    return false;
  }

  // The datasheet specifies approximately 20 ms for conversion.
  delay(20);

  // Read the three pressure bytes.
  uint8_t data[3];

  if (!this->read_register(PRESSURE_MSB, data, 3)) {
    ESP_LOGW(TAG, "Failed to read pressure registers");
    return false;
  }

  uint32_t raw =
      (static_cast<uint32_t>(data[0]) << 16) |
      (static_cast<uint32_t>(data[1]) << 8) |
      static_cast<uint32_t>(data[2]);

  // Convert unsigned 24-bit representation to signed 24-bit
  // two's-complement value.
  int32_t signed_raw;

  if (raw & 0x800000) {
    signed_raw = static_cast<int32_t>(raw) - 0x1000000;
  } else {
    signed_raw = static_cast<int32_t>(raw);
  }

  // K = 64 for the -100 to +100 kPa version.
  //
  // Datasheet result is in Pa.
  float pressure_pa =
      static_cast<float>(signed_raw) / PRESSURE_K;

  *pressure_kpa = pressure_pa / 1000.0f;

  ESP_LOGD(
      TAG,
      "Raw=%ld Pressure=%.3f kPa",
      static_cast<long>(signed_raw),
      *pressure_kpa
  );

  status_clear_warning();
  return true;
}

}  // namespace 6847d
}  // namespace esphome
