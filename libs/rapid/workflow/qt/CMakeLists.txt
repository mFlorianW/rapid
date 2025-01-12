set(RAPID_WORKFLOW_QT_PUBLIC_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/ISessionAnalyzeWorkflow.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/SessionAnalyzeWorkflow.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/RestSessionManagementWorkflow.hpp
)
install(FILES ${RAPID_WORKFLOW_QT_PUBLIC_HEADERS} DESTINATION "${INCLUDE_INSTALL_DIR}/workflow/qt")

target_sources(RapidLibraryQt
PRIVATE
    ${RAPID_WORKFLOW_QT_PUBLIC_HEADERS}
    ${CMAKE_CURRENT_SOURCE_DIR}/SessionAnalyzeWorkflow.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/RestSessionManagementWorkflow.cpp
)
