include(Catch)

set(CONFIG_FILE_PATH ${CMAKE_CURRENT_SOURCE_DIR})
set(CONFIG_APP_PATH ${CMAKE_CURRENT_SOURCE_DIR}/testapps)
configure_file(test_FilePath.hpp.in ${CMAKE_CURRENT_BINARY_DIR}/FilePath.hpp)

set(DUMMY_PROCESS_BINARY_PATH ${CMAKE_CURRENT_BINARY_DIR}/dummy_process)
set(DUMMY_PROCESS_BINARY_NAME "dummy_process")
configure_file(DummyProcessPath.hpp.in ${CMAKE_CURRENT_BINARY_DIR}/DummyProcessPath.hpp)

add_executable(test_rapid_shell_application)
add_executable(dummy_process DummyProcess.cpp)

target_sources(test_rapid_shell_application
PRIVATE
    test_ApplicationConfigReader.cpp
    test_ApplicationVersion.cpp
    test_ApplicationModel.cpp
    test_ApplicationListModel.cpp
    test_GlobalSettingsWriter.cpp
    test_ProcessManager.cpp
)

target_include_directories(test_rapid_shell_application
PRIVATE
    ${CMAKE_CURRENT_BINARY_DIR}
)

target_link_libraries(test_rapid_shell_application
PRIVATE
    Catch2::Catch2
    Qt6::Test
    Rapid::ShellApplications
    Rapid::ShellSettings
    Rapid::TestHelperQt
)

catch_discover_tests(test_rapid_shell_application
    DISCOVERY_MODE PRE_TEST
)
