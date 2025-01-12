// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <atomic>
#include <cstdint>

namespace Rapid::Common
{

/**
 * Base class for all Copy On Write classes.
 */
class SharedData
{
public:
    SharedData() noexcept
        : ref{0} {};
    SharedData(SharedData const& ohter)
        : ref{0} {};

    SharedData(SharedData&&) noexcept = delete;
    SharedData& operator=(SharedData&&) noexcept = delete;
    SharedData& operator=(SharedData&) = delete;
    virtual ~SharedData() = default;
    std::atomic_uint8_t ref;
};

/**
 * The SharedDataPointer is used to point to a shared data object.
 * This implements the copy on write mechanism.
 * Classes derived by the SharedData and hold by the SharedDataPointer can be hold by may different objects.
 * As long as no object writes to the SharedDataObject.
 * A write operation detaches the SharedDataObject from the manipulation instance and creates a new SharedData object.
 * The copy is only created when the SharedData object is hold by at least two instances.
 *
 * @note Objects that shall be hold with a SharedDataPointer must be derived of SharedData.
 *
 * @tparam T The SharedDataPointer type must be derived from SharedData.
 */
template <class T>
class SharedDataPointer
{
public:
    /**
     * Default constructed SharedDataPointer with invalid object
     */
    SharedDataPointer() = default;

    /**
     * Constructs the SharedDataPointer with a shared object.
     * @param data The shared data object
     */
    SharedDataPointer(T* data)
        : mData{data}
    {
        if (mData != nullptr) {
            mData->ref++;
        }
    }

    /**
     * Destructor deletes the shared data if the reference count is 0.
     * it.
     */
    ~SharedDataPointer()
    {
        if (mData == nullptr) {
            return;
        }

        mData->ref--;
        if (mData->ref == 0) {
            delete mData;
        }
    }

    /**
     * Copy constructor
     * @param other The other shared data pointer.
     */
    SharedDataPointer(SharedDataPointer<T> const& other)
        : mData(other.mData)
    {
        if (mData != nullptr) {
            mData->ref++;
        }
    }

    /**
     * Copy assignment
     * @param other The other shared data pointer.
     * @return SharedDataPointer& The copied shared data pointer.
     */
    SharedDataPointer& operator=(SharedDataPointer<T> const& other)
    {
        if (mData == other.mData or &other == this) {
            return *this;
        }

        if (other.mData != nullptr) {
            other.mData->ref++;
        }

        T* oldData = mData;
        mData = other.mData;
        if (oldData != nullptr) {
            oldData->ref--;
            if (oldData->ref == 0) {
                delete oldData; // NOLINT(cppcoreguidelines-owning-memory)
            }
        }

        return *this;
    }

    /**
     * Move constructor for the SharedDataPointer
     * @param other The object to move from.
     */
    SharedDataPointer(SharedDataPointer&& other)
        : mData{std::move(other.mData)}
    {
        other.mData = nullptr;
    }

    /**
     * Move assignament operator for the SharedDataPointer
     * @param other  The object to move from.
     * @return SharedDataPointer& The moved intialized object.
     */
    SharedDataPointer<T>& operator=(SharedDataPointer<T>&& other)
    {
        SharedDataPointer moved(std::move(other));
        std::swap(moved.mData, mData);
        return *this;
    }

    /**
     * Gives the reference count of the shared data.
     * If not data is set then the reference count will be 0.
     * @return std::uint8_t The reference count of the shared data.
     */
    std::uint8_t getRefCount() const
    {
        if (mData == nullptr) {
            return 0;
        }

        return mData->ref;
    }

    /**
     * Provides constant access to the shared data object member without detach.
     * @return const T* A pointer to the shared object.
     */
    T const* operator->() const
    {
        return mData;
    }

    /**
     * Provides mutable access to the shared data object member. This will detach
     * the shared may triggers copy of the shared data object.
     * @return T* A mutable pointer to the shared data object.
     */
    T* operator->()
    {
        detach();
        return mData;
    }

    /**
     * Gives a mutable reference to the shared data.
     * @return T& The mutable reference of the shared data.
     */
    T& operator*()
    {
        detach();
        return *mData;
    }

    /**
     * Provides constant access to the shared data object as reference without detach.
     * @return const T& A constant reference to the shared data object.
     */
    T const& operator*() const
    {
        return *mData;
    }

    operator T*()
    {
        detach();
        return mData;
    }

    /**
     * Provides access to the shared data object without calling detach.
     * @return const T * The pointer to the shared object.
     */
    operator T const*() const
    {
        return mData;
    }

    bool operator==(SharedDataPointer<T> const& other) const
    {
        return mData == other.mData;
    }

    bool operator!=(SharedDataPointer<T> const& other) const
    {
        return mData != other.mData;
    }

private:
    /**
     * Creates a new shared data object, when the reference count > 1.
     * @note Don't call that function manually the function is autmatically called by the
     * overloaded operatores.
     */
    void detach()
    {
        if (mData == nullptr) {
            return;
        }

        if (mData->ref > 1) {
            T* newData = new T(*mData); // NOLINT(cppcoreguidelines-owning-memory)
            newData->ref++;
            mData->ref--;
            if (mData->ref == 0) {
                delete mData;
            }

            mData = newData;
        }
    }

private:
    T* mData{nullptr};
};

} // namespace Rapid::Common
