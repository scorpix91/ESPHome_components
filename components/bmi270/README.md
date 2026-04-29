# BMI270 ESPHome Component

An ESPHome external component for the **Bosch BMI270** 6-axis IMU (3-axis accelerometer + 3-axis gyroscope with integrated temperature sensor).

## Features

- Accelerometer X / Y / Z (unit: g)
- Gyroscope X / Y / Z (unit: °/s)
- On-chip temperature sensor (unit: °C)
- Configurable accelerometer range and output data rate (ODR)
- Configurable gyroscope range and output data rate (ODR)
- Asynchronous non-blocking setup (does not block the main loop)
- I²C interface (default address `0x68`, secondary `0x69`)

---

## Minimal configuration

```yaml
external_components:
  - source: github://scorpix/ESPHome_components/components/bmi270

i2c:
  sda: GPIO21
  scl: GPIO22

sensor:
  - platform: bmi270
    acceleration_x:
      name: "Accel X"
    acceleration_y:
      name: "Accel Y"
    acceleration_z:
      name: "Accel Z"
    gyroscope_x:
      name: "Gyro X"
    gyroscope_y:
      name: "Gyro Y"
    gyroscope_z:
      name: "Gyro Z"
    temperature:
      name: "IMU Temperature"
```

---

## Full configuration reference

```yaml
sensor:
  - platform: bmi270
    # I²C address: 0x68 (default, SDO → GND) or 0x69 (SDO → VDD)
    address: 0x68

    # How often to publish sensor values (PollingComponent interval)
    update_interval: 60s

    # Accelerometer full-scale range
    # Options: 2g (default) | 4g | 8g | 16g
    accel_range: 2g

    # Accelerometer output data rate (hardware sampling frequency)
    # Options: 0.78hz | 1.56hz | 3.12hz | 6.25hz | 12.5hz | 25hz | 50hz
    #          100hz (default) | 200hz | 400hz | 800hz | 1600hz
    accel_odr: 100hz

    # Gyroscope full-scale range
    # Options: 125dps | 250dps | 500dps | 1000dps | 2000dps (default)
    gyro_range: 2000dps

    # Gyroscope output data rate (hardware sampling frequency)
    # Options: 25hz | 50hz | 100hz (default) | 200hz | 400hz | 800hz
    #          1600hz | 3200hz
    gyro_odr: 100hz

    # Sensor channels (all optional – omit any you don't need)
    acceleration_x:
      name: "Accel X"
    acceleration_y:
      name: "Accel Y"
    acceleration_z:
      name: "Accel Z"
    gyroscope_x:
      name: "Gyro X"
    gyroscope_y:
      name: "Gyro Y"
    gyroscope_z:
      name: "Gyro Z"
    temperature:
      name: "IMU Temperature"
```

---

## Configuration options

### `accel_range`

Controls the full-scale measurement range of the accelerometer.
A smaller range gives higher resolution; a larger range avoids saturation for high-g applications.

| Value  | Range  | Resolution (LSB/g) |
|--------|--------|--------------------|
| `2g`   | ±2 g   | 16384              |
| `4g`   | ±4 g   | 8192               |
| `8g`   | ±8 g   | 4096               |
| `16g`  | ±16 g  | 2048               |

**Default:** `2g`

### `accel_odr`

Hardware output data rate of the accelerometer. This affects how fresh the data is when `update_interval` fires.
Setting the ODR lower than your `update_interval` is fine; setting it much higher than needed wastes power.

| Value     | Rate      |
|-----------|-----------|
| `0.78hz`  | 0.78 Hz   |
| `1.56hz`  | 1.56 Hz   |
| `3.12hz`  | 3.12 Hz   |
| `6.25hz`  | 6.25 Hz   |
| `12.5hz`  | 12.5 Hz   |
| `25hz`    | 25 Hz     |
| `50hz`    | 50 Hz     |
| `100hz`   | 100 Hz    |
| `200hz`   | 200 Hz    |
| `400hz`   | 400 Hz    |
| `800hz`   | 800 Hz    |
| `1600hz`  | 1600 Hz   |

**Default:** `100hz`

### `gyro_range`

Controls the full-scale measurement range of the gyroscope.

| Value      | Range       | Resolution (LSB/°/s) |
|------------|-------------|----------------------|
| `125dps`   | ±125 °/s    | 262.1                |
| `250dps`   | ±250 °/s    | 131.1                |
| `500dps`   | ±500 °/s    | 65.5                 |
| `1000dps`  | ±1000 °/s   | 32.8                 |
| `2000dps`  | ±2000 °/s   | 16.4                 |

**Default:** `2000dps`

### `gyro_odr`

Hardware output data rate of the gyroscope.

| Value     | Rate      |
|-----------|-----------|
| `25hz`    | 25 Hz     |
| `50hz`    | 50 Hz     |
| `100hz`   | 100 Hz    |
| `200hz`   | 200 Hz    |
| `400hz`   | 400 Hz    |
| `800hz`   | 800 Hz    |
| `1600hz`  | 1600 Hz   |
| `3200hz`  | 3200 Hz   |

**Default:** `100hz`

---

## Notes

- `update_interval` (inherited from `PollingComponent`) controls **how often ESPHome reads and publishes** values. It is independent from `accel_odr` / `gyro_odr`, which control the **hardware sampling rate** of the sensor.
- Only enabled channels consume memory; omit any sensor key you don't need.
- The component uses a non-blocking state machine during `setup()` to comply with the ESPHome cooperative scheduler (no `delay()` calls).

---

## Hardware wiring

| BMI270 pin | ESP32 pin (example) |
|------------|---------------------|
| VDD        | 3.3 V               |
| GND        | GND                 |
| SDA        | GPIO21              |
| SCL        | GPIO22              |
| SDO        | GND (address 0x68)  |

---

## License

MIT
