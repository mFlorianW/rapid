set(RAPID_WORKFLOW_PUBLIC_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/ITrackDetectionWorkflow.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/TrackDetectionWorkflow.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/IActiveSessionWorkflow.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ActiveSessionWorkflow.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ISessionDownloader.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/RestSessionDownloader.hpp
)
install(FILES ${RAPID_WORKFLOW_PUBLIC_HEADERS} DESTINATION "${INCLUDE_INSTALL_DIR}/workflow")

target_sources(RapidLibrary
PRIVATE
    ${RAPID_WORKFLOW_PUBLIC_HEADERS}
    ${CMAKE_CURRENT_SOURCE_DIR}/TrackDetectionWorkflow.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ActiveSessionWorkflow.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/RestSessionDownloader.cpp
)

if(ENABLE_QT)
    add_subdirectory(qt)
endif()
