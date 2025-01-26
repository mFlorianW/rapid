if(ENABLE_DESKTOP)
    set(RAPID_STORAGE_QT_PUBLIC_HEADERS
        ${CMAKE_CURRENT_SOURCE_DIR}/SessionDatabaseIpcClient.hpp
    )

    target_sources(RapidLibraryQt
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/SessionDatabaseIpcClient.cpp
    )
endif()

set(RAPID_STORAGE_QT_PUBLIC_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/SessionMetaDataProvider.hpp
)
install(FILES ${RAPID_COMMON_QT_PUBLIC_HEADERS} DESTINATION "${INCLUDE_INSTALL_DIR}/storage/qt")

target_sources(RapidLibraryQt
PRIVATE
    ${RAPID_STORAGE_QT_PUBLIC_HEADERS}
    ${CMAKE_CURRENT_SOURCE_DIR}/SessionMetaDataProvider.cpp
)
