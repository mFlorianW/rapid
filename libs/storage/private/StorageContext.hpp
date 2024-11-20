// SPDX-FileCopyrightText: 2024 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <AsyncResult.hpp>
#include <FutureWatcher.hpp>
#include <SessionData.hpp>
#include <thread>

namespace Rapid::Storage::Private
{

struct StorageContextBase
{
    StorageContextBase(std::shared_ptr<System::AsyncResult> result = std::make_shared<System::AsyncResult>())
        : mResult{std::move(result)}
    {
        mStorageResult.setFuture(mStoragePromise.get_future());
        mStorageResult.finished.connect([this] {
            done.emit(this);
        });
    }
    virtual ~StorageContextBase() = default;

    StorageContextBase(StorageContextBase const& other) = delete;
    StorageContextBase& operator=(StorageContextBase const& ohter) = delete;

    StorageContextBase(StorageContextBase&& other) noexcept = delete;
    StorageContextBase& operator=(StorageContextBase&& ohter) = delete;

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
    ~StorageContext() override = default;

    StorageContext(StorageContext const& other) = delete;
    StorageContext& operator=(StorageContext const& ohter) = delete;

    StorageContext(StorageContext&& other) noexcept = delete;
    StorageContext& operator=(StorageContext&& ohter) = delete;

    T mStorageObject;
};

struct SessionStorageContext : public StorageContext<Common::SessionData>
{
    bool mIsUpdateContext = false;
    std::size_t mSessionId = std::size_t{0};
};

struct TrackStorageContext : public StorageContext<Common::TrackData>
{
    std::size_t mTrackIndex = std::size_t{0};
};

} // namespace Rapid::Storage::Private
