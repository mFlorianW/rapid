add_subdirectory(SessionManager)
add_executable(rapid_session_manager)

target_sources(rapid_session_manager
PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/rapid_session_manager.cpp
)

target_link_libraries(rapid_session_manager
PRIVATE
    Rapid::RapidQt
    Rapid::SessionManager
)

set(RAPID_SESSION_MANAGER_DATADIR ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_DATAROOTDIR}/rapid)
set(TEST ${CMAKE_INSTALL_FULL_BINDIR})
set(ICON_URL ${RAPID_SESSION_MANAGER_DATADIR})
set(APP_CONFIG ${CMAKE_CURRENT_BINARY_DIR}/rapid_session_manager.json)
configure_file(rapid_session_manager.json.in ${APP_CONFIG})

install(TARGETS rapid_session_manager DESTINATION ${BINDIR})
install(FILES ${APP_CONFIG} DESTINATION ${RAPID_SESSION_MANAGER_DATADIR})
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/rapid_session_manager.svg DESTINATION ${RAPID_SESSION_MANAGER_DATADIR})
