cmake_minimum_required(VERSION 3.18)
set(CMAKE_POLICY_VERSION_MINIMUM 3.18)

# Set the project name
project(algoConPokemon LANGUAGES CXX)

################################################################################
#####                                                                      #####
##### Build options                                                        #####
#####                                                                      #####
################################################################################

# Build type: Debug or Release
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type (Debug or Release)")
endif()

# Build third party libraries
option(BUILD_THIRD_PARTY "Build third party libraries" ON)

# Build SDL utils library
option(BUILD_SDL_UTILS "Build sdl_utils library" ON)

# Build pk_high_low project
option(BUILD_PK_HIGH_LOW "Build pk_high_low project" ON)

# Build tests
option(BUILD_TESTING "Build tests" ON)


# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Set source, lib and bin directories
# Note: We probably won't use the lib directory, but it's like good practice
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

# Set testing
if(BUILD_TESTING)
    enable_testing()
endif()

# Subdirectories
if (BUILD_THIRD_PARTY)
    add_subdirectory(third_party)
endif()

if (BUILD_SDL_UTILS)
    add_subdirectory(sdl_utils)
endif()

if (BUILD_PK_HIGH_LOW)
    add_subdirectory(pk_high_low)
endif()