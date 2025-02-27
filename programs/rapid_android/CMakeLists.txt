add_subdirectory(RapidAndroid)

qt_add_executable(rapid_android)

target_sources(rapid_android
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/rapid_android.cpp
)

target_link_libraries(rapid_android
    PRIVATE
        spdlog::spdlog
        Qt6::QuickControls2
        Rapid::Rapid
        Rapid::RapidQt
        Rapid::Android
        Rapid::AndroidPlugin
)

add_dependencies(rapid_android db)

set_source_files_properties(${DB_FILE} PROPERTIES QT_RESOURCE_ALIAS "rapid.db")

qt_add_resources(rapid_android "rapid"
    PREFIX "rapid_android"
    FILES ${DB_FILE}
)

if(ENABLE_ANDROID)
    set_property(TARGET rapid_android PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
              "${CMAKE_CURRENT_SOURCE_DIR}/templates")
endif()
