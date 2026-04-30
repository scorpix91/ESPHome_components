import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import i2c, sensor
from esphome.const import (
    CONF_ID,
    CONF_TEMPERATURE,
    CONF_ACCELERATION_X,
    CONF_ACCELERATION_Y,
    CONF_ACCELERATION_Z,
    DEVICE_CLASS_TEMPERATURE,
    STATE_CLASS_MEASUREMENT,
    UNIT_G,
    ICON_ACCELERATION_X,
    ICON_ACCELERATION_Y,
    ICON_ACCELERATION_Z,
    UNIT_CELSIUS,
)

CODEOWNERS = ["@DennisGaida"]
DEPENDENCIES = ["i2c"]

# ---------------------------------------------------------------------------
# Namespace / class binding
# ---------------------------------------------------------------------------
lis3dh_ns = cg.esphome_ns.namespace("lis3dh")
LIS3DHSensor = lis3dh_ns.class_("LIS3DHSensor", cg.PollingComponent, i2c.I2CDevice)

AccelRange    = lis3dh_ns.enum("AccelRange",    is_class=True)
AccelODR      = lis3dh_ns.enum("AccelODR",      is_class=True)
OperatingMode = lis3dh_ns.enum("OperatingMode", is_class=True)

# ---------------------------------------------------------------------------
# Configuration keys
# ---------------------------------------------------------------------------
CONF_ACCEL_RANGE    = "accel_range"
CONF_ACCEL_ODR      = "accel_odr"
CONF_OPERATING_MODE = "operating_mode"

# ---------------------------------------------------------------------------
# Enum mappings — YAML string → C++ enum value
# ---------------------------------------------------------------------------
ACCEL_RANGE_OPTIONS = {
    "2g":  AccelRange.RANGE_2G,
    "4g":  AccelRange.RANGE_4G,
    "8g":  AccelRange.RANGE_8G,
    "16g": AccelRange.RANGE_16G,
}

ACCEL_ODR_OPTIONS = {
    "1hz":    AccelODR.ODR_1HZ,
    "10hz":   AccelODR.ODR_10HZ,
    "25hz":   AccelODR.ODR_25HZ,
    "50hz":   AccelODR.ODR_50HZ,
    "100hz":  AccelODR.ODR_100HZ,
    "200hz":  AccelODR.ODR_200HZ,
    "400hz":  AccelODR.ODR_400HZ,
    "1344hz": AccelODR.ODR_1344HZ,
}

OPERATING_MODE_OPTIONS = {
    "normal":    OperatingMode.NORMAL,
    "high_res":  OperatingMode.HIGH_RES,
    "low_power": OperatingMode.LOW_POWER,
}

# ---------------------------------------------------------------------------
# Reusable sub-schema for accelerometer channels
# ---------------------------------------------------------------------------
_ACCEL_SENSOR_SCHEMA = sensor.sensor_schema(
    unit_of_measurement=UNIT_G,
    accuracy_decimals=4,
    state_class=STATE_CLASS_MEASUREMENT,
)

# ---------------------------------------------------------------------------
# Main config schema
# ---------------------------------------------------------------------------
CONFIG_SCHEMA = (
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(LIS3DHSensor),

            # --- Sensor channels ---
            cv.Optional(CONF_ACCELERATION_X): _ACCEL_SENSOR_SCHEMA.extend(
                {"icon": cv.icon(ICON_ACCELERATION_X)}
            ),
            cv.Optional(CONF_ACCELERATION_Y): _ACCEL_SENSOR_SCHEMA.extend(
                {"icon": cv.icon(ICON_ACCELERATION_Y)}
            ),
            cv.Optional(CONF_ACCELERATION_Z): _ACCEL_SENSOR_SCHEMA.extend(
                {"icon": cv.icon(ICON_ACCELERATION_Z)}
            ),
            cv.Optional(CONF_TEMPERATURE): sensor.sensor_schema(
                unit_of_measurement=UNIT_CELSIUS,
                accuracy_decimals=1,
                device_class=DEVICE_CLASS_TEMPERATURE,
                state_class=STATE_CLASS_MEASUREMENT,
            ),

            # --- Hardware parameters ---
            cv.Optional(CONF_ACCEL_RANGE, default="2g"): cv.enum(
                ACCEL_RANGE_OPTIONS, lower=True
            ),
            cv.Optional(CONF_ACCEL_ODR, default="100hz"): cv.enum(
                ACCEL_ODR_OPTIONS, lower=True
            ),
            cv.Optional(CONF_OPERATING_MODE, default="high_res"): cv.enum(
                OPERATING_MODE_OPTIONS, lower=True
            ),
        }
    )
    .extend(cv.polling_component_schema("60s"))
    .extend(i2c.i2c_device_schema(0x18))  # 0x18 = SDO→GND, 0x19 = SDO→VDD
)

# ---------------------------------------------------------------------------
# Code generation
# ---------------------------------------------------------------------------
async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await i2c.register_i2c_device(var, config)

    # Hardware configuration
    cg.add(var.set_accel_range(config[CONF_ACCEL_RANGE]))
    cg.add(var.set_accel_odr(config[CONF_ACCEL_ODR]))
    cg.add(var.set_operating_mode(config[CONF_OPERATING_MODE]))

    # Sensor channels
    for axis in ("x", "y", "z"):
        key = f"acceleration_{axis}"
        if key in config:
            sens = await sensor.new_sensor(config[key])
            cg.add(getattr(var, f"set_accel_{axis}_sensor")(sens))

    if CONF_TEMPERATURE in config:
        sens = await sensor.new_sensor(config[CONF_TEMPERATURE])
        cg.add(var.set_temperature_sensor(sens))
