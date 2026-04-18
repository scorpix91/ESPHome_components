#include "esphome/core/hal.h"
#include "esphome/core/log.h"

#include "bmi270.h"
#include "bmi270_config.h"

namespace esphome {
namespace bmi270 {

static const char *const TAG = "bmi270";

// addresses - see https://github.com/m5stack/M5Unified/blob/master/src/utility/imu/BMI270_Class.hpp
static constexpr const uint8_t CHIP_ID                 = 0x00;
static constexpr const uint8_t CMD_REG_ADDR            = 0x7E;
static constexpr const uint8_t PWR_CONF_ADDR           = 0x7C;
static constexpr const uint8_t INIT_CTRL_ADDR          = 0x59;
static constexpr const uint8_t INT_MAP_DATA_ADDR       = 0x58;
static constexpr const uint8_t INIT_ADDR_0             = 0x5B;
static constexpr const uint8_t INIT_DATA_ADDR          = 0x5E;
static constexpr const uint8_t INTERNAL_STATUS_ADDR    = 0x21;
static constexpr const uint8_t IF_CONF_ADDR            = 0x6B;
static constexpr const uint8_t PWR_CTRL_ADDR           = 0x7D;
static constexpr const uint8_t ACC_X_LSB_ADDR          = 0x0C; // start add ACC
static constexpr const uint8_t ACC_RANGE               = 0x41; // set acc range 2, 4, 8, 16g
static constexpr const uint8_t ACC_CONF                = 0x40; // set acc config
static constexpr const uint8_t STATUS_ADDR             = 0x03;
static constexpr const uint8_t INT_STATUS_1_ADDR       = 0x1D;
static constexpr const uint8_t TEMPERATURE_0_ADDR      = 0x22;

// commands
static constexpr const uint8_t SOFT_RESET_CMD          = 0xB6;

void BMI270Sensor::setup() {
  this->internal_setup_(0);
}

void BMI270Sensor::internal_setup_(int stage, int retry) {
  // see https://github.com/m5stack/M5Unified/blob/5d359529b05d2f92d9e91bcf09dbd47b722538d5/src/utility/imu/BMI270_Class.cpp#L35
  // and https://github.com/boschsensortec/BMI270_SensorAPI/blob/4f0b6990dfa24130052d1713147c6f35a5d3a1da/bmi270.c#L1354

  switch (stage) {
    case 0: {
      // check correct communication (chip_id needs to return 0x24)
      ESP_LOGCONFIG(TAG, "Setting up BMI270 ...");
      uint8_t chipid = 0;
      if (this->read_byte(CHIP_ID, &chipid) && chipid != 0x24) {
        ESP_LOGE(TAG, "Communication error. chipid=%02X", chipid);
        this->mark_failed();
      }

      // perform soft-reset to bring all register values to default
      ESP_LOGD(TAG, "Soft reset...");
      // don't check whether this succeeds, as you will get i2c::ERROR_NOT_ACKNOWLEDGED 
      this->write_register_(CMD_REG_ADDR, &SOFT_RESET_CMD);
      
      ESP_LOGV(TAG, "Waiting for successful soft reset ...");
      // wait 2ms according to API, retry 3 times
      // see https://github.com/boschsensortec/BMI270_SensorAPI/blob/4f0b6990dfa24130052d1713147c6f35a5d3a1da/bmi2.c#L2083C8-L2083C23
      this->set_timeout(2, [this]() { this->internal_setup_(1, 3); });

      break;
    }
    case 1: {
      // power configuration
      uint8_t power_conf;
      ESP_LOGVV(TAG, "Waiting ... (retry: %d)", retry);
      // check if PWR_CONF is not 0
      if (!this->read_byte(PWR_CONF_ADDR, &power_conf) || retry == 0) {
        ESP_LOGE(TAG, "Power config error. retry=%d", retry);
        this->mark_failed();
        return;
      } else if (power_conf == 0 && retry != 0) {
        this->set_timeout(0.25, [this, retry]() { this->internal_setup_(1, retry-1); });
        break;
      }

      ESP_LOGD(TAG, "Soft reset sucessful (power_conf=%02X). Disabling advanced power save mode ...", power_conf); //power_conf default is 0x03
      uint8_t power_save_disabled = 0x00;
      write_register_(PWR_CONF_ADDR, &power_save_disabled); // disable advanced power save mode
      // wait for minimum 450μs, waiting 500μs
      this->set_timeout(0.5, [this]() { this->internal_setup_(2); });

      break;
    }
    case 2: {
      // load initial configuration
      ESP_LOGV(TAG, "Prepare config load");
      uint8_t init_ctrl = 0x00;
      write_register_(INIT_CTRL_ADDR, &init_ctrl); // prepare config load
      bool upload_succesful = _upload_file(DEFAULT_CONFIGURATION, sizeof(DEFAULT_CONFIGURATION)); // burst write to reg INIT_DATA start with byte 0
      init_ctrl = 0x01;
      write_register_(INIT_CTRL_ADDR, &init_ctrl); // complete config load
      uint8_t int_map_data = 0xFF;
      write_register_(INT_MAP_DATA_ADDR, &int_map_data, 1);

      // return the IMU specification
      if (!upload_succesful)
      {
        specification_ = imu_spec_none;
        return;
      }

      //need to wait at least 20ms according to datasheet
      this->set_timeout(20, [this, retry]() { this->internal_setup_(3, 9); });

      break;
    }
    case 3: {
      // check the correct initialization status (internal_status needs to return 1)
      uint8_t internal_status = 0;

      // check if internal_status is 1
      if (!this->read_byte(INTERNAL_STATUS_ADDR, &internal_status) || retry == 0) {
        ESP_LOGE(TAG, "Internal status error. retry=%d", retry);
        this->mark_failed();
        return;
      } else if (internal_status != 1 && retry != 0) {
        ESP_LOGVV(TAG, "internal_status: %d, retry: %d", internal_status, retry);
        this->set_timeout(1, [this, retry]() { this->internal_setup_(3, retry-1); });
        break;
      }

      specification_ = (imu_spec_t)(imu_spec_accel | imu_spec_gyro);
      ESP_LOGD(TAG, "Config loaded successfully (internal_status: %d)", internal_status);

      uint8_t temp_en = 0x0E; // 1110
      write_register_(PWR_CTRL_ADDR, &temp_en); // Enable temp | ACC | GYR
      
      this->setup_complete_ = true;
      ESP_LOGCONFIG(TAG, "Setup complete without auxilliary sensor!");
      ESP_LOGD(TAG, "IMU Spec: %d", specification_);
      
      break;
    }
  }
}


bool BMI270Sensor::_upload_file(const uint8_t *config_data, size_t write_len)
{
  ESP_LOGV(TAG, "Uploading config (size=%d)...", write_len);
  size_t index = 0;
  uint8_t addr_array[2] = {
    (uint8_t)((index >> 1) & 0x0F),
    (uint8_t)(index >> 5)
  };

  if (config_data != nullptr
    && this->write_register_( INIT_ADDR_0, addr_array, 2 )
    && this->write_register_( INIT_DATA_ADDR, (uint8_t *)config_data, write_len))
  {
    return true;
  }
  return true;
}

void BMI270Sensor::checkStatus(int retry, StatusCallback callback) {
  uint8_t status = 0;

  if (!this->read_byte(STATUS_ADDR, &status) || retry == 0) {
    ESP_LOGW(TAG, "Status error. retry=%d", retry);
    this->status_set_warning();
    callback(false);
    return;
  } else if (status & 0b100 && retry != 0) { //checks whether the third bit of data is set
    ESP_LOGVV(TAG, "Current status: %02X", status);
    this->set_timeout(1, [this, retry, callback]() { this->checkStatus(retry-1, callback); });
    return;
  }
  callback(true);
}

BMI270Sensor::imu_spec_t BMI270Sensor::getImuRawData(imu_raw_data_t *data)
{
  imu_spec_t res = imu_spec_none;
  uint8_t intstat = 0;
  this->read_register(INT_STATUS_1_ADDR, &intstat, 1);
  ESP_LOGVV(TAG, "intstat: %02X", intstat);
  if (intstat & 0xC0) //E0
  {
    std::int16_t buf[6];
    auto buffer = this->read_register(ACC_X_LSB_ADDR, (std::uint8_t*)&buf, 12);
    ESP_LOGVV(TAG, "buf: %02X, buffer: %02X", buf, buffer);

    //TODO: Acceleration & Gyro are switched (!), though the following does it like this
    //   https://github.com/m5stack/M5Unified/blob/5d359529b05d2f92d9e91bcf09dbd47b722538d5/src/utility/imu/BMI270_Class.cpp#L130
    if (buffer == esphome::i2c::NO_ERROR)
    {
      if (intstat & 0x80u)
      {
        data->accel.x = buf[0];
        data->accel.y = buf[1];
        data->accel.z = buf[2];
        ESP_LOGVV(TAG, "accelX: %02X", buf[0]);
        res = (imu_spec_t)(res | imu_spec_accel);
      }
      if (intstat & 0x40u)
      {
        data->gyro.x = buf[3];
        data->gyro.y = buf[4];
        data->gyro.z = buf[5];
        ESP_LOGVV(TAG, "gyroX: %02X", buf[3]);
        res = (imu_spec_t)(res | imu_spec_gyro);
      }
    }
  }
  ESP_LOGVV(TAG, "imu returned spec: %02X", res);
  return res;
}


void BMI270Sensor::getImuData(imu_data_t *data) {
  imu_spec_t spec = getImuRawData(&raw_data_);

  // TODO:
  // - not sure about the conversions
  //   https://github.com/m5stack/M5Unified/blob/5d359529b05d2f92d9e91bcf09dbd47b722538d5/src/utility/IMU_Class.cpp#L446
  //   does multiply with (1.0f / 65536.0f), but the commented code does not:
  //   https://github.com/m5stack/M5Unified/blob/5d359529b05d2f92d9e91bcf09dbd47b722538d5/src/utility/imu/BMI270_Class.cpp#L172
  // - Missing the complete part about offsets and calibration
  if (spec != imu_spec_none) {
    if (spec & imu_spec_accel) {
      data->accel.x = raw_data_.accel.x * convert_param_.accel_res;
      data->accel.y = raw_data_.accel.y * convert_param_.accel_res;
      data->accel.z = raw_data_.accel.z * convert_param_.accel_res;
    }
    if (spec & imu_spec_gyro) {
      data->gyro.x = raw_data_.gyro.x * convert_param_.gyro_res;
      data->gyro.y = raw_data_.gyro.y * convert_param_.gyro_res;
      data->gyro.z = raw_data_.gyro.z * convert_param_.gyro_res;
    }
  }
}

bool BMI270Sensor::getTemp(float *t)
{
  std::int16_t temp;
  bool res = this->read_register(TEMPERATURE_0_ADDR, (std::uint8_t*)&temp, 2);
  ESP_LOGVV(TAG, "raw temp: %02X",temp);
  if (res == esphome::i2c::NO_ERROR) {
    *t = temp * convert_param_.temp_res + convert_param_.temp_offset;
    ESP_LOGVV(TAG, "t: %.3f°C", *t);
  }
  return res;
}

void BMI270Sensor::dump_config() {
  ESP_LOGCONFIG(TAG, "BMI270:");
  LOG_I2C_DEVICE(this);
  if (this->is_failed()) {
    ESP_LOGE(TAG, "Communication with BMI270 failed!");
  }
  LOG_UPDATE_INTERVAL(this);
  LOG_SENSOR("  ", "Acceleration X", this->accel_x_sensor_);
  LOG_SENSOR("  ", "Acceleration Y", this->accel_y_sensor_);
  LOG_SENSOR("  ", "Acceleration Z", this->accel_z_sensor_);
  LOG_SENSOR("  ", "Gyro X", this->gyro_x_sensor_);
  LOG_SENSOR("  ", "Gyro Y", this->gyro_y_sensor_);
  LOG_SENSOR("  ", "Gyro Z", this->gyro_z_sensor_);
  LOG_SENSOR("  ", "Temperature", this->temperature_sensor_);
}

bool BMI270Sensor::read_register_(uint8_t reg, uint8_t data) {
  if (this->is_failed()) {
    ESP_LOGD(TAG, "Device marked failed");
    return false;
  }

  // using defaults for length (1) and stop (true)
  this->last_error_ = this->read_register(reg, &data, 1);
  if (this->last_error_ != i2c::ERROR_OK) {
    this->status_set_warning();
    ESP_LOGE(TAG, "read_register_(): I2C I/O error: Reg=0x%02X, Data=0x%02X, Err=%d", reg, data, (int) this->last_error_);
    return false;
  }

  this->status_clear_warning();
  return true;
}

bool BMI270Sensor::write_register_(uint8_t reg, const uint8_t *value, size_t len) {
  if (this->is_failed()) {
    ESP_LOGD(TAG, "Device marked failed");
    return false;
  }

  // using defaults for length (1) and stop (true)
  this->last_error_ = this->write_register(reg, value, len);
  if (this->last_error_ != i2c::ERROR_OK) {
    this->status_set_warning("I2C I/O error");
    ESP_LOGE(TAG, "write_register_(): I2C I/O error: Reg=0x%02X, Err=%s", reg, this->last_error_);
    return false;
  }

  this->status_clear_warning();
  return true;
}

void BMI270Sensor::update() {
  if (!this->setup_complete_) {
    return;
  }

  ESP_LOGV(TAG, "    Updating BMI270...");

  float temperature;
  if (getTemp(&temperature)) {
    ESP_LOGVV(TAG, "temperature: %.1f°C", temperature);
  }
    
  BMI270Sensor::imu_data_t data;
  getImuData(&data);
  
  float gyro_x = (float) data.gyro.x;
  float gyro_y = (float) data.gyro.y;
  float gyro_z = (float) data.gyro.z;
  float accel_x = (float) data.accel.x;
  float accel_y = (float) data.accel.y;
  float accel_z = (float) data.accel.z;

  ESP_LOGD(TAG,
    "Got accel={x=%.3f m/s², y=%.3f m/s², z=%.3f m/s²}, "
    "gyro={x=%.3f °/s, y=%.3f °/s, z=%.3f °/s}, "
    "temp=%.1f°C",
    accel_x, accel_y, accel_z,
    gyro_x, gyro_y, gyro_z, 
    temperature);


  if (this->accel_x_sensor_ != nullptr)
    this->accel_x_sensor_->publish_state(accel_x);
  if (this->accel_y_sensor_ != nullptr)
    this->accel_y_sensor_->publish_state(accel_y);
  if (this->accel_z_sensor_ != nullptr)
    this->accel_z_sensor_->publish_state(accel_z);

  if (this->gyro_x_sensor_ != nullptr)
    this->gyro_x_sensor_->publish_state(gyro_x);
  if (this->gyro_y_sensor_ != nullptr)
    this->gyro_y_sensor_->publish_state(gyro_y);
  if (this->gyro_z_sensor_ != nullptr)
    this->gyro_z_sensor_->publish_state(gyro_z);

  if (this->temperature_sensor_ != nullptr)
    this->temperature_sensor_->publish_state(temperature);

  this->status_clear_warning();
}

float BMI270Sensor::get_setup_priority() const { return setup_priority::DATA; }

}  // namespace bmi270
}  // namespace esphome
