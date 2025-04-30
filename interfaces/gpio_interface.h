#pragma once

/**
 * @brief Abstract interface for GPIO pin control.
 */
class GpioInterface {
public:
    virtual ~GpioInterface() = default;

    /**
     * @brief Drive the GPIO pin low.
     */
    virtual void SetLow() = 0;

    /**
     * @brief Drive the GPIO pin high.
     */
    virtual void SetHigh() = 0;

    /**
     * @brief Set the GPIO pin to a specified state.
     * @param high If true, sets pin high; otherwise sets it low.
     */
    virtual void SetState(bool high) = 0;

    /**
     * @brief Toggle the current state of the GPIO pin.
     */
    virtual void Toggle() = 0;

    /**
     * @brief Read the current state of the GPIO pin.
     * @return True if high, false if low.
     */
    virtual bool Read() const = 0;
};
