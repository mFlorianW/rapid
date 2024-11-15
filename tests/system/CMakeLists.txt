include(Catch)

add_executable(test_system)

target_sources(test_system
PRIVATE
    test_Timer.cpp
    test_FutureWatcher.cpp
    test_AsyncResult.cpp
    test_Event.cpp
    test_EventLoop.cpp
)

target_link_libraries(test_system
PRIVATE
    Catch2::Catch2WithMain
    Rapid::SystemQt
    Rapid::TestHelper
)

catch_discover_tests(test_system)

if(ENABLE_QT)
    find_package(Qt6Test REQUIRED)

    add_executable(test_QEventDispatcher test_QEventDispatcher.cpp)
    target_link_libraries(test_QEventDispatcher PRIVATE Qt6::Test Rapid::SystemQt)
endif()
