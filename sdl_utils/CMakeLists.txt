#####
##### CMakeLists.txt for sdl_utils #####
#####

# Need to re-find packages to make them available in this directory
# https://stackoverflow.com/questions/77063244/how-to-link-against-a-target-found-by-find-package-in-a-sibling-cmakelists-txt

find_package(SDL2 QUIET)
find_package(SDL2_image QUIET)
find_package(SDL2_ttf QUIET)

file(GLOB_RECURSE SDL_UTILS_SRCS "src/**/*.cpp")

add_library(sdl_utils STATIC ${SDL_UTILS_SRCS})
target_include_directories(sdl_utils PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(sdl_utils PUBLIC SDL2::SDL2-static SDL2_image::SDL2_image-static SDL2_ttf)
