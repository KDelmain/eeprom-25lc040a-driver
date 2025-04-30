#pragma once
#include <cstdint>

/**
 * @brief Abstract interface for SPI communication.
 */
class SpiInterface {
public:
    virtual ~SpiInterface() = default;

    /**
     * @brief Transmit data over SPI.
     * @param data Pointer to data buffer to transmit.
     * @param size Number of bytes to transmit.
     * @return True on success, false on failure.
     */
    virtual bool Transmit(const uint8_t* data, uint16_t size) = 0;

    /**
     * @brief Receive data over SPI.
     * @param data Pointer to buffer to store received data.
     * @param size Number of bytes to receive.
     * @return True on success, false on failure.
     */
    virtual bool Receive(uint8_t* data, uint16_t size) = 0;

    /**
     * @brief Simultaneously transmit and receive data over SPI.
     * @param tx_data Pointer to data buffer to transmit.
     * @param rx_data Pointer to buffer to store received data.
     * @param size Number of bytes to transmit and receive.
     * @return True on success, false on failure.
     */
    virtual bool TransmitReceive(const uint8_t* tx_data, uint8_t* rx_data, uint16_t size) = 0;
};
