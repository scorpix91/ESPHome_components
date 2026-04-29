import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import i2c, sensor
from esphome.const import (
    CONF_ID,
    CONF_TEMPERATURE,
    CONF_ACCELERATION_X,
    CONF_ACCELERATION_Y,
    CONF_ACCELERATION_Z,
    CONF_GYROSCOPE_X,
    CONF_GYROSCOPE_Y,
    CONF_GYROSCOPE_Z,
    DEVICE_CLASS_TEMPERATURE,
    STATE_CLASS_MEASUREMENT,
    UNIT_G,
    ICON_ACCELERATION_X,
    ICON_ACCELERATION_Y,
    ICON_ACCELERATION_Z,
    ICON_GYROSCOPE_X,
    ICON_GYROSCOPE_Y,
    ICON_GYROSCOPE_Z,
    UNIT_DEGREE_PER_SECOND,
    UNIT_CELSIUS,
)

CODEOWNERS = ["@scorpix91"]
DEPENDENCIES = ["i2c"]

# ---------------------------------------------------------------------------
# Namespace / class binding
# ---------------------------------------------------------------------------
bmi270_ns = cg.esphome_ns.namespace("bmi270")
BMI270Sensor = bmi270_ns.class_("BMI270Sensor", cg.PollingComponent, i2c.I2CDevice)

AccelRange = bmi270_ns.enum("AccelRange", is_class=True)
AccelODR   = bmi270_ns.enum("AccelODR",   is_class=True)
GyroRange  = bmi270_ns.enum("GyroRange",  is_class=True)
GyroODR    = bmi270_ns.enum("GyroODR",    is_class=True)

# ---------------------------------------------------------------------------
# Configuration keys (new)
# ---------------------------------------------------------------------------
CONF_ACCEL_RANGE = "accel_range"
CONF_ACCEL_ODR   = "accel_odr"
CONF_GYRO_RANGE  = "gyro_range"
CONF_GYRO_ODR    = "gyro_odr"

# ---------------------------------------------------------------------------
# Enum mappings – YAML string → C++ enum value
# ---------------------------------------------------------------------------
ACCEL_RANGE_OPTIONS = {
    "2g":  AccelRange.RANGE_2G,
    "4g":  AccelRange.RANGE_4G,
    "8g":  AccelRange.RANGE_8G,
    "16g": AccelRange.RANGE_16G,
}

ACCEL_ODR_OPTIONS = {
    "0.78hz":  AccelODR.ODR_0_78HZ,
    "1.56hz":  AccelODR.ODR_1_56HZ,
    "3.12hz":  AccelODR.ODR_3_12HZ,
    "6.25hz":  AccelODR.ODR_6_25HZ,
    "12.5hz":  AccelODR.ODR_12_5HZ,
    "25hz":    AccelODR.ODR_25HZ,
    "50hz":    AccelODR.ODR_50HZ,
    "100hz":   AccelODR.ODR_100HZ,
    "200hz":   AccelODR.ODR_200HZ,
    "400hz":   AccelODR.ODR_400HZ,
    "800hz":   AccelODR.ODR_800HZ,
    "1600hz":  AccelODR.ODR_1600HZ,
}

GYRO_RANGE_OPTIONS = {
    "125dps":  GyroRange.RANGE_125,
    "250dps":  GyroRange.RANGE_250,
    "500dps":  GyroRange.RANGE_500,
    "1000dps": GyroRange.RANGE_1000,
    "2000dps": GyroRange.RANGE_2000,
}

GYRO_ODR_OPTIONS = {
    "25hz":   GyroODR.ODR_25HZ,
    "50hz":   GyroODR.ODR_50HZ,
    "100hz":  GyroODR.ODR_100HZ,
    "200hz":  GyroODR.ODR_200HZ,
    "400hz":  GyroODR.ODR_400HZ,
    "800hz":  GyroODR.ODR_800HZ,
    "1600hz": GyroODR.ODR_1600HZ,
    "3200hz": GyroODR.ODR_3200HZ,
}

# ---------------------------------------------------------------------------
# Reusable sub-schemas for sensor channels
# ---------------------------------------------------------------------------
_ACCEL_SENSOR_SCHEMA = {
    "unit_of_measurement": UNIT_G,
    "accuracy_decimals": 3,
    "state_class": STATE_CLASS_MEASUREMENT,
}
_GYRO_SENSOR_SCHEMA = {
    "unit_of_measurement": UNIT_DEGREE_PER_SECOND,
    "accuracy_decimals": 2,
    "state_class": STATE_CLASS_MEASUREMENT,
}

# ---------------------------------------------------------------------------
# Main config schema
# ---------------------------------------------------------------------------
CONFIG_SCHEMA = (
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(BMI270Sensor),

            # --- Sensor channels ---
            cv.Optional(CONF_ACCELERATION_X): sensor.sensor_schema(
                icon=ICON_ACCELERATION_X,
                **_ACCEL_SENSOR_SCHEMA
            ),
            cv.Optional(CONF_ACCELERATION_Y): sensor.sensor_schema(
                icon=ICON_ACCELERATION_Y,
                **_ACCEL_SENSOR_SCHEMA
            ),
            cv.Optional(CONF_ACCELERATION_Z): sensor.sensor_schema(
                icon=ICON_ACCELERATION_Z,
                **_ACCEL_SENSOR_SCHEMA
            ),
            cv.Optional(CONF_GYROSCOPE_X): sensor.sensor_schema(
                icon=ICON_GYROSCOPE_X,
                **_GYRO_SENSOR_SCHEMA
            ),
            cv.Optional(CONF_GYROSCOPE_Y): sensor.sensor_schema(
                icon=ICON_GYROSCOPE_Y,
                **_GYRO_SENSOR_SCHEMA
            ),
            cv.Optional(CONF_GYROSCOPE_Z): sensor.sensor_schema(
                icon=ICON_GYROSCOPE_Z,
                **_GYRO_SENSOR_SCHEMA
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
            cv.Optional(CONF_GYRO_RANGE, default="2000dps"): cv.enum(
                GYRO_RANGE_OPTIONS, lower=True
            ),
            cv.Optional(CONF_GYRO_ODR, default="100hz"): cv.enum(
                GYRO_ODR_OPTIONS, lower=True
            ),
        }
    )
    .extend(cv.polling_component_schema("60s"))
    .extend(i2c.i2c_device_schema(0x68))  # 0x68 is the BMI270 default I2C address
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
    cg.add(var.set_gyro_range(config[CONF_GYRO_RANGE]))
    cg.add(var.set_gyro_odr(config[CONF_GYRO_ODR]))

    # Sensor channels
    for axis in ("x", "y", "z"):
        accel_key = f"acceleration_{axis}"
        if accel_key in config:
            sens = await sensor.new_sensor(config[accel_key])
            cg.add(getattr(var, f"set_accel_{axis}_sensor")(sens))

        gyro_key = f"gyroscope_{axis}"
        if gyro_key in config:
            sens = await sensor.new_sensor(config[gyro_key])
            cg.add(getattr(var, f"set_gyro_{axis}_sensor")(sens))

    if CONF_TEMPERATURE in config:
        sens = await sensor.new_sensor(config[CONF_TEMPERATURE])
        cg.add(var.set_temperature_sensor(sens))
