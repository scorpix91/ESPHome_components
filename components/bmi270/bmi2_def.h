#include <stdio.h>
#include <stdint.h>
#include <stddef.h>


/*! @name BMI2 register addresses */
#define BMI2_CHIP_ID_ADDR                             UINT8_C(0x00)
#define BMI2_STATUS_ADDR                              UINT8_C(0x03)
#define BMI2_AUX_X_LSB_ADDR                           UINT8_C(0x04)
#define BMI2_ACC_X_LSB_ADDR                           UINT8_C(0x0C)
#define BMI2_GYR_X_LSB_ADDR                           UINT8_C(0x12)
#define BMI2_SENSORTIME_ADDR                          UINT8_C(0x18)
#define BMI2_EVENT_ADDR                               UINT8_C(0x1B)
#define BMI2_INT_STATUS_0_ADDR                        UINT8_C(0x1C)
#define BMI2_INT_STATUS_1_ADDR                        UINT8_C(0x1D)
#define BMI2_SC_OUT_0_ADDR                            UINT8_C(0x1E)
#define BMI2_SYNC_COMMAND_ADDR                        UINT8_C(0x1E)
#define BMI2_GYR_CAS_GPIO0_ADDR                       UINT8_C(0x1E)
#define BMI2_DSD_OUT_ADDR                             UINT8_C(0x1E)
#define BMI2_INTERNAL_STATUS_ADDR                     UINT8_C(0x21)
#define BMI2_TEMPERATURE_0_ADDR                       UINT8_C(0x22)
#define BMI2_TEMPERATURE_1_ADDR                       UINT8_C(0x23)
#define BMI2_FIFO_LENGTH_0_ADDR                       UINT8_C(0x24)
#define BMI2_FIFO_DATA_ADDR                           UINT8_C(0x26)
#define BMI2_FEAT_PAGE_ADDR                           UINT8_C(0x2F)
#define BMI2_FEATURES_REG_ADDR                        UINT8_C(0x30)
#define BMI2_ACC_CONF_ADDR                            UINT8_C(0x40)
#define BMI2_GYR_CONF_ADDR                            UINT8_C(0x42)
#define BMI2_AUX_CONF_ADDR                            UINT8_C(0x44)
#define BMI2_FIFO_DOWNS_ADDR                          UINT8_C(0x45)
#define BMI2_FIFO_WTM_0_ADDR                          UINT8_C(0x46)
#define BMI2_FIFO_WTM_1_ADDR                          UINT8_C(0x47)
#define BMI2_FIFO_CONFIG_0_ADDR                       UINT8_C(0x48)
#define BMI2_FIFO_CONFIG_1_ADDR                       UINT8_C(0x49)
#define BMI2_SATURATION_ADDR                          UINT8_C(0x4A)
#define BMI2_AUX_DEV_ID_ADDR                          UINT8_C(0x4B)
#define BMI2_AUX_IF_CONF_ADDR                         UINT8_C(0x4C)
#define BMI2_AUX_RD_ADDR                              UINT8_C(0x4D)
#define BMI2_AUX_WR_ADDR                              UINT8_C(0x4E)
#define BMI2_AUX_WR_DATA_ADDR                         UINT8_C(0x4F)
#define BMI2_ERR_REG_MSK_ADDR                         UINT8_C(0x52)
#define BMI2_INT1_IO_CTRL_ADDR                        UINT8_C(0x53)
#define BMI2_INT2_IO_CTRL_ADDR                        UINT8_C(0x54)
#define BMI2_INT_LATCH_ADDR                           UINT8_C(0x55)
#define BMI2_INT1_MAP_FEAT_ADDR                       UINT8_C(0x56)
#define BMI2_INT2_MAP_FEAT_ADDR                       UINT8_C(0x57)
#define BMI2_INT_MAP_DATA_ADDR                        UINT8_C(0x58)
#define BMI2_INIT_CTRL_ADDR                           UINT8_C(0x59)
#define BMI2_INIT_ADDR_0                              UINT8_C(0x5B)
#define BMI2_INIT_ADDR_1                              UINT8_C(0x5C)
#define BMI2_INIT_DATA_ADDR                           UINT8_C(0x5E)
#define BMI2_INTERNAL_ERR_ADDR                        UINT8_C(0x5F)
#define BMI2_AUX_IF_TRIM                              UINT8_C(0x68)
#define BMI2_GYR_CRT_CONF_ADDR                        UINT8_C(0x69)
#define BMI2_NVM_CONF_ADDR                            UINT8_C(0x6A)
#define BMI2_IF_CONF_ADDR                             UINT8_C(0x6B)
#define BMI2_DRV_STR_ADDR                             UINT8_C(0x6C)
#define BMI2_ACC_SELF_TEST_ADDR                       UINT8_C(0x6D)
#define BMI2_GYR_SELF_TEST_AXES_ADDR                  UINT8_C(0x6E)
#define BMI2_SELF_TEST_MEMS_ADDR                      UINT8_C(0x6F)
#define BMI2_NV_CONF_ADDR                             UINT8_C(0x70)
#define BMI2_ACC_OFF_COMP_0_ADDR                      UINT8_C(0x71)
#define BMI2_GYR_OFF_COMP_3_ADDR                      UINT8_C(0x74)
#define BMI2_GYR_OFF_COMP_6_ADDR                      UINT8_C(0x77)
#define BMI2_GYR_USR_GAIN_0_ADDR                      UINT8_C(0x78)
#define BMI2_PWR_CONF_ADDR                            UINT8_C(0x7C)
#define BMI2_PWR_CTRL_ADDR                            UINT8_C(0x7D)
#define BMI2_CMD_REG_ADDR                             UINT8_C(0x7E)

/*! @name BMI2 I2C address */
#define BMI2_I2C_PRIM_ADDR                            UINT8_C(0x68)
#define BMI2_I2C_SEC_ADDR                             UINT8_C(0x69)

/*! @name BMI2 Commands */
#define BMI2_G_TRIGGER_CMD                            UINT8_C(0x02)
#define BMI2_USR_GAIN_CMD                             UINT8_C(0x03)
#define BMI2_NVM_PROG_CMD                             UINT8_C(0xA0)
#define BMI2_SOFT_RESET_CMD                           UINT8_C(0xB6)
#define BMI2_FIFO_FLUSH_CMD                           UINT8_C(0xB0)

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
#define BMI2_ACC_OSR4_AVG1                            UINT8_C(0x00)
#define BMI2_ACC_OSR2_AVG2                            UINT8_C(0x01)
#define BMI2_ACC_NORMAL_AVG4                          UINT8_C(0x02)
#define BMI2_ACC_CIC_AVG8                             UINT8_C(0x03)
#define BMI2_ACC_RES_AVG16                            UINT8_C(0x04)
#define BMI2_ACC_RES_AVG32                            UINT8_C(0x05)
#define BMI2_ACC_RES_AVG64                            UINT8_C(0x06)
#define BMI2_ACC_RES_AVG128                           UINT8_C(0x07)

/*! @name Accelerometer Output Data Rate */
#define BMI2_ACC_ODR_0_78HZ                           UINT8_C(0x01)
#define BMI2_ACC_ODR_1_56HZ                           UINT8_C(0x02)
#define BMI2_ACC_ODR_3_12HZ                           UINT8_C(0x03)
#define BMI2_ACC_ODR_6_25HZ                           UINT8_C(0x04)
#define BMI2_ACC_ODR_12_5HZ                           UINT8_C(0x05)
#define BMI2_ACC_ODR_25HZ                             UINT8_C(0x06)
#define BMI2_ACC_ODR_50HZ                             UINT8_C(0x07)
#define BMI2_ACC_ODR_100HZ                            UINT8_C(0x08)
#define BMI2_ACC_ODR_200HZ                            UINT8_C(0x09)
#define BMI2_ACC_ODR_400HZ                            UINT8_C(0x0A)
#define BMI2_ACC_ODR_800HZ                            UINT8_C(0x0B)
#define BMI2_ACC_ODR_1600HZ                           UINT8_C(0x0C)

/*! @name Accelerometer G Range */
#define BMI2_ACC_RANGE_2G                             UINT8_C(0x00)
#define BMI2_ACC_RANGE_4G                             UINT8_C(0x01)
#define BMI2_ACC_RANGE_8G                             UINT8_C(0x02)
#define BMI2_ACC_RANGE_16G                            UINT8_C(0x03)

/*! @name Mask definitions for accelerometer configuration register */
#define BMI2_ACC_RANGE_MASK                           UINT8_C(0x03)
#define BMI2_ACC_ODR_MASK                             UINT8_C(0x0F)
#define BMI2_ACC_BW_PARAM_MASK                        UINT8_C(0x70)
#define BMI2_ACC_FILTER_PERF_MODE_MASK                UINT8_C(0x80)

/*! @name Bit position definitions for accelerometer configuration register */
#define BMI2_ACC_BW_PARAM_POS                         UINT8_C(0x04)
#define BMI2_ACC_FILTER_PERF_MODE_POS                 UINT8_C(0x07)

/*! @name Self test macro to define range */
#define BMI2_ACC_SELF_TEST_RANGE                      UINT8_C(16)

/*! @name Self test macro to show resulting minimum and maximum difference
 * signal of the axes in mg
 */
#define BMI2_ST_ACC_X_SIG_MIN_DIFF                    INT16_C(16000)
#define BMI2_ST_ACC_Y_SIG_MIN_DIFF                    INT16_C(-15000)
#define BMI2_ST_ACC_Z_SIG_MIN_DIFF                    INT16_C(10000)

/*! @name Mask definitions for accelerometer self-test */
#define BMI2_ACC_SELF_TEST_EN_MASK                    UINT8_C(0x01)
#define BMI2_ACC_SELF_TEST_SIGN_MASK                  UINT8_C(0x04)
#define BMI2_ACC_SELF_TEST_AMP_MASK                   UINT8_C(0x08)

/*! @name Bit Positions for accelerometer self-test */
#define BMI2_ACC_SELF_TEST_SIGN_POS                   UINT8_C(0x02)
#define BMI2_ACC_SELF_TEST_AMP_POS                    UINT8_C(0x03)

/*! @name MASK definition for gyro self test status  */
#define BMI2_GYR_ST_AXES_DONE_MASK                    UINT8_C(0x01)
#define BMI2_GYR_AXIS_X_OK_MASK                       UINT8_C(0x02)
#define BMI2_GYR_AXIS_Y_OK_MASK                       UINT8_C(0x04)
#define BMI2_GYR_AXIS_Z_OK_MASK                       UINT8_C(0x08)

/*! @name Bit position for gyro self test status  */
#define BMI2_GYR_AXIS_X_OK_POS                        UINT8_C(0x01)
#define BMI2_GYR_AXIS_Y_OK_POS                        UINT8_C(0x02)
#define BMI2_GYR_AXIS_Z_OK_POS                        UINT8_C(0x03)

/******************************************************************************/
/*! @name       Gyroscope Macro Definitions               */
/******************************************************************************/
/*! @name Gyroscope Bandwidth parameters */
#define BMI2_GYR_OSR4_MODE                            UINT8_C(0x00)
#define BMI2_GYR_OSR2_MODE                            UINT8_C(0x01)
#define BMI2_GYR_NORMAL_MODE                          UINT8_C(0x02)

/*! @name Gyroscope Output Data Rate */
#define BMI2_GYR_ODR_25HZ                             UINT8_C(0x06)
#define BMI2_GYR_ODR_50HZ                             UINT8_C(0x07)
#define BMI2_GYR_ODR_100HZ                            UINT8_C(0x08)
#define BMI2_GYR_ODR_200HZ                            UINT8_C(0x09)
#define BMI2_GYR_ODR_400HZ                            UINT8_C(0x0A)
#define BMI2_GYR_ODR_800HZ                            UINT8_C(0x0B)
#define BMI2_GYR_ODR_1600HZ                           UINT8_C(0x0C)
#define BMI2_GYR_ODR_3200HZ                           UINT8_C(0x0D)

/*! @name Gyroscope OIS Range */
#define BMI2_GYR_OIS_250                              UINT8_C(0x00)
#define BMI2_GYR_OIS_2000                             UINT8_C(0x01)

/*! @name Gyroscope Angular Rate Measurement Range */
#define BMI2_GYR_RANGE_2000                           UINT8_C(0x00)
#define BMI2_GYR_RANGE_1000                           UINT8_C(0x01)
#define BMI2_GYR_RANGE_500                            UINT8_C(0x02)
#define BMI2_GYR_RANGE_250                            UINT8_C(0x03)
#define BMI2_GYR_RANGE_125                            UINT8_C(0x04)

/*! @name Mask definitions for gyroscope configuration register */
#define BMI2_GYR_RANGE_MASK                           UINT8_C(0x07)
#define BMI2_GYR_OIS_RANGE_MASK                       UINT8_C(0x08)
#define BMI2_GYR_ODR_MASK                             UINT8_C(0x0F)
#define BMI2_GYR_BW_PARAM_MASK                        UINT8_C(0x30)
#define BMI2_GYR_NOISE_PERF_MODE_MASK                 UINT8_C(0x40)
#define BMI2_GYR_FILTER_PERF_MODE_MASK                UINT8_C(0x80)

/*! @name Bit position definitions for gyroscope configuration register */
#define BMI2_GYR_OIS_RANGE_POS                        UINT8_C(0x03)
#define BMI2_GYR_BW_PARAM_POS                         UINT8_C(0x04)
#define BMI2_GYR_NOISE_PERF_MODE_POS                  UINT8_C(0x06)
#define BMI2_GYR_FILTER_PERF_MODE_POS                 UINT8_C(0x07)