// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "RapidHeadless.hpp"
#include "positioning/UartUbloxDevice.hpp"
#include "positioning/UbloxGpsPositionInformationProvider.hpp"
#include <DatabaseFile.hpp>
#include <array>
#include <boost/program_options.hpp>
#include <common/PositionData.hpp>
#include <csignal>
#include <filesystem>
#include <fstream>
#include <positioning/ConstantGpsPositionProvider.hpp>
#include <pwd.h>
#include <spdlog/spdlog.h>
#include <sstream>
#include <storage/SqliteSessionDatabase.hpp>
#include <storage/SqliteTrackDatabase.hpp>
#include <string>
#include <system/EventLoop.hpp>
#include <unistd.h>
#include <vector>

using namespace Rapid::System;
using namespace Rapid::Positioning;
using namespace Rapid::Storage;
using namespace Rapid::LappyHeadless;
using namespace boost::program_options;

namespace
{

#ifdef __linux__

#include <pwd.h>
#include <sys/types.h>
#include <unistd.h>

std::optional<std::string> setupDatabase()
{
    namespace fs = std::filesystem;
    char const* hDir = getpwuid(getuid())->pw_dir;
    auto const dbLocation = fs::path{std::format("{}/.local/share/rapid-headless", hDir)};
    auto const dbFile = fs::path{std::format("{}/rapid.db", dbLocation.generic_string())};
    try {
        if (not fs::exists(dbLocation) && not fs::create_directories(dbLocation)) {
            SPDLOG_ERROR("Failed to create database location: {} ", dbLocation.generic_string());
            return std::nullopt;
        }
        if (not fs::exists(dbFile) && not fs::copy_file(DATABASE_FILE, dbFile)) {
            SPDLOG_ERROR("Failed to copy database file from {} to {}", DATABASE_FILE, dbLocation.generic_string());
            return std::nullopt;
        }
    } catch (fs::filesystem_error const& e) {
        SPDLOG_ERROR("Failed to setup database. Filesystem error: {}", e.what());
        return std::nullopt;
    }
    SPDLOG_INFO("Successful setup database file {}", dbFile.generic_string());
    return dbFile;
}

#endif

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
    auto const maybeDbFile = setupDatabase();
    if (not maybeDbFile.has_value()) {
        return 0;
    }
    auto sessionDatabase = SqliteSessionDatabase{maybeDbFile.value()};

    // Setup track database
    auto trackDatabase = SqliteTrackDatabase{maybeDbFile.value()};

    // Setup headless laptimer
    auto laptimer = LappyHeadless{positionProvider, sessionDatabase, trackDatabase};

    eventLoop.exec();
    return 0;
}
