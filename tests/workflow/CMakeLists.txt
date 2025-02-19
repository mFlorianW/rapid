include(Catch)

add_executable(test_workflow)

target_sources(test_workflow
PRIVATE
    test_ActiveSessionWorkflow.cpp
    test_RestSessionManagementWorkflow.cpp
    test_TrackDetectionWorkflow.cpp
    test_ActiveSessionEndpoint.cpp
    test_RestActiveSession.cpp
)

target_link_libraries(test_workflow
PRIVATE
    Catch2::Catch2WithMain
    Rapid::Rapid
    Rapid::TestHelper
    spdlog::spdlog
)

catch_discover_tests(test_workflow
    DISCOVERY_MODE PRE_TEST
)

if(ENABLE_DESKTOP)
    add_executable(test_workflow_qt)
    target_sources(test_workflow_qt
        PRIVATE
            test_qt_workflow_main.cpp
            test_SessionAnalyzeWorkflow.cpp
            test_qt_RestSessionDownloader.cpp
            test_qt_DeviceManagement.cpp
    )
    target_link_libraries(test_workflow_qt
            Catch2::Catch2
            spdlog::spdlog
            Qt6::Test
            Rapid::RapidQt
            Rapid::TestHelperQt
    )
    catch_discover_tests(test_workflow_qt
        DISCOVERY_MODE PRE_TEST
    )
endif()
