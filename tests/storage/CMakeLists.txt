include(Catch)
# Create empty database on every build
add_custom_command(OUTPUT test_trackmanagement.db
    COMMAND rm -f ${CMAKE_CURRENT_BINARY_DIR}/test_trackmanagement.db && sqlite3 test_trackmanagement.db < "${CMAKE_SOURCE_DIR}/db/schema.sql"
    COMMAND sqlite3 "${CMAKE_CURRENT_BINARY_DIR}/test_trackmanagement.db" < "${CMAKE_CURRENT_SOURCE_DIR}/test_tracks.sql"
    DEPENDS ${CMAKE_SOURCE_DIR}/db/schema.sql
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/test_tracks.sql
VERBATIM)
add_custom_target(test_trackmangement_db ALL DEPENDS test_trackmanagement.db)

# Create empty database on every build
add_custom_command(OUTPUT test_session.db
    COMMAND rm -f ${CMAKE_CURRENT_BINARY_DIR}/test_session.db && sqlite3 test_session.db < "${CMAKE_SOURCE_DIR}/db/schema.sql"
    COMMAND sqlite3 "${CMAKE_CURRENT_BINARY_DIR}/test_session.db" < "${CMAKE_CURRENT_SOURCE_DIR}/test_session.sql"
    DEPENDS ${CMAKE_SOURCE_DIR}/db/schema.sql
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/test_session.sql
VERBATIM)
add_custom_target(test_session_db ALL DEPENDS test_session.db)

add_executable(test_storage_sqlitesession_database)
add_dependencies(test_storage_sqlitesession_database test_session_db)

target_sources(test_storage_sqlitesession_database
PRIVATE
    test_SqliteSessionDatabase.cpp
)
target_link_libraries(test_storage_sqlitesession_database
PRIVATE
    Catch2::Catch2WithMain
    spdlog::spdlog
    SQLite::SQLite3
    Rapid::Rapid
    Rapid::TestHelper
)
catch_discover_tests(test_storage_sqlitesession_database
    DISCOVERY_MODE PRE_TEST
)

add_executable(test_storage_sqlitetrack_database)
add_dependencies(test_storage_sqlitetrack_database test_trackmangement_db)

target_sources(test_storage_sqlitetrack_database
PRIVATE
    test_SqliteTrackDatabase.cpp
)
target_link_libraries(test_storage_sqlitetrack_database
PRIVATE
    Catch2::Catch2WithMain
    spdlog::spdlog
    SQLite::SQLite3
    Rapid::Rapid
    Rapid::TestHelper
)
catch_discover_tests(test_storage_sqlitetrack_database
    DISCOVERY_MODE PRE_TEST
)

if(ENABLE_DESKTOP)
    qt_add_dbus_adaptor(SESSION_DATABASE_ADAPTOR
        ${CMAKE_SOURCE_DIR}/libs/rapid/storage/qt/SessionDatabase.xml
        SessionDatabaseServer.hpp
        SessionDatabaseServer
        SessionDatabaseAdaptor
    )

    add_executable(test_storage_qt)
    target_sources(test_storage_qt
    PRIVATE
        ${SESSION_DATABASE_ADAPTOR}
        SessionDatabaseServer.hpp
        test_SessionDatabaseIpcClient.cpp
        test_SessionMetaDataProvider.cpp
    )
    target_link_libraries(test_storage_qt
    PRIVATE
        Catch2::Catch2
        Qt6::Test
        spdlog::spdlog
        Rapid::RapidQt
        Rapid::TestHelper
        Rapid::TestHelperQt
    )
    target_include_directories(test_storage_qt
    PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    )
    catch_discover_tests(test_storage_qt
        DISCOVERY_MODE PRE_TEST
    )
endif()
