#pragma once
#include <cstdint>

/**
 * @brief Abstract timer for platform-independent drivers.
 */
class TimerInterface {
public:
    virtual ~TimerInterface() = default;

    /**
     * @brief Sleep for the specified number of milliseconds.
     */
    virtual void Sleep(uint32_t ms) = 0;
};
