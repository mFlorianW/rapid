// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once
#include <common/SharedDataPointer.hpp>
#include <optional>
#include <string>

namespace Rapid::Rest
{
class SharedPath;

/**
 * A path in a REST URL and provide access to the
 * elements in the URL.
 */
class Path final
{
public:
    /**
     * Creates an empty path.
     */
    Path() noexcept;

    /**
     * Creates an instance of the path.
     * @param path The that shall be used.
     */
    Path(std::string path) noexcept;

    /**
     * Empty default destructor
     */
    ~Path() noexcept;

    /**
     * Copy constructor for Path
     * @param ohter The object to copy from.
     */
    Path(Path const& ohter);

    /**
     * The copy assignment operator for Path.
     * @param other The object to copy from.
     * @return Path& A reference to the copied path.
     */
    Path& operator=(Path const& other);

    /**
     * Move constructor for Path
     * @param other The object to move from.
     */
    Path(Path&& other);

    /**
     * The move assignment operator for the Path.
     * @param other The object to move from.
     * @return Path& A reference to the moved path.
     */
    Path& operator=(Path&& other);

    /**
     * Gives the whole path.
     * @return The whole stored path.
     */
    std::string_view getPath() const noexcept;

    /**
     * Gives the depth of the path.
     * @return The depth of the path
     */
    std::size_t getDepth() const noexcept;

    /**
     * Gives the entry by the index when the index is not valid an
     * nullopt will be returned.
     * @return The entry or an invalid index if the is not present.
     */
    std::optional<std::string_view> getEntry(std::size_t index) const noexcept;

    /**
     * Equal operator
     * @return true The two objects are the same.
     * @return false The two objects are not the same.
     */
    friend bool operator==(Path const& lhs, Path const& rhs);

    /**
     * Not Equal operator
     * @return true The two objects are not the same.
     * @return false The two objects are the same.
     */
    friend bool operator!=(Path const& lhs, Path const& rhs);

private:
    Common::SharedDataPointer<SharedPath> mData;
};
} // namespace Rapid::Rest
