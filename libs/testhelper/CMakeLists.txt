if(UNIX)
    # Make sure that we can start inside test a pgrep process
    find_program(PGREP pgrep REQUIRED)
endif()

add_library(TestHelper OBJECT)
add_library(Rapid::TestHelper ALIAS TestHelper)

target_sources(TestHelper
PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/Tracks.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Tracks.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Positions.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Positions.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Sessions.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Sessions.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/SqliteDatabaseTestHelper.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/SqliteDatabaseTestHelper.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Laptimer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Laptimer.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/PositionDateTimeProvider.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/CompareHelper.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/SessionDatabaseMock.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/SignalSpy.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/RestClientMock.hpp
)

target_include_directories(TestHelper
PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
)

target_link_libraries(TestHelper
PUBLIC
    Rapid::Rapid
    Catch2::Catch2
    trompeloeil::trompeloeil
)

if(ENABLE_QT)
    add_subdirectory(qt)
endif()
