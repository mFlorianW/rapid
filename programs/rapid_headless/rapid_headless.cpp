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
#include <boost/program_options.hpp>
#include <csignal>
#include <fstream>
#include <pwd.h>
#include <spdlog/spdlog.h>
#include <sstream>
#include <string>
#include <unistd.h>
#include <vector>

using namespace Rapid::System;
using namespace Rapid::Positioning;
using namespace Rapid::Storage;
using namespace Rapid::LappyHeadless;
using namespace boost::program_options;

namespace
{

void signalHandler(int)
{
    EventLoop::instance().quit();
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

void printHelp(options_description const& opts)
{
    std::cout << opts << "\n";
}

} // namespace

int main(int argc, char** argv)
{
    auto& eventLoop = EventLoop::instance();

    // clang-format off
    struct sigaction action{};
    // clant-format on
    action.sa_handler = signalHandler;
    sigaction(SIGINT, &action, nullptr);
    sigaction(SIGTERM, &action, nullptr);

    auto options = options_description{"Options"};
    std::string gpsSourceFile{};
    // clang-format off
    options.add_options()
        ("help,h", "Show options overivew")
        ("gps-fake,g", "Enables a fake GPS source (useful for testing)")
        ("gps-source-file,f", value<std::string>(&gpsSourceFile), "Path to CSV file that contains GPS positions (useful for testing)")
    ;
    // clang-format on
    variables_map optionsMap;
    try {
        store(parse_command_line(argc, argv, options), optionsMap);
    } catch (std::exception const& e) {
        SPDLOG_ERROR("Inavlid option: {}", e.what());
        printHelp(options);
    }

    if (optionsMap.count("help") > 0) {
        printHelp(options);
        return 0;
    }

    if (optionsMap.contains("gps-source-file") > 0) {
        gpsSourceFile = optionsMap["gps-source-file"].as<std::string>();
    }

    if (optionsMap.count("gps-fake") > 0 and gpsSourceFile.empty()) {
        SPDLOG_ERROR("Fake GPS enabled but no GPS position source file passed.");
        printHelp(options);
        return 0;
    }

    if (gpsSourceFile.empty()) {
        SPDLOG_ERROR("No GPS position source file passed.");
        printHelp(options);
        return 0;
    }

    // Load GPS position file
    auto positions = loadCsvPositions(gpsSourceFile);
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
