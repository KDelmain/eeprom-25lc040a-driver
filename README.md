# EEPROM 25LC040A Driver

A C++17 driver for the **Microchip 25LC040A**, a 4Kbit SPI EEPROM memory chip.  
This driver is portable, testable, and designed with hardware abstraction in mind — making it easy to reuse across embedded platforms or host-based simulations.

[Microchip 25LC040A Product Page](https://www.microchip.com/en-us/product/25LC040A)

---

## Features

- Interface-based architecture (abstract `SpiInterface` and `GpioInterface`)
- Fully testable with mock implementations
- Minimal, modern CMake build setup
- Includes GitHub Actions CI pipeline
- Suitable for embedded firmware or host-side simulation/testing

---

## Supported Device

This driver targets the following EEPROM device:

- **Microchip 25LC040A** (4Kbit SPI EEPROM)  
- [Datasheet PDF](docs/25AA040A-25LC040A4-Kbit-SPI-Bus-Serial-EEPROM-20001827J.pdf)


---

## Requirements

- CMake ≥ 3.10
- C++17-compatible compiler:
  - GCC 7.1+
  - Clang 5+
  - MSVC 2017 (v19.10)+

---

## Building & Running Tests

*TBD*

## Additional Notes

**Disclaimer:** This driver is currently untested on actual hardware. 

It was developed for learning and demonstration purposes, and verified using mock interfaces only.  

If you have hardware available, contributions to test and validate are welcome.
**