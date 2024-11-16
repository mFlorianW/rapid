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

add_executable(test_storage)
add_dependencies(test_storage test_session_db)

target_sources(test_storage
PRIVATE
    test_SessionDatabase.cpp
    test_FileSystemSessionDatabaseBackend.cpp
)

target_link_libraries(test_storage
PRIVATE
    Catch2::Catch2WithMain
    Rapid::Storage
    Rapid::TestHelper
)
catch_discover_tests(test_storage)

add_executable(test_storage_sqlitesession_database)
add_dependencies(test_storage_sqlitesession_database test_session_db)

target_sources(test_storage_sqlitesession_database
PRIVATE
    test_SqliteSessionDatabase.cpp
)
target_link_libraries(test_storage_sqlitesession_database
PRIVATE
    Catch2::Catch2WithMain
    Rapid::Storage
    Rapid::TestHelper
)
catch_discover_tests(test_storage_sqlitesession_database)

add_executable(test_storage_sqlitetrack_database)
add_dependencies(test_storage_sqlitetrack_database test_trackmangement_db)

target_sources(test_storage_sqlitetrack_database
PRIVATE
    test_SqliteTrackDatabase.cpp
)
target_link_libraries(test_storage_sqlitetrack_database
PRIVATE
    Catch2::Catch2WithMain
    Rapid::Storage
    Rapid::TestHelper
)
catch_discover_tests(test_storage_sqlitetrack_database)
