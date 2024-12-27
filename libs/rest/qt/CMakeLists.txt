add_library(RestQt STATIC)
add_library(Rapid::RestQt ALIAS RestQt)

target_sources(RestQt
PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/QRestClient.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/QRestClient.hpp
)

target_link_libraries(RestQt
PUBLIC
    Rapid::Rest
PRIVATE
    Qt6::Core
    Qt6::Network
)

target_include_directories(RestQt
PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
)

target_compile_definitions(RestQt PRIVATE QT_NO_KEYWORDS)
