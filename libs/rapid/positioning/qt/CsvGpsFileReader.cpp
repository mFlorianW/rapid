// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "CsvGpsFileReader.hpp"
#include <QDebug>
#include <QFile>

namespace Rapid::Positioning::Qt
{

void CsvGpsFileReader::setFileName(QString const& fileName)
{
    mFileName = fileName;
}

bool CsvGpsFileReader::read()
{
    if (!QFile::exists(mFileName)) {
        qWarning() << "File doesn't exists:" << mFileName;
        return false;
    }

    auto file = QFile{mFileName};
    if (!file.open(QFile::Text | QFile::ReadOnly)) {
        qWarning() << "File can't be opended:" << mFileName;
        return false;
    }

    QTextStream in(&file);
    while (!in.atEnd()) {
        auto line = in.readLine();
        auto splitted = line.split(",");
        if (splitted.size() < 2) {
            return false;
        }
        bool ok = false;
        float lon = splitted[0].toFloat(&ok);
        if (!ok) {
            return false;
        }

        ok = false;
        float lat = splitted[1].toFloat(&ok);
        if (!ok) {
            return false;
        }

        mPositions.emplace_back(lat, lon);
    }

    return true;
}

std::vector<Rapid::Common::PositionData> CsvGpsFileReader::getPostions() const noexcept
{
    return mPositions;
}

} // namespace Rapid::Positioning::Qt
