add_library(CommonObject OBJECT)
add_library(Rapid::CommonObject ALIAS CommonObject)

target_sources(CommonObject
PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/TrackData.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/TrackData.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/PositionData.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/PositionData.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Timestamp.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Timestamp.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/LapData.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/LapData.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/SessionData.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/SessionData.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Date.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Date.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/JsonSerializer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/JsonSerializer.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/JsonDeserializer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/JsonDeserializer.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/SharedDataPointer.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/GpsPositionData.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/GpsPositionData.hpp
)

target_include_directories(CommonObject
PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(CommonObject
PUBLIC
    KDAB::KDBindings
)

add_library(Common STATIC)
add_library(Rapid::Common ALIAS Common)

target_link_libraries(Common
PUBLIC
    Rapid::CommonObject
)

if(ENABLE_QT)
    add_subdirectory(qt)
endif()
