qt_wrap_ui(UI
    MainWindow.ui
)

qt_add_qml_module(Rapid
    URI "Rapid.Shell"
    OUTPUT_DIRECTORY "Rapid/Shell"
    IMPORT_PATH "Rapid/Shell"
    VERSION 1.0
    STATIC
    SOURCES
        ${UI}
        RapidShell.cpp
        RapidShell.hpp
)

add_library(Rapid::Shell ALIAS Rapid)
add_library(Rapid::ShellPlugin ALIAS Rapidplugin)

target_link_libraries(Rapid
    PUBLIC
        Qt6::Widgets
        Rapid::ShellSettings
        Rapid::ShellSettingsPlugin
        Rapid::ShellStorage
    PRIVATE
        Rapid::DatabaseFile
        Rapid::ShellCommon
        Rapid::ShellApplications
        Rapid::ShellApplicationsPlugin
        Rapid::RapidQt
)

target_include_directories(Rapid
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
)
