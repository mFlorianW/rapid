qt_add_qml_module(RapidAndroid
    URI "Rapid.Android"
    OUTPUT_DIRECTORY "Rapid/Android"
    IMPORT_PATH "Rapid/Android"
    STATIC
    QML_FILES
        qml/RapidAndroid.qml
        # Elements
        qml/elements/MenuList.qml
        # pages
        qml/pages/ActiveSessionPage.qml
        qml/pages/MainPage.qml
        qml/pages/Laptimer.qml
        qml/pages/LaptimerSessionPage.qml
        qml/pages/GpsPage.qml
        # controls
        qml/controls/ListItem.qml
        qml/controls/Header.qml
        qml/controls/Session.qml
        qml/controls/ContextMenu.qml
    SOURCES
        ActiveSessionPageModel.cpp
        ActiveSessionPageModel.hpp
        RapidAndroidModel.cpp
        RapidAndroidModel.hpp
        Database.cpp
        Database.hpp
        SessionPageModel.cpp
        SessionPageModel.hpp
        LaptimerSessionPageModel.cpp
        LaptimerSessionPageModel.hpp
        ContextMenuModel.cpp
        ContextMenuModel.hpp
        GpsPageModel.cpp
        GpsPageModel.hpp
    RESOURCES
        img/BurgerButton.svg
        img/ContextMenu.svg
        img/Download.svg
        img/Edit.svg
        img/Refresh.svg
        img/Stopwatch.svg
        img/Trash.svg
        img/Pause.svg
        img/Play.svg
        img/Wifi-High.svg
        img/Wifi-Low.svg
        img/Wifi-None.svg
)

add_library(Rapid::Android ALIAS RapidAndroid)
add_library(Rapid::AndroidPlugin ALIAS RapidAndroidplugin)

target_link_libraries(RapidAndroid
    PUBLIC
        Qt6::Quick
        Rapid::RapidQt
)

if(NOT ENABLE_ANDROID)
    target_link_libraries(RapidAndroid
        PRIVATE
            Rapid::DatabaseFile
    )
endif()

target_include_directories(RapidAndroid
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)
