// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef CSVGPSFILEREADER_HPP
#define CSVGPSFILEREADER_HPP

#include <QString>
#include <common/PositionData.hpp>
#include <vector>

namespace Rapid::Positioning::Qt
{

/**
 * Reads position data from a GPS file and copies them into the memory.
 */
class CsvGpsFileReader
{
public:
    /**
     * Sets a new file name to read the the positions data from. The filename can be a path as well.
     * @param fileName The new filename.
     */
    void setFileName(QString const& fileName);

    /**
     * Reads the positions from the file.
     * @return True all positions from the file are read.
     * @return False Something went wrong by reading the file.
     */
    bool read();

    /**
     * Gives the list of read positions data.
     * @return The list of all positions data.
     */
    std::vector<Common::PositionData> getPostions() const noexcept;

private:
    QString mFileName;
    std::vector<Common::PositionData> mPositions;
};

} // namespace Rapid::Positioning::Qt

#endif // CSVGPSFILEREADER_HPP
