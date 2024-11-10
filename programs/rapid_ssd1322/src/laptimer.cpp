// SPDX-FileCopyrightText: 2024 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "Controls.hpp"
#include <ConstantGpsPositionProvider.hpp>
#include <LibraryPath.hpp>
#include <SDL2/SDL.h>
#include <ScreenModel.hpp>
#include <SqliteSessionDatabase.hpp>
#include <SqliteTrackDatabase.hpp>
#include <StaticGpsInformationProvider.hpp>
#include <StaticPositionDateTimeProvider.hpp>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <monitor.h>
#include <mouse.h>
#include <pwd.h>
#include <unistd.h>

[[noreturn]] static int tick_thread(void* data)
{
    (void)data;

    while (true) {
        SDL_Delay(5); /*Sleep for 5 millisecond*/
        lv_tick_inc(5); /*Tell LittelvGL that 5 milliseconds were elapsed*/
    }
}

static void memory_monitor(lv_task_t* param)
{
    (void)param; /*Unused*/
    lv_mem_monitor_t mon;
    lv_mem_monitor(&mon);
}

static void hal_init()
{
    /* Use the 'monitor' driver which creates window on PC's monitor to simulate a display*/
    monitor_init();

    /*Create a display buffer*/
    static lv_disp_buf_t disp_buf1;
    static lv_color_t buf1_1[LV_HOR_RES_MAX * 120]; // NOLINT modernize-avoid-c-arrays
    lv_disp_buf_init(&disp_buf1, &buf1_1[0], nullptr, LV_HOR_RES_MAX * 120);

    /*Create a display*/
    lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv); /*Basic initialization*/
    disp_drv.buffer = &disp_buf1;
    disp_drv.flush_cb = monitor_flush;
    lv_disp_drv_register(&disp_drv);

    /* Add the mouse as input device
     * Use the 'mouse' driver which reads the PC's mouse*/
    mouse_init();
    lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv); /*Basic initialization*/
    indev_drv.type = LV_INDEV_TYPE_POINTER;

    /*This function will be called periodically (by the library) to get the mouse position and state*/
    indev_drv.read_cb = mouse_read;
    lv_indev_drv_register(&indev_drv);

    /* Tick init.
     * You have to call 'lv_tick_inc()' in periodically to inform LittelvGL about
     * how much time were elapsed Create an SDL thread to do this*/
    SDL_CreateThread(tick_thread, "tick", nullptr);

    /* Optional:
     * Create a memory monitor task which prints the memory usage in
     * periodically.*/
    lv_task_create(memory_monitor, 5000, LV_TASK_PRIO_MID, nullptr);
}

std::vector<Rapid::Common::PositionData> loadPositions(std::string const filePath)
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

int main(int argc, char* argv[])
{
    (void)argc;
    (void)argv;

    // Initialize LVGL
    lv_init();

    // Initialize the HAL (display, input devices, tick) for LVGL
    hal_init();

    // get home folder for the session database backend.
    auto databaseFolder = std::string{getpwuid(getuid())->pw_dir} + "/.local/share/laptimer/session";
    if (!std::filesystem::exists(databaseFolder)) {
        std::cout << "Laptimer data folder doesn't exists. Creating:" << databaseFolder;
        std::filesystem::create_directories(databaseFolder);
    }

    // Initialize the Controls to navigate the Shell
    Controls ctl;

    // Load GPS position file
    auto positions = loadPositions("/home/florian/Coding/laptimer_core/laps/Oschersleben.csv");

    auto eventLoop = Rapid::System::EventLoop{};
    auto gpsInfoProvider = Rapid::Positioning::StaticGpsInformationProvider{};
    auto posDateTimeProvider = Rapid::Positioning::ConstantGpsPositionProvider{positions};
    auto sessionDatabase = Rapid::Storage::SqliteSessionDatabase{LIBRARY_FILE};
    auto trackDatabase = Rapid::Storage::SqliteTrackDatabase{LIBRARY_FILE};
    auto screenModel = ScreenModel{gpsInfoProvider, posDateTimeProvider, sessionDatabase, trackDatabase};
    screenModel.activateMainScreen();
    posDateTimeProvider.setVelocityInMeterPerSecond(80.6667);
    posDateTimeProvider.start();

    while (true) {
        /* Periodically call the lv_task handler.
         * It could be done in a timer interrupt or an OS task too.*/
        lv_task_handler();
        eventLoop.processEvents();
        usleep(1 * 1000);
    }
}
