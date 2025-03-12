set(DB_FILE ${CMAKE_CURRENT_BINARY_DIR}/rapid.db CACHE STRING "Clean Database File")
# Create empty database on first CMake configure run
if(NOT EXISTS ${DB_FILE})
    execute_process(COMMAND touch "${CMAKE_CURRENT_BINARY_DIR}/rapid.db")
endif()

# Create empty database on every build
add_custom_command(OUTPUT rapid.db
    COMMAND rm -rf rapid.db
    COMMAND sqlite3 rapid.db < "${CMAKE_CURRENT_SOURCE_DIR}/schema.sql"
    COMMAND sqlite3 "${CMAKE_CURRENT_BINARY_DIR}/rapid.db" < "${CMAKE_CURRENT_SOURCE_DIR}/track.sql"
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/schema.sql
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/track.sql
    VERBATIM
)
add_custom_target(db ALL DEPENDS rapid.db)

if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/tracks-custom.sql)
    add_custom_command(TARGET db
        POST_BUILD
        COMMAND sqlite3 "${CMAKE_CURRENT_BINARY_DIR}/rapid.db" < "${CMAKE_CURRENT_SOURCE_DIR}/tracks-custom.sql"
    )
endif()

set(DATABASE_FILE ${CMAKE_INSTALL_PREFIX}/${SHARED_DATADIR}/rapid.db)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/DatabaseFile.hpp.in ${CMAKE_CURRENT_BINARY_DIR}/DatabaseFile.hpp @ONLY)

add_library(DatabaseFile INTERFACE)
add_library(Rapid::DatabaseFile ALIAS DatabaseFile)
add_dependencies(DatabaseFile db)

target_include_directories(DatabaseFile
INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
)

install(FILES ${DB_FILE} DESTINATION ${SHARED_DATADIR} RENAME rapid.db)
