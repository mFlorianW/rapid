// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "Path.hpp"
#include <algorithm>
#include <string>
#include <vector>

namespace Rapid::Rest
{
class SharedPath : public Common::SharedData
{
public:
    std::string mPath;
    std::vector<std::size_t> mPositions{};

    friend bool operator==(SharedPath const& lhs, SharedPath const& rhs)
    {
        return (lhs.mPositions == rhs.mPositions) && (lhs.mPath == rhs.mPath);
    }
};

Path::Path() noexcept
    : mData{new(std::nothrow) SharedPath{}}
{
}

Path::Path(std::string path) noexcept
    : mData{new(std::nothrow) SharedPath{}}
{
    mData->mPath = std::move(path);
    mData->mPositions.reserve(getDepth());
    for (auto i = std::size_t{0}; i < mData->mPath.size(); ++i) {
        if (mData->mPath.at(i) == '/') {
            mData->mPositions.emplace_back(i);
        }
    }
}

Path::~Path() noexcept = default;

Path::Path(Path const& other) = default;
Path& Path::operator=(Path const& other) = default;

Path::Path(Path&& other)
    : mData{std::move(other.mData)}
{
    other.mData = nullptr;
}

Path& Path::operator=(Path&& other)
{
    Path moved{std::move(other)};
    std::swap(mData, moved.mData);
    return *this;
}

std::string_view Path::getPath() const noexcept
{
    return mData->mPath;
}

std::size_t Path::getDepth() const noexcept
{
    if (mData->mPath.empty()) {
        return 0;
    }

    auto const count = std::count(mData->mPath.cbegin(), mData->mPath.cend(), '/');
    return count;
}

std::optional<std::string_view> Path::getEntry(std::size_t index) const noexcept
{
    if (mData->mPath.empty() || index > mData->mPositions.size()) {
        return std::nullopt;
    }

    constexpr auto pathDelimiterSize = std::size_t{1};
    auto const entryBegin = mData->mPositions.at(index) + pathDelimiterSize;
    auto const entryEnd = index + 1 >= mData->mPositions.size() ? mData->mPath.size() : mData->mPositions.at(index + 1);
    auto const entry = std::string_view{mData->mPath.data() + entryBegin, entryEnd - entryBegin};
    return entry;
}

bool operator==(Path const& lhs, Path const& rhs)
{
    return lhs.mData == rhs.mData || *lhs.mData == *rhs.mData;
}

bool operator!=(Path const& lhs, Path const& rhs)
{
    return !(lhs == rhs);
}

} // namespace Rapid::Rest
