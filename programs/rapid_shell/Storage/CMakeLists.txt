add_library(ShellStorage OBJECT)
add_library(Rapid::ShellStorage ALIAS ShellStorage)

target_sources(ShellStorage
PRIVATE
    ${GENERATED_HEADER}
    ${CMAKE_CURRENT_SOURCE_DIR}/SessionDatabaseIpcServer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/SessionDatabaseIpcServer.hpp
)

target_link_libraries(ShellStorage
PUBLIC
    Qt6::Core
    Qt6::DBus
    spdlog::spdlog
    Rapid::Rapid
)

target_include_directories(ShellStorage
PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
)
