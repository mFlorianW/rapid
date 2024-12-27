add_library(CommonObjectQt OBJECT)
add_library(Rapid::CommonObjectQt ALIAS CommonObjectQt)

target_sources(CommonObjectQt
PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/GlobalSettingsBackend.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/GlobalSettingsBackend.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/GlobalSettingsReader.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/GlobalSettingsReader.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/GlobalSettingsTypes.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/TableModelDataProvider.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/GenericTableModel.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/LapDataProvider.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/LapDataProvider.hpp
)

target_link_libraries(CommonObjectQt
PUBLIC
    Qt6::Core
    Qt6::Network
    Rapid::Common
)

target_include_directories(CommonObjectQt
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../..>
)

add_library(CommonQt STATIC)
add_library(Rapid::CommonQt ALIAS CommonQt)

target_link_libraries(CommonQt
PUBLIC
    Rapid::CommonObjectQt
)
