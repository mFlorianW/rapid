add_library(SystemQt STATIC)
add_library(Rapid::SystemQt ALIAS SystemQt)

target_sources(SystemQt
PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/EventLoopIntegration.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/EventLoopIntegration.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/RapidApplication.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/RapidApplication.hpp
)

target_link_libraries(SystemQt
PUBLIC
    Rapid::System
    Qt6::Gui
    Qt6::GuiPrivate
    Qt6::Widgets
)

target_include_directories(SystemQt
PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../..>
)

target_compile_definitions(SystemQt PRIVATE QT_NO_KEYWORDS)
