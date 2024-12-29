qt_wrap_ui(UI
    SessionAnalyzer.ui
    SessionSelectionDialog.ui
)

qt_add_qml_module(SessionAnalyzer
    URI "Rapid.SessionAnalyzer"
    OUTPUT_DIRECTORY "Rapid/SessionAnalyzer"
    IMPORT_PATH "Rapid/SessionAnalyzer"
    VERSION 1.0
    STATIC
    SOURCES
        ${UI}
        SessionAnalyzer.cpp
        SessionAnalyzer.hpp
        SessionSelectionDialog.cpp
        SessionSelectionDialog.hpp

)

add_library(Rapid::SessionAnalyzer ALIAS SessionAnalyzer)
add_library(Rapid::SessionAnalyzerPlugin ALIAS SessionAnalyzerplugin)

target_link_libraries(SessionAnalyzer
    PUBLIC
        Qt6::Widgets
        Qt6::Quick
        spdlog::spdlog
        Rapid::RapidQt
)

target_include_directories(SessionAnalyzer
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
)
