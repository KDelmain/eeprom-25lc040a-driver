#pragma once
#include <cstdint>
#include "interfaces/gpio_interface.h"
#include "interfaces/spi_interface.h"
#include "interfaces/timer_interface.h"
#include "driver/eeprom_25lc040a_constants.h"

/**
 * @brief Driver for the Microchip 25LC040A SPI EEPROM.
 */
class Eeprom25LC040A {
public:
    static constexpr uint16_t MEMORY_SIZE_BYTES = 512;
    static constexpr uint8_t PAGE_SIZE_BYTES = 16;

    Eeprom25LC040A(SpiInterface& spi,
                    GpioInterface& cs,
                    GpioInterface& hold,
                    GpioInterface& wp,
                    TimerInterface& timer);

    /**
     * @brief Sets GPIO pins to idle state.
     */
    void Initialize();

/*** Read Operations ***/

    /**
     * @brief Read a single byte from memory.
     */
    bool ReadByte(uint16_t address, uint8_t& data);

    /**
     * @brief Read up to 16 bytes from a single memory page.
     * @note The range must not cross a page boundary.
     */
    bool ReadPage(uint16_t address, uint8_t* buffer, uint8_t length);

    /**
     * @brief Read any number of bytes, crossing page boundaries if needed.
     */
    bool ReadSequential(uint16_t address, uint8_t* buffer, uint16_t length);

/*** Write Operations ***/

    /**
     * @brief Write a single byte to memory.
     */
    bool WriteByte(uint16_t address, uint8_t data);

    /**
     * @brief Write up to 16 bytes to a single memory page.
     * @note The range must not cross a page boundary.
     */
    bool WritePage(uint16_t address, const uint8_t* data, uint8_t length);

    /**
     * @brief Write any number of bytes, splitting across pages as needed.
     */
    bool WriteSequential(uint16_t address, const uint8_t* data, uint16_t length);

/*** Status ***/

    /**
     * @brief Read the STATUS register.
     */
    bool ReadStatus(uint8_t& status);

    /**
     * @brief Write the STATUS register.
     */
    bool WriteStatus(uint8_t status);

private:
    SpiInterface& spi_interface_;
    GpioInterface& chip_select_pin_;
    GpioInterface& hold_pin_;
    GpioInterface& write_protect_pin_;
    TimerInterface& timer_;

    void Select();
    void Deselect();

    bool WriteEnable();
    bool WaitForWriteComplete();

    bool IsSamePage(uint16_t address, uint8_t length);
};
