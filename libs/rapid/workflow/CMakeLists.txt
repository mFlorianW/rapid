set(RAPID_WORKFLOW_PUBLIC_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/ITrackDetectionWorkflow.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/TrackDetectionWorkflow.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/IActiveSessionWorkflow.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ActiveSessionWorkflow.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/IRestSessionManagementWorkflow.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/RestSessionManagementWorkflow.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ActiveSessionEndpoint.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/IRestActiveSession.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/RestActiveSession.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/RestActiveSession.cpp
)
install(FILES ${RAPID_WORKFLOW_PUBLIC_HEADERS} DESTINATION "${INCLUDE_INSTALL_DIR}/workflow")

target_sources(RapidLibrary
PRIVATE
    ${RAPID_WORKFLOW_PUBLIC_HEADERS}
    ${CMAKE_CURRENT_SOURCE_DIR}/TrackDetectionWorkflow.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ActiveSessionWorkflow.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/RestSessionManagementWorkflow.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ActiveSessionEndpoint.cpp
)

if(ENABLE_DESKTOP OR ENABLE_ANDROID)
    add_subdirectory(qt)
endif()
