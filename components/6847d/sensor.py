import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor
from esphome.const import CONF_ID, CONF_UPDATE_INTERVAL

AUTO_LOAD = ["sensor"]

ns = cg.esphome_ns.namespace("6847d")
Sensor6847D = ns.class_("Sensor6847D", sensor.Sensor, cg.PollingComponent)

CONFIG_SCHEMA = sensor.sensor_schema().extend({
    cv.GenerateID(): cv.declare_id(Sensor6847D),
    cv.Optional(CONF_UPDATE_INTERVAL, default="50ms"): cv.update_interval,
})

def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    yield cg.register_component(var, config)
    yield sensor.register_sensor(var, config)


