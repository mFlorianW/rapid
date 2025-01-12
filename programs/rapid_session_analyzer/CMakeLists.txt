add_subdirectory(SessionAnalyzer)

add_executable(rapid_session_analyzer)

target_sources(rapid_session_analyzer
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/rapid_session_analyzer.cpp
)

target_link_libraries(rapid_session_analyzer
    PRIVATE
        Rapid::RapidQt
        Rapid::SessionAnalyzer
        Rapid::SessionAnalyzerPlugin
)

set(LAPPY_SESSION_DL_DATADIR ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_DATAROOTDIR}/rapid)
set(TEST ${CMAKE_INSTALL_FULL_BINDIR})
set(ICON_URL ${LAPPY_SESSION_DL_DATADIR})
set(APP_CONFIG ${CMAKE_CURRENT_BINARY_DIR}/rapid_session_analyzer.json)
configure_file(rapid_session_analyzer.json.in ${APP_CONFIG})

install(TARGETS rapid_session_analyzer DESTINATION ${BINDIR})
install(FILES ${APP_CONFIG} DESTINATION ${LAPPY_SESSION_DL_DATADIR})
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/rapid_session_analyzer.svg DESTINATION ${LAPPY_SESSION_DL_DATADIR})
