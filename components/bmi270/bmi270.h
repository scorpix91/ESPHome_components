#pragma once

#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/i2c/i2c.h"

namespace esphome {
namespace bmi270 {

// Accelerometer range options (±G)
enum class AccelRange : uint8_t {
  RANGE_2G  = 0x00,
  RANGE_4G  = 0x01,
  RANGE_8G  = 0x02,
  RANGE_16G = 0x03,
};

// Accelerometer Output Data Rate
enum class AccelODR : uint8_t {
  ODR_0_78HZ  = 0x01,
  ODR_1_56HZ  = 0x02,
  ODR_3_12HZ  = 0x03,
  ODR_6_25HZ  = 0x04,
  ODR_12_5HZ  = 0x05,
  ODR_25HZ    = 0x06,
  ODR_50HZ    = 0x07,
  ODR_100HZ   = 0x08,
  ODR_200HZ   = 0x09,
  ODR_400HZ   = 0x0A,
  ODR_800HZ   = 0x0B,
  ODR_1600HZ  = 0x0C,
};

// Gyroscope range options (±°/s)
enum class GyroRange : uint8_t {
  RANGE_2000 = 0x00,
  RANGE_1000 = 0x01,
  RANGE_500  = 0x02,
  RANGE_250  = 0x03,
  RANGE_125  = 0x04,
};

// Gyroscope Output Data Rate
enum class GyroODR : uint8_t {
  ODR_25HZ   = 0x06,
  ODR_50HZ   = 0x07,
  ODR_100HZ  = 0x08,
  ODR_200HZ  = 0x09,
  ODR_400HZ  = 0x0A,
  ODR_800HZ  = 0x0B,
  ODR_1600HZ = 0x0C,
  ODR_3200HZ = 0x0D,
};

class BMI270Sensor : public PollingComponent, public i2c::I2CDevice {
 public:
  // --- Lifecycle ---
  void setup() override;
  void dump_config() override;
  void update() override;
  float get_setup_priority() const override;

  // --- Sensor setters (called from sensor.py) ---
  void set_accel_x_sensor(sensor::Sensor *s) { accel_x_sensor_ = s; }
  void set_accel_y_sensor(sensor::Sensor *s) { accel_y_sensor_ = s; }
  void set_accel_z_sensor(sensor::Sensor *s) { accel_z_sensor_ = s; }
  void set_gyro_x_sensor(sensor::Sensor *s) { gyro_x_sensor_ = s; }
  void set_gyro_y_sensor(sensor::Sensor *s) { gyro_y_sensor_ = s; }
  void set_gyro_z_sensor(sensor::Sensor *s) { gyro_z_sensor_ = s; }
  void set_temperature_sensor(sensor::Sensor *s) { temperature_sensor_ = s; }

  // --- Configuration setters (called from sensor.py) ---
  void set_accel_range(AccelRange range) { accel_range_ = range; }
  void set_accel_odr(AccelODR odr) { accel_odr_ = odr; }
  void set_gyro_range(GyroRange range) { gyro_range_ = range; }
  void set_gyro_odr(GyroODR odr) { gyro_odr_ = odr; }

 protected:
  // --- Internal data structures ---
  enum ImuSpec : uint8_t {
    IMU_SPEC_NONE  = 0,
    IMU_SPEC_ACCEL = 0b0001,
    IMU_SPEC_GYRO  = 0b0010,
  };

  struct Point3DI16 {
    union {
      int16_t value[3];
      struct {
        int16_t x;
        int16_t y;
        int16_t z;
      };
    } __attribute__((__packed__));
  };

  struct ImuRawData {
    union {
      int16_t value[7] = {0};
      Point3DI16 sensor[2];
      struct {
        Point3DI16 accel;
        Point3DI16 gyro;
        int16_t temp;
      } __attribute__((__packed__));
    };
  };

  struct ImuVec3 {
    float x{0};
    float y{0};
    float z{0};
  };

  struct ImuData {
    ImuVec3 accel;
    ImuVec3 gyro;
  };

  // --- Setup state machine ---
  void internal_setup_(int stage, int retry = 1);
  bool upload_config_(const uint8_t *data, size_t len);
  void configure_accel_();
  void configure_gyro_();
  void update_accel_res_();
  void update_gyro_res_();

  // --- Data acquisition ---
  ImuSpec read_raw_imu_(ImuRawData *data);
  void read_imu_data_(ImuData *data);
  bool read_temperature_(float *t);

  // --- I2C helpers (thin wrappers that log errors and track last_error_) ---
  bool write_reg_(uint8_t reg, const uint8_t *value, size_t len = 1);
  bool read_reg_(uint8_t reg, uint8_t *value, size_t len = 1);

 private:
  // Sensors
  sensor::Sensor *accel_x_sensor_{nullptr};
  sensor::Sensor *accel_y_sensor_{nullptr};
  sensor::Sensor *accel_z_sensor_{nullptr};
  sensor::Sensor *gyro_x_sensor_{nullptr};
  sensor::Sensor *gyro_y_sensor_{nullptr};
  sensor::Sensor *gyro_z_sensor_{nullptr};
  sensor::Sensor *temperature_sensor_{nullptr};

  // User-configurable parameters
  AccelRange accel_range_{AccelRange::RANGE_2G};
  AccelODR   accel_odr_{AccelODR::ODR_100HZ};
  GyroRange  gyro_range_{GyroRange::RANGE_2000};
  GyroODR    gyro_odr_{GyroODR::ODR_100HZ};

  // Derived conversion factors (recomputed when range changes)
  float accel_res_{2.0f / 32768.0f};
  float gyro_res_{2000.0f / 32768.0f};

  // State
  bool setup_complete_{false};
  ImuSpec specification_{IMU_SPEC_NONE};
  ImuRawData raw_data_{};
  i2c::ErrorCode last_error_{i2c::ERROR_OK};
};

}  // namespace bmi270
}  // namespace esphome
