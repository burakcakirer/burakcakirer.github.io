# Source Consolidation Audit

| Original archive | Portfolio decision |
|---|---|
| `STM32_FAN_DRIVER(1).zip` | Published as `stm32g4-pwm-fan-monitor` |
| `STM32_Deneme.zip` | Selected as cleaner SPI iteration → `stm32g4-icm20601-spi-telemetry` |
| `SPI_DENEME.zip` | Earlier/redundant SPI iteration; not published separately |
| `STM32_ICM20601.zip` | Published as `stm32g4-icm20601-i2c-monitor` |
| `STM32_SPI.zip` | Byte-identical application sources to `STM32_ICM20601`; duplicate not published |
| `STM32_OLED.zip` | Published as supporting `stm32g4-sh1107-display-driver` |

## Public-source sanitation performed
- Removed build-output directories and launch files.
- Renamed CubeIDE project identifiers to portfolio-safe names.
- Replaced the company-branded fan OLED title with `PWM FAN MONITOR`.
- Removed obvious commented duplicate IMU header/source blocks from the I²C project.
- Added a missing `SH1107.h` include to the standalone OLED demo.
- Preserved STM32 vendor license files in `Drivers/`.
