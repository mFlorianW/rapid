add_executable(restgpssource)

qt_add_resources(RESOURCE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/resource.qrc
)
list(APPEND QML_IMPORT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/qml)

target_sources(restgpssource
PRIVATE
    ${RESOURCE_FILES}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/restgpssource.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/RestGpsSource.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/RestGpsSource.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/MainWindowViewModel.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/MainWindowViewModel.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/PositionListModel.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/PositionListModel.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/RestHttpClient.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/RestHttpClient.hpp
)

target_link_libraries(restgpssource
PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Quick
    Qt6::Location
    Rapid::Rapid
    Rapid::RapidQt
)
