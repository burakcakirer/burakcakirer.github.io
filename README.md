# Burak Bekir Çakırer — Engineering Portfolio

[![Live Site](https://img.shields.io/badge/Live_Site-burakcakirer.github.io-0A66C2?style=for-the-badge&logo=googlechrome&logoColor=white)](https://burakcakirer.github.io/)
[![Digital Card](https://img.shields.io/badge/Digital_Card-Open-3a3a3a?style=for-the-badge&logo=contactlesspayment&logoColor=white)](https://burakcakirer.github.io/card/)
[![CV](https://img.shields.io/badge/CV-PDF-B31B1B?style=for-the-badge&logo=adobeacrobatreader&logoColor=white)](https://burakcakirer.github.io/Burak_Bekir_Cakirer_CV.pdf)
[![LinkedIn](https://img.shields.io/badge/LinkedIn-Connect-0A66C2?style=for-the-badge&logo=linkedin&logoColor=white)](https://www.linkedin.com/in/burak-bekir-%C3%A7akirer-58178b1a3/)

![HTML5](https://img.shields.io/badge/HTML5-E34F26?logo=html5&logoColor=white)
![CSS3](https://img.shields.io/badge/CSS3-1572B6?logo=css3&logoColor=white)
![JavaScript](https://img.shields.io/badge/JavaScript-F7DF1E?logo=javascript&logoColor=black)
![GitHub Pages](https://img.shields.io/badge/Hosted_on-GitHub_Pages-222222?logo=githubpages&logoColor=white)

Source code of my personal engineering portfolio: a single-page, dependency-free static site that presents
my hardware and embedded-systems work with photos, measurements and links to the project repositories.

> **Hardware first. Evidence driven.** Projects are presented with the prototypes, oscilloscope captures and
> PCB renders behind them, not just simulations.

---

## What's on the site

| Section | Content |
|---|---|
| **Projects** | STM32 firmware (PWM fan monitor, ICM-20601 SPI/I²C telemetry, SH1107 OLED driver), PCB designs (STM32F405 flight controller, I²C sensor shield, Core45 IoT & battery-management PCB, TEKNOFEST power distribution board), analog & digital electronics, Göktaş Autonomous Forklift AGV (senior design project), transformer FEM optimization, galvo laser scanner, Karbon Pusula (climate tech) |
| **Honors & Awards** | element14 Project14 / em-smart PCBC 1st Place, COP31 Climate Technologies Creathon 3rd Place, TEKNOFEST Çelikkubbe (GÖKTAŞ Team) |
| **Experience** | Hardware Design Engineering Intern — Vestel Elektronik (EV Charger team) · Embedded Systems Engineering Intern — MTM Biotech |
| **Stack** | Embedded, interfaces, hardware, PCB & signal integrity, engineering tools |
| **Contact** | Email, LinkedIn, GitHub, downloadable CV |
| **`/card`** | Mobile digital business card with QR code and vCard download |

## Repository structure

```text
burakcakirer.github.io/
├── index.html                    # Single-page portfolio
├── styles.css                    # Layout, theming and responsive styles
├── script.js                     # Project category filters and UI interactions
├── assets/images/                # Project photos, PCB renders, scope captures
├── card/                         # Digital business card (index.html, QR, .vcf, CV)
└── Burak_Bekir_Cakirer_CV.pdf    # Downloadable CV
```

## Running locally

No build step or dependencies are needed.

```bash
git clone https://github.com/burakcakirer/burakcakirer.github.io.git
cd burakcakirer.github.io
python -m http.server 8000   # then open http://localhost:8000
```

Opening `index.html` directly in a browser also works.

## Deployment

The site is served by **GitHub Pages** from the `main` branch. Every push to `main` is published automatically
to <https://burakcakirer.github.io/>.

## Featured repositories

- [stm32f405-flight-controller-pcb](https://github.com/burakcakirer/stm32f405-flight-controller-pcb)
- [adxl345-ltc2990-i2c-shield](https://github.com/burakcakirer/adxl345-ltc2990-i2c-shield)
- [stm32g4-pwm-fan-monitor](https://github.com/burakcakirer/stm32g4-pwm-fan-monitor)
- [stm32g4-icm20601-spi-telemetry](https://github.com/burakcakirer/stm32g4-icm20601-spi-telemetry)
- [stm32g4-icm20601-i2c-monitor](https://github.com/burakcakirer/stm32g4-icm20601-i2c-monitor)
- [stm32g4-sh1107-display-driver](https://github.com/burakcakirer/stm32g4-sh1107-display-driver)
- [virtual-ecu-simulator](https://github.com/burakcakirer/virtual-ecu-simulator)

---

© Burak Bekir Çakırer. Content, photos and design files on this site are my own work unless stated otherwise.
