set(RAPID_COMMON_PUBLIC_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/TrackData.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/PositionData.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Timestamp.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/LapData.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/SessionData.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Date.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/JsonSerializer.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/JsonDeserializer.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/SharedDataPointer.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/VelocityData.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/SessionMetaData.hpp
)
install(FILES ${RAPID_COMMON_PUBLIC_HEADERS} DESTINATION "${INCLUDE_INSTALL_DIR}/common")

target_sources(RapidLibrary
PRIVATE
    ${RAPID_COMMON_PUBLIC_HEADERS}
    ${CMAKE_CURRENT_SOURCE_DIR}/TrackData.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/PositionData.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Timestamp.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/LapData.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/SessionData.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Date.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/JsonSerializer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/JsonDeserializer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/GpsPositionData.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/GpsPositionData.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/VelocityData.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/SessionMetaData.cpp
)

if(ENABLE_QT)
    add_subdirectory(qt)
endif()
