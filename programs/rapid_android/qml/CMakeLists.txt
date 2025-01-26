qt_add_qml_module(RapidAndroid
    URI "Rapid.Android"
    OUTPUT_DIRECTORY "Rapid/Android"
    IMPORT_PATH "Rapid/Android"
    STATIC
    QML_FILES
        RapidAndroid.qml
)

add_library(Rapid::Android ALIAS RapidAndroid)
add_library(Rapid::AndroidPlugin ALIAS RapidAndroidplugin)

target_link_libraries(RapidAndroid
    PUBLIC
        Qt6::Quick
)
