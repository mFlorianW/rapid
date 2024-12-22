# Create empty database on every build
add_custom_command(OUTPUT rapid.db
    COMMAND rm -f ${CMAKE_CURRENT_BINARY_DIR}/rapid.db && sqlite3 rapid.db < "${CMAKE_CURRENT_SOURCE_DIR}/schema.sql"
    COMMAND sqlite3 "${CMAKE_CURRENT_BINARY_DIR}/rapid.db" < "${CMAKE_CURRENT_SOURCE_DIR}/track.sql"
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/schema.sql
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/track.sql
    VERBATIM)

add_custom_target(db ALL DEPENDS rapid.db)

set(DATABASE_FILE ${CMAKE_INSTALL_PREFIX}/${SHARED_DATADIR}/rapid.db)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/DatabaseFile.hpp.in ${CMAKE_CURRENT_BINARY_DIR}/DatabaseFile.hpp @ONLY)

add_library(DatabaseFile INTERFACE)
add_library(Rapid::DatabaseFile ALIAS DatabaseFile)
add_dependencies(DatabaseFile db)

target_include_directories(DatabaseFile
INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/rapid.db DESTINATION ${SHARED_DATADIR} RENAME rapid.db)
