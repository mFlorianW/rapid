set(RAPID_SYSTEM_PUBLIC_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/Timer.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/AsyncResult.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/FutureWatcher.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Event.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/EventHandler.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/FdNotifier.hpp
)
install(FILES ${RAPID_SYSTEM_PUBLIC_HEADERS} DESTINATION "${INCLUDE_INSTALL_DIR}/system")

set(RAPID_SYSTEM_PRIVATE_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/private/BoostTimer.hpp
)
install(FILES ${RAPID_SYSTEM_PRIVATE_HEADERS} DESTINATION "${INCLUDE_INSTALL_DIR}/system/private")

if(UNIX)
target_sources(RapidLibrary
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/private/linux/FdNotifierImpl.cpp
)
set(RAPID_SYSTEM_PRIVATE_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/private/linux/FdNotifierImpl.hpp
)
install(FILES ${RAPID_SYSTEM_PRIVATE_HEADERS} DESTINATION "${INCLUDE_INSTALL_DIR}/system/private/linux")
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

        # Private
        ${CMAKE_CURRENT_SOURCE_DIR}/private/BoostTimer.cpp
)

if(ENABLE_QT)
    add_subdirectory(qt)
endif()
