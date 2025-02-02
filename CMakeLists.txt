cmake_minimum_required(VERSION 3.19)

project(Rapid
    VERSION 0.1.0
)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/configuration/cmake/")

include(FeatureSummary)
include(CheckCCompilerFlag)
include(GNUInstallDirs)
include(PreCommit)
include(CMakePackageConfigHelpers)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(SHARED_DATADIR ${CMAKE_INSTALL_DATAROOTDIR}/rapid)
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
# the RPATH to be used when installing, but only if it's not a system directory
list(FIND CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES "${CMAKE_INSTALL_PREFIX}/lib" isSystemDir)
if("${isSystemDir}" STREQUAL "-1")
    SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_LIBDIR}/lib")
endif("${isSystemDir}" STREQUAL "-1")

option(ENABLE_TEST "Enable unit tests" OFF)
option(ENABLE_DESKTOP "This option enables QT implementations" OFF)
option(ENABLE_PRECOMMIT "Enables automatic pre-commit install." OFF)
option(ENABLE_VCPKG "This option is enabled when building with VCPKG" OFF)
option(ENABLE_ANDROID "This option enables the Android build" OFF)
add_feature_info(PreCommit ENABLE_PRECOMMIT "Automatic install of the Python pre-commit tool.")

if(ENABLE_PRECOMMIT)
    setup_precommit()
endif()

if(ENABLE_DESKTOP OR ENABLE_ANDROID)
    if(ENABLE_ANDROID AND NOT UNIX)
        MESSAGE(FATAL_ERROR "ENABLE_ANDROID is only supported on Linux")
    endif()

    if(ENABLE_ANDROID AND ENABLE_VCPKG)
        MESSAGE(FATAL_ERROR "ENABLE_ANDROID and ENABLE_VCPKG is not supported")
    endif()

    if(ENABLE_ANDROID AND ENABLE_TESTING)
        MESSAGE(FATAL_ERROR "ENABLE_ANDROID and ENABLE_TESTING is not supported")
    endif()

    if(ENABLE_ANDROID AND ENABLE_DESKTOP)
        MESSAGE(FATAL_ERROR "ENABLE_ANDROID and ENABLE_DESKTOP is not supported")
    endif()

    if(ENABLE_ANDROID)
        if(ANDROID_NDK_ROOT)
            set(ANDROID_NDK_HOME ${ANDROID_NDK_ROOT})
            MESSAGE(STATUS "Use Android NDK ROOT ${ANDROID_NDK_ROOT}")
        else()
            set(ANDROID_NDK_HOME ${ANDROID_SDK_ROOT}/ndk/26.3.11579264)
            set(ENV{ANDROID_NDK_HOME} ${ANDROID_NDK_HOME})
            MESSAGE(STATUS "Android NDK root not set use default: ${ANDROID_NDK_HOME}")
        endif()

        set(ENV{ANDROID_NDK_HOME} ${ANDROID_NDK_HOME})
        set(VCPKG_TARGET_TRIPLET ${VCPKG_TRIPLET})
        include($ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake)
    endif()

    set(CMAKE_AUTOMOC ON)
    set(CMAKE_AUTORCC ON)
    set(CMAKE_AUTOUIC ON)

    ADD_DEFINITIONS(-DQT_NO_KEYWORDS)

    if(QT_KNOWN_POLICY_QTP0001)
        qt_policy(SET QTP0001 NEW)
    endif()
    find_package(Qt6 REQUIRED COMPONENTS
        Core
        Qml
        Gui
        Quick
        Widgets
        Network
        Location
        QuickControls2
    )

    if(ENABLE_DESKTOP)
        find_package(Qt6 REQUIRED COMPONENTS
            DBus
            QuickWidgets
        )
    endif()

    check_c_compiler_flag(-Wall HAS_WALL)
    if(HAS_WALL)
        set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -Wall")
    endif()
endif()

find_package(KDBindings REQUIRED)
find_package(spdlog CONFIG REQUIRED)
if(ENABLE_VCPKG)
    find_package(unofficial-sqlite3 CONFIG REQUIRED)
    add_library(SQLite::SQLite3 ALIAS unofficial::sqlite3::sqlite3)
else()
    find_package(SQLite3 REQUIRED)
endif()
find_package(Boost CONFIG REQUIRED COMPONENTS program_options)
find_package(nlohmann_json CONFIG REQUIRED)
if(ENABLE_VCPKG OR ENABLE_ANDROID)
    find_path(COMMS_UBLOX_INCLUDE_DIRS "cc_ublox/Message.h")
    add_library(ccublox INTERFACE)
    add_library(cc::cc_ublox ALIAS ccublox)
    target_include_directories(ccublox INTERFACE ${COMMS_UBLOX_INCLUDE_DIRS})
else()
    find_package(cc_ublox CONFIG REQUIRED)
endif()
find_package(LibComms CONFIG REQUIRED)

if(ENABLE_TEST)
    include(CTest)
    find_package(Catch2 CONFIG REQUIRED)
    find_package(trompeloeil CONFIG REQUIRED)
    if(ENABLE_DESKTOP)
        find_package(Qt6 REQUIRED Test)
    endif()
    enable_testing()
    add_subdirectory(tests)
endif(ENABLE_TEST)

add_subdirectory(libs)
add_subdirectory(db)
add_subdirectory(programs)
add_subdirectory(thirdparty)
add_subdirectory(laps)

feature_summary(WHAT ALL)
