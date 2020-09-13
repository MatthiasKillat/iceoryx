#pragma once

#include <cstring>
#include <stdint.h>

// compatility to const char* (todo: char* and char[n] as well?)
// internal helper to satisfy concept interface
class const_char_array
{
  public:
    const_char_array(const char* str)
        : m_str(str)
        , m_capacity(strlen(m_str))
    {
    }

    const char* c_str() const noexcept
    {
        return m_str;
    }

    uint64_t size() const noexcept
    {
        return m_capacity; // cannot assign, so size cannot change
    }

    uint64_t capacity() const noexcept
    {
        return m_capacity;
    }

    void assign(const char*)
    {
        // do nothing, we cannot assign here
        std::cerr << "cannot assign to const_char_array" << std::endl;
    }

  private:
    const char* m_str; // must be 0 terminated
    uint64_t m_capacity;
};