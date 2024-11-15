include(Catch)

add_executable(test_rest)

target_sources(test_rest
PRIVATE
    test_GpsEndpoint.cpp
    test_Path.cpp
    test_SessionEndpoint.cpp
    test_RestCall.cpp
    test_RestServer.cpp
)

target_link_libraries(test_rest
PRIVATE
    Catch2::Catch2WithMain
    Rapid::Rest
    Rapid::TestHelper
)

catch_discover_tests(test_rest)
