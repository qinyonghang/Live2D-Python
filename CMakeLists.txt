cmake_minimum_required(VERSION 3.22)

project(pylive2d)

include(FetchContent)

option(CMAKE_POSITION_INDEPENDENT_CODE "Build as a PIC library" ON)

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

add_subdirectory(qlib)

# Find opengl libraries.
set(FRAMEWORK_SOURCE OpenGL)
set(OpenGL_GL_PREFERENCE "GLVND")
find_package(OpenGL REQUIRED)
message("OpenGL_INCLUDE_DIR=" ${OPENGL_INCLUDE_DIR})
message("OpenGL_LIBRARIES=" ${OPENGL_LIBRARIES})

set(LAppSrc
    src/LApp/LAppTextureManager.cpp
    src/LApp/TouchManager.cpp
    src/LApp/LAppModel.cpp
    src/LApp/LAppView.cpp
    src/LApp/LAppAllocator.cpp
    src/LApp/LAppPal.cpp
    src/LApp/LAppWavFileHandler.cpp
    src/LApp/LAppSprite.cpp
)

file(GLOB source_files ${PROJECT_SOURCE_DIR}/src/Live2D*.cpp)
if (BUILD_PYTHON_MODULE)
add_library(${PROJECT_NAME} MODULE ${source_files} ${LAppSrc})
else ()
add_library(${PROJECT_NAME} SHARED ${source_files} ${LAppSrc})
endif ()

set_target_properties(${PROJECT_NAME} PROPERTIES
    PREFIX ""
    C_STANDARD 17
    C_STANDARD_REQUIRED ON
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

if(BUILD_PYTHON_MODULE AND MSVC)
    set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".pyd")
endif()

target_include_directories(${PROJECT_NAME} PRIVATE
    ${PROJECT_SOURCE_DIR}/src/LApp
    ${PROJECT_SOURCE_DIR}/src/stb
)

target_link_libraries(${PROJECT_NAME} PRIVATE ${OPENGL_LIBRARIES})
target_link_libraries(${PROJECT_NAME} PUBLIC qlib::qlib)

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
target_compile_definitions(${PROJECT_NAME} PRIVATE LIVE2D_EXPORTS /MP)
target_link_libraries(${PROJECT_NAME} PRIVATE
    debug -NODEFAULTLIB:libcmtd.lib
    optimized -NODEFAULTLIB:libcmt.lib
)
endif ()

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

file(GLOB_RECURSE cmake_source_files ${PROJECT_SOURCE_DIR}/cmake/project/*.cmake)
foreach(cmake_file IN LISTS cmake_source_files)
    if(cmake_file MATCHES "python3?\.cmake$")
        message(STATUS "Including ${cmake_file}")
        include(${cmake_file})
    endif()
endforeach()

list(FILTER cmake_source_files EXCLUDE REGEX "python3?\\.cmake$")
foreach(cmake_file IN LISTS cmake_source_files)
    message(STATUS "Including ${cmake_file}")
    include(${cmake_file})
endforeach()

if (NOT BUILD_PYTHON_MODULE)
add_executable(sample ${PROJECT_SOURCE_DIR}/src/main.cpp)
set_target_properties(sample PROPERTIES
    C_STANDARD 17
    C_STANDARD_REQUIRED ON
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

target_link_libraries(sample PRIVATE ${PROJECT_NAME})
# include(cmake/glfw.cmake)
# include(cmake/argparse.cmake)

FetchContent_Declare(
  glfw
  GIT_REPOSITORY https://github.com/glfw/glfw.git
  GIT_TAG        3.4
)

FetchContent_MakeAvailable(glfw)

target_link_libraries(sample PRIVATE glfw)

endif ()
