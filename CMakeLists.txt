###################################################
# Build Soapy SDR support module for Airspy Devices
###################################################
cmake_minimum_required(VERSION 3.3.0)
project(SoapyAirspy CXX)
set(CMAKE_CXX_STANDARD 11)

find_package(SoapySDR "0.4.0" NO_MODULE REQUIRED)
if (NOT SoapySDR_FOUND)
    message(FATAL_ERROR "Soapy SDR development files not found...")
endif ()

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR})

find_package(LibAIRSPY)

include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${LIBAIRSPY_INCLUDE_DIRS})

list(APPEND AIRSPY_LIBS ${LIBAIRSPY_LIBRARIES})

SOAPY_SDR_MODULE_UTIL(
    TARGET airspySupport
    SOURCES
        SoapyAirspy.hpp
        Registration.cpp
        Settings.cpp
        Streaming.cpp
    LIBRARIES
        Threads::Threads
        ${AIRSPY_LIBS}
)
