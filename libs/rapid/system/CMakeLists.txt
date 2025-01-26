set(RAPID_SYSTEM_PUBLIC_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/Timer.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/AsyncResult.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/FutureWatcher.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Event.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/EventHandler.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/FdNotifier.hpp
)
install(FILES ${RAPID_SYSTEM_PUBLIC_HEADERS} DESTINATION "${INCLUDE_INSTALL_DIR}/system")

if(UNIX)
target_sources(RapidLibrary
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/private/linux/Timer.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/private/linux/Timer.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/private/linux/FdNotifierImpl.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/private/linux/FdNotifierImpl.hpp
)
endif()

target_sources(RapidLibrary
    PRIVATE
        ${RAPID_SYSTEM_PUBLIC_HEADERS}
        ${RAPID_SYSTEM_PRIVATE_HEADERS}
        ${CMAKE_CURRENT_SOURCE_DIR}/Timer.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/AsyncResult.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Event.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/EventLoop.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/EventLoop.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/EventHandler.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/FdNotifier.cpp
)

if(ENABLE_DESKTOP OR ENABLE_ANDROID)
    add_subdirectory(qt)
endif()
