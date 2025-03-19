add_executable(test_rapid_android)
target_sources(test_rapid_android
    PRIVATE
        test_main.cpp
        test_ContextMenuModel.cpp
)
target_link_libraries(test_rapid_android
        Catch2::Catch2
        Qt6::Test
        spdlog::spdlog
        Rapid::RapidQt
        Rapid::TestHelperQt
        Rapid::Android
)
catch_discover_tests(test_rapid_android
    DISCOVERY_MODE PRE_TEST
)

