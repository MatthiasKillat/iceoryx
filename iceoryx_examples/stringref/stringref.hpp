#pragma once
#include <memory>
#include <stdint.h>

#include "char_array.hpp"

#include "iceoryx_utils/cxx/string.hpp"

// todo: no heap, no reliance on shared pointer or similar


// this is a pass around read only type, if manipulation is desired this needs
// to be reconsidered and extended (e.g. assign, operator[])
// prototype assignment functionality is shown
class stringref
{
  public:
    template <typename T>
    stringref(const T& native)
        : m_impl(std::make_shared<Realization<T>>(native))
    {
    }

    stringref(const char* str)
    {
        // construction can be optimized but we will not use the heap later anyway

        // we create our own helper object to reference but have to delete it when done
        auto native = new const_char_array(str);
        auto ptr = new Realization<const_char_array>(*native);
        auto deleter = [=](Realization<const_char_array>* ptr) {
            delete ptr;
            delete native;
        };
        m_impl = std::shared_ptr<Realization<const_char_array>>(ptr, deleter);
    }

    const char* c_str() const noexcept
    {
        // could be stored once to avoid indirection (optimization)
        return m_impl->c_str();
    }
    uint64_t size() const noexcept
    {
        return m_impl->size();
    }
    uint64_t capacity() const noexcept
    {
        return m_impl->capacity();
    }

    // todo: add operator[] to cxx string?
    // no bounds check, but read only
    const char& operator[](uint64_t index) const
    {
        return c_str()[index];
    }

    // allows modification ... can be debated, problem for const_char_array(segfault)
    // could be solved with implementation of operator that does nothing (or error message)
    // but extends the concept...
    char& operator[](uint64_t index)
    {
        return const_cast<char*>(c_str())[index];
    }

    // operator= and modification in general needs some thought wrt (reasonable) guarantees
    stringref& operator=(const stringref& rhs)
    {
        if (&rhs != this)
        {
            m_impl->assign(rhs.c_str());
        }
        return *this;
    }

    // copy content to a cxx::string
    // note that the capacity cannot be deduced at compile time and must be specified)
    // (this information is lost/erased)
    template <uint64_t Capacity>
    iox::cxx::string<Capacity> to_string() const
    {
        return iox::cxx::string<Capacity>(iox::cxx::TruncateToCapacity, c_str());
    }

    // maybe too dangerous since it easily allows to generate std::strings by "accident" (with e.g. auto)
    std::string to_string() const
    {
        return std::string(c_str());
    }

  private:
    // needed for general compatibility with e.g. std::string and
    // other interfaces implementing this concept
    class Concept
    {
      public:
        virtual ~Concept(){};

        // read only
        virtual const char* c_str() const = 0;
        virtual uint64_t size() const = 0;
        virtual uint64_t capacity() const = 0;

        // write/modify
        virtual void assign(const char*) = 0;
    };

    template <typename T>
    class Realization : public Concept
    {
      public:
        Realization(const T& native)
            : m_native(const_cast<T*>(&native)) // todo: eliminate this cast
        {
        }

        const char* c_str() const
        {
            return m_native->c_str();
        }
        uint64_t size() const
        {
            return m_native->size();
        }
        uint64_t capacity() const
        {
            return m_native->capacity();
        }

        void assign(const char* str)
        {
            m_native->assign(str);
        }

      private:
        T* m_native; // we do not store a copy and hence rely on external object lifetime
    };

    std::shared_ptr<Concept> m_impl;
};

// could also redirect to internal operators but this would
// require the concept to implement the operators
std::ostream& operator<<(std::ostream& stream, const stringref& str)
{
    stream << str.c_str();
    return stream;
}

bool operator==(const stringref& lhs, const stringref& rhs)
{
    auto n = std::max(lhs.size(), rhs.size()); // a little safety against missing 0 termination in the larger string
    return strncmp(lhs.c_str(), rhs.c_str(), n) == 0;
}

bool operator!=(const stringref& lhs, const stringref& rhs)
{
    return !(lhs == rhs);
}