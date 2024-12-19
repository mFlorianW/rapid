include(Catch)

add_executable(test_workflow)

target_sources(test_workflow
PRIVATE
    test_ActiveSessionWorkflow.cpp
    test_RestSessionDownloader.cpp
    test_TrackDetectionWorkflow.cpp
)

target_link_libraries(test_workflow
PRIVATE
    Catch2::Catch2WithMain
    Rapid::Workflow
    Rapid::TestHelper
)

catch_discover_tests(test_workflow)

if(ENABLE_QT)
    add_executable(test_workflow_qt)
    target_sources(test_workflow_qt
        PRIVATE
            test_workflow_qt_main.cpp
            test_SessionAnalyzeWorkflow.cpp
    )
    target_link_libraries(test_workflow_qt
            Catch2::Catch2
            Qt6::Test
            Rapid::WorkflowQt
            Rapid::TestHelperQt
    )
    catch_discover_tests(test_workflow_qt)
endif()
