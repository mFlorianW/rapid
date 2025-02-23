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
    test_SessionMetaData.cpp
)

target_link_libraries(test_common
PRIVATE
    Catch2::Catch2WithMain
    Rapid::Rapid
    Rapid::TestHelper
)

target_include_directories(test_common
PRIVATE
    ${CMAKE_CURRENT_BINARY_DIR}
)

catch_discover_tests(test_common
    DISCOVERY_MODE PRE_TEST
)

if(ENABLE_DESKTOP)
    add_executable(test_common_qt)
    target_sources(test_common_qt
        PRIVATE
            test_common_qt_main.cpp
            test_GlobalSettingsReader.cpp
            test_TableModelDataProvider.cpp
            test_GenericTableModel.cpp
            test_LapDataProvider.cpp
            test_SessionMetaDataProvider.cpp
            test_DeviceSettingsProvider.cpp
            test_qt_GenericListModel.cpp
            test_qt_DeviceSettingsListModel.cpp
            test_qt_SessionMetaDataListModel.cpp
    )
    target_link_libraries(test_common_qt
        PRIVATE
            Catch2::Catch2
            Qt6::Test
            Rapid::RapidQt
            Rapid::TestHelperQt
    )
    catch_discover_tests(test_common_qt
        DISCOVERY_MODE PRE_TEST
    )
endif()
