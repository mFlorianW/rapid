add_library(Rest STATIC)
add_library(Rapid::Rest ALIAS Rest)

target_sources(Rest
PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/GpsEndpoint.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/GpsEndpoint.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/RestRequest.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/RestRequest.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/IRestRequestHandler.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/IRestServer.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/SessionEndpoint.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/SessionEndpoint.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Path.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Path.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/RestCall.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/RestCall.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/IRestClient.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/RestServer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/RestServer.hpp
)

if(ENABLE_BOOST)
    target_sources(Rest
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/private/RestServerImpl.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/private/RestServerImpl.hpp
    )
endif()

target_link_libraries(Rest
PRIVATE
    spdlog::spdlog
PUBLIC
    KDAB::KDBindings
    Rapid::Positioning
    Rapid::Storage
)

target_include_directories(Rest
PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
)

if(ENABLE_QT)
    add_subdirectory(qt)
endif()


