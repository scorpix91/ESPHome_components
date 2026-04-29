#include "bmi270.h"
#include "bmi270_config.h"
#include "bmi2_def.h"

#include "esphome/core/hal.h"
#include "esphome/core/log.h"

namespace esphome {
namespace bmi270 {

static const char *const TAG = "bmi270";

// ---------------------------------------------------------------------------
// Helpers: resolve range → scale factor
// ---------------------------------------------------------------------------

static float accel_range_to_res(AccelRange range) {
  switch (range) {
    case AccelRange::RANGE_2G:  return 2.0f / 32768.0f;
    case AccelRange::RANGE_4G:  return 4.0f / 32768.0f;
    case AccelRange::RANGE_8G:  return 8.0f / 32768.0f;
    case AccelRange::RANGE_16G: return 16.0f / 32768.0f;
    default:                    return 2.0f / 32768.0f;
  }
}

static float gyro_range_to_res(GyroRange range) {
  switch (range) {
    case GyroRange::RANGE_2000: return 2000.0f / 32768.0f;
    case GyroRange::RANGE_1000: return 1000.0f / 32768.0f;
    case GyroRange::RANGE_500:  return 500.0f  / 32768.0f;
    case GyroRange::RANGE_250:  return 250.0f  / 32768.0f;
    case GyroRange::RANGE_125:  return 125.0f  / 32768.0f;
    default:                    return 2000.0f / 32768.0f;
  }
}

static const char *accel_range_str(AccelRange r) {
  switch (r) {
    case AccelRange::RANGE_2G:  return "±2g";
    case AccelRange::RANGE_4G:  return "±4g";
    case AccelRange::RANGE_8G:  return "±8g";
    case AccelRange::RANGE_16G: return "±16g";
    default: return "unknown";
  }
}

static const char *gyro_range_str(GyroRange r) {
  switch (r) {
    case GyroRange::RANGE_2000: return "±2000°/s";
    case GyroRange::RANGE_1000: return "±1000°/s";
    case GyroRange::RANGE_500:  return "±500°/s";
    case GyroRange::RANGE_250:  return "±250°/s";
    case GyroRange::RANGE_125:  return "±125°/s";
    default: return "unknown";
  }
}

// ---------------------------------------------------------------------------
// setup() – kicks off the asynchronous state machine
// ---------------------------------------------------------------------------

void BMI270Sensor::setup() {
  ESP_LOGCONFIG(TAG, "Setting up BMI270...");
  this->internal_setup_(0);
}

// ---------------------------------------------------------------------------
// State machine
//   stage 0 – chip ID check + soft-reset
//   stage 1 – wait for reset, disable advanced power-save
//   stage 2 – upload firmware blob
//   stage 3 – verify init status, apply user config
// ---------------------------------------------------------------------------

void BMI270Sensor::internal_setup_(int stage, int retry) {
  switch (stage) {
    // -----------------------------------------------------------------------
    case 0: {
      // Verify chip ID (must be 0x24 for BMI270)
      uint8_t chip_id = 0;
      if (!this->read_reg_(BMI2_CHIP_ID_ADDR, &chip_id) || chip_id != 0x24) {
        ESP_LOGE(TAG, "Wrong chip ID: 0x%02X (expected 0x24)", chip_id);
        this->mark_failed();
        return;
      }

      // Soft-reset (NAK on this write is normal – sensor reboots immediately)
      ESP_LOGD(TAG, "Soft reset...");
      this->write_reg_(BMI2_CMD_REG_ADDR, &BMI2_SOFT_RESET_CMD);

      // Datasheet: wait ≥2 ms, then poll PWR_CONF up to 3 times
      this->set_timeout(2, [this]() { this->internal_setup_(1, 3); });
      break;
    }

    // -----------------------------------------------------------------------
    case 1: {
      // After reset PWR_CONF defaults to 0x03 (adv-power-save enabled).
      // It reads 0x00 while the sensor is still booting – retry in that case.
      uint8_t power_conf = 0;
      if (!this->read_reg_(BMI2_PWR_CONF_ADDR, &power_conf)) {
        ESP_LOGE(TAG, "Failed to read PWR_CONF");
        this->mark_failed();
        return;
      }

      if (power_conf == 0x00) {
        if (retry == 0) {
          ESP_LOGE(TAG, "Sensor did not come out of reset (PWR_CONF still 0)");
          this->mark_failed();
          return;
        }
        // Not ready yet – try again in 250 µs
        this->set_timeout(1, [this, retry]() { this->internal_setup_(1, retry - 1); });
        return;
      }

      ESP_LOGD(TAG, "Reset OK (PWR_CONF=0x%02X). Disabling advanced power-save...", power_conf);
      const uint8_t adv_pwr_save_off = 0x00;
      this->write_reg_(BMI2_PWR_CONF_ADDR, &adv_pwr_save_off);

      // Datasheet: wait ≥450 µs before loading config
      this->set_timeout(1, [this]() { this->internal_setup_(2); });
      break;
    }

    // -----------------------------------------------------------------------
    case 2: {
      ESP_LOGV(TAG, "Uploading firmware config blob (%u bytes)...", sizeof(DEFAULT_CONFIGURATION));

      const uint8_t init_ctrl_off = 0x00;
      this->write_reg_(BMI2_INIT_CTRL_ADDR, &init_ctrl_off);

      if (!this->upload_config_(DEFAULT_CONFIGURATION, sizeof(DEFAULT_CONFIGURATION))) {
        ESP_LOGE(TAG, "Firmware upload failed");
        this->mark_failed();
        return;
      }

      const uint8_t init_ctrl_on = 0x01;
      this->write_reg_(BMI2_INIT_CTRL_ADDR, &init_ctrl_on);

      // Map all data-ready interrupts to INT1 (acc + gyro ready bits)
      const uint8_t int_map = 0xFF;
      this->write_reg_(BMI2_INT_MAP_DATA_ADDR, &int_map);

      // Datasheet: wait ≥20 ms for the init sequencer to finish
      this->set_timeout(20, [this]() { this->internal_setup_(3, 9); });
      break;
    }

    // -----------------------------------------------------------------------
    case 3: {
      uint8_t internal_status = 0;
      if (!this->read_reg_(BMI2_INTERNAL_STATUS_ADDR, &internal_status)) {
        ESP_LOGE(TAG, "Failed to read INTERNAL_STATUS");
        this->mark_failed();
        return;
      }

      if (internal_status != 0x01) {
        if (retry == 0) {
          ESP_LOGE(TAG, "Firmware init failed (INTERNAL_STATUS=0x%02X)", internal_status);
          this->mark_failed();
          return;
        }
        this->set_timeout(1, [this, retry]() { this->internal_setup_(3, retry - 1); });
        return;
      }

      ESP_LOGD(TAG, "Firmware init OK (INTERNAL_STATUS=0x%02X)", internal_status);

      // Enable temperature sensor + accelerometer + gyroscope (bits 3:1)
      const uint8_t pwr_ctrl = 0x0E;  // temp | acc | gyr
      if (!this->write_reg_(BMI2_PWR_CTRL_ADDR, &pwr_ctrl)) {
        ESP_LOGE(TAG, "Failed to enable sensors");
        this->mark_failed();
        return;
      }

      // Apply user-configured ranges and ODRs
      this->configure_accel_();
      this->configure_gyro_();

      specification_ = static_cast<ImuSpec>(IMU_SPEC_ACCEL | IMU_SPEC_GYRO);
      setup_complete_ = true;
      ESP_LOGCONFIG(TAG, "BMI270 setup complete.");
      break;
    }
  }
}

// ---------------------------------------------------------------------------
// Accelerometer configuration
// ---------------------------------------------------------------------------

void BMI270Sensor::configure_accel_() {
  // ACC_CONF (0x40): ODR[3:0] | BW_PARAM[6:4] | FILTER_PERF[7]
  // Use performance-optimised filter (bit7=1) + normal BW (0x02 << 4)
  uint8_t acc_conf = (static_cast<uint8_t>(accel_odr_) & 0x0F) |
                     (0x02 << 4) |   // BMI2_ACC_NORMAL_AVG4
                     (1 << 7);       // performance mode

  if (!this->write_reg_(BMI2_ACC_CONF_ADDR, &acc_conf)) {
    ESP_LOGE(TAG, "Failed to write ACC_CONF");
    return;
  }

  uint8_t range_reg = static_cast<uint8_t>(accel_range_) & 0x03;
  if (!this->write_reg_(BMI2_ACC_RANGE_ADDR, &range_reg)) {
    ESP_LOGE(TAG, "Failed to write ACC_RANGE");
    return;
  }

  accel_res_ = accel_range_to_res(accel_range_);
  ESP_LOGCONFIG(TAG, "  Accel range: %s, ODR reg: 0x%02X, res: %.9f g/LSB",
                accel_range_str(accel_range_), static_cast<uint8_t>(accel_odr_), accel_res_);
}

// ---------------------------------------------------------------------------
// Gyroscope configuration
// ---------------------------------------------------------------------------

void BMI270Sensor::configure_gyro_() {
  // GYR_CONF (0x42): ODR[3:0] | BW_PARAM[5:4] | NOISE_PERF[6] | FILTER_PERF[7]
  uint8_t gyr_conf = (static_cast<uint8_t>(gyro_odr_) & 0x0F) |
                     (0x02 << 4) |   // BMI2_GYR_NORMAL_MODE
                     (1 << 6) |      // noise performance mode
                     (1 << 7);       // filter performance mode

  if (!this->write_reg_(BMI2_GYR_CONF_ADDR, &gyr_conf)) {
    ESP_LOGE(TAG, "Failed to write GYR_CONF");
    return;
  }

  uint8_t range_reg = static_cast<uint8_t>(gyro_range_) & 0x07;
  if (!this->write_reg_(BMI2_GYR_RANGE_ADDR, &range_reg)) {
    ESP_LOGE(TAG, "Failed to write GYR_RANGE");
    return;
  }

  gyro_res_ = gyro_range_to_res(gyro_range_);
  ESP_LOGCONFIG(TAG, "  Gyro range: %s, ODR reg: 0x%02X, res: %.6f °/s/LSB",
                gyro_range_str(gyro_range_), static_cast<uint8_t>(gyro_odr_), gyro_res_);
}

// ---------------------------------------------------------------------------
// Firmware blob upload (burst write)
// ---------------------------------------------------------------------------

bool BMI270Sensor::upload_config_(const uint8_t *data, size_t len) {
  // Tell the sensor the starting byte address within the blob (always 0)
  const uint8_t addr_bytes[2] = {0x00, 0x00};
  if (!this->write_reg_(BMI2_INIT_ADDR_0, addr_bytes, 2)) {
    return false;
  }
  return this->write_reg_(BMI2_INIT_DATA_ADDR, data, len);
}

// ---------------------------------------------------------------------------
// Raw IMU read (uses INT_STATUS_1 to know which sensors are ready)
// ---------------------------------------------------------------------------

BMI270Sensor::ImuSpec BMI270Sensor::read_raw_imu_(ImuRawData *data) {
  ImuSpec result = IMU_SPEC_NONE;

  uint8_t int_status = 0;
  if (!this->read_reg_(BMI2_INT_STATUS_1_ADDR, &int_status)) {
    return result;
  }

  // Bit 7 = accel data ready, bit 6 = gyro data ready
  const bool accel_ready = (int_status & 0x80) != 0;
  const bool gyro_ready  = (int_status & 0x40) != 0;

  if (!accel_ready && !gyro_ready) {
    return result;
  }

  // Read acc (12 bytes = 3×int16 acc + 3×int16 gyro) starting at ACC_X_LSB
  int16_t buf[6] = {0};
  if (!this->read_reg_(BMI2_ACC_X_LSB_ADDR, reinterpret_cast<uint8_t *>(buf), 12)) {
    return result;
  }

  if (accel_ready) {
    data->accel.x = buf[0];
    data->accel.y = buf[1];
    data->accel.z = buf[2];
    result = static_cast<ImuSpec>(result | IMU_SPEC_ACCEL);
  }
  if (gyro_ready) {
    data->gyro.x = buf[3];
    data->gyro.y = buf[4];
    data->gyro.z = buf[5];
    result = static_cast<ImuSpec>(result | IMU_SPEC_GYRO);
  }

  return result;
}

// ---------------------------------------------------------------------------
// Converted IMU read
// ---------------------------------------------------------------------------

void BMI270Sensor::read_imu_data_(ImuData *data) {
  ImuSpec spec = read_raw_imu_(&raw_data_);

  if (spec & IMU_SPEC_ACCEL) {
    data->accel.x = raw_data_.accel.x * accel_res_;
    data->accel.y = raw_data_.accel.y * accel_res_;
    data->accel.z = raw_data_.accel.z * accel_res_;
  }
  if (spec & IMU_SPEC_GYRO) {
    data->gyro.x = raw_data_.gyro.x * gyro_res_;
    data->gyro.y = raw_data_.gyro.y * gyro_res_;
    data->gyro.z = raw_data_.gyro.z * gyro_res_;
  }
}

// ---------------------------------------------------------------------------
// Temperature read
//   Raw value: 0x0000 = 23°C, resolution 1/512°C per LSB (signed int16)
// ---------------------------------------------------------------------------

bool BMI270Sensor::read_temperature_(float *t) {
  int16_t raw = 0;
  if (!this->read_reg_(BMI2_TEMPERATURE_0_ADDR, reinterpret_cast<uint8_t *>(&raw), 2)) {
    return false;
  }
  *t = static_cast<float>(raw) * (1.0f / 512.0f) + 23.0f;
  ESP_LOGVV(TAG, "raw_temp=0x%04X → %.2f°C", static_cast<uint16_t>(raw), *t);
  return true;
}

// ---------------------------------------------------------------------------
// update() – called by the PollingComponent scheduler
// ---------------------------------------------------------------------------

void BMI270Sensor::update() {
  if (!setup_complete_) {
    return;
  }

  float temperature = NAN;
  if (!read_temperature_(&temperature)) {
    ESP_LOGW(TAG, "Temperature read failed");
  }

  ImuData imu{};
  read_imu_data_(&imu);

  ESP_LOGD(TAG,
           "accel={x=%.3f g, y=%.3f g, z=%.3f g} "
           "gyro={x=%.3f °/s, y=%.3f °/s, z=%.3f °/s} "
           "temp=%.1f°C",
           imu.accel.x, imu.accel.y, imu.accel.z,
           imu.gyro.x, imu.gyro.y, imu.gyro.z,
           temperature);

  if (accel_x_sensor_ != nullptr) accel_x_sensor_->publish_state(imu.accel.x);
  if (accel_y_sensor_ != nullptr) accel_y_sensor_->publish_state(imu.accel.y);
  if (accel_z_sensor_ != nullptr) accel_z_sensor_->publish_state(imu.accel.z);
  if (gyro_x_sensor_  != nullptr) gyro_x_sensor_->publish_state(imu.gyro.x);
  if (gyro_y_sensor_  != nullptr) gyro_y_sensor_->publish_state(imu.gyro.y);
  if (gyro_z_sensor_  != nullptr) gyro_z_sensor_->publish_state(imu.gyro.z);
  if (temperature_sensor_ != nullptr && !std::isnan(temperature))
    temperature_sensor_->publish_state(temperature);

  this->status_clear_warning();
}

// ---------------------------------------------------------------------------
// dump_config() – printed once at boot
// ---------------------------------------------------------------------------

void BMI270Sensor::dump_config() {
  ESP_LOGCONFIG(TAG, "BMI270:");
  LOG_I2C_DEVICE(this);
  if (this->is_failed()) {
    ESP_LOGE(TAG, "  Communication with BMI270 failed!");
    return;
  }
  ESP_LOGCONFIG(TAG, "  Accel range : %s", accel_range_str(accel_range_));
  ESP_LOGCONFIG(TAG, "  Gyro range  : %s", gyro_range_str(gyro_range_));
  LOG_UPDATE_INTERVAL(this);
  LOG_SENSOR("  ", "Acceleration X", accel_x_sensor_);
  LOG_SENSOR("  ", "Acceleration Y", accel_y_sensor_);
  LOG_SENSOR("  ", "Acceleration Z", accel_z_sensor_);
  LOG_SENSOR("  ", "Gyro X",         gyro_x_sensor_);
  LOG_SENSOR("  ", "Gyro Y",         gyro_y_sensor_);
  LOG_SENSOR("  ", "Gyro Z",         gyro_z_sensor_);
  LOG_SENSOR("  ", "Temperature",    temperature_sensor_);
}

float BMI270Sensor::get_setup_priority() const { return setup_priority::DATA; }

// ---------------------------------------------------------------------------
// I2C helpers
// ---------------------------------------------------------------------------

bool BMI270Sensor::write_reg_(uint8_t reg, const uint8_t *value, size_t len) {
  if (this->is_failed()) return false;
  last_error_ = this->write_register(reg, value, len);
  if (last_error_ != i2c::ERROR_OK) {
    this->status_set_warning();
    ESP_LOGE(TAG, "write_reg(0x%02X) failed: err=%d", reg, static_cast<int>(last_error_));
    return false;
  }
  return true;
}

bool BMI270Sensor::read_reg_(uint8_t reg, uint8_t *value, size_t len) {
  if (this->is_failed()) return false;
  last_error_ = this->read_register(reg, value, len);
  if (last_error_ != i2c::ERROR_OK) {
    this->status_set_warning();
    ESP_LOGE(TAG, "read_reg(0x%02X) failed: err=%d", reg, static_cast<int>(last_error_));
    return false;
  }
  return true;
}

}  // namespace bmi270
}  // namespace esphome
