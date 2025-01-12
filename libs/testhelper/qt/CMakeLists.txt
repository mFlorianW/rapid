add_library(TestHelperQt STATIC)
add_library(Rapid::TestHelperQt ALIAS TestHelperQt)

target_sources(TestHelperQt
PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/SettingsMemoryBackend.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/SettingsMemoryBackend.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ProcessStatusChecker.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ProcessStatusChecker.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ProcessStopper.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ProcessStopper.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/QtTestHelper.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/SettingsBackendMock.hpp
)

target_link_libraries(TestHelperQt
PUBLIC
    Rapid::TestHelper
    Rapid::RapidQt
)

target_include_directories(TestHelperQt
PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/../..
)
