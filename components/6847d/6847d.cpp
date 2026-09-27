#include "6847d.h"
#include "driver/i2c.h"

static const i2c_port_t I2C_PORT = I2C_NUM_0;
static const uint8_t ADDR = 0x6D;  // your sensor's I2C address

void Sensor6847D::setup() {
  i2c_config_t conf = {
    .mode = I2C_MODE_MASTER,
    .sda_io_num = GPIO_NUM_25,
    .scl_io_num = GPIO_NUM_22,
    .sda_pullup_en = GPIO_PULLUP_ENABLE,
    .scl_pullup_en = GPIO_PULLUP_ENABLE,
    .master = {.clk_speed = 100000}
  };

  i2c_param_config(I2C_PORT, &conf);
  i2c_driver_install(I2C_PORT, conf.mode, 0, 0, 0);
}

void Sensor6847D::update() {
  int32_t raw;
  if (read_pressure(&raw) == ESP_OK) {
    publish_state(raw);
  }
}

esp_err_t Sensor6847D::read_pressure(int32_t *value) {
  uint8_t cmd = 0x00;  // depends on your module; many 6847D modules use 0x00 to trigger a read
  i2c_master_write_to_device(I2C_PORT, ADDR, &cmd, 1, 100 / portTICK_PERIOD_MS);

  uint8_t data[3];
  esp_err_t err = i2c_master_read_from_device(I2C_PORT, ADDR, data, 3, 100 / portTICK_PERIOD_MS);
  if (err != ESP_OK) return err;

  *value = (data[0] << 16) | (data[1] << 8) | data[2];
  return ESP_OK;
}

