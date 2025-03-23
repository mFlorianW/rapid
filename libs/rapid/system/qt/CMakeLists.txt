set(RAPID_SYSTEM_QT_PUBLIC_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/EventLoopIntegration.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/RapidApplication.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Logger.hpp
)

if(NOT ENABLE_ANDROID)
    set(RAPID_SYSTEM_QT_PUBLIC_HEADERS
        ${CMAKE_CURRENT_SOURCE_DIR}/RapidQApplication.hpp
    )

    target_sources(RapidLibraryQt
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/RapidQApplication.cpp
    )
endif()

install(FILES ${RAPID_SYSTEM_QT_PUBLIC_HEADERS} DESTINATION "${INCLUDE_INSTALL_DIR}/system/qt")

target_sources(RapidLibraryQt
    PRIVATE
        ${RAPID_SYSTEM_QT_PUBLIC_HEADERS}
        ${CMAKE_CURRENT_SOURCE_DIR}/EventLoopIntegration.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/RapidApplication.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Logger.cpp
)
