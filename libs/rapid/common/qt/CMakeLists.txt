set(RAPID_COMMON_QT_PUBLIC_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/GlobalSettingsBackend.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/GlobalSettingsReader.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/GlobalSettingsTypes.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/TableModelDataProvider.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/GenericTableModel.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/LapDataProvider.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/SessionMetadataProvider.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/DeviceSettingsProvider.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/GenericListModel.hpp
)
install(FILES ${RAPID_COMMON_QT_PUBLIC_HEADERS} DESTINATION "${INCLUDE_INSTALL_DIR}/common/qt")

set(RAPID_COMMON_QT_PRIVATE_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/private/GlobalSettingsKeys.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/private/GlobalSettingsWriter.hpp
)
install(FILES ${RAPID_COMMON_QT_PRIVATE_HEADERS} DESTINATION "${INCLUDE_INSTALL_DIR}/common/qt/private")

target_sources(RapidLibraryQt
PRIVATE
    ${RAPID_COMMON_QT_PUBLIC_HEADERS}
    ${RAPID_COMMON_QT_PRIVATE_HEADERS}
    ${CMAKE_CURRENT_SOURCE_DIR}/private/GlobalSettingsWriter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/GlobalSettingsBackend.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/GlobalSettingsReader.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/LapDataProvider.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/SessionMetadataProvider.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/DeviceSettingsProvider.cpp
)
