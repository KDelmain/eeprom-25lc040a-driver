#pragma once
#include <cstdint>

/**
 * @brief Constants specific to the 25LC040A EEPROM device
 */
namespace eeprom_25lc040a {

    /**
     * @brief Instruction opcodes for EEPROM SPI commands
     */
    namespace instruction {
        constexpr uint8_t READ_MEMORY            = 0x03;
        constexpr uint8_t WRITE_MEMORY           = 0x02;
        constexpr uint8_t WRITE_DISABLE          = 0x04;
        constexpr uint8_t WRITE_ENABLE           = 0x06;
        constexpr uint8_t READ_STATUS_REGISTER   = 0x05;
        constexpr uint8_t WRITE_STATUS_REGISTER  = 0x01;
    }

    /**
     * @brief Bit masks for the STATUS register
     */
    namespace status {
        constexpr uint8_t WRITE_IN_PROGRESS      = 0x01;  // Set when a write is in progress
        constexpr uint8_t WRITE_ENABLE_LATCH     = 0x02;  // Indicates if writes are enabled
        constexpr uint8_t BLOCK_PROTECT_BIT_0    = 0x04;  // Block protect LSB (BP0)
        constexpr uint8_t BLOCK_PROTECT_BIT_1    = 0x08;  // Block protect MSB (BP1)
    }

}  // namespace eeprom_25lc040a
