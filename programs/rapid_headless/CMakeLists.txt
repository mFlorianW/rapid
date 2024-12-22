add_executable(rapid_headless)

target_sources(rapid_headless
PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/rapid_headless.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/RapidHeadless.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/RapidHeadless.hpp
)

target_include_directories(rapid_headless
PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/
)

target_link_libraries(rapid_headless
PRIVATE
    spdlog::spdlog
    Boost::program_options
    Rapid::LibraryPath
    Rapid::System
    Rapid::Positioning
    Rapid::Storage
    Rapid::Workflow
)
