add_library(UTM INTERFACE)
add_library(Rapid::ThirdParty::UTM ALIAS UTM)

target_sources(RapidLibrary
PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/UTM.hpp
)

target_include_directories(UTM
INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)
