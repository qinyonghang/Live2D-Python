cmake_minimum_required(VERSION 3.22)

project(pylive2d)

include(FetchContent)

option(CMAKE_POSITION_INDEPENDENT_CODE "Build as a PIC library" ON)
option(BUILD_EXAMPLES "Build examples" OFF)
option(BUILD_PYTHON_MODULE "Build Python module" OFF)
option(pylive2d_BUILD_EXAMPLES "Build examples" OFF)

if(MSVC)
    set(CRT MD)
endif()

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    message(STATUS "Building for 64-bit")
else()
    message(STATUS "Building for 32-bit")
endif()

message(STATUS "CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")
message(STATUS "CMAKE_INSTALL_PREFIX: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "CMAKE_TOOLCHAIN_FILE: ${CMAKE_TOOLCHAIN_FILE}")
message(STATUS "CMAKE_SYSTEM_NAME: ${CMAKE_SYSTEM_NAME}")
message(STATUS "CMAKE_SYSTEM_PROCESSOR: ${CMAKE_SYSTEM_PROCESSOR}")

# Find opengl libraries.
set(FRAMEWORK_SOURCE OpenGL)
set(OpenGL_GL_PREFERENCE "GLVND")
find_package(OpenGL REQUIRED)
message("OpenGL_INCLUDE_DIR=" ${OPENGL_INCLUDE_DIR})
message("OpenGL_LIBRARIES=" ${OPENGL_LIBRARIES})

if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
find_program(MAKE_EXECUTABLE NAMES make DOC "Path to Make build tool")
if (NOT MAKE_EXECUTABLE)
message(FATAL_ERROR 
    "=============================================\n"
    "ERROR: Make is not found even though you installed it!\n"
    "Please install Make:\n"
    "\n"
    "  [Linux]    Install via package manager:\n"
    "             sudo apt install make         # Debian/Ubuntu 安装 Make\n"
    "             sudo dnf install make         # Fedora 安装 Make\n"
    "=============================================\n")
endif()
endif()

set(source_files
    src/LApp/LAppTextureManager.cpp
    src/LApp/TouchManager.cpp
    src/LApp/LAppModel.cpp
    src/LApp/LAppView.cpp
    src/LApp/LAppAllocator.cpp
    src/LApp/LAppPal.cpp
    src/LApp/LAppWavFileHandler.cpp
    src/LApp/LAppSprite.cpp
)

if (BUILD_PYTHON_MODULE)
add_library(pylive2d MODULE ${source_files})
else ()
add_library(pylive2d SHARED ${source_files})
endif ()

set_target_properties(pylive2d PROPERTIES
    PREFIX ""
    C_STANDARD 17
    C_STANDARD_REQUIRED ON
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
target_compile_definitions(pylive2d PRIVATE LIVE2D_EXPORTS /MP)
target_link_libraries(pylive2d PRIVATE
    debug -NODEFAULTLIB:libcmtd.lib
    optimized -NODEFAULTLIB:libcmt.lib
)
endif ()

target_include_directories(pylive2d PRIVATE
    ${PROJECT_SOURCE_DIR}/src/LApp
    ${PROJECT_SOURCE_DIR}/src/stb
)
target_include_directories(pylive2d PUBLIC
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/src
)

target_link_libraries(pylive2d PRIVATE ${OPENGL_LIBRARIES})
include(${PROJECT_SOURCE_DIR}/cmake/glew.cmake)
include(${PROJECT_SOURCE_DIR}/cmake/CubismSdk.cmake)
target_link_libraries(pylive2d PUBLIC live2d)

if(BUILD_PYTHON_MODULE)
if(MSVC)
set_target_properties(pylive2d PROPERTIES SUFFIX ".pyd")
endif()
include(${PROJECT_SOURCE_DIR}/cmake/python3.cmake)
include(${PROJECT_SOURCE_DIR}/cmake/boost.cmake)
target_sources(pylive2d PRIVATE ${PROJECT_SOURCE_DIR}/src/wrapper.cpp)
target_include_directories(pylive2d PRIVATE ${Python3_INCLUDE_DIRS})
target_link_libraries(pylive2d PRIVATE ${Python3_LIBRARIES})
target_compile_definitions(pylive2d PRIVATE BOOST_PYTHON_STATIC_LIB)
target_link_libraries(pylive2d PRIVATE ${Boost_LIBRARIES})
endif()

add_subdirectory(qlib)
target_link_libraries(pylive2d PUBLIC qlib::qlib)

if (BUILD_EXAMPLES OR pylive2d_BUILD_EXAMPLES)
file(GLOB examples ${CMAKE_CURRENT_LIST_DIR}/examples/*)
foreach(example ${examples})
    if (IS_DIRECTORY ${example} AND EXISTS ${example}/CMakeLists.txt)
        add_subdirectory(${example})
    endif ()
endforeach()
endif()
