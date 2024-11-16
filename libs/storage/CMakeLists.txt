add_library(Storage STATIC)
add_library(Rapid::Storage ALIAS Storage)

target_sources(Storage
PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/private/Connection.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/private/Connection.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/private/AsyncResultDb.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/private/AsyncResultDb.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/private/Statement.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/private/Statement.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/FileSystemSessionDatabaseBackend.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/FileSystemSessionDatabaseBackend.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ISessionDatabase.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ISessionDatabaseBackend.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ITrackDatabase.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/SessionDatabase.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/SessionDatabase.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/SqliteSessionDatabase.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/SqliteSessionDatabase.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/SqliteTrackDatabase.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/SqliteTrackDatabase.hpp
)

target_link_libraries(Storage
PRIVATE
    spdlog::spdlog
PUBLIC
    SQLite::SQLite3
    KDAB::KDBindings
    Rapid::System
    Rapid::Common
)

target_include_directories(Storage
PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)
