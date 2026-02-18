# PK_HIGH_LOW

This is a simple game where the player has to guess if the next card will be higher or lower than the current card. The game is implemented in C++ and compiled to WebAssembly, allowing it to run in the browser.

## How to compile to WASM
To compile the game to WebAssembly, you can use Emscripten. First, make sure you have Emscripten installed and set up correctly. Second, you need to take into account a couple of caviats, ie. the subdependency of SDL2_TTF, freetype may fail if used the system version as it may not been compiled with weasm support. You need to make sure the downloaded external version is used. Then, navigate to the project directory and run the following command:

```bash
../emsdk/emsdk activate latest
source ../emsdk/emsdk_env.sh
emcmake cmake -S . -B wasm_build   -DSDL2TTF_VENDORED=ON   -DSDL2TTF_FREETYPE_VENDORED=ON   -DSDL2TTF_HARFBUZZ=OFF   -DSDL2IMAGE_VENDORED=ON   -DSDL2IMAGE_DEPS_SHARED=OFF   -DSDL2IMAGE_AVIF=OFF   -DSDL2IMAGE_DAV1D=OFF   -DSDL2IMAGE_INSTALL=OFF -DCMAKE_EXE_LINKER_FLAGS="--preload-file ${PWD}/pk_high_low/assets@/assets"
cmake --build wasm_build --target pk_high_low -j
```

Problems to solve:
- [x] SDL2_ttf freetype dependency may fail if the system version is used. Make sure the downloaded external version is used.
- [x] SDL2_image has to be compiled for webasm, so make sure to use the downloaded external version and clean the build directory before compiling, as the native library from the native compilation build may be used instead.
- [x] Make sure to preload the assets folder using the `--preload-file` flag when linking, so that the game can access the images in the browser.

In my system ../emsdk is the path to the emsdk installation. After running these commands, you should find the compiled WebAssembly files in the `wasm_build` directory. You can then serve these files using a local web server to play the game in your browser.

For example emscripten provides a simple way to serve the files:

```bash
emrun wasm_build/pk_high_low.html
```