set(INSTALL_SEARCH_DIR ${CMAKE_INSTALL_PREFIX}/${SHARED_DATADIR}/)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/ApplicationSearchDir.hpp.in ${CMAKE_CURRENT_BINARY_DIR}/ApplicationSearchDir.hpp @ONLY)

qt_add_qml_module(Applications
    URI "Rapid.Shell.Applications"
    OUTPUT_DIRECTORY "Rapid/Shell/Applications"
    IMPORT_PATH "Rapid/Shell/Applications"
    VERSION 1.0
    STATIC
    QML_FILES
        qml/ApplicationOverview.qml
        qml/controls/ApplicationButton.qml
    SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/ApplicationConfig.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/ApplicationConfig.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/ApplicationConfigReader.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/ApplicationConfigReader.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/ApplicationFinder.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/ApplicationFinder.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/ApplicationListModel.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/ApplicationListModel.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/ApplicationModel.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/ApplicationModel.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/ApplicationOverviewWidget.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/ApplicationOverviewWidget.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/ProcessManager.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/ProcessManager.hpp
)

add_library(Rapid::ShellApplications ALIAS Applications)
add_library(Rapid::ShellApplicationsPlugin ALIAS Applicationsplugin)

target_include_directories(Applications
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

target_link_libraries(Applications
    PRIVATE
        spdlog::spdlog
    PUBLIC
        Rapid::ShellCommon
        Qt6::QuickWidgets
)
