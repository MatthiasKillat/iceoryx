#pragma once

#include <stdint.h>

#include <iostream>
#include <memory>

// simple storage class that allocates on the stack or heap
// can only store one object, whose type is determined at runtime
// can/will be fine tuned later (just proof of concept, we need the same storage size for all different types
// but cannot use dynamic memory in the final stage)
template <uint64_t Capacity>
class storage
{
  public:
    template <typename T>
    T* allocate()
    {
        return reinterpret_cast<T*>(allocate(alignof(T), sizeof(T)));
    }

    void* allocate(uint64_t align, uint64_t size)
    {
        if (m_ptr)
        {
            std::cout << "storage in use" << std::endl;
            return nullptr; // cannot allocate, already in use
        }

        uint64_t space = Capacity;
        m_ptr = m_bytes;
        if (std::align(align, size, m_ptr, space))
        {
            // fits, ptr was potentially modified to reflect alignent
            std::cout << "static storage is large enough" << std::endl;
            return m_ptr;
        }

        // does not fit, allocate on heap (could also return nullptr)

        std::cout << "static storage is NOT large enough - dynamic storage used" << std::endl;

        space = size + align;
        m_ptr = malloc(space);
        if (!m_ptr)
        {
            std::cout << "dynamic allocation failed" << std::endl;
            return nullptr;
        }

        m_delete = true;
        auto ptr = m_ptr;
        std::align(align, size, ptr, space);
        return ptr;
    }

    void deallocate()
    {
        if (m_delete)
        {
            free(m_ptr);
            m_delete = false;
        }
        m_ptr = nullptr;
    }

    ~storage()
    {
        deallocate();
    }

  private:
    uint8_t m_bytes[Capacity];
    void* m_ptr{nullptr};
    bool m_delete{false};
};