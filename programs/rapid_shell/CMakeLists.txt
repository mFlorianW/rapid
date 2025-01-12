add_subdirectory(Common)
add_subdirectory(Shell)
add_subdirectory(Applications)
add_subdirectory(Settings)
add_subdirectory(Storage)

add_executable(rapid_shell)

target_sources(rapid_shell
PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/rapid_shell.cpp
)

target_link_libraries(rapid_shell
PRIVATE
    Qt6::Widgets
    Rapid::ShellApplications
    Rapid::ShellApplicationsPlugin
    Rapid::Shell
    Rapid::ShellPlugin
    Rapid::RapidQt
)

install(TARGETS rapid_shell DESTINATION ${CMAKE_INSTALL_BINDIR})
