cmake_minimum_required(VERSION 3.5)
project(PIPC VERSION 0.1.0 LANGUAGES CXX)

set(DEFAULT_BUILD_TYPE "Release")

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message(STATUS "Setting build type to '${DEFAULT_BUILD_TYPE}' as none was specified.")
endif()

include(GNUInstallDirs)
set(SOURCE_FILES
        src/api/IPC_process_creation.cpp
        src/linux/linux_proc_creation.cpp
        src/api/process_creation.cpp)

add_library(${PROJECT_NAME} ${SOURCE_FILES})

target_include_directories(${PROJECT_NAME} PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/inc/api>
        $<INSTALL_INTERFACE:inc/api>
        PRIVATE inc/linux)

set_target_properties(${PROJECT_NAME} PROPERTIES
        VERSION ${PROJECT_VERSION}
        SOVERSION 0.1)

install(TARGETS ${PROJECT_NAME} EXPORT PIPCConfig
        ARCHIVE  DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY  DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME  DESTINATION ${CMAKE_INSTALL_BINDIR})
install(DIRECTORY inc/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME})

install(EXPORT PIPCConfig DESTINATION share/PIPC/cmake)

export(TARGETS ${PROJECT_NAME} FILE PIPCConfig.cmake)