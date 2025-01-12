// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <common/SessionData.hpp>
#include <system/AsyncResult.hpp>
#include <system/FutureWatcher.hpp>
#include <thread>

namespace Rapid::Storage::Private
{

struct StorageContextBase
{
    StorageContextBase(std::shared_ptr<System::AsyncResult> result = std::make_shared<System::AsyncResult>())
        : mResult{std::move(result)}
    {
        mStorageResult.setFuture(mStoragePromise.get_future());
        std::ignore = mStorageResult.finished.connect([this] {
            done.emit(this);
        });
    }

    virtual ~StorageContextBase()
    {
        if (mStorageThread.joinable()) {
            mStorageThread.join();
        }
    }

    StorageContextBase(StorageContextBase const& other) = delete;
    StorageContextBase& operator=(StorageContextBase const& ohter) = delete;

    StorageContextBase(StorageContextBase&& other) noexcept = delete;
    StorageContextBase& operator=(StorageContextBase&& ohter) = delete;

    template <typename T>
    std::shared_ptr<T> getResultAs()
    {
        return std::dynamic_pointer_cast<T>(mResult);
    }

    template <typename T>
    static std::shared_ptr<T> getStorageAs(std::shared_ptr<StorageContextBase> const& ctx)
    {
        return std::dynamic_pointer_cast<T>(ctx);
    }

    std::thread mStorageThread{};
    std::promise<bool> mStoragePromise;
    System::FutureWatcher<bool> mStorageResult;
    std::shared_ptr<System::AsyncResult> mResult;

    KDBindings::Signal<StorageContextBase*> done;
};

template <typename T>
struct StorageContext : public StorageContextBase
{
    StorageContext() = default;

    StorageContext(std::shared_ptr<System::AsyncResult> result)
        : StorageContextBase{result}
    {
    }

    ~StorageContext() override = default;

    StorageContext(StorageContext const& other) = delete;
    StorageContext& operator=(StorageContext const& ohter) = delete;

    StorageContext(StorageContext&& other) noexcept = delete;
    StorageContext& operator=(StorageContext&& ohter) = delete;

    T mStorageObject;
};

struct SessionStorageContext : public StorageContext<Common::SessionData>
{
    SessionStorageContext() = default;

    SessionStorageContext(std::shared_ptr<System::AsyncResult> result)
        : StorageContext<Common::SessionData>{result}
    {
    }

    ~SessionStorageContext() override = default;

    SessionStorageContext(SessionStorageContext const& other) = delete;
    SessionStorageContext& operator=(SessionStorageContext const& ohter) = delete;

    SessionStorageContext(SessionStorageContext&& other) noexcept = delete;
    SessionStorageContext& operator=(SessionStorageContext&& ohter) = delete;

    bool mIsUpdateContext = false;
    std::size_t mSessionId = std::size_t{0};
    Common::SessionMetaData mSessionData;
};

struct SessionMetaDataStorageContext : public StorageContext<Common::SessionMetaData>
{
    SessionMetaDataStorageContext() = default;

    SessionMetaDataStorageContext(std::shared_ptr<System::AsyncResult> result)
        : StorageContext<Common::SessionMetaData>{result}
    {
    }

    ~SessionMetaDataStorageContext() override = default;

    SessionMetaDataStorageContext(SessionMetaDataStorageContext const& other) = delete;
    SessionMetaDataStorageContext& operator=(SessionMetaDataStorageContext const& ohter) = delete;

    SessionMetaDataStorageContext(SessionMetaDataStorageContext&& other) noexcept = delete;
    SessionMetaDataStorageContext& operator=(SessionMetaDataStorageContext&& ohter) = delete;
};

struct TrackStorageContext : public StorageContext<Common::TrackData>
{
    TrackStorageContext() = default;

    TrackStorageContext(std::shared_ptr<System::AsyncResult> result)
        : StorageContext<Common::TrackData>{result}
    {
    }

    ~TrackStorageContext() override = default;

    TrackStorageContext(TrackStorageContext const& other) = delete;
    TrackStorageContext& operator=(TrackStorageContext const& ohter) = delete;

    TrackStorageContext(TrackStorageContext&& other) noexcept = delete;
    TrackStorageContext& operator=(TrackStorageContext&& ohter) = delete;

    std::size_t mTrackIndex = std::size_t{0};
};

template <typename T>
struct TrackStorageContextWithValue : public TrackStorageContext
{
    TrackStorageContextWithValue(std::shared_ptr<System::AsyncResult> result)
        : TrackStorageContext{result}
    {
    }

    ~TrackStorageContextWithValue() override = default;

    TrackStorageContextWithValue(TrackStorageContextWithValue const& other) = delete;
    TrackStorageContextWithValue& operator=(TrackStorageContextWithValue const& ohter) = delete;

    TrackStorageContextWithValue(TrackStorageContextWithValue&& other) noexcept = delete;
    TrackStorageContextWithValue& operator=(TrackStorageContextWithValue&& ohter) = delete;

    T value;
};

template <typename T>
struct SessionStorageContextWithValue : public SessionStorageContext
{
    SessionStorageContextWithValue(std::shared_ptr<System::AsyncResult> result)
        : SessionStorageContext{result}
    {
    }

    ~SessionStorageContextWithValue() override = default;

    SessionStorageContextWithValue(SessionStorageContextWithValue const& other) = delete;
    SessionStorageContextWithValue& operator=(SessionStorageContextWithValue const& ohter) = delete;

    SessionStorageContextWithValue(SessionStorageContextWithValue&& other) noexcept = delete;
    SessionStorageContextWithValue& operator=(SessionStorageContextWithValue&& ohter) = delete;

    T value;
};

} // namespace Rapid::Storage::Private
