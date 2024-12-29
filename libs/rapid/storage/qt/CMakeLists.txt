qt_add_dbus_interface(SESSION_DBUS_INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/SessionDatabase.xml
    SessionDatabaseIpcInterface
)

add_library(StorageQt STATIC)
add_library(Rapid::StorageQt ALIAS StorageQt)

target_sources(StorageQt
PRIVATE
    ${SESSION_DBUS_INTERFACE}
    ${CMAKE_CURRENT_SOURCE_DIR}/SessionDatabaseIpcClient.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/SessionDatabaseIpcClient.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/SessionMetaDataProvider.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/SessionMetaDataProvider.hpp
)

target_link_libraries(StorageQt
PRIVATE
    spdlog::spdlog
PUBLIC
    Qt6::DBus
    Rapid::Storage
    Rapid::CommonQt
)

target_include_directories(StorageQt
    PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../..>
)
