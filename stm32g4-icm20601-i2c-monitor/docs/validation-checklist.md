# Pre-Publication Validation Checklist

- [ ] Clean build from a fresh STM32CubeIDE workspace.
- [ ] No `Debug/`, `.elf`, `.o`, `.map` or local launch files committed.
- [ ] Search repository for company names, emails, internal product codes and confidential labels.
- [ ] Verify README pin map against the actual hardware wiring.
- [ ] Confirm WHO_AM_I = 0xAC.
- [ ] Capture the 14-byte I²C burst.
- [ ] Compare raw vs moving-average output.
- [ ] Add authorized `docs/media/hero.jpg`.
- [ ] Add measurement evidence.
- [ ] Confirm publication/IP permission before changing repository visibility to Public.
