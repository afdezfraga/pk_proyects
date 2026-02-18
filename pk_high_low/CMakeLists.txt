#####
##### CMakeLists.txt for pk_high_low #####
#####

# Need to re-find packages to make them available in this directory
# https://stackoverflow.com/questions/77063244/how-to-link-against-a-target-found-by-find-package-in-a-sibling-cmakelists-txt
find_package(nlohmann_json QUIET)

find_package(SDL2 QUIET)
find_package(SDL2_image QUIET)
find_package(SDL2_ttf QUIET)

# Add the source files
file(GLOB_RECURSE SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
file(GLOB_RECURSE NO_MAIN_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
list(REMOVE_ITEM NO_MAIN_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp")

# Name of the executable
set(EXECUTABLE_NAME pk_high_low)



#####
#####
# Create the executable
#####
#####
if (EMSCRIPTEN)
    set(CMAKE_EXECUTABLE_SUFFIX ".html")
endif()
add_executable(${EXECUTABLE_NAME} ${SOURCES})

# Add the include directories for pk_proyects/pk_high_low/include/
target_include_directories(${EXECUTABLE_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Add the include directories for sdl_utils/include/
target_include_directories(${EXECUTABLE_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/sdl_utils/include)

# Link third-party libraries
target_link_libraries(${EXECUTABLE_NAME} PRIVATE nlohmann_json::nlohmann_json)
target_link_libraries(${EXECUTABLE_NAME} PRIVATE SDL2::SDL2-static SDL2_image::SDL2_image-static SDL2_ttf)


# If testing is enabled, create the test executable
if(BUILD_TESTING)

    # Find GTest package
    find_package(GTest QUIET)

    # Add the testing source files
    file(GLOB_RECURSE TEST_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/test/*.cpp")

    # Create the test executable
    add_executable(${EXECUTABLE_NAME}_test ${NO_MAIN_SOURCES} ${TEST_SOURCES})

    # Add the include directories for pk_proyects/pk_high_low/include/ and pk_proyects/pk_high_low/test/
    target_include_directories(${EXECUTABLE_NAME}_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

    # Link third-party libraries
    target_link_libraries(${EXECUTABLE_NAME}_test PRIVATE nlohmann_json::nlohmann_json)
    target_link_libraries(${EXECUTABLE_NAME}_test PRIVATE GTest::gtest_main)
    target_link_libraries(${EXECUTABLE_NAME}_test PRIVATE SDL2::SDL2-static SDL2_image::SDL2_image-static SDL2_ttf)

    # Add tests
    include(GoogleTest)
    gtest_discover_tests(${EXECUTABLE_NAME}_test)
    
endif()