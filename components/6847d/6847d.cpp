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
  // ------------------------------------------------------------
  // Start a combined temperature + pressure conversion.
  // Register 0x30, value 0x0A.
  // ------------------------------------------------------------

  uint8_t command = CMD_MEASURE;

  auto err = this->write_register(CMD_REGISTER, &command, 1);

  if (err != i2c::ERROR_OK) {
    ESP_LOGW(
        TAG,
        "Failed to start conversion, I2C error=%d",
        static_cast<int>(err)
    );
    return false;
  }

  // The datasheet specifies approximately 20 ms for conversion.
  delay(20);

  // ------------------------------------------------------------
  // Read the command/status register.
  //
  // Bit 3 = Sco
  //   1 = conversion in progress
  //   0 = conversion complete
  // ------------------------------------------------------------

  uint8_t cmd_status = 0;

  err = this->read_register(CMD_REGISTER, &cmd_status, 1);

  if (err != i2c::ERROR_OK) {
    ESP_LOGW(
        TAG,
        "Failed to read CMD register, I2C error=%d",
        static_cast<int>(err)
    );
    return false;
  }

  ESP_LOGD(
      TAG,
      "CMD register = 0x%02X, Sco=%d",
      cmd_status,
      (cmd_status >> 3) & 0x01
  );

  // ------------------------------------------------------------
  // Read the three pressure registers:
  //
  // 0x06 = DATA_MSB
  // 0x07 = DATA_CSB
  // 0x08 = DATA_LSB
  // ------------------------------------------------------------

  uint8_t data[3] = {0, 0, 0};

  err = this->read_register(PRESSURE_MSB, data, 3);

  if (err != i2c::ERROR_OK) {
    ESP_LOGW(
        TAG,
        "Failed to read pressure registers, I2C error=%d",
        static_cast<int>(err)
    );
    return false;
  }

  // Print the actual bytes received.
  ESP_LOGD(
      TAG,
      "Pressure registers: 0x%02X 0x%02X 0x%02X",
      data[0],
      data[1],
      data[2]
  );

  // ------------------------------------------------------------
  // Assemble the 24-bit pressure value.
  // ------------------------------------------------------------

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

  // ------------------------------------------------------------
  // Current diagnostic pressure conversion.
  //
  // For the -100 to +100 kPa version:
  // K = 64
  // Result is in Pa.
  //
  // We deliberately leave this calculation unchanged for now.
  // ------------------------------------------------------------

  const float pressure_pa =
      static_cast<float>(signed_raw) / PRESSURE_K;

  *pressure_kpa = pressure_pa / 1000.0f;

  // ------------------------------------------------------------
  // Log everything together so we can compare the raw bytes
  // with the calculated pressure.
  // ------------------------------------------------------------

  ESP_LOGD(
      TAG,
      "Raw=%ld (0x%06lX) Pressure=%.3f kPa",
      static_cast<long>(signed_raw),
      static_cast<unsigned long>(raw),
      *pressure_kpa
  );

  this->status_clear_warning();

  return true;
}

}  // namespace xgzp6847d
}  // namespace esphome
