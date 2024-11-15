#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "Poco::Prometheus" for configuration "Debug"
set_property(TARGET Poco::Prometheus APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(Poco::Prometheus PROPERTIES
  IMPORTED_IMPLIB_DEBUG "${_IMPORT_PREFIX}/debug/lib/PocoPrometheusd.lib"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/debug/bin/PocoPrometheusd.dll"
  )

list(APPEND _cmake_import_check_targets Poco::Prometheus )
list(APPEND _cmake_import_check_files_for_Poco::Prometheus "${_IMPORT_PREFIX}/debug/lib/PocoPrometheusd.lib" "${_IMPORT_PREFIX}/debug/bin/PocoPrometheusd.dll" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
