// SPDX-FileCopyrightText: 2024 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "RapidHeadless.hpp"
#include <ConstantGpsPositionProvider.hpp>
#include <EventLoop.hpp>
#include <LibraryPath.hpp>
#include <PositionData.hpp>
#include <SqliteSessionDatabase.hpp>
#include <SqliteTrackDatabase.hpp>
#include <array>
#include <csignal>
#include <fstream>
#include <pwd.h>
#include <sstream>
#include <string>
#include <unistd.h>
#include <vector>

using namespace Rapid::System;
using namespace Rapid::Positioning;
using namespace Rapid::Storage;
using namespace Rapid::LappyHeadless;

namespace
{

void signalHandler(int)
{
    EventLoop{}.quit();
}

std::vector<Rapid::Common::PositionData> loadCsvPositions(std::string const filePath)
{
    auto positions = std::vector<Rapid::Common::PositionData>{};
    auto file = std::fstream(filePath);
    auto line = std::string{};

    while (std::getline(file, line)) {
        std::istringstream input(line);
        std::array<std::string, 2> splittedLine;
        for (auto& i : splittedLine) {
            getline(input, i, ',');
        }

        auto longitude = std::stof(splittedLine[0]);
        auto latitude = std::stof(splittedLine[1]);

        positions.emplace_back(latitude, longitude);
    }

    return positions;
}

} // namespace

int main(int argc, char** argv)
{
    EventLoop eventLoop;

    // clang-format off
    struct sigaction action{};
    // clant-format on
    action.sa_handler = signalHandler;
    sigaction(SIGINT, &action, nullptr);
    sigaction(SIGTERM, &action, nullptr);

    // Load GPS position file
    auto positions = loadCsvPositions("/home/florian/Coding/rapid/laps/Oschersleben.csv");
    auto positionProvider = ConstantGpsPositionProvider{positions};
    positionProvider.setVelocityInMeterPerSecond(80.3333);
    positionProvider.start();

    // Setup session database
    auto sessionDatabase = SqliteSessionDatabase{LIBRARY_FILE};

    // Setup track database
    auto trackDatabase = SqliteTrackDatabase{LIBRARY_FILE};

    // Setup headless laptimer
    auto laptimer = LappyHeadless{positionProvider, sessionDatabase, trackDatabase};

    eventLoop.exec();
    return 0;
}
