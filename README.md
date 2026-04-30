# ESPHome Component

An ESPHome external component for the 
  **Bosch BMI270** 6-axis IMU (3-axis accelerometer + 3-axis gyroscope with integrated temperature sensor).
  **STMicroelectronics LIS3DH / LIS3DHTR** — ultra-low-power 3-axis MEMS accelerometer with an optional on-chip temperature sensor.

## Features

- Accelerometer X / Y / Z (unit: g)
- Gyroscope X / Y / Z (unit: °/s)
- On-chip temperature sensor (unit: °C)
- Configurable accelerometer range and output data rate (ODR)
- Configurable gyroscope range and output data rate (ODR)
- Asynchronous non-blocking setup (does not block the main loop)
- I²C interface (default address `0x68`, secondary `0x69`)

## Minimal configuration

Go to the component folder to see detailed features.

```yaml
external_components:
  - source: github://scorpix/ESPHome_components/components/...

i2c:
  sda: GPIO21
  scl: GPIO22

sensor:
  - platform: ...
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
## Work based on following repo
DennisGaida and 7semi-solutions