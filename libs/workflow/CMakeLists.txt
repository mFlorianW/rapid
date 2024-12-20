add_library(Workflow OBJECT)
add_library(Rapid::Workflow ALIAS Workflow)

target_sources(Workflow
PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/ITrackDetectionWorkflow.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/TrackDetectionWorkflow.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/TrackDetectionWorkflow.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/IActiveSessionWorkflow.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ActiveSessionWorkflow.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ActiveSessionWorkflow.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ISessionDownloader.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/RestSessionDownloader.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/RestSessionDownloader.hpp
)

target_include_directories(Workflow
PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(Workflow
PRIVATE
    spdlog::spdlog
PUBLIC
    Rapid::Common
    Rapid::Positioning
    Rapid::Algorithm
    Rapid::Storage
    Rapid::Rest
)

if(ENABLE_QT)
    add_subdirectory(qt)
endif()
