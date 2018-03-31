cmake_minimum_required(VERSION 3.10)
project(myriad)

set(CMAKE_AUTOMOC ON)

set(3RDPARTY_DIR "3rdparty")
set(SRC_DIR      "src")

set(PHASH_DIR     "${CMAKE_CURRENT_SOURCE_DIR}/${3RDPARTY_DIR}/libphash")
set(PHASH_SRC_DIR "${PHASH_DIR}/src/libphash")
set(PHASH_LIBRARY "${PHASH_DIR}/lib/libpHash.a")

include(ExternalProject)

# libphash does not provide an official git repository; Debian has one for its own packaged version,
# but it doesn't support out-of-tree builds well and tracks its generated configure script, causing
# a submodule tracking that repository to immediately become dirty when built.

ExternalProject_Add(libphash
    PREFIX ${PHASH_DIR}
    SOURCE_DIR ${PHASH_SRC_DIR}
    URL http://www.phash.org/releases/pHash-0.9.6.tar.gz
    CONFIGURE_COMMAND ${PHASH_SRC_DIR}/configure --disable-video-hash --disable-audio-hash --disable-pthread --prefix=${PHASH_DIR}
    BUILD_COMMAND make
    INSTALL_COMMAND make install
)

# libjpeg and libpng are not used directly, but required by libphash.

find_package(JPEG REQUIRED)
find_package(PNG  REQUIRED)
find_package(Qt5  REQUIRED COMPONENTS Gui)

add_executable(myriad "")
add_dependencies(myriad libphash)

set_property(TARGET myriad PROPERTY CXX_STANDARD 17)

target_include_directories(myriad SYSTEM PRIVATE ${PHASH_DIR}/include)
target_include_directories(myriad PRIVATE ${SRC_DIR})
target_link_libraries(myriad Qt5::Gui ${PHASH_LIBRARY} ${PNG_LIBRARY} ${JPEG_LIBRARIES})
include("${SRC_DIR}/CMakeLists.txt")

install(TARGETS myriad RUNTIME DESTINATION bin)
