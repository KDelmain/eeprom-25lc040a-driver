#include "driver/eeprom_25lc040a.h"

#include <array>
#include <cstdint>
#include <algorithm>

using namespace eeprom_25lc040a;

namespace {
    constexpr uint16_t PAGE_ALIGNMENT_MASK = Eeprom25LC040A::PAGE_SIZE_BYTES - 1;
}

// Constructor

Eeprom25LC040A::Eeprom25LC040A(SpiInterface& spi,
                               GpioInterface& cs,
                               GpioInterface& hold,
                               GpioInterface& wp,
                               TimerInterface& timer)
    : spi_interface_{spi},
      chip_select_pin_{cs},
      hold_pin_{hold},
      write_protect_pin_{wp},
      timer_{timer}
{}

// Public methods

void Eeprom25LC040A::Initialize()
{
    chip_select_pin_.SetHigh();
    hold_pin_.SetHigh();
    write_protect_pin_.SetHigh();
}

bool Eeprom25LC040A::ReadByte(uint16_t address, uint8_t& data)
{
    return ReadSequential(address, &data, 1);
}

bool Eeprom25LC040A::ReadPage(uint16_t address, uint8_t* buffer, uint8_t length)
{
    if (!IsValidBuffer(buffer, length) || length > PAGE_SIZE_BYTES)
        return false;

    if (!IsSamePage(address, length) || !IsWithinMemoryBounds(address, length))
        return false;

    return ReadSequential(address, buffer, length);
}

bool Eeprom25LC040A::ReadSequential(uint16_t address, uint8_t* buffer, uint16_t length)
{
    if (!IsValidBuffer(buffer, length))
        return false;

    if (!IsWithinMemoryBounds(address, length))
        return false;

    const uint8_t a8 = static_cast<uint8_t>((address & internal::ADDRESS_A8_MASK) >> internal::ADDRESS_A8_BIT_POS);
    const uint8_t opcode = static_cast<uint8_t>(instruction::READ_MEMORY | (a8 << internal::OPCODE_A8_BIT_POS));
    const uint8_t addr_low = static_cast<uint8_t>(address & 0xFF);

    std::array<uint8_t, 2> tx_header = {opcode, addr_low};

    Select();
    if (!spi_interface_.Transmit(tx_header.data(), tx_header.size()))
    {
        Deselect();
        return false;
    }

    bool ok = spi_interface_.Receive(buffer, length);
    Deselect();
    return ok;
}

bool Eeprom25LC040A::WriteByte(uint16_t address, uint8_t data)
{
    return WriteSequential(address, &data, 1);
}

bool Eeprom25LC040A::WritePage(uint16_t address, const uint8_t* data, uint8_t length)
{
    if (!IsValidBuffer(data, length) || length > PAGE_SIZE_BYTES)
        return false;

    if (!IsValidPageWrite(address, length))
        return false;

    if (!WriteEnable())
        return false;

    const uint8_t a8 = static_cast<uint8_t>((address & internal::ADDRESS_A8_MASK) >> internal::ADDRESS_A8_BIT_POS);
    const uint8_t opcode = static_cast<uint8_t>(instruction::WRITE_MEMORY | (a8 << internal::OPCODE_A8_BIT_POS));
    const uint8_t addr_low = static_cast<uint8_t>(address & 0xFF);

    std::array<uint8_t, PAGE_SIZE_BYTES + 2> tx = {};
    tx[0] = opcode;
    tx[1] = addr_low;
    std::copy_n(data, length, tx.begin() + 2);

    Select();
    bool ok = spi_interface_.Transmit(tx.data(), static_cast<uint16_t>(length + 2));
    Deselect();

    return ok && WaitForWriteComplete();
}

bool Eeprom25LC040A::WriteSequential(uint16_t address, const uint8_t* data, uint16_t length)
{
    if (!IsValidBuffer(data, length))
        return false;

    if (!IsWithinMemoryBounds(address, length))
        return false;

    uint16_t remaining = length;
    uint16_t current_address = address;
    const uint8_t* data_ptr = data;

    while (remaining > 0)
    {
        uint8_t space_in_page = static_cast<uint8_t>(PAGE_SIZE_BYTES - (current_address % PAGE_SIZE_BYTES));
        uint8_t chunk_size = (remaining < space_in_page) ? static_cast<uint8_t>(remaining) : space_in_page;

        if (!WritePage(current_address, data_ptr, chunk_size))
            return false;

        current_address += chunk_size;
        data_ptr        += chunk_size;
        remaining       -= chunk_size;
    }

    return true;
}

bool Eeprom25LC040A::ReadStatus(uint8_t& status)
{
    const uint8_t cmd = instruction::READ_STATUS_REGISTER;
    status = 0;

    Select();
    bool ok = spi_interface_.Transmit(&cmd, 1);
    if (ok)
        ok = spi_interface_.Receive(&status, 1);

    Deselect();
    return ok;
}

bool Eeprom25LC040A::WriteStatus(uint8_t status)
{
    if (!WriteEnable())
        return false;

    uint8_t tx[2] = {instruction::WRITE_STATUS_REGISTER, status};

    Select();
    bool ok = spi_interface_.Transmit(tx, 2);
    Deselect();
    return ok && WaitForWriteComplete();
}

// Private helpers

void Eeprom25LC040A::Select()
{
    chip_select_pin_.SetLow();
}

void Eeprom25LC040A::Deselect()
{
    chip_select_pin_.SetHigh();
}

bool Eeprom25LC040A::WriteEnable()
{
    const uint8_t cmd = instruction::WRITE_ENABLE;

    Select();
    bool ok = spi_interface_.Transmit(&cmd, 1);
    Deselect();

    if (!ok)
        return false;

    uint8_t status = 0;
    return ReadStatus(status) && (status & status::WRITE_ENABLE_LATCH);
}

bool Eeprom25LC040A::WaitForWriteComplete()
{
    uint8_t status = 0;
    while (true)
    {
        if (!ReadStatus(status))
            return false;

        if ((status & status::WRITE_IN_PROGRESS) == 0)
            break;

        timer_.Sleep(1);
    }
    return true;
}

bool Eeprom25LC040A::IsSamePage(uint16_t address, uint8_t length)
{
    return (address & ~PAGE_ALIGNMENT_MASK) == ((address + length - 1) & ~PAGE_ALIGNMENT_MASK);
}

bool Eeprom25LC040A::IsValidBuffer(const void* ptr, uint16_t length) const
{
    return ptr != nullptr && length > 0;
}

bool Eeprom25LC040A::IsWithinMemoryBounds(uint16_t address, uint16_t length) const
{
    return (address + length) <= MEMORY_SIZE_BYTES;
}

bool Eeprom25LC040A::IsValidPageWrite(uint16_t address, uint8_t length) const
{
    return IsSamePage(address, length) && IsWithinMemoryBounds(address, length);
}
