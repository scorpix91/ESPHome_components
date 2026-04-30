#pragma once

#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/i2c/i2c.h"

namespace esphome {
namespace lis3dh {

// ---------------------------------------------------------------------------
// Accelerometer full-scale range
// ---------------------------------------------------------------------------
enum class AccelRange : uint8_t {
  RANGE_2G  = 0x00,
  RANGE_4G  = 0x01,
  RANGE_8G  = 0x02,
  RANGE_16G = 0x03,
};

// ---------------------------------------------------------------------------
// Output Data Rate
// Normal/HR mode supports: 1, 10, 25, 50, 100, 200, 400, 1344 Hz
// ---------------------------------------------------------------------------
enum class AccelODR : uint8_t {
  ODR_1HZ    = 0x01,
  ODR_10HZ   = 0x02,
  ODR_25HZ   = 0x03,
  ODR_50HZ   = 0x04,
  ODR_100HZ  = 0x05,
  ODR_200HZ  = 0x06,
  ODR_400HZ  = 0x07,
  ODR_1344HZ = 0x09,  // normal/HR mode only
};

// ---------------------------------------------------------------------------
// Operating mode
// ---------------------------------------------------------------------------
enum class OperatingMode : uint8_t {
  NORMAL      = 0,  // 10-bit (LPen=0, HR=0)
  HIGH_RES    = 1,  // 12-bit (LPen=0, HR=1) — best accuracy
  LOW_POWER   = 2,  // 8-bit  (LPen=1, HR=0) — lowest power consumption
};

// ---------------------------------------------------------------------------
// Main component class
// ---------------------------------------------------------------------------
class LIS3DHSensor : public PollingComponent, public i2c::I2CDevice {
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
  void set_temperature_sensor(sensor::Sensor *s) { temperature_sensor_ = s; }

  // --- Configuration setters (called from sensor.py) ---
  void set_accel_range(AccelRange range) { accel_range_ = range; }
  void set_accel_odr(AccelODR odr) { accel_odr_ = odr; }
  void set_operating_mode(OperatingMode mode) { operating_mode_ = mode; }

 protected:
  // --- Setup helpers ---
  bool configure_();
  void update_accel_res_();

  // --- Data acquisition ---
  bool read_accel_(float *x, float *y, float *z);
  bool read_temperature_(float *t);

  // --- I2C helpers ---
  bool write_reg_(uint8_t reg, uint8_t value);
  bool read_reg_(uint8_t reg, uint8_t *value);
  bool read_regs_(uint8_t reg, uint8_t *data, size_t len);

 private:
  // Sensors
  sensor::Sensor *accel_x_sensor_{nullptr};
  sensor::Sensor *accel_y_sensor_{nullptr};
  sensor::Sensor *accel_z_sensor_{nullptr};
  sensor::Sensor *temperature_sensor_{nullptr};

  // User-configurable parameters
  AccelRange    accel_range_{AccelRange::RANGE_2G};
  AccelODR      accel_odr_{AccelODR::ODR_100HZ};
  OperatingMode operating_mode_{OperatingMode::HIGH_RES};

  // Derived conversion factor (g/LSB), updated when range or mode changes
  float accel_res_{0.0f};

  // State
  bool setup_complete_{false};
  i2c::ErrorCode last_error_{i2c::ERROR_OK};
};

}  // namespace lis3dh
}  // namespace esphome
