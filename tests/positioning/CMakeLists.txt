include(Catch)

add_executable(test_positioning)

target_sources(test_positioning
    PRIVATE
        test_ConstantGpsPositionProvider.cpp
        test_UbloxGpsPositionInformationProvider.cpp
)

target_link_libraries(test_positioning
    PRIVATE
        Catch2::Catch2WithMain
        Rapid::Rapid
        Rapid::TestHelper
)

catch_discover_tests(test_positioning
    DISCOVERY_MODE PRE_TEST
)
