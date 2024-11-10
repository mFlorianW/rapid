add_library(Positioning STATIC)
add_library(Rapid::Positioning ALIAS Positioning)

target_sources(Positioning
PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/ConstantVelocityPositionDateTimeProvider.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/IGPSInformationProvider.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/IGpsPositionProvider.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/StaticGpsInformationProvider.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/StaticPositionDateTimeProvider.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ConstantVelocityPositionDateTimeProvider.hpp
)

target_include_directories(Positioning
PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

target_link_libraries(Positioning
PUBLIC
    Rapid::Common
    Rapid::System
    Rapid::ThirdParty::UTM
    KDAB::KDBindings
)

if(ENABLE_QT)
    add_library(PositioningQt STATIC)
    add_library(Rapid::PositioningQt ALIAS PositioningQt)

    target_sources(PositioningQt
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/qt/CsvGpsFileReader.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/qt/CsvGpsFileReader.hpp
    )

    target_include_directories(PositioningQt
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/qt>
    )

    target_link_libraries(PositioningQt
    PUBLIC
        Rapid::Positioning
        Qt6::Core
    )
endif()
