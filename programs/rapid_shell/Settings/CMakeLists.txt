qt_wrap_ui(UI
    GlobalSettingsWindow.ui
    GeneralSettingsPage.ui
    DevicePage.ui
)

qt_add_qml_module(Settings
    URI "Rapid.Settings"
    OUTPUT_DIRECTORY "Rapid/Settings"
    IMPORT_PATH "Rapid/Settings"
    VERSION 1.0
    STATIC
    SOURCES
        ${UI}
        GlobalSettingsWindow.cpp
        GlobalSettingsWindow.hpp
        DevicePage.cpp
        DevicePage.hpp
        GeneralSettingsPage.cpp
        GeneralSettingsPage.hpp
        DeviceModel.cpp
        DeviceModel.hpp
    RESOURCES
        icons/settings.svg
        icons/device.svg
)

add_library(Rapid::ShellSettings ALIAS Settings)
add_library(Rapid::ShellSettingsPlugin ALIAS Settingsplugin)

target_link_libraries(Settings
    PUBLIC
        Qt6::Widgets
        Rapid::RapidQt
    PRIVATE
        Rapid::DatabaseFile
        Rapid::ShellCommon
)

target_include_directories(Settings
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
)
