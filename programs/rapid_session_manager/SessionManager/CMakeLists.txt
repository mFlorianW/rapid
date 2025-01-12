qt_wrap_ui(UI
    SessionManager.ui
)

add_library(SessionManager STATIC)
add_library(Rapid::SessionManager ALIAS SessionManager)

target_sources(SessionManager
    PRIVATE
        ${UI}
        SessionManager.cpp
        SessionManager.hpp
)

target_link_libraries(SessionManager
    PUBLIC
        Qt6::Widgets
        Rapid::RapidQt
)

target_include_directories(SessionManager
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
)
