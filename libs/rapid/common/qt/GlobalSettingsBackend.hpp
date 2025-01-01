// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once
#include <QSettings>

namespace Rapid::Common::Qt
{

class GlobalSettingsBackend
{
public:
    /**
     * Default destructor
     */
    virtual ~GlobalSettingsBackend() = default;

    /**
     * Deleted copy constructor
     */
    GlobalSettingsBackend(GlobalSettingsBackend const&) = delete;

    /**
     * Deleted copy operator
     */
    GlobalSettingsBackend& operator=(GlobalSettingsBackend const&) = delete;

    /**
     * Deleted copy constructor
     */
    GlobalSettingsBackend(GlobalSettingsBackend&&) noexcept = delete;

    /**
     * Deleted move operator
     */
    GlobalSettingsBackend& operator=(GlobalSettingsBackend&&) noexcept = delete;

    /**
     * Stores a value under the given key.
     * @param key The key under which the value shall be stored to load it later.
     * @param value The value that shall be stored.
     * @return true Successful stored.
     *         false Failed to store.
     */
    [[nodiscard]] virtual bool storeValue(QAnyStringView const& key, QVariant const& value) noexcept = 0;

    /**
     * Gives the stored value for the passed key.
     * If the key is not found an invalid QVariant is returned.
     * @param key The key of the settings parameter
     * @return Success The value in a QVariant
     *         Failure An invalud QVariant
     */
    [[nodiscard]] virtual QVariant getValue(QAnyStringView const& key) const noexcept = 0;

protected:
    GlobalSettingsBackend() = default;
};

/**
 * QSettings based implementation of the @ref ISettingsBackend
 */
class QSettingsBackend : public GlobalSettingsBackend
{
public:
    /**
     * Creates an instance of the QSettings backend
     */
    QSettingsBackend();

    /**
     * Default empty destructor
     */
    ~QSettingsBackend() override = default;

    /**
     * Deleted copy constructor
     */
    QSettingsBackend(QSettingsBackend const&) = delete;

    /**
     * Deleted copy operator
     */
    QSettingsBackend& operator=(QSettingsBackend const&) = delete;

    /**
     * Deleted copy constructor
     */
    QSettingsBackend(QSettingsBackend&&) noexcept = delete;

    /**
     * Deleted move operator
     */
    QSettingsBackend& operator=(QSettingsBackend&&) noexcept = delete;

    /**
     * @copydoc GlobalSettingsBackend::storeValue
     */
    [[nodiscard]] bool storeValue(QAnyStringView const& key, QVariant const& value) noexcept override;

    /**
     * @copydoc GlobalSettingsBackend::getValue
     */
    [[nodiscard]] QVariant getValue(QAnyStringView const& key) const noexcept override;

private:
    QSettings mSettings;
};

} // namespace Rapid::Common::Qt
