#pragma once

#include <stdint.h>
#include <stddef.h>

/*! @name BMI2 register addresses */
inline constexpr uint8_t BMI2_CHIP_ID_ADDR             = 0x00;
inline constexpr uint8_t BMI2_STATUS_ADDR              = 0x03;
inline constexpr uint8_t BMI2_AUX_X_LSB_ADDR           = 0x04;
inline constexpr uint8_t BMI2_ACC_X_LSB_ADDR           = 0x0C;
inline constexpr uint8_t BMI2_GYR_X_LSB_ADDR           = 0x12;
inline constexpr uint8_t BMI2_SENSORTIME_ADDR          = 0x18;
inline constexpr uint8_t BMI2_EVENT_ADDR               = 0x1B;
inline constexpr uint8_t BMI2_INT_STATUS_0_ADDR        = 0x1C;
inline constexpr uint8_t BMI2_INT_STATUS_1_ADDR        = 0x1D;
inline constexpr uint8_t BMI2_INTERNAL_STATUS_ADDR     = 0x21;
inline constexpr uint8_t BMI2_TEMPERATURE_0_ADDR       = 0x22;
inline constexpr uint8_t BMI2_TEMPERATURE_1_ADDR       = 0x23;
inline constexpr uint8_t BMI2_FIFO_LENGTH_0_ADDR       = 0x24;
inline constexpr uint8_t BMI2_FIFO_DATA_ADDR           = 0x26;
inline constexpr uint8_t BMI2_FEAT_PAGE_ADDR           = 0x2F;
inline constexpr uint8_t BMI2_FEATURES_REG_ADDR        = 0x30;
inline constexpr uint8_t BMI2_ACC_CONF_ADDR            = 0x40;
inline constexpr uint8_t BMI2_ACC_RANGE_ADDR           = 0x41;
inline constexpr uint8_t BMI2_GYR_CONF_ADDR            = 0x42;
inline constexpr uint8_t BMI2_GYR_RANGE_ADDR           = 0x43;
inline constexpr uint8_t BMI2_FIFO_CONFIG_0_ADDR       = 0x48;
inline constexpr uint8_t BMI2_FIFO_CONFIG_1_ADDR       = 0x49;
inline constexpr uint8_t BMI2_INT1_IO_CTRL_ADDR        = 0x53;
inline constexpr uint8_t BMI2_INT2_IO_CTRL_ADDR        = 0x54;
inline constexpr uint8_t BMI2_INT_LATCH_ADDR           = 0x55;
inline constexpr uint8_t BMI2_INT1_MAP_FEAT_ADDR       = 0x56;
inline constexpr uint8_t BMI2_INT2_MAP_FEAT_ADDR       = 0x57;
inline constexpr uint8_t BMI2_INT_MAP_DATA_ADDR        = 0x58;
inline constexpr uint8_t BMI2_INIT_CTRL_ADDR           = 0x59;
inline constexpr uint8_t BMI2_INIT_ADDR_0              = 0x5B;
inline constexpr uint8_t BMI2_INIT_ADDR_1              = 0x5C;
inline constexpr uint8_t BMI2_INIT_DATA_ADDR           = 0x5E;
inline constexpr uint8_t BMI2_NVM_CONF_ADDR            = 0x6A;
inline constexpr uint8_t BMI2_IF_CONF_ADDR             = 0x6B;
inline constexpr uint8_t BMI2_NV_CONF_ADDR             = 0x70;
inline constexpr uint8_t BMI2_PWR_CONF_ADDR            = 0x7C;
inline constexpr uint8_t BMI2_PWR_CTRL_ADDR            = 0x7D;
inline constexpr uint8_t BMI2_CMD_REG_ADDR             = 0x7E;

/*! @name BMI2 I2C addresses */
inline constexpr uint8_t BMI2_I2C_PRIM_ADDR            = 0x68;
inline constexpr uint8_t BMI2_I2C_SEC_ADDR             = 0x69;

/*! @name BMI2 Commands */
inline constexpr uint8_t BMI2_SOFT_RESET_CMD           = 0xB6;
inline constexpr uint8_t BMI2_FIFO_FLUSH_CMD           = 0xB0;

/*! @name Chip ID */
inline constexpr uint8_t BMI270_CHIP_ID                = 0x24;

/*! @name Accelerometer Bandwidth parameters */
inline constexpr uint8_t BMI2_ACC_OSR4_AVG1            = 0x00;
inline constexpr uint8_t BMI2_ACC_OSR2_AVG2            = 0x01;
inline constexpr uint8_t BMI2_ACC_NORMAL_AVG4          = 0x02;
inline constexpr uint8_t BMI2_ACC_CIC_AVG8             = 0x03;
inline constexpr uint8_t BMI2_ACC_RES_AVG16            = 0x04;
inline constexpr uint8_t BMI2_ACC_RES_AVG32            = 0x05;
inline constexpr uint8_t BMI2_ACC_RES_AVG64            = 0x06;
inline constexpr uint8_t BMI2_ACC_RES_AVG128           = 0x07;

/*! @name Accelerometer Output Data Rate */
inline constexpr uint8_t BMI2_ACC_ODR_0_78HZ           = 0x01;
inline constexpr uint8_t BMI2_ACC_ODR_1_56HZ           = 0x02;
inline constexpr uint8_t BMI2_ACC_ODR_3_12HZ           = 0x03;
inline constexpr uint8_t BMI2_ACC_ODR_6_25HZ           = 0x04;
inline constexpr uint8_t BMI2_ACC_ODR_12_5HZ           = 0x05;
inline constexpr uint8_t BMI2_ACC_ODR_25HZ             = 0x06;
inline constexpr uint8_t BMI2_ACC_ODR_50HZ             = 0x07;
inline constexpr uint8_t BMI2_ACC_ODR_100HZ            = 0x08;
inline constexpr uint8_t BMI2_ACC_ODR_200HZ            = 0x09;
inline constexpr uint8_t BMI2_ACC_ODR_400HZ            = 0x0A;
inline constexpr uint8_t BMI2_ACC_ODR_800HZ            = 0x0B;
inline constexpr uint8_t BMI2_ACC_ODR_1600HZ           = 0x0C;

/*! @name Accelerometer G Range */
inline constexpr uint8_t BMI2_ACC_RANGE_2G             = 0x00;
inline constexpr uint8_t BMI2_ACC_RANGE_4G             = 0x01;
inline constexpr uint8_t BMI2_ACC_RANGE_8G             = 0x02;
inline constexpr uint8_t BMI2_ACC_RANGE_16G            = 0x03;

/*! @name Mask definitions for accelerometer configuration register */
inline constexpr uint8_t BMI2_ACC_RANGE_MASK           = 0x03;
inline constexpr uint8_t BMI2_ACC_ODR_MASK             = 0x0F;
inline constexpr uint8_t BMI2_ACC_BW_PARAM_MASK        = 0x70;
inline constexpr uint8_t BMI2_ACC_FILTER_PERF_MODE_MASK = 0x80;

/*! @name Bit position definitions for accelerometer configuration register */
inline constexpr uint8_t BMI2_ACC_BW_PARAM_POS         = 0x04;
inline constexpr uint8_t BMI2_ACC_FILTER_PERF_MODE_POS = 0x07;

/*! @name Gyroscope Bandwidth parameters */
inline constexpr uint8_t BMI2_GYR_OSR4_MODE            = 0x00;
inline constexpr uint8_t BMI2_GYR_OSR2_MODE            = 0x01;
inline constexpr uint8_t BMI2_GYR_NORMAL_MODE          = 0x02;

/*! @name Gyroscope Output Data Rate */
inline constexpr uint8_t BMI2_GYR_ODR_25HZ             = 0x06;
inline constexpr uint8_t BMI2_GYR_ODR_50HZ             = 0x07;
inline constexpr uint8_t BMI2_GYR_ODR_100HZ            = 0x08;
inline constexpr uint8_t BMI2_GYR_ODR_200HZ            = 0x09;
inline constexpr uint8_t BMI2_GYR_ODR_400HZ            = 0x0A;
inline constexpr uint8_t BMI2_GYR_ODR_800HZ            = 0x0B;
inline constexpr uint8_t BMI2_GYR_ODR_1600HZ           = 0x0C;
inline constexpr uint8_t BMI2_GYR_ODR_3200HZ           = 0x0D;

/*! @name Gyroscope Angular Rate Measurement Range */
inline constexpr uint8_t BMI2_GYR_RANGE_2000           = 0x00;
inline constexpr uint8_t BMI2_GYR_RANGE_1000           = 0x01;
inline constexpr uint8_t BMI2_GYR_RANGE_500            = 0x02;
inline constexpr uint8_t BMI2_GYR_RANGE_250            = 0x03;
inline constexpr uint8_t BMI2_GYR_RANGE_125            = 0x04;

/*! @name Mask definitions for gyroscope configuration register */
inline constexpr uint8_t BMI2_GYR_RANGE_MASK           = 0x07;
inline constexpr uint8_t BMI2_GYR_ODR_MASK             = 0x0F;
inline constexpr uint8_t BMI2_GYR_BW_PARAM_MASK        = 0x30;
inline constexpr uint8_t BMI2_GYR_NOISE_PERF_MODE_MASK = 0x40;
inline constexpr uint8_t BMI2_GYR_FILTER_PERF_MODE_MASK = 0x80;

/*! @name Bit position definitions for gyroscope configuration register */
inline constexpr uint8_t BMI2_GYR_BW_PARAM_POS         = 0x04;
inline constexpr uint8_t BMI2_GYR_NOISE_PERF_MODE_POS  = 0x06;
inline constexpr uint8_t BMI2_GYR_FILTER_PERF_MODE_POS = 0x07;

/*! @name Filter/Noise performance modes */
inline constexpr uint8_t BMI2_POWER_OPT_MODE           = 0;
inline constexpr uint8_t BMI2_PERF_OPT_MODE            = 1;
