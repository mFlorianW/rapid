set(RAPID_STORAGE_QT_PUBLIC_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/SessionMetaDataProvider.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/SessionDatabaseIpcClient.hpp
)
install(FILES ${RAPID_COMMON_QT_PUBLIC_HEADERS} DESTINATION "${INCLUDE_INSTALL_DIR}/storage/qt")

target_sources(RapidLibraryQt
PRIVATE
    ${RAPID_STORAGE_QT_PUBLIC_HEADERS}
    ${CMAKE_CURRENT_SOURCE_DIR}/SessionDatabaseIpcClient.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/SessionMetaDataProvider.cpp
)
