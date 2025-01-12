add_library(ShellCommonObject OBJECT)
add_library(Rapid::ShellCommonObject ALIAS ShellCommonObject)

target_sources(ShellCommonObject
PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/LoggingCategories.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/LoggingCategories.hpp
)

target_link_libraries(ShellCommonObject
PUBLIC
    Qt6::Core
)

target_include_directories(ShellCommonObject
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

add_library(ShellCommon STATIC)
add_library(Rapid::ShellCommon ALIAS ShellCommon)

target_link_libraries(ShellCommon
PUBLIC
    Rapid::ShellCommonObject
)
