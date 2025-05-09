import esphome.codegen as cg
from esphome.components import sensor, spi
import esphome.config_validation as cv
from esphome.const import (
    DEVICE_CLASS_TEMPERATURE,
    STATE_CLASS_MEASUREMENT,
    UNIT_CELSIUS,
)

max6675_ns = cg.esphome_ns.namespace("max6675")
MAX6675Sensor = max6675_ns.class_(
    "MAX6675Sensor", sensor.Sensor, cg.PollingComponent, spi.SPIDevice
)

CONFIG_SCHEMA = (
    sensor.sensor_schema(
        MAX6675Sensor,
        unit_of_measurement=UNIT_CELSIUS,
        accuracy_decimals=1,
        device_class=DEVICE_CLASS_TEMPERATURE,
        state_class=STATE_CLASS_MEASUREMENT,
    )
    .extend(cv.polling_component_schema("60s"))
    .extend(spi.spi_device_schema())
)


async def to_code(config):
    var = await sensor.new_sensor(config)
    await cg.register_component(var, config)
    await spi.register_spi_device(var, config)
