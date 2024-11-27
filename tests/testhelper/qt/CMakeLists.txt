add_library(TestHelperQt INTERFACE)
add_library(Rapid::TestHelperQt ALIAS TestHelperQt)

target_sources(TestHelperQt
INTERFACE
    QtTestHelper.hpp
)

target_link_libraries(TestHelperQt
INTERFACE
    Rapid::SystemQt
)

target_include_directories(TestHelperQt
INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)
