add_library(PositioningQt STATIC)
add_library(Rapid::PositioningQt ALIAS PositioningQt)

target_sources(PositioningQt
PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/CsvGpsFileReader.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/CsvGpsFileReader.hpp
)

target_include_directories(PositioningQt
PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
)

target_link_libraries(PositioningQt
PUBLIC
    Rapid::Positioning
    Qt6::Core
)

