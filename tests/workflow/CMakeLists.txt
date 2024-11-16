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
