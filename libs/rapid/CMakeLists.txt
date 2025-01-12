#################################################################################
# Define Rapid Library
#################################################################################
add_library(RapidLibrary SHARED)
add_library(Rapid::Rapid ALIAS RapidLibrary)

target_include_directories(RapidLibrary
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

target_link_libraries(RapidLibrary
PRIVATE
    spdlog::spdlog
    nlohmann_json::nlohmann_json
    Rapid::ThirdParty::UTM
    SQLite::SQLite3
PUBLIC
    KDAB::KDBindings
)

set(INCLUDE_INSTALL_DIR ${CMAKE_INSTALL_INCLUDEDIR}/Rapid CACHE PATH "Location of rapid header files" )

configure_package_config_file(
    RapidConfig.cmake.in
    "${CMAKE_CURRENT_BINARY_DIR}/RapidConfig.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/Rapid
    PATH_VARS INCLUDE_INSTALL_DIR
)

write_basic_package_version_file(
    ${PROJECT_NAME}Version.cmake
    VERSION "${PROJECT_VERSION}"
    COMPATIBILITY SameMinorVersion
)

set_target_properties(RapidLibrary PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
)

install(TARGETS RapidLibrary EXPORT RapidLibraryTargets
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    COMPONENT RapidLibrary_Runtime
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    COMPONENT RapidLibrary_Development
    NAMELINK_COMPONENT RapidLibrary_Development
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/RapidConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/RapidVersion.cmake"
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/Rapid"
)

if(ENABLE_QT)
    #################################################################################
    # Define Generated DBus Interfaces
    #################################################################################
    qt_add_dbus_interface(SESSION_DBUS_INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/storage/qt/SessionDatabase.xml
        SessionDatabaseIpcInterface
    )

    #################################################################################
    # Define Rapid Library Qt
    #################################################################################
    add_library(RapidLibraryQt SHARED)
    add_library(Rapid::RapidQt ALIAS RapidLibraryQt)

    target_sources(RapidLibraryQt
        PRIVATE
            ${SESSION_DBUS_INTERFACE}
    )

    target_include_directories(RapidLibraryQt
        PRIVATE
            $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
        PUBLIC
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    )

    target_link_libraries(RapidLibraryQt
        PRIVATE
            spdlog::spdlog
        PUBLIC
            Rapid::Rapid
            Qt6::Core
            Qt6::Network
            Qt6::Gui
            Qt6::GuiPrivate
            Qt6::Widgets
            Qt6::DBus
    )
    target_compile_definitions(RapidLibraryQt PRIVATE QT_NO_KEYWORDS)

    set_target_properties(RapidLibraryQt PROPERTIES
        VERSION ${PROJECT_VERSION}
        SOVERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    )

    configure_package_config_file(
        RapidQtConfig.cmake.in
        "${CMAKE_CURRENT_BINARY_DIR}/RapidQtConfig.cmake"
        INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/RapidQt
        PATH_VARS INCLUDE_INSTALL_DIR
    )

    write_basic_package_version_file(
        RapidQtVersion.cmake
        VERSION "${PROJECT_VERSION}"
        COMPATIBILITY SameMinorVersion
    )

    install(TARGETS RapidLibraryQt EXPORT RapidLibraryQtTargets
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        COMPONENT RapidLibraryQt_Runtime
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        COMPONENT RapidLibraryQt_Development
        NAMELINK_COMPONENT RapidLibraryQt_Development
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )

    install(FILES
        "${CMAKE_CURRENT_BINARY_DIR}/RapidQtConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/RapidQtVersion.cmake"
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/RapidQt"
    )
endif(ENABLE_QT)

#################################################################################
# Include Sources and Header Install Targets
#################################################################################
add_subdirectory(algorithm)
add_subdirectory(common)
add_subdirectory(positioning)
add_subdirectory(workflow)
add_subdirectory(rest)
add_subdirectory(system)
add_subdirectory(storage)
