import esphome.codegen as cg
from esphome.components import sensor, voltage_sampler
import esphome.config_validation as cv
from esphome.const import CONF_ID, CONF_NUMBER

from .. import MCP3204, mcp3204_ns

AUTO_LOAD = ["voltage_sampler"]

DEPENDENCIES = ["mcp3204"]

MCP3204Sensor = mcp3204_ns.class_(
    "MCP3204Sensor", sensor.Sensor, cg.PollingComponent, voltage_sampler.VoltageSampler
)
CONF_MCP3204_ID = "mcp3204_id"

CONFIG_SCHEMA = (
    sensor.sensor_schema(MCP3204Sensor)
    .extend(
        {
            cv.GenerateID(CONF_MCP3204_ID): cv.use_id(MCP3204),
            cv.Required(CONF_NUMBER): cv.int_range(min=0, max=7),
        }
    )
    .extend(cv.polling_component_schema("60s"))
)


async def to_code(config):
    var = cg.new_Pvariable(
        config[CONF_ID],
        config[CONF_NUMBER],
    )
    await cg.register_parented(var, config[CONF_MCP3204_ID])
    await cg.register_component(var, config)
    await sensor.register_sensor(var, config)
