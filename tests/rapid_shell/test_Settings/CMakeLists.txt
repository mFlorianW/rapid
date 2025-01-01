include(Catch)

add_executable(test_rapid_shell_settings)

target_sources(test_rapid_shell_settings
PRIVATE
    test_DeviceModel.cpp
)

target_link_libraries(test_rapid_shell_settings
PRIVATE
    Catch2::Catch2WithMain
    Rapid::ShellSettings
    Rapid::TestHelperQt
)

catch_discover_tests(test_rapid_shell_settings
    DISCOVERY_MODE PRE_TEST
)
