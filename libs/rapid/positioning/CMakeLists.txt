set(RAPID_POSITIONING_PUBLIC_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/ConstantGpsPositionProvider.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/IGPSInformationProvider.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/IGpsPositionProvider.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/StaticGpsInformationProvider.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/StaticPositionDateTimeProvider.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/IUbloxDevice.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/UbloxGpsPositionInformationProvider.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/UbloxGpsPositionInformationProvider.hpp
)

target_sources(RapidLibrary
    PRIVATE
        ${RAPID_POSITIONING_PUBLIC_HEADERS}
        ${CMAKE_CURRENT_SOURCE_DIR}/ConstantGpsPositionProvider.cpp
)

if(UNIX)
    set(RAPID_POSITIONING_PUBLIC_LINUX_HEADERS
        ${CMAKE_CURRENT_SOURCE_DIR}/UartUbloxDevice.hpp
    )
    target_sources(RapidLibrary
        PRIVATE
        ${RAPID_POSITIONING_PUBLIC_LINUX_HEADERS}
        ${CMAKE_CURRENT_SOURCE_DIR}/UartUbloxDevice.cpp
    )
endif()

install(FILES ${RAPID_POSITIONING_PUBLIC_HEADERS} ${RAPID_POSITIONING_PUBLIC_LINUX_HEADERS} DESTINATION "${INCLUDE_INSTALL_DIR}/positioning")

if(ENABLE_DESKTOP)
    add_subdirectory(qt)
endif()
