#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "PIPC" for configuration "Debug"
set_property(TARGET PIPC APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(PIPC PROPERTIES
  IMPORTED_IMPLIB_DEBUG "${_IMPORT_PREFIX}/lib/libPIPC.dll.a"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/bin/libPIPC.dll"
  )

list(APPEND _cmake_import_check_targets PIPC )
list(APPEND _cmake_import_check_files_for_PIPC "${_IMPORT_PREFIX}/lib/libPIPC.dll.a" "${_IMPORT_PREFIX}/bin/libPIPC.dll" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
