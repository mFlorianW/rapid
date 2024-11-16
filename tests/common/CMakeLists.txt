include(Catch)

add_executable(test_common)

target_sources(test_common
PRIVATE
   test_LapData.cpp
   test_Timestamp.cpp
   test_Date.cpp
   test_JsonSerializer.cpp
   test_JsonDeserializer.cpp
   test_PositionDateTime.cpp
   test_LapData.cpp
   test_SharedDataPointer.cpp
)

target_link_libraries(test_common
PRIVATE
    Catch2::Catch2WithMain
    Rapid::Common
    Rapid::TestHelper
)

catch_discover_tests(test_common)

if(ENABLE_QT)
    add_executable(test_GlobalSettingsReader test_GlobalSettingsReader.cpp)
    target_link_libraries(test_GlobalSettingsReader
        PRIVATE
            Catch2::Catch2WithMain
            Rapid::CommonQt
            Rapid::TestHelper
    )
endif()
