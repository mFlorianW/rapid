include(Catch)

add_executable(test_system)

target_sources(test_system
PRIVATE
    test_Timer.cpp
    test_FutureWatcher.cpp
    test_AsyncResult.cpp
    test_Event.cpp
    test_EventLoop.cpp
    test_FdNotifier.cpp
)

target_link_libraries(test_system
PRIVATE
    Catch2::Catch2WithMain
    Rapid::TestHelper
)

catch_discover_tests(test_system
    DISCOVERY_MODE PRE_TEST
)

if(ENABLE_QT)
    add_executable(test_system_qt)
    target_sources(test_system_qt
    PRIVATE
        test_EventLoopIntegration.cpp
    )

    target_link_libraries(test_system_qt
    PRIVATE
        Catch2::Catch2
        Rapid::RapidQt
        Rapid::TestHelper
        Rapid::TestHelperQt
        Qt6::Test
    )
    catch_discover_tests(test_system_qt test_system_qt
        DISCOVERY_MODE PRE_TEST
    )
endif()
