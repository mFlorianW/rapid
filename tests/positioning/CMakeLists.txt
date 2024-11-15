include(Catch)

add_executable(test_positioning)

target_sources(test_positioning
PRIVATE
    test_ConstantVelocityPostionDateTimeProvider.cpp
)

target_link_libraries(test_positioning
PRIVATE
    Catch2::Catch2WithMain
    Rapid::Positioning
    Rapid::TestHelper
)

catch_discover_tests(test_positioning)
