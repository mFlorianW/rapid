add_library(Positioning STATIC)
add_library(Rapid::Positioning ALIAS Positioning)

target_sources(Positioning
PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/ConstantGpsPositionProvider.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ConstantGpsPositionProvider.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/IGPSInformationProvider.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/IGpsPositionProvider.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/StaticGpsInformationProvider.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/StaticPositionDateTimeProvider.hpp
)

target_include_directories(Positioning
PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
)

target_link_libraries(Positioning
PRIVATE
    spdlog::spdlog
PUBLIC
    Rapid::Common
    Rapid::System
    Rapid::ThirdParty::UTM
    KDAB::KDBindings
)

if(ENABLE_QT)
    add_subdirectory(qt)
endif()
