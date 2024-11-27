add_subdirectory(Common)
add_subdirectory(SessionDl)
add_executable(rapid_sessiondl)

target_sources(rapid_sessiondl
PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/rapid_sessiondl.cpp
)

target_link_libraries(rapid_sessiondl
PRIVATE
    Rapid::LappySessionDl
    Rapid::LappySessionDlPlugin
)

set(LAPPY_SESSION_DL_DATADIR ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_DATAROOTDIR}/rapid)
set(TEST ${CMAKE_INSTALL_FULL_BINDIR})
set(ICON_URL ${LAPPY_SESSION_DL_DATADIR})
set(APP_CONFIG ${CMAKE_CURRENT_BINARY_DIR}/rapid_sessiondl.json)
configure_file(rapid_sessiondl.json.in ${APP_CONFIG})

install(TARGETS rapid_sessiondl DESTINATION ${BINDIR})
install(FILES ${APP_CONFIG} DESTINATION ${LAPPY_SESSION_DL_DATADIR})
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/rapid_sessiondl.svg DESTINATION ${LAPPY_SESSION_DL_DATADIR})
