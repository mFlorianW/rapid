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
        qml/pages/MainPage.qml
        qml/pages/Laptimer.qml
        # controls
        qml/controls/ListItem.qml
        qml/controls/Header.qml
        qml/controls/Session.qml
    SOURCES
        GlobalState.cpp
        GlobalState.hpp
        Database.cpp
        Database.hpp
    RESOURCES
        img/BurgerButton.svg
        img/Stopwatch.svg
        img/ContextMenu.svg
        img/Edit.svg
        img/Trash.svg
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
