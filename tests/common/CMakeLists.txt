set(TEST_FILE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/RealWorld.session)
configure_file(TestFile.hpp.in ${CMAKE_CURRENT_BINARY_DIR}/TestFile.hpp)

add_executable(test_common)

target_sources(test_common
PRIVATE
    test_LapData.cpp
    test_Timestamp.cpp
    test_Date.cpp
    test_JsonSerializer.cpp
    test_JsonDeserializer.cpp
    test_GpsPositionData.cpp
    test_LapData.cpp
    test_SharedDataPointer.cpp
    test_VelocityData.cpp
)

target_link_libraries(test_common
PRIVATE
    Catch2::Catch2WithMain
    Rapid::Common
    Rapid::TestHelper
)

target_include_directories(test_common
PRIVATE
    ${CMAKE_CURRENT_BINARY_DIR}
)

if(ENABLE_QT)
    add_executable(test_GlobalSettingsReader test_GlobalSettingsReader.cpp)
    target_link_libraries(test_GlobalSettingsReader
        PRIVATE
            Catch2::Catch2WithMain
            Rapid::CommonQt
            Rapid::TestHelper
    )
    catch_discover_tests(test_GlobalSettingsReader)
endif()

catch_discover_tests(test_common)
