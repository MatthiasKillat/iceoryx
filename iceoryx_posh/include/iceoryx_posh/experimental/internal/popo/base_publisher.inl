// Copyright (c) 2020 by Robert Bosch GmbH. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef IOX_EXPERIMENTAL_POSH_POPO_BASE_PUBLISHER_INL
#define IOX_EXPERIMENTAL_POSH_POPO_BASE_PUBLISHER_INL

#include "iceoryx_posh/experimental/popo/base_publisher.hpp"

#include "iceoryx_posh/runtime/posh_runtime.hpp"

#include <iostream>

namespace iox
{
namespace popo
{

template<typename T>
using SamplePtr = iox::cxx::unique_ptr<T>;

using uid_t = uint64_t;

// ======================================== Generic ======================================== //

template<typename T, typename port_t>
BasePublisher<T, port_t>::BasePublisher(const capro::ServiceDescription& service)
    : m_port(iox::runtime::PoshRuntime::getInstance().getMiddlewareSender(service, ""))
{}

template<typename T, typename port_t>
inline uid_t
BasePublisher<T, port_t>::uid() const noexcept
{
    std::cout << "uid()" << std::endl;
    return 0u;
}

template<typename T, typename port_t>
inline cxx::expected<Sample<T>, AllocationError>
BasePublisher<T, port_t>::loan(uint64_t size) noexcept
{
    auto header = m_port.reserveChunk(size, m_useDynamicPayloadSize);
    if (header == nullptr)
    {
        // Old API does not provide error handling, so return unknown error.
        return cxx::error<AllocationError>(AllocationError::UNKNOWN);
    }
    return cxx::success<Sample<T>>(
                cxx::unique_ptr<T>(
                    header->payload(),
                    [this](T* const p){
                        auto header = iox::mepoo::convertPayloadPointerToChunkHeader(reinterpret_cast<void*>(p));
                        this->m_port.freeChunk(header);
                    }
                ),
                *this
            );
}

template<typename T, typename port_t>
inline void
BasePublisher<T, port_t>::release(Sample<T>& sample) noexcept
{
    auto header = iox::mepoo::convertPayloadPointerToChunkHeader(sample.allocation());
    m_port.freeChunk(header);
}

template<typename T, typename port_t>
inline cxx::expected<AllocationError>
BasePublisher<T, port_t>::publish(Sample<T>& sample) noexcept
{
    /// @todo - ensure sample points to valid shared memory location
    auto header = iox::mepoo::convertPayloadPointerToChunkHeader(reinterpret_cast<void* const>(sample.allocation()));
    m_port.deliverChunk(header);
    return iox::cxx::success<>();
}

template<typename T, typename port_t>
inline cxx::expected<SampleRecallError>
BasePublisher<T, port_t>::previousSample() const noexcept
{
    assert(false && "Not yet supported");
    return iox::cxx::error<SampleRecallError>(SampleRecallError::NO_PREVIOUS_CHUNK);
}

template<typename T, typename port_t>
inline void
BasePublisher<T, port_t>::offer() noexcept
{
    m_port.activate();
}

template<typename T, typename port_t>
inline void
BasePublisher<T, port_t>::stopOffer() noexcept
{
    m_port.deactivate();
}

template<typename T, typename port_t>
inline bool
BasePublisher<T, port_t>::isOffered() noexcept
{
    assert(false && "Not yet supported");
}

template<typename T, typename port_t>
inline bool
BasePublisher<T, port_t>::hasSubscribers() noexcept
{
    return m_port.hasSubscribers();
}

// ======================================== Untyped Specialization ======================================== //

template<typename port_t>
BasePublisher<Untyped, port_t>::BasePublisher(const capro::ServiceDescription& service)
    : m_port(iox::runtime::PoshRuntime::getInstance().getMiddlewareSender(service, ""))
{}

template<typename port_t>
inline cxx::expected<Sample<void>, AllocationError>
BasePublisher<Untyped, port_t>::loan(uint64_t size) noexcept
{
//    auto header = m_port.reserveChunk(size, m_useDynamicPayloadSize);
//    if (header == nullptr)
//    {
//        // Old API does not provide error handling, so return unknown error.
//        return cxx::error<AllocationError>(AllocationError::UNKNOWN);
//    }
//    return cxx::success<Sample<void>>(
//                cxx::unique_ptr<void>(
//                    header->payload(),
//                    [this](void* const p){
//                        auto header = iox::mepoo::convertPayloadPointerToChunkHeader(p);
//                        this->m_port.freeChunk(header);
//                    }
//                ),
//                *this
//            );
}

template<typename port_t>
inline void
BasePublisher<Untyped, port_t>::offer() noexcept
{
    m_port.activate();
}

template<typename port_t>
inline void
BasePublisher<Untyped, port_t>::stopOffer() noexcept
{
    m_port.deactivate();
}

template<typename port_t>
inline bool
BasePublisher<Untyped, port_t>::isOffered() noexcept
{
    assert(false && "Not yet supported");
}

template<typename port_t>
inline bool
BasePublisher<Untyped, port_t>::hasSubscribers() noexcept
{
    return m_port.hasSubscribers();
}

} // namespace popo
} // namespace iox

#endif // IOX_EXPERIMENTAL_POSH_POPO_BASE_PUBLISHER_INL