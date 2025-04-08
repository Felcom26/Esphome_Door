import esphome.codegen as cg
from esphome.components import display
import esphome.config_validation as cv
from esphome.const import CONF_ID, CONF_LAMBDA
from esphome.pins import gpio_pin_schema

auto_door_ns = cg.esphome_ns.namespace("auto_door")
AUTODOORComponent = auto_door_ns.class_("AUTODOORComponent", cg.PollingComponent)
AUTODOORComponentRef = AUTODOORComponent.operator("ref")

CONF_DRIVE_PIN = "drive_pin"
CONF_DIR_PIN = "dir_pin"
CONF_ROT_SENSOR_PIN = "rotsen_pin"
CONF_ENGAGE_MOTOR_PIN = "engage_pin"
CONF_POT_PIN = "pot_pin"
CONF_ES_OFF_PIN = "esoff_pin"
CONF_ES_ON_PIN = "eson_pin"
CONF_ANG_OPEN = "ang_open"
CONF_ANG_CLOSE = "ang_close"

CONFIG_SCHEMA = display.BASIC_DISPLAY_SCHEMA.extend(
    {
        cv.GenerateID(): cv.declare_id(AUTODOORComponent),
        cv.Required(CONF_DRIVE_PIN): gpio_pin_schema,
        cv.Required(CONF_DIR_PIN): gpio_pin_schema,
        cv.Required(CONF_ROT_SENSOR_PIN): gpio_pin_schema,
        cv.Required(CONF_ENGAGE_MOTOR_PIN): gpio_pin_schema,
        cv.Required(CONF_POT_PIN): gpio_pin_schema,
        cv.Required(CONF_ES_OFF_PIN): gpio_pin_schema,
        cv.Required(CONF_ES_ON_PIN): gpio_pin_schema,
        cv.Optional(CONF_ANG_OPEN, default=238): cv.int_,
        cv.Optional(CONF_ANG_CLOSE, default=118): cv.int_,
    }
).extend(cv.polling_component_schema("50ms"))


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await display.register_display(var, config)

    drive_pin = await cg.gpio_pin_expression(config[CONF_DRIVE_PIN])
    cg.add(var.set_drive_pin(drive_pin))

    dir_pin = await cg.gpio_pin_expression(config[CONF_DIR_PIN])
    cg.add(var.set_dir_pin(dir_pin))

    rotsen_pin = await cg.gpio_pin_expression(config[CONF_ROT_SENSOR_PIN])
    cg.add(var.set_rotsen_pin(rotsen_pin))

    engage_pin = await cg.gpio_pin_expression(config[CONF_ENGAGE_MOTOR_PIN])
    cg.add(var.set_engage_pin(engage_pin))

    pot_pin = await cg.gpio_pin_expression(config[CONF_POT_PIN])
    cg.add(var.set_pot_pin(pot_pin))

    esoff_pin = await cg.gpio_pin_expression(config[CONF_ES_OFF_PIN])
    cg.add(var.set_esoff_pin(esoff_pin))

    eson_pin = await cg.gpio_pin_expression(config[CONF_ES_ON_PIN])
    cg.add(var.set_eson_pin(eson_pin))

    open_angle = config[CONF_ANG_OPEN]
    if open_angle < 0 or open_angle > 270:
        raise cv.Invalid(f"ang_open out of range (0-270): {open_angle}")
    cg.add(var.set_ang_open(open_angle))

    close_angle = config[CONF_ANG_CLOSE]
    if close_angle < 0 or close_angle > 270:
        raise cv.Invalid(f"ang_close out of range (0-270): {close_angle}")
    cg.add(var.set_ang_close(close_angle))

    # cg.add(var.set_ang_open(config[CONF_ANG_OPEN]))
    # cg.add(var.set_ang_close(config[CONF_ANG_CLOSE]))

    if CONF_LAMBDA in config:
        lambda_ = await cg.process_lambda(
            config[CONF_LAMBDA], [(AUTODOORComponentRef, "it")], return_type=cg.void
        )
        cg.add(var.set_writer(lambda_))

    print("DEBUG CONFIG:")
    print(config)
