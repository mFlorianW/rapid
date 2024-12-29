add_library(AlgorithmObject OBJECT)
add_library(Rapid::AlgorithmObject ALIAS AlgorithmObject)

target_sources(AlgorithmObject
PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/TrackDetection.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/DistanceCalculator.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/DistanceCalculator.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/SimpleLaptimer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ITrackDetection.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/TrackDetection.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ILaptimer.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/SimpleLaptimer.hpp
)

target_link_libraries(AlgorithmObject
PUBLIC
    Rapid::Common
    KDAB::KDBindings
)

target_include_directories(AlgorithmObject
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../>
)

add_library(Algorithm STATIC)
add_library(Rapid::Algorithm ALIAS Algorithm)

target_link_libraries(Algorithm
PUBLIC
    Rapid::AlgorithmObject
)
