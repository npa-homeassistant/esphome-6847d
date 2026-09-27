import esphome.codegen as cg
import esphome.config_validation as cv

from esphome.components import i2c, sensor

DEPENDENCIES = ["i2c"]

ns = cg.esphome_ns.namespace("xgzp6847d")

Sensor6847D = ns.class_(
    "Sensor6847D",
    cg.PollingComponent,
    sensor.Sensor,
    i2c.I2CDevice,
)

CONFIG_SCHEMA = sensor.sensor_schema(
    Sensor6847D,
).extend(
    cv.polling_component_schema("50ms")
).extend(
    i2c.i2c_device_schema(0x6D)
)


async def to_code(config):
    var = await sensor.new_sensor(config)
    await cg.register_component(var, config)
    await i2c.register_i2c_device(var, config)
