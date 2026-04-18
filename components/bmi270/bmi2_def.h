#include <stdio.h>
#include <stdint.h>
#include <stddef.h>


/*! @name BMI2 register addresses */
static constexpr const uint8_t BMI2_CHIP_ID_ADDR                             0x00;
static constexpr const uint8_t BMI2_STATUS_ADDR                              0x03;
static constexpr const uint8_t BMI2_AUX_X_LSB_ADDR                           0x04;
static constexpr const uint8_t BMI2_ACC_X_LSB_ADDR                           0x0C;
static constexpr const uint8_t BMI2_GYR_X_LSB_ADDR                           0x12;
static constexpr const uint8_t BMI2_SENSORTIME_ADDR                          0x18;
static constexpr const uint8_t BMI2_EVENT_ADDR                               0x1B;
static constexpr const uint8_t BMI2_INT_STATUS_0_ADDR                        0x1C;
static constexpr const uint8_t BMI2_INT_STATUS_1_ADDR                        0x1D;
static constexpr const uint8_t BMI2_SC_OUT_0_ADDR                            0x1E;
static constexpr const uint8_t BMI2_SYNC_COMMAND_ADDR                        0x1E;
static constexpr const uint8_t BMI2_GYR_CAS_GPIO0_ADDR                       0x1E;
static constexpr const uint8_t BMI2_DSD_OUT_ADDR                             0x1E;
static constexpr const uint8_t BMI2_INTERNAL_STATUS_ADDR                     0x21;
static constexpr const uint8_t BMI2_TEMPERATURE_0_ADDR                       0x22;
static constexpr const uint8_t BMI2_TEMPERATURE_1_ADDR                       0x23;
static constexpr const uint8_t BMI2_FIFO_LENGTH_0_ADDR                       0x24;
static constexpr const uint8_t BMI2_FIFO_DATA_ADDR                           0x26;
static constexpr const uint8_t BMI2_FEAT_PAGE_ADDR                           0x2F;
static constexpr const uint8_t BMI2_FEATURES_REG_ADDR                        0x30;
static constexpr const uint8_t BMI2_ACC_CONF_ADDR                            0x40;
static constexpr const uint8_t BMI2_GYR_CONF_ADDR                            0x42;
static constexpr const uint8_t BMI2_AUX_CONF_ADDR                            0x44;
static constexpr const uint8_t BMI2_FIFO_DOWNS_ADDR                          0x45;
static constexpr const uint8_t BMI2_FIFO_WTM_0_ADDR                          0x46;
static constexpr const uint8_t BMI2_FIFO_WTM_1_ADDR                          0x47;
static constexpr const uint8_t BMI2_FIFO_CONFIG_0_ADDR                       0x48;
static constexpr const uint8_t BMI2_FIFO_CONFIG_1_ADDR                       0x49;
static constexpr const uint8_t BMI2_SATURATION_ADDR                          0x4A;
static constexpr const uint8_t BMI2_AUX_DEV_ID_ADDR                          0x4B;
static constexpr const uint8_t BMI2_AUX_IF_CONF_ADDR                         0x4C;
static constexpr const uint8_t BMI2_AUX_RD_ADDR                              0x4D;
static constexpr const uint8_t BMI2_AUX_WR_ADDR                              0x4E;
static constexpr const uint8_t BMI2_AUX_WR_DATA_ADDR                         0x4F;
static constexpr const uint8_t BMI2_ERR_REG_MSK_ADDR                         0x52;
static constexpr const uint8_t BMI2_INT1_IO_CTRL_ADDR                        0x53;
static constexpr const uint8_t BMI2_INT2_IO_CTRL_ADDR                        0x54;
static constexpr const uint8_t BMI2_INT_LATCH_ADDR                           0x55;
static constexpr const uint8_t BMI2_INT1_MAP_FEAT_ADDR                       0x56;
static constexpr const uint8_t BMI2_INT2_MAP_FEAT_ADDR                       0x57;
static constexpr const uint8_t BMI2_INT_MAP_DATA_ADDR                        0x58;
static constexpr const uint8_t BMI2_INIT_CTRL_ADDR                           0x59;
static constexpr const uint8_t BMI2_INIT_ADDR_0                              0x5B;
static constexpr const uint8_t BMI2_INIT_ADDR_1                              0x5C;
static constexpr const uint8_t BMI2_INIT_DATA_ADDR                           0x5E;
static constexpr const uint8_t BMI2_INTERNAL_ERR_ADDR                        0x5F;
static constexpr const uint8_t BMI2_AUX_IF_TRIM                              0x68;
static constexpr const uint8_t BMI2_GYR_CRT_CONF_ADDR                        0x69;
static constexpr const uint8_t BMI2_NVM_CONF_ADDR                            0x6A;
static constexpr const uint8_t BMI2_IF_CONF_ADDR                             0x6B;
static constexpr const uint8_t BMI2_DRV_STR_ADDR                             0x6C;
static constexpr const uint8_t BMI2_ACC_SELF_TEST_ADDR                       0x6D;
static constexpr const uint8_t BMI2_GYR_SELF_TEST_AXES_ADDR                  0x6E;
static constexpr const uint8_t BMI2_SELF_TEST_MEMS_ADDR                      0x6F;
static constexpr const uint8_t BMI2_NV_CONF_ADDR                             0x70;
static constexpr const uint8_t BMI2_ACC_OFF_COMP_0_ADDR                      0x71;
static constexpr const uint8_t BMI2_GYR_OFF_COMP_3_ADDR                      0x74;
static constexpr const uint8_t BMI2_GYR_OFF_COMP_6_ADDR                      0x77;
static constexpr const uint8_t BMI2_GYR_USR_GAIN_0_ADDR                      0x78;
static constexpr const uint8_t BMI2_PWR_CONF_ADDR                            0x7C;
static constexpr const uint8_t BMI2_PWR_CTRL_ADDR                            0x7D;
static constexpr const uint8_t BMI2_CMD_REG_ADDR                             0x7E;

/*! @name BMI2 I2C address */
static constexpr const uint8_t BMI2_I2C_PRIM_ADDR                            0x68;
static constexpr const uint8_t BMI2_I2C_SEC_ADDR                             0x69;

/*! @name BMI2 Commands */
static constexpr const uint8_t BMI2_G_TRIGGER_CMD                            0x02;
static constexpr const uint8_t BMI2_USR_GAIN_CMD                             0x03;
static constexpr const uint8_t BMI2_NVM_PROG_CMD                             0xA0;
static constexpr const uint8_t BMI2_SOFT_RESET_CMD                           0xB6;
static constexpr const uint8_t BMI2_FIFO_FLUSH_CMD                           0xB0;

// addresses - see https://github.com/m5stack/M5Unified/blob/master/src/utility/imu/BMI270_Class.hpp
// static constexpr const uint8_t CHIP_ID                 = 0x00;
// static constexpr const uint8_t CMD_REG_ADDR            = 0x7E;
// static constexpr const uint8_t PWR_CONF_ADDR           = 0x7C;
// static constexpr const uint8_t INIT_CTRL_ADDR          = 0x59;
// static constexpr const uint8_t INT_MAP_DATA_ADDR       = 0x58;
// static constexpr const uint8_t INIT_ADDR_0             = 0x5B;
// static constexpr const uint8_t INIT_DATA_ADDR          = 0x5E;
// static constexpr const uint8_t INTERNAL_STATUS_ADDR    = 0x21;
// static constexpr const uint8_t IF_CONF_ADDR            = 0x6B;
// static constexpr const uint8_t PWR_CTRL_ADDR           = 0x7D;
// static constexpr const uint8_t ACC_X_LSB_ADDR          = 0x0C; // start add ACC
// static constexpr const uint8_t ACC_CONF                = 0x40; // set acc config
// static constexpr const uint8_t STATUS_ADDR             = 0x03;
// static constexpr const uint8_t INT_STATUS_1_ADDR       = 0x1D;
// static constexpr const uint8_t TEMPERATURE_0_ADDR      = 0x22;

// commands
// static constexpr const uint8_t SOFT_RESET_CMD          = 0xB6;


/******************************************************************************/
/*! @name       Accelerometer Macro Definitions               */
/******************************************************************************/
/*! @name Accelerometer Bandwidth parameters */
static constexpr const uint8_t BMI2_ACC_OSR4_AVG1                            0x00;
static constexpr const uint8_t BMI2_ACC_OSR2_AVG2                            0x01;
static constexpr const uint8_t BMI2_ACC_NORMAL_AVG4                          0x02;
static constexpr const uint8_t BMI2_ACC_CIC_AVG8                             0x03;
static constexpr const uint8_t BMI2_ACC_RES_AVG16                            0x04;
static constexpr const uint8_t BMI2_ACC_RES_AVG32                            0x05;
static constexpr const uint8_t BMI2_ACC_RES_AVG64                            0x06;
static constexpr const uint8_t BMI2_ACC_RES_AVG128                           0x07;

/*! @name Accelerometer Output Data Rate */
static constexpr const uint8_t BMI2_ACC_ODR_0_78HZ                           0x01;
static constexpr const uint8_t BMI2_ACC_ODR_1_56HZ                           0x02;
static constexpr const uint8_t BMI2_ACC_ODR_3_12HZ                           0x03;
static constexpr const uint8_t BMI2_ACC_ODR_6_25HZ                           0x04;
static constexpr const uint8_t BMI2_ACC_ODR_12_5HZ                           0x05;
static constexpr const uint8_t BMI2_ACC_ODR_25HZ                             0x06;
static constexpr const uint8_t BMI2_ACC_ODR_50HZ                             0x07;
static constexpr const uint8_t BMI2_ACC_ODR_100HZ                            0x08;
static constexpr const uint8_t BMI2_ACC_ODR_200HZ                            0x09;
static constexpr const uint8_t BMI2_ACC_ODR_400HZ                            0x0A;
static constexpr const uint8_t BMI2_ACC_ODR_800HZ                            0x0B;
static constexpr const uint8_t BMI2_ACC_ODR_1600HZ                           0x0C;

/*! @name Accelerometer G Range */
static constexpr const uint8_t BMI2_ACC_RANGE_2G                             0x00;
static constexpr const uint8_t BMI2_ACC_RANGE_4G                             0x01;
static constexpr const uint8_t BMI2_ACC_RANGE_8G                             0x02;
static constexpr const uint8_t BMI2_ACC_RANGE_16G                            0x03;

/*! @name Mask definitions for accelerometer configuration register */
static constexpr const uint8_t BMI2_ACC_RANGE_MASK                           0x03;
static constexpr const uint8_t BMI2_ACC_ODR_MASK                             0x0F;
static constexpr const uint8_t BMI2_ACC_BW_PARAM_MASK                        0x70;
static constexpr const uint8_t BMI2_ACC_FILTER_PERF_MODE_MASK                0x80;

/*! @name Bit position definitions for accelerometer configuration register */
static constexpr const uint8_t BMI2_ACC_BW_PARAM_POS                         0x04;
static constexpr const uint8_t BMI2_ACC_FILTER_PERF_MODE_POS                 0x07;

/*! @name Self test macro to define range */
static constexpr const uint8_t BMI2_ACC_SELF_TEST_RANGE                      16;

/*! @name Self test macro to show resulting minimum and maximum difference
 * signal of the axes in mg
 */
static constexpr const uint16_t BMI2_ST_ACC_X_SIG_MIN_DIFF                    16000;
static constexpr const uint16_t BMI2_ST_ACC_Y_SIG_MIN_DIFF                    -15000;
static constexpr const uint16_t BMI2_ST_ACC_Z_SIG_MIN_DIFF                    10000;

/*! @name Mask definitions for accelerometer self-test */
static constexpr const uint8_t BMI2_ACC_SELF_TEST_EN_MASK                    0x01;
static constexpr const uint8_t BMI2_ACC_SELF_TEST_SIGN_MASK                  0x04;
static constexpr const uint8_t BMI2_ACC_SELF_TEST_AMP_MASK                   0x08;

/*! @name Bit Positions for accelerometer self-test */
static constexpr const uint8_t BMI2_ACC_SELF_TEST_SIGN_POS                   0x02;
static constexpr const uint8_t BMI2_ACC_SELF_TEST_AMP_POS                    0x03;

/*! @name MASK definition for gyro self test status  */
static constexpr const uint8_t BMI2_GYR_ST_AXES_DONE_MASK                    0x01;
static constexpr const uint8_t BMI2_GYR_AXIS_X_OK_MASK                       0x02;
static constexpr const uint8_t BMI2_GYR_AXIS_Y_OK_MASK                       0x04;
static constexpr const uint8_t BMI2_GYR_AXIS_Z_OK_MASK                       0x08;

/*! @name Bit position for gyro self test status  */
static constexpr const uint8_t BMI2_GYR_AXIS_X_OK_POS                        0x01;
static constexpr const uint8_t BMI2_GYR_AXIS_Y_OK_POS                        0x02;
static constexpr const uint8_t BMI2_GYR_AXIS_Z_OK_POS                        0x03;

/******************************************************************************/
/*! @name       Gyroscope Macro Definitions               */
/******************************************************************************/
/*! @name Gyroscope Bandwidth parameters */
static constexpr const uint8_t BMI2_GYR_OSR4_MODE                            0x00;
static constexpr const uint8_t BMI2_GYR_OSR2_MODE                            0x01;
static constexpr const uint8_t BMI2_GYR_NORMAL_MODE                          0x02;

/*! @name Gyroscope Output Data Rate */
static constexpr const uint8_t BMI2_GYR_ODR_25HZ                             0x06;
static constexpr const uint8_t BMI2_GYR_ODR_50HZ                             0x07;
static constexpr const uint8_t BMI2_GYR_ODR_100HZ                            0x08;
static constexpr const uint8_t BMI2_GYR_ODR_200HZ                            0x09;
static constexpr const uint8_t BMI2_GYR_ODR_400HZ                            0x0A;
static constexpr const uint8_t BMI2_GYR_ODR_800HZ                            0x0B;
static constexpr const uint8_t BMI2_GYR_ODR_1600HZ                           0x0C;
static constexpr const uint8_t BMI2_GYR_ODR_3200HZ                           0x0D;

/*! @name Gyroscope OIS Range */
static constexpr const uint8_t BMI2_GYR_OIS_250                              0x00;
static constexpr const uint8_t BMI2_GYR_OIS_2000                             0x01;

/*! @name Gyroscope Angular Rate Measurement Range */
static constexpr const uint8_t BMI2_GYR_RANGE_2000                           0x00;
static constexpr const uint8_t BMI2_GYR_RANGE_1000                           0x01;
static constexpr const uint8_t BMI2_GYR_RANGE_500                            0x02;
static constexpr const uint8_t BMI2_GYR_RANGE_250                            0x03;
static constexpr const uint8_t BMI2_GYR_RANGE_125                            0x04;

/*! @name Mask definitions for gyroscope configuration register */
static constexpr const uint8_t BMI2_GYR_RANGE_MASK                           0x07;
static constexpr const uint8_t BMI2_GYR_OIS_RANGE_MASK                       0x08;
static constexpr const uint8_t BMI2_GYR_ODR_MASK                             0x0F;
static constexpr const uint8_t BMI2_GYR_BW_PARAM_MASK                        0x30;
static constexpr const uint8_t BMI2_GYR_NOISE_PERF_MODE_MASK                 0x40;
static constexpr const uint8_t BMI2_GYR_FILTER_PERF_MODE_MASK                0x80;

/*! @name Bit position definitions for gyroscope configuration register */
static constexpr const uint8_t BMI2_GYR_OIS_RANGE_POS                        0x03;
static constexpr const uint8_t BMI2_GYR_BW_PARAM_POS                         0x04;
static constexpr const uint8_t BMI2_GYR_NOISE_PERF_MODE_POS                  0x06;
static constexpr const uint8_t BMI2_GYR_FILTER_PERF_MODE_POS                 0x07;