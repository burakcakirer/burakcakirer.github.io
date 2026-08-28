# GitHub Portfolio Release Plan

## Recommended public portfolio structure

### Primary / pinned repositories
1. **stm32g4-pwm-fan-monitor** — strongest timer/interrupt/peripheral-integration story
2. **stm32g4-icm20601-spi-telemetry** — strongest register-level communication story
3. **stm32g4-icm20601-i2c-monitor** — strongest acquisition/filtering architecture story

### Supporting repository
4. **stm32g4-sh1107-display-driver** — useful foundation, but lower priority than the three system-level projects

The duplicate `STM32_SPI` archive was intentionally not turned into another repository because its firmware is identical to the I²C `STM32_ICM20601` archive. The earlier `SPI_DENEME` iteration was also not published separately; the cleaner `STM32_Deneme` SPI iteration was selected instead.

## Why this is more professional
- No build binaries or `Debug/` folders.
- No “deneme/test/final” naming.
- One engineering story per repository.
- Architecture diagram + technical report + known limitations.
- Explicit validation checklist instead of unsupported performance claims.
- Company-specific UI branding removed from public source.
- IP/publication caution documented rather than hidden.

## GitHub publishing order
1. Confirm public-release permission for internship-authored code.
2. Run the validation checklist inside each repository.
3. Add one clean hardware photo and one measurement artifact per primary project.
4. Create repositories using `REPOSITORY_SETTINGS.md`.
5. Push the prepared project folders.
6. Create/update your GitHub profile README.
7. Deploy the included portfolio site through GitHub Pages.
8. Pin only the three primary embedded repositories initially.

## What not to upload
Do not publish internal company documents, source material you did not author, employee/customer data, serial numbers, product roadmaps, unreleased hardware, internal network details, screenshots containing company tools/data, or files whose ownership is uncertain.

## Suggested CV/LinkedIn phrasing
“Developed STM32G4-based embedded prototypes involving SPI/I²C sensor interfaces, timer-based PWM generation, input-capture RPM measurement, interrupt handling, digital filtering, and OLED telemetry; documented the work as reproducible engineering portfolio projects.”

This wording demonstrates the work without claiming that the prototypes were official company products.
