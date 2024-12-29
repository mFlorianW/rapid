set(RAPID_PUBLIC_ALGORITHM_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/ITrackDetection.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/TrackDetection.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ILaptimer.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/SimpleLaptimer.hpp
)
install(FILES ${RAPID_PUBLIC_ALGORITHM_HEADERS} DESTINATION "${INCLUDE_INSTALL_DIR}/algorithm")

target_sources(RapidLibrary
    PRIVATE
        ${RAPID_PUBLIC_ALGORITHM_HEADERS}
        ${CMAKE_CURRENT_SOURCE_DIR}/TrackDetection.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/DistanceCalculator.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/DistanceCalculator.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/SimpleLaptimer.cpp
)
