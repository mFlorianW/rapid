// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef COMMON_QT_SESSIONMETADATAPROVIDER_HPP
#define COMMON_QT_SESSIONMETADATAPROVIDER_HPP

#include <common/SessionMetaData.hpp>
#include <common/qt/TableModelDataProvider.hpp>

namespace Rapid::Common::Qt
{

/**
 * @brief Base class for more specific SessionMetaDataProvider.
 *
 * @details This class can be used together with the @ref Common::GenericTableModel
 *          Setups the column names and the data extractor for the function.
 */
class SessionMetadataProvider : public Common::Qt::TableModelDataProvider<Common::SessionMetaData>
{
public:
    /**
     * @brief Creates an instance of the @ref SessionMetadataProvider
     *
     * @details Setups the colum names and the extractor for the @ref Common::SessionMetaData type.
     */
    SessionMetadataProvider();
};

} // namespace Rapid::Common::Qt

#endif // COMMON_QT_SESSIONMETADATAPROVIDER_HPP
