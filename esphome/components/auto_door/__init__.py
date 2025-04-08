# from esphome import automation
import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.const import CONF_ID, CONF_UPDATE_INTERVAL

auto_door_ns = cg.esphome_ns.namespace("auto_door")
AUTODOORComponent = auto_door_ns.class_("AUTODOORComponent", cg.PollingComponent)
AUTODOORComponentRef = AUTODOORComponent.operator("ref")

# CONF_DRIVE_PIN = "drive_pin"
# CONF_DIR_PIN = "dir_pin"
# CONF_ROT_SENSOR_PIN = "rotsen_pin"
# CONF_ENGAGE_MOTOR_PIN = "engage_pin"
# CONF_POT_PIN = "pot_pin"
# CONF_ES_OFF_PIN = "esoff_pin"
# CONF_ES_ON_PIN = "eson_pin"
CONF_ANG_OPEN = "ang_open"
CONF_ANG_CLOSE = "ang_close"
CONF_WRITER = "writer"

# CONFIG_SCHEMA = cv.COMPONENT_SCHEMA.extend(
#    {
#        cv.Optional(CONF_UPDATE_INTERVAL, default="500ms"): cv.update_interval,
#        cv.Optional(CONF_WRITER): automation.validate_automation(single=True),
#        cv.GenerateID(): cv.declare_id(AUTODOORComponent),
#        # cv.Required(CONF_DRIVE_PIN): gpio_pin_schema,
#        # cv.Required(CONF_DIR_PIN): gpio_pin_schema,
#        # cv.Required(CONF_ROT_SENSOR_PIN): gpio_pin_schema,
#        # cv.Required(CONF_ENGAGE_MOTOR_PIN): gpio_pin_schema,
#        # cv.Required(CONF_POT_PIN): gpio_pin_schema,
#        # cv.Required(CONF_ES_OFF_PIN): gpio_pin_schema,
#        # cv.Required(CONF_ES_ON_PIN): gpio_pin_schema,
#        cv.Optional(CONF_ANG_OPEN, default=238): cv.int_,
#        cv.Optional(CONF_ANG_CLOSE, default=118): cv.int_,
#    }
# ).extend(cv.polling_component_schema("50ms"))


CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(AUTODOORComponent),
        cv.Required(CONF_ANG_OPEN): cv.int_,
        cv.Required(CONF_ANG_CLOSE): cv.int_,
        cv.Optional(CONF_UPDATE_INTERVAL): cv.update_interval,
        cv.Optional(CONF_WRITER): cv.lambda_,
        # cv.Optional(CONF_WRITER): automation.validate_automation(single=True),
    }
).extend(cv.polling_component_schema("50ms"))


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)

    # var = cg.new_Pvariable(config[CONF_ID])
    # await display.register_display(var, config)

    # drive_pin = await cg.gpio_pin_expression(config[CONF_DRIVE_PIN])
    # cg.add(var.set_drive_pin(drive_pin))

    # dir_pin = await cg.gpio_pin_expression(config[CONF_DIR_PIN])
    # cg.add(var.set_dir_pin(dir_pin))

    # rotsen_pin = await cg.gpio_pin_expression(config[CONF_ROT_SENSOR_PIN])
    # cg.add(var.set_rotsen_pin(rotsen_pin))

    # engage_pin = await cg.gpio_pin_expression(config[CONF_ENGAGE_MOTOR_PIN])
    # cg.add(var.set_engage_pin(engage_pin))

    # pot_pin = await cg.gpio_pin_expression(config[CONF_POT_PIN])
    # cg.add(var.set_pot_pin(pot_pin))

    # esoff_pin = await cg.gpio_pin_expression(config[CONF_ES_OFF_PIN])
    # cg.add(var.set_esoff_pin(esoff_pin))

    # eson_pin = await cg.gpio_pin_expression(config[CONF_ES_ON_PIN])
    # cg.add(var.set_eson_pin(eson_pin))

    cg.add(var.set_ang_open(config[CONF_ANG_OPEN]))
    cg.add(var.set_ang_close(config[CONF_ANG_CLOSE]))

    # if CONF_LAMBDA in config:
    #    lambda_ = await cg.process_lambda(
    #        config[CONF_LAMBDA], [(AUTODOORComponentRef, "it")], return_type=cg.void
    #    )
    #    cg.add(var.set_writer(lambda_))

    if CONF_WRITER in config:
        lambda_ = await cg.process_lambda(
            config[CONF_WRITER], [(AUTODOORComponentRef, "it")], return_type=cg.void
        )
        cg.add(var.set_writer(lambda_))

    # if CONF_WRITER in config:
    #    trigger = cg.new_Pvariable("writer_trigger", automation.Trigger.new())
    #    await automation.build_automation(trigger, [], config[CONF_WRITER])
    #    cg.add(var.set_writer(trigger))

    # if CONF_WRITER in config:
    #    await automation.build_automation(
    #        var.get_writer_trigger(),
    #        [(AUTODOORComponent, "it")],
    #        config[CONF_WRITER],
    #    )
