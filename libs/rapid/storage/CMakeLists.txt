set(RAPID_STORAGE_PUBLIC_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/ISessionDatabase.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ITrackDatabase.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/SqliteSessionDatabase.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/SqliteTrackDatabase.hpp
)
install(FILES ${RAPID_COMMON_PUBLIC_HEADERS} DESTINATION "${INCLUDE_INSTALL_DIR}/storage")

set(RAPID_STORAGE_PRIVATE_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/private/Connection.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/private/Statement.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/private/StorageContext.hpp
)
install(FILES ${RAPID_COMMON_PUBLIC_HEADERS} DESTINATION "${INCLUDE_INSTALL_DIR}/storage/private")

target_sources(RapidLibrary
PRIVATE
    ${RAPID_STORAGE_PRIVATE_HEADERS}
    ${RAPID_STORAGE_PUBLIC_HEADERS}
    ${CMAKE_CURRENT_SOURCE_DIR}/private/Connection.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/private/Statement.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/SqliteSessionDatabase.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/SqliteTrackDatabase.cpp
)

if(ENABLE_QT)
    add_subdirectory(qt)
endif()
