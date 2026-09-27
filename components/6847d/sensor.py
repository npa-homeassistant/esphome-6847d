import esphome.codegen as cg
import esphome.config_validation as cv

from esphome.components import sensor

ns = cg.esphome_ns.namespace("6847d")

Sensor6847D = ns.class_(
    "Sensor6847D",
    cg.PollingComponent,
    sensor.Sensor,
)

CONFIG_SCHEMA = sensor.sensor_schema(
    Sensor6847D,
).extend(
    cv.polling_component_schema("50ms")
).extend(
    {
        cv.Optional("test_option"): cv.string,
    }
)

async def to_code(config):
    var = await sensor.new_sensor(config)
    await cg.register_component(var, config)
