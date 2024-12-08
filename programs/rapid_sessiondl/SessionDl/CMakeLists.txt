qt_add_qml_module(SessionDl
    URI "Rapid.SessionDownloader"
    OUTPUT_DIRECTORY "Rapid/SessionDownloader"
    IMPORT_PATH "Rapid/SessionDownloader"
    VERSION 1.0
    STATIC
    QML_FILES
        qml/MainWindow.qml
    SOURCES
        CliOptions.cpp
        CliOptions.hpp
        MainWindowModel.cpp
        MainWindowModel.hpp
        SessionDownloader.cpp
        SessionDownloader.hpp
)

add_library(Rapid::LappySessionDl ALIAS SessionDl)
add_library(Rapid::LappySessionDlPlugin ALIAS SessionDlplugin)

target_link_libraries(SessionDl
    PUBLIC
        Qt6::Core
        Qt6::Gui
        Qt6::Quick
        Qt6::Network
        Qt6::Widgets
        spdlog::spdlog
        Rapid::RestQt
        Rapid::SystemQt
        Rapid::Workflow
        Rapid::StorageQt
    PRIVATE
        Rapid::CommonQt
        Rapid::SessionDlCommon
)

target_include_directories(SessionDl
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)
