set(RAPID_WORKFLOW_QT_PUBLIC_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/ISessionAnalyzeWorkflow.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/SessionAnalyzeWorkflow.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/RestSessionManagementWorkflow.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/IDeviceManagement.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/DeviceManagement.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/RestActiveSession.hpp
)
install(FILES ${RAPID_WORKFLOW_QT_PUBLIC_HEADERS} DESTINATION "${INCLUDE_INSTALL_DIR}/workflow/qt")

target_sources(RapidLibraryQt
PRIVATE
    ${RAPID_WORKFLOW_QT_PUBLIC_HEADERS}
    ${CMAKE_CURRENT_SOURCE_DIR}/SessionAnalyzeWorkflow.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/RestSessionManagementWorkflow.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/DeviceManagement.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/RestActiveSession.cpp
)
