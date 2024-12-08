qt_add_qml_module(SessionAnalyzer
    URI "Rapid.SessionAnalyzer"
    OUTPUT_DIRECTORY "Rapid/SessionAnalyzer"
    IMPORT_PATH "Rapid/SessionAnalyzer"
    VERSION 1.0
    STATIC
    QML_FILES
        qml/MainWindow.qml
    SOURCES
        SessionAnalyzer.cpp
        SessionAnalyzer.hpp

)

add_library(Rapid::SessionAnalyzer ALIAS SessionAnalyzer)
add_library(Rapid::SessionAnalyzerPlugin ALIAS SessionAnalyzerplugin)

target_link_libraries(SessionAnalyzer
    PUBLIC
        Qt6::Quick
)

target_include_directories(SessionAnalyzer
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)
