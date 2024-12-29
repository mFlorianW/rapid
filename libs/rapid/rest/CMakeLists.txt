set(RAPID_REST_PUBLIC_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/GpsEndpoint.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/RestRequest.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/IRestRequestHandler.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/IRestServer.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/SessionEndpoint.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/RestCall.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/IRestClient.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/RestServer.hpp
)
install(FILES ${RAPID_REST_PUBLIC_HEADERS} DESTINATION "${INCLUDE_INSTALL_DIR}/rest")

set(RAPID_REST_PRIVATE_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/private/RestServerImpl.hpp
)
install(FILES ${RAPID_REST_PRIVATE_HEADERS} DESTINATION "${INCLUDE_INSTALL_DIR}/rest/private")

target_sources(RapidLibrary
    PRIVATE
        ${RAPID_REST_PUBLIC_HEADERS}
        ${RAPID_REST_PRIVATE_HEADERS}
        ${CMAKE_CURRENT_SOURCE_DIR}/GpsEndpoint.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/RestRequest.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/SessionEndpoint.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Path.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Path.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/RestCall.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/RestServer.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/private/RestServerImpl.cpp
)

if(ENABLE_QT)
    add_subdirectory(qt)
endif()


