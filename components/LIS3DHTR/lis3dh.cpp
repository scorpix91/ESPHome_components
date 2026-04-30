#include "lis3dh.h"
#include "lis3dh_def.h"

#include "esphome/core/hal.h"
#include "esphome/core/log.h"

#include <cmath>

namespace esphome {
namespace lis3dh {

static const char *const TAG = "lis3dh";

// ---------------------------------------------------------------------------
// Sensitivity table (datasheet Table 4, high-resolution 12-bit mode)
//
//   HR mode  (12-bit, shift >> 4): 1 LSB = sensitivity_mg * 1e-3 g
//   Normal   (10-bit, shift >> 6): 1 LSB = sensitivity_mg * 4 * 1e-3 g
//   Low-power (8-bit, shift >> 8): 1 LSB = sensitivity_mg * 16 * 1e-3 g
//
// Rather than shifting raw values we compute a unified scale factor assuming
// always reading the full 16-bit register (left-aligned):
//   accel_res = full_scale_g / (32768 / bits_used) / 32768 * full_16bit_value
// Simplified: accel_res = full_scale_g * 2 / 65536  (in g per raw LSB)
//
// The raw int16 value is always read as 16-bit; the unused LSBs are zero.
// Dividing by 32768 normalises to ±full_scale_g correctly in all modes.
// ---------------------------------------------------------------------------

static float compute_accel_res(AccelRange range, OperatingMode mode) {
  // Full-scale peak value in g
  float fs_g;
  switch (range) {
    case AccelRange::RANGE_2G:  fs_g = 2.0f;  break;
    case AccelRange::RANGE_4G:  fs_g = 4.0f;  break;
    case AccelRange::RANGE_8G:  fs_g = 8.0f;  break;
    case AccelRange::RANGE_16G: fs_g = 16.0f; break;
    default:                    fs_g = 2.0f;  break;
  }

  // The raw register is always 16-bit left-aligned.
  // Dividing by 32768 gives ±fs_g — independent of actual bit depth.
  (void) mode;  // resolution affects noise floor but not the scale factor
  return fs_g / 32768.0f;
}

// Human-readable helpers for dump_config()
static const char *range_str(AccelRange r) {
  switch (r) {
    case AccelRange::RANGE_2G:  return "±2g";
    case AccelRange::RANGE_4G:  return "±4g";
    case AccelRange::RANGE_8G:  return "±8g";
    case AccelRange::RANGE_16G: return "±16g";
    default: return "unknown";
  }
}

static const char *odr_str(AccelODR o) {
  switch (o) {
    case AccelODR::ODR_1HZ:    return "1 Hz";
    case AccelODR::ODR_10HZ:   return "10 Hz";
    case AccelODR::ODR_25HZ:   return "25 Hz";
    case AccelODR::ODR_50HZ:   return "50 Hz";
    case AccelODR::ODR_100HZ:  return "100 Hz";
    case AccelODR::ODR_200HZ:  return "200 Hz";
    case AccelODR::ODR_400HZ:  return "400 Hz";
    case AccelODR::ODR_1344HZ: return "1344 Hz";
    default: return "unknown";
  }
}

static const char *mode_str(OperatingMode m) {
  switch (m) {
    case OperatingMode::NORMAL:    return "normal (10-bit)";
    case OperatingMode::HIGH_RES:  return "high-resolution (12-bit)";
    case OperatingMode::LOW_POWER: return "low-power (8-bit)";
    default: return "unknown";
  }
}

// ---------------------------------------------------------------------------
// setup()
// The LIS3DHTR needs no firmware blob — configuration is immediate.
// ---------------------------------------------------------------------------

void LIS3DHSensor::setup() {
  ESP_LOGCONFIG(TAG, "Setting up LIS3DH...");

  // 1. Verify chip identity
  uint8_t who_am_i = 0;
  if (!this->read_reg_(LIS3DH_WHO_AM_I, &who_am_i) || who_am_i != LIS3DH_CHIP_ID) {
    ESP_LOGE(TAG, "Wrong WHO_AM_I: 0x%02X (expected 0x%02X)", who_am_i, LIS3DH_CHIP_ID);
    this->mark_failed();
    return;
  }
  ESP_LOGD(TAG, "WHO_AM_I OK (0x%02X)", who_am_i);

  // 2. Reboot memory content (CTRL_REG5 bit 7) — brings registers to defaults
  if (!this->write_reg_(LIS3DH_CTRL_REG5, 0x80)) {
    ESP_LOGE(TAG, "Reboot failed");
    this->mark_failed();
    return;
  }
  // Datasheet: reboot takes ~5 ms
  delay(5);

  // 3. Apply configuration
  if (!this->configure_()) {
    this->mark_failed();
    return;
  }

  setup_complete_ = true;
  ESP_LOGCONFIG(TAG, "LIS3DH setup complete.");
}

// ---------------------------------------------------------------------------
// configure_() — applies user settings to hardware registers
// ---------------------------------------------------------------------------

bool LIS3DHSensor::configure_() {
  // --- CTRL_REG1 (0x20) ---
  // [ODR3|ODR2|ODR1|ODR0|LPen|Zen|Yen|Xen]
  // Enable all three axes, set ODR, set low-power bit if needed
  uint8_t lpen = (operating_mode_ == OperatingMode::LOW_POWER) ? LIS3DH_CTRL_REG1_LPEN : 0x00;
  uint8_t ctrl_reg1 = (static_cast<uint8_t>(accel_odr_) << LIS3DH_CTRL_REG1_ODR_SHIFT)
                    | lpen
                    | LIS3DH_CTRL_REG1_ZEN
                    | LIS3DH_CTRL_REG1_YEN
                    | LIS3DH_CTRL_REG1_XEN;

  if (!this->write_reg_(LIS3DH_CTRL_REG1, ctrl_reg1)) {
    ESP_LOGE(TAG, "Failed to write CTRL_REG1");
    return false;
  }
  ESP_LOGV(TAG, "CTRL_REG1 = 0x%02X", ctrl_reg1);

  // --- CTRL_REG4 (0x23) ---
  // [BDU|BLE|FS1|FS0|HR|ST1|ST0|SIM]
  //  BDU=1  block data update (recommended: prevents reading mixed old/new bytes)
  //  HR=1   high-resolution mode (12-bit), 0 for normal/low-power
  uint8_t hr = (operating_mode_ == OperatingMode::HIGH_RES) ? LIS3DH_CTRL_REG4_HR : 0x00;
  uint8_t ctrl_reg4 = LIS3DH_CTRL_REG4_BDU
                    | (static_cast<uint8_t>(accel_range_) << LIS3DH_CTRL_REG4_FS_SHIFT)
                    | hr;

  if (!this->write_reg_(LIS3DH_CTRL_REG4, ctrl_reg4)) {
    ESP_LOGE(TAG, "Failed to write CTRL_REG4");
    return false;
  }
  ESP_LOGV(TAG, "CTRL_REG4 = 0x%02X", ctrl_reg4);

  // --- TEMP_CFG_REG (0x1F) --- (only if temperature sensor is requested)
  // ADC_EN and TEMP_EN must both be set to route ADC3 to the temperature sensor
  if (temperature_sensor_ != nullptr) {
    uint8_t temp_cfg = LIS3DH_TEMP_CFG_ADC_EN | LIS3DH_TEMP_CFG_TEMP_EN;
    if (!this->write_reg_(LIS3DH_TEMP_CFG_REG, temp_cfg)) {
      ESP_LOGE(TAG, "Failed to write TEMP_CFG_REG");
      return false;
    }
    ESP_LOGV(TAG, "Temperature sensor enabled");
  }

  // Pre-compute conversion factor
  accel_res_ = compute_accel_res(accel_range_, operating_mode_);
  ESP_LOGD(TAG, "Accel: range=%s, ODR=%s, mode=%s, res=%.8f g/LSB",
           range_str(accel_range_), odr_str(accel_odr_), mode_str(operating_mode_), accel_res_);

  return true;
}

// ---------------------------------------------------------------------------
// read_accel_()
//
// Data registers OUT_X_L…OUT_Z_H (0x28–0x2D).
// Setting bit 7 of the start address enables the I2C auto-increment so all
// six bytes are read in a single transaction (required by the datasheet).
//
// The sensor outputs left-aligned 16-bit signed values:
//   HR mode    → 12 significant bits, 4 zero LSBs  (shift >> 4 gives 12-bit)
//   Normal     → 10 significant bits, 6 zero LSBs
//   Low-power  →  8 significant bits, 8 zero LSBs
//
// We do NOT shift — we divide by 32768.0f instead, which gives the correct
// result in all three modes because the register is always left-aligned.
// ---------------------------------------------------------------------------

bool LIS3DHSensor::read_accel_(float *x, float *y, float *z) {
  // Poll STATUS_REG: bit 3 (ZYXDA) = new X/Y/Z data available
  uint8_t status = 0;
  if (!this->read_reg_(LIS3DH_STATUS_REG, &status)) {
    return false;
  }
  if (!(status & LIS3DH_STATUS_ZYXDA)) {
    ESP_LOGVV(TAG, "No new accel data (STATUS=0x%02X)", status);
    return false;
  }

  // Read 6 bytes starting at OUT_X_L with auto-increment (bit 7 set)
  uint8_t buf[6] = {0};
  if (!this->read_regs_(LIS3DH_OUT_X_L | LIS3DH_ADDR_AUTO_INC, buf, 6)) {
    return false;
  }

  auto raw_x = static_cast<int16_t>((buf[1] << 8) | buf[0]);
  auto raw_y = static_cast<int16_t>((buf[3] << 8) | buf[2]);
  auto raw_z = static_cast<int16_t>((buf[5] << 8) | buf[4]);

  *x = static_cast<float>(raw_x) * accel_res_;
  *y = static_cast<float>(raw_y) * accel_res_;
  *z = static_cast<float>(raw_z) * accel_res_;

  ESP_LOGVV(TAG, "raw x=%d y=%d z=%d → %.4fg %.4fg %.4fg",
            raw_x, raw_y, raw_z, *x, *y, *z);
  return true;
}

// ---------------------------------------------------------------------------
// read_temperature_()
//
// The LIS3DH has an internal temperature sensor routed to ADC channel 3.
// Output is a 16-bit left-aligned signed value stored in OUT_ADC3_L/H.
// The value is relative: 0 LSB = 25°C (factory calibrated at 2.5V).
//
// Sensitivity:
//   HR / Normal mode (LPen=0) → 10-bit ADC, 1 LSB = sensitivity in mV
//   The temperature output is in 8-bit in low-power mode (ADC truncated).
//
// Conversion (from ST app note AN3308):
//   temp_°C = 25 + (raw_16bit >> 6) / 4   [normal/HR mode, 10-bit]
//   Simplified: raw >> 8 gives degrees relative to 25°C (1 LSB ≈ 1°C)
//
// We use right-shift 8 which works in all modes as an approximation.
// ---------------------------------------------------------------------------

bool LIS3DHSensor::read_temperature_(float *t) {
  uint8_t buf[2] = {0};
  if (!this->read_regs_(LIS3DH_OUT_ADC3_L | LIS3DH_ADDR_AUTO_INC, buf, 2)) {
    return false;
  }

  // 16-bit signed, left-aligned
  int16_t raw = static_cast<int16_t>((buf[1] << 8) | buf[0]);
  // Right-shift 8 → ~1°C per count, offset by 25°C
  *t = 25.0f + static_cast<float>(raw >> 8);

  ESP_LOGVV(TAG, "raw_temp=0x%04X → %.1f°C", static_cast<uint16_t>(raw), *t);
  return true;
}

// ---------------------------------------------------------------------------
// update() — called by the PollingComponent scheduler
// ---------------------------------------------------------------------------

void LIS3DHSensor::update() {
  if (!setup_complete_) {
    return;
  }

  ESP_LOGV(TAG, "Updating LIS3DH...");

  float x = NAN, y = NAN, z = NAN;
  if (read_accel_(&x, &y, &z)) {
    ESP_LOGD(TAG, "accel={x=%.4f g, y=%.4f g, z=%.4f g}", x, y, z);

    if (accel_x_sensor_ != nullptr) accel_x_sensor_->publish_state(x);
    if (accel_y_sensor_ != nullptr) accel_y_sensor_->publish_state(y);
    if (accel_z_sensor_ != nullptr) accel_z_sensor_->publish_state(z);
  } else {
    ESP_LOGW(TAG, "No new accelerometer data available");
  }

  if (temperature_sensor_ != nullptr) {
    float temp = NAN;
    if (read_temperature_(&temp)) {
      ESP_LOGD(TAG, "temp=%.1f°C", temp);
      temperature_sensor_->publish_state(temp);
    } else {
      ESP_LOGW(TAG, "Temperature read failed");
    }
  }

  this->status_clear_warning();
}

// ---------------------------------------------------------------------------
// dump_config()
// ---------------------------------------------------------------------------

void LIS3DHSensor::dump_config() {
  ESP_LOGCONFIG(TAG, "LIS3DH:");
  LOG_I2C_DEVICE(this);
  if (this->is_failed()) {
    ESP_LOGE(TAG, "  Communication with LIS3DH failed!");
    return;
  }
  ESP_LOGCONFIG(TAG, "  Range         : %s", range_str(accel_range_));
  ESP_LOGCONFIG(TAG, "  ODR           : %s", odr_str(accel_odr_));
  ESP_LOGCONFIG(TAG, "  Operating mode: %s", mode_str(operating_mode_));
  LOG_UPDATE_INTERVAL(this);
  LOG_SENSOR("  ", "Acceleration X", accel_x_sensor_);
  LOG_SENSOR("  ", "Acceleration Y", accel_y_sensor_);
  LOG_SENSOR("  ", "Acceleration Z", accel_z_sensor_);
  LOG_SENSOR("  ", "Temperature",    temperature_sensor_);
}

float LIS3DHSensor::get_setup_priority() const { return setup_priority::DATA; }

// ---------------------------------------------------------------------------
// I2C helpers
// ---------------------------------------------------------------------------

bool LIS3DHSensor::write_reg_(uint8_t reg, uint8_t value) {
  if (this->is_failed()) return false;
  last_error_ = this->write_register(reg, &value, 1);
  if (last_error_ != i2c::ERROR_OK) {
    this->status_set_warning();
    ESP_LOGE(TAG, "write_reg(0x%02X, 0x%02X) failed: err=%d", reg, value, static_cast<int>(last_error_));
    return false;
  }
  return true;
}

bool LIS3DHSensor::read_reg_(uint8_t reg, uint8_t *value) {
  if (this->is_failed()) return false;
  last_error_ = this->read_register(reg, value, 1);
  if (last_error_ != i2c::ERROR_OK) {
    this->status_set_warning();
    ESP_LOGE(TAG, "read_reg(0x%02X) failed: err=%d", reg, static_cast<int>(last_error_));
    return false;
  }
  return true;
}

bool LIS3DHSensor::read_regs_(uint8_t reg, uint8_t *data, size_t len) {
  if (this->is_failed()) return false;
  last_error_ = this->read_register(reg, data, len);
  if (last_error_ != i2c::ERROR_OK) {
    this->status_set_warning();
    ESP_LOGE(TAG, "read_regs(0x%02X, len=%u) failed: err=%d", reg, len, static_cast<int>(last_error_));
    return false;
  }
  return true;
}

}  // namespace lis3dh
}  // namespace esphome
