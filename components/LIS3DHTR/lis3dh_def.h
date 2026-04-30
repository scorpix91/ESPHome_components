#pragma once

#include <stdint.h>

// ---------------------------------------------------------------------------
// LIS3DH/LIS3DHTR register map (ST DocID17530 Rev 2)
// ---------------------------------------------------------------------------

// Read-only registers
static constexpr uint8_t LIS3DH_STATUS_REG_AUX  = 0x07;
static constexpr uint8_t LIS3DH_OUT_ADC1_L       = 0x08;
static constexpr uint8_t LIS3DH_OUT_ADC1_H       = 0x09;
static constexpr uint8_t LIS3DH_OUT_ADC2_L       = 0x0A;
static constexpr uint8_t LIS3DH_OUT_ADC2_H       = 0x0B;
static constexpr uint8_t LIS3DH_OUT_ADC3_L       = 0x0C;  // temperature LSB (when TEMP_EN=1)
static constexpr uint8_t LIS3DH_OUT_ADC3_H       = 0x0D;  // temperature MSB
static constexpr uint8_t LIS3DH_WHO_AM_I         = 0x0F;  // fixed value: 0x33

// Configuration registers
static constexpr uint8_t LIS3DH_CTRL_REG0        = 0x1E;
static constexpr uint8_t LIS3DH_TEMP_CFG_REG     = 0x1F;
static constexpr uint8_t LIS3DH_CTRL_REG1        = 0x20;
static constexpr uint8_t LIS3DH_CTRL_REG2        = 0x21;
static constexpr uint8_t LIS3DH_CTRL_REG3        = 0x22;
static constexpr uint8_t LIS3DH_CTRL_REG4        = 0x23;
static constexpr uint8_t LIS3DH_CTRL_REG5        = 0x24;
static constexpr uint8_t LIS3DH_CTRL_REG6        = 0x25;
static constexpr uint8_t LIS3DH_REFERENCE        = 0x26;
static constexpr uint8_t LIS3DH_STATUS_REG       = 0x27;

// Data output registers (X, Y, Z — each 16-bit little-endian)
static constexpr uint8_t LIS3DH_OUT_X_L          = 0x28;
static constexpr uint8_t LIS3DH_OUT_X_H          = 0x29;
static constexpr uint8_t LIS3DH_OUT_Y_L          = 0x2A;
static constexpr uint8_t LIS3DH_OUT_Y_H          = 0x2B;
static constexpr uint8_t LIS3DH_OUT_Z_L          = 0x2C;
static constexpr uint8_t LIS3DH_OUT_Z_H          = 0x2D;

// FIFO
static constexpr uint8_t LIS3DH_FIFO_CTRL_REG    = 0x2E;
static constexpr uint8_t LIS3DH_FIFO_SRC_REG     = 0x2F;

// Interrupt generators
static constexpr uint8_t LIS3DH_INT1_CFG         = 0x30;
static constexpr uint8_t LIS3DH_INT1_SRC         = 0x31;
static constexpr uint8_t LIS3DH_INT1_THS         = 0x32;
static constexpr uint8_t LIS3DH_INT1_DURATION    = 0x33;
static constexpr uint8_t LIS3DH_INT2_CFG         = 0x34;
static constexpr uint8_t LIS3DH_INT2_SRC         = 0x35;
static constexpr uint8_t LIS3DH_INT2_THS         = 0x36;
static constexpr uint8_t LIS3DH_INT2_DURATION    = 0x37;

// ---------------------------------------------------------------------------
// WHO_AM_I expected value
// ---------------------------------------------------------------------------
static constexpr uint8_t LIS3DH_CHIP_ID          = 0x33;

// ---------------------------------------------------------------------------
// CTRL_REG1 (0x20) bit definitions
//   [ODR3|ODR2|ODR1|ODR0|LPen|Zen|Yen|Xen]
// ---------------------------------------------------------------------------
static constexpr uint8_t LIS3DH_CTRL_REG1_XEN    = 0x01;
static constexpr uint8_t LIS3DH_CTRL_REG1_YEN    = 0x02;
static constexpr uint8_t LIS3DH_CTRL_REG1_ZEN    = 0x04;
static constexpr uint8_t LIS3DH_CTRL_REG1_LPEN   = 0x08;  // low-power mode
static constexpr uint8_t LIS3DH_CTRL_REG1_ODR_SHIFT = 4;

// ODR codes for CTRL_REG1 bits [7:4]
static constexpr uint8_t LIS3DH_ODR_POWER_DOWN   = 0x00;
static constexpr uint8_t LIS3DH_ODR_1HZ          = 0x01;
static constexpr uint8_t LIS3DH_ODR_10HZ         = 0x02;
static constexpr uint8_t LIS3DH_ODR_25HZ         = 0x03;
static constexpr uint8_t LIS3DH_ODR_50HZ         = 0x04;
static constexpr uint8_t LIS3DH_ODR_100HZ        = 0x05;
static constexpr uint8_t LIS3DH_ODR_200HZ        = 0x06;
static constexpr uint8_t LIS3DH_ODR_400HZ        = 0x07;
static constexpr uint8_t LIS3DH_ODR_1620HZ_LP    = 0x08;  // low-power mode only
static constexpr uint8_t LIS3DH_ODR_1344HZ_NM    = 0x09;  // normal/HR mode
static constexpr uint8_t LIS3DH_ODR_5376HZ_LP    = 0x09;  // low-power mode only

// ---------------------------------------------------------------------------
// CTRL_REG4 (0x23) bit definitions
//   [BDU|BLE|FS1|FS0|HR|ST1|ST0|SIM]
// ---------------------------------------------------------------------------
static constexpr uint8_t LIS3DH_CTRL_REG4_SIM    = 0x01;  // SPI 3-wire
static constexpr uint8_t LIS3DH_CTRL_REG4_HR     = 0x08;  // high-resolution (12-bit)
static constexpr uint8_t LIS3DH_CTRL_REG4_FS_SHIFT = 4;
static constexpr uint8_t LIS3DH_CTRL_REG4_BLE    = 0x40;  // big-endian
static constexpr uint8_t LIS3DH_CTRL_REG4_BDU    = 0x80;  // block data update

// Full-scale codes for CTRL_REG4 bits [5:4]
static constexpr uint8_t LIS3DH_FS_2G            = 0x00;
static constexpr uint8_t LIS3DH_FS_4G            = 0x01;
static constexpr uint8_t LIS3DH_FS_8G            = 0x02;
static constexpr uint8_t LIS3DH_FS_16G           = 0x03;

// ---------------------------------------------------------------------------
// TEMP_CFG_REG (0x1F) bit definitions
// ---------------------------------------------------------------------------
static constexpr uint8_t LIS3DH_TEMP_CFG_TEMP_EN = 0x40;  // temperature sensor enable
static constexpr uint8_t LIS3DH_TEMP_CFG_ADC_EN  = 0x80;  // ADC enable (required for temp)

// ---------------------------------------------------------------------------
// STATUS_REG (0x27) bit definitions
// ---------------------------------------------------------------------------
static constexpr uint8_t LIS3DH_STATUS_ZYXDA     = 0x08;  // X/Y/Z new data available

// ---------------------------------------------------------------------------
// I2C addresses (SDO/SA0 pin)
// ---------------------------------------------------------------------------
static constexpr uint8_t LIS3DH_I2C_ADDR_LOW     = 0x18;  // SDO → GND
static constexpr uint8_t LIS3DH_I2C_ADDR_HIGH    = 0x19;  // SDO → VDD

// ---------------------------------------------------------------------------
// Multi-byte read: set bit 7 of register address (auto-increment)
// ---------------------------------------------------------------------------
static constexpr uint8_t LIS3DH_ADDR_AUTO_INC    = 0x80;
