cmake_minimum_required(VERSION 3.12)
project(IPC)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 14)
set(DEFAULT_BUILD_TYPE "Release")
set(SO_VERSION 0.1)
set(CMAKE_INSTALL_LIBDIR /usr/local/lib/ipc/)
set(CMAKE_INSTALL_LIBDIR /usr/local/lib/ipc/inc)

add_library(IPC SHARED
        src/linux/linux_proc_creation.cpp
        src/api/IPC_process_creation.cpp
        src/api/process_creation.cpp
        IPC.h
)

target_include_directories(IPC PUBLIC
       ${CMAKE_CURRENT_SOURCE_DIR}/inc/linux
)

set_target_properties(IPC PROPERTIES VERSION ${SO_VERSION})
set_target_properties(IPC PROPERTIES PUBLIC_HEADER inc/*)

include(GNUInstallDirs)
install(TARGETS IPC
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})