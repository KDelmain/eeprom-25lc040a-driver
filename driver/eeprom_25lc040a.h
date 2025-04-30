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

    /** @brief Array-level block-protection options (STATUS BP1:BP0). */
    enum class BlockProtect : uint8_t {
        None    = 0,  // 00 – no protection
        Quarter = 1,  // 01 – upper-quarter protected
        Half    = 2,  // 10 – upper-half protected
        All     = 3   // 11 – full-array protected
    };

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

/*** Control ***/

    /**
     * @brief Set block protection level (BP1:BP0 bits) to restrict write access.
     */
    bool SetBlockProtect(BlockProtect level);

    /**
     * @brief Disable hardware write protection by setting WP# high.
     */
    void DisableHardwareWriteProtect();

    /**
     * @brief Enable hardware write protection by setting WP# low.
     */
    void EnableHardwareWriteProtect();

    /**
     * @brief Pause EEPROM activity using HOLD# (active low).
     */
    void Pause();

    /**
     * @brief Resume EEPROM activity by deasserting HOLD#.
     */
    void Resume();

private:
    SpiInterface& spi_interface_;
    GpioInterface& chip_select_pin_;
    GpioInterface& hold_pin_;
    GpioInterface& write_protect_pin_;
    TimerInterface& timer_;

    void Select();
    void Deselect();

    bool WriteEnable();
    bool WriteDisable();

    bool WaitForWriteComplete();

    bool IsSamePage(uint16_t address, uint8_t length);
    bool IsValidBuffer(const void* ptr, uint16_t length) const;
    bool IsWithinMemoryBounds(uint16_t address, uint16_t length) const;
    bool IsValidPageWrite(uint16_t address, uint8_t length) const;
};
