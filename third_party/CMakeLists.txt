# Include FetchContent module dawd
include(FetchContent)
set(FETCHCONTENT_QUIET FALSE) # Print git progress information


# Third-party libraries
find_package(nlohmann_json QUIET)
find_package(GTest QUIET)
find_package(SDL2 QUIET)
find_package(SDL2_image QUIET)
find_package(SDL2_ttf QUIET)

# Include nlohmann json library
if(NOT nlohmann_json_FOUND)
  FetchContent_Declare(
    nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.12.0 # Use a specific version
    GIT_SHALLOW TRUE
    GIT_PROGRESS TRUE
  )
  FetchContent_MakeAvailable(nlohmann_json)
endif()

# Include GTest library
if(NOT GTest_FOUND)
  FetchContent_Declare(
    GTest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.17.0 # Use a specific version
    GIT_SHALLOW TRUE
    GIT_PROGRESS TRUE
  )
  set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
  FetchContent_MakeAvailable(GTest)
endif()

# Find SDL2
if(NOT SDL2_FOUND)
  FetchContent_Declare(
    SDL2
    GIT_REPOSITORY https://github.com/libsdl-org/SDL
    GIT_TAG release-2.32.0 # Use a specific version
    GIT_SHALLOW TRUE
    GIT_PROGRESS TRUE
  )
  FetchContent_MakeAvailable(SDL2)  
endif()

# Find SDL2_image
# ??? See: https://stackoverflow.com/questions/75551185/cmake-using-sdl2-and-sdl2-image-via-fetchcontent
if(NOT SDL2_IMAGE_FOUND)
  FetchContent_Declare(
    SDL2_image
    GIT_REPOSITORY https://github.com/libsdl-org/SDL_image.git
    #GIT_TAG release-2.8.8 # Use a specific version
    #GIT_SHALLOW TRUE
    GIT_TAG 6bfab27263f0390cda797f1815370ba8e96e37cf ## release-2.8.8 fails building due to undeclared variable 'SDL_PIXELFORMAT_RGBX32'. This specific commit fixes it
    GIT_SHALLOW FALSE ## needs to be FALSE to fetch a specific commit
    GIT_PROGRESS TRUE
  )
  set(SDL2IMAGE_AVIF OFF)      # AVIF format requires dav1d
  set(SDL2IMAGE_DAV1D OFF)     # Explicitly disable dav1d
  set(SDL2IMAGE_INSTALL OFF)
  set(BUILD_SHARED_LIBS FALSE)
  set(SDL2IMAGE_VENDORED ON)  # Use bundled libs instead of system ones
  set(SDL2IMAGE_DEPS_SHARED OFF)  # Build dependencies as static
  FetchContent_MakeAvailable(SDL2_image)
endif()

# Find SDL2_ttf
if(NOT SDL2_ttf_FOUND)
  FetchContent_Declare(
    SDL2_ttf
    GIT_REPOSITORY https://github.com/libsdl-org/SDL_ttf
    GIT_TAG release-2.24.0 # Use a specific version
    GIT_SHALLOW TRUE
    GIT_PROGRESS TRUE
  )
  FetchContent_MakeAvailable(SDL2_ttf)
endif()