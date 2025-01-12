set(TestDBusSrcs)
qt_add_dbus_interface(TestDBusSrcs
    ${CMAKE_SOURCE_DIR}/libs/rapid/storage/qt/SessionDatabase.xml
    TestSessionDatabaseIpcClient
)

add_executable(test_rapid_shell_storage)

target_sources(test_rapid_shell_storage
PRIVATE
    ${TestDBusSrcs}
    test_SessionDatabaseIpcServer.cpp
)

target_include_directories(test_rapid_shell_storage
PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
)

target_link_libraries(test_rapid_shell_storage
PRIVATE
    Catch2::Catch2
    Rapid::ShellStorage
    Rapid::TestHelper
    Rapid::TestHelperQt
    Qt6::Test
)

catch_discover_tests(test_rapid_shell_storage
    DISCOVERY_MODE PRE_TEST
)
