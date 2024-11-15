cmake_minimum_required(VERSION 3.15)

# Set project name and language
project(P2PVideoStreaming 
    VERSION 1.0.0 
    DESCRIPTION "Peer to Peer Video Streaming Application"
    LANGUAGES CXX
)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Enable debug symbols
set(CMAKE_BUILD_TYPE Debug)

# Set vcpkg toolchain file if not already set
if(DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
endif()

# Find required packages
find_package(Poco REQUIRED COMPONENTS Foundation Net NetSSL Util)
find_package(OpenCV REQUIRED)
find_package(fmt REQUIRED)

# Collect all source files
file(GLOB_RECURSE SOURCES 
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

# Create executable
add_executable(${PROJECT_NAME} 
    ${SOURCES}
)

# Add include directories
target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${OpenCV_INCLUDE_DIRS}
)

# Link libraries
target_link_libraries(${PROJECT_NAME}
    PRIVATE
        Poco::Foundation
        Poco::Net
        Poco::NetSSL
        Poco::Util
        ${OpenCV_LIBS}
        fmt::fmt
)

# Set output directories
set_target_properties(${PROJECT_NAME}
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
)

# Add compile options
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Install configuration
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

# Enable testing
enable_testing()

# Add subdirectories if you have tests
# add_subdirectory(tests)

# Print configuration summary
message(STATUS "")
message(STATUS "Project configuration summary:")
message(STATUS "  Project name:        ${PROJECT_NAME}")
message(STATUS "  Project version:     ${PROJECT_VERSION}")
message(STATUS "  C++ Standard:        ${CMAKE_CXX_STANDARD}")
message(STATUS "  Build type:          ${CMAKE_BUILD_TYPE}")
message(STATUS "  OpenCV version:      ${OpenCV_VERSION}")
message(STATUS "  Poco version:        ${Poco_VERSION}")
message(STATUS "  Install prefix:      ${CMAKE_INSTALL_PREFIX}")
message(STATUS "")