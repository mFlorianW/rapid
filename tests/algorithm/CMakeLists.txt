include(Catch)

add_executable(test_algorithm)

target_sources(test_algorithm
PRIVATE
    test_SimpleLaptimer.cpp
    test_TrackDetection.cpp
)

target_link_libraries(test_algorithm
PRIVATE
    Catch2::Catch2WithMain
    Rapid::Rapid
    Rapid::TestHelper
)

catch_discover_tests(test_algorithm
    DISCOVERY_MODE PRE_TEST
)
