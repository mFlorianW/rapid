set(RAPID_POSITIONING_PUBLIC_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/ConstantGpsPositionProvider.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/IGPSInformationProvider.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/IGpsPositionProvider.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/StaticGpsInformationProvider.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/StaticPositionDateTimeProvider.hpp
)

target_sources(RapidLibrary
    PRIVATE
        ${RAPID_POSITIONING_PUBLIC_HEADERS}
        ${CMAKE_CURRENT_SOURCE_DIR}/ConstantGpsPositionProvider.cpp
)

install(FILES ${RAPID_POSITIONING_PUBLIC_HEADERS} DESTINATION "${INCLUDE_INSTALL_DIR}/positioning")

if(ENABLE_QT)
    add_subdirectory(qt)
endif()
