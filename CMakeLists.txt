#cmake .. -G "NMake Makefiles" -DCMAKE_BUILD_TYPE=Release -DPython3_EXECUTABLE=D:/Apps/miniconda3/python.exe
cmake_minimum_required(VERSION 3.22)

project(PyLive2D)

include(FetchContent)
set(FETCHCONTENT_QUIET FALSE)
include(cmake/config.cmake)
# configure_file("${PROJECT_SOURCE_DIR}/config.h.in" "${PROJECT_SOURCE_DIR}/include/config.h")

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

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    add_definitions(-fPIC)
endif()

# Find opengl libraries.
set(FRAMEWORK_SOURCE OpenGL)
set(OpenGL_GL_PREFERENCE "GLVND")
find_package(OpenGL REQUIRED)

find_package(Python3 COMPONENTS Interpreter Development REQUIRED)
message("Python3_EXECUTABLE=" ${Python3_EXECUTABLE})
message("Python3_INCLUDE_DIRS=" ${Python3_INCLUDE_DIRS})
message("Python3_LIBRARIES=" ${Python3_LIBRARIES})

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/boost.cmake)

FetchContent_Declare(
    common
    GIT_REPOSITORY https://github.com/qinyonghang/common.git
    GIT_TAG v1
    DOWNLOAD_DIR ${PROJECT_SOURCE_DIR}/third_party/
    SOURCE_DIR ${PROJECT_SOURCE_DIR}/third_party/common
)
FetchContent_MakeAvailable(common)

set(GLEW_VERSION 2.2.0)
FetchContent_Declare(
    glew
    URL https://github.com/nigels-com/glew/releases/download/glew-${GLEW_VERSION}/glew-${GLEW_VERSION}.zip
    URL_HASH SHA256=a9046a913774395a095edcc0b0ac2d81c3aacca61787b39839b941e9be14e0d4
    DOWNLOAD_DIR ${PROJECT_SOURCE_DIR}/third_party/
    SOURCE_DIR ${PROJECT_SOURCE_DIR}/third_party/glew
)
FetchContent_MakeAvailable(glew)
set (glew_dir ${PROJECT_SOURCE_DIR}/third_party/glew)
add_subdirectory(${glew_dir}/build/cmake ${glew_dir})

set(GLFW_VERSION 3.4)
FetchContent_Declare(
    glfw
    URL https://github.com/glfw/glfw/releases/download/${GLFW_VERSION}/glfw-${GLFW_VERSION}.zip
    URL_HASH SHA256=b5ec004b2712fd08e8861dc271428f048775200a2df719ccf575143ba749a3e9
    DOWNLOAD_DIR ${PROJECT_SOURCE_DIR}/third_party/
    SOURCE_DIR ${PROJECT_SOURCE_DIR}/third_party/glfw
)
FetchContent_MakeAvailable(glfw)

FetchContent_Declare(
    CubismSdkForNative
    URL https://cubism.live2d.com/sdk-native/bin/CubismSdkForNative-5-r.1.zip
    URL_HASH SHA256=073f623fc3fb06c192e68bb5287fdb9dab1952164c2a956dec6d4b45fd342ffb
    DOWNLOAD_DIR ${PROJECT_SOURCE_DIR}/third_party/
    SOURCE_DIR ${PROJECT_SOURCE_DIR}/third_party/CubismSdkForNative
)
FetchContent_MakeAvailable(CubismSdkForNative)

find_package(OpenGL REQUIRED)
message("OpenGL_INCLUDE_DIR=" ${OPENGL_INCLUDE_DIR})
message("OpenGL_LIBRARIES=" ${OPENGL_LIBRARIES})

# Add Cubism Core.
set (core_dir ${PROJECT_SOURCE_DIR}/third_party/CubismSdkForNative/Core)
# Import as static library.
add_library(Live2DCubismCore STATIC IMPORTED)
# Find library path.
if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
set_target_properties(Live2DCubismCore
    PROPERTIES
    IMPORTED_LOCATION ${core_dir}/lib/linux/x86_64/libLive2DCubismCore.a
    INTERFACE_INCLUDE_DIRECTORIES ${core_dir}/include
)
elseif (CMAKE_SYSTEM_NAME STREQUAL "Windows")
set(CORE_LIB_SUFFIX ${core_dir}/lib/windows/x86_64/143)
set_target_properties(Live2DCubismCore
  PROPERTIES
    IMPORTED_LOCATION_DEBUG
      ${CORE_LIB_SUFFIX}/Live2DCubismCore_${CRT}d.lib
    IMPORTED_LOCATION_RELEASE
      ${CORE_LIB_SUFFIX}/Live2DCubismCore_${CRT}.lib
    IMPORTED_LOCATION_MINSIZEREL
      ${CORE_LIB_SUFFIX}/Live2DCubismCore_${CRT}.lib
    IMPORTED_LOCATION_RELWITHDEBINFO
      ${CORE_LIB_SUFFIX}/Live2DCubismCore_${CRT}.lib
    INTERFACE_INCLUDE_DIRECTORIES ${core_dir}/include
)
endif ()

set (framework_dir ${PROJECT_SOURCE_DIR}/third_party/CubismSdkForNative/Framework)
add_subdirectory(${framework_dir})
if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    target_compile_definitions(Framework PUBLIC CSM_TARGET_LINUX_GL)
elseif (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    target_compile_definitions(Framework PUBLIC CSM_TARGET_WIN_GL)
endif ()
# target_compile_options(Framework PUBLIC -fPIC -O3)
target_include_directories(Framework PUBLIC ${PROJECT_SOURCE_DIR}/third_party/glew/include)
target_link_libraries(Framework Live2DCubismCore glew_s)

add_library(LAppSrc STATIC src/LApp/LAppTextureManager.cpp  src/LApp/TouchManager.cpp src/LApp/LAppModel.cpp          src/LApp/LAppView.cpp
src/LApp/LAppAllocator.cpp           src/LApp/LAppPal.cpp            src/LApp/LAppWavFileHandler.cpp            src/LApp/LAppSprite.cpp)

set_target_properties(LAppSrc PROPERTIES
    C_STANDARD 17
    C_STANDARD_REQUIRED ON
)

set_target_properties(LAppSrc PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

target_include_directories(LAppSrc PUBLIC
    ${PROJECT_SOURCE_DIR}/third_party/stb
)

target_link_libraries(LAppSrc PUBLIC
    ${OPENGL_LIBRARIES}
    spdlog::spdlog
    Framework
    common
)

file (GLOB source_files ${PROJECT_SOURCE_DIR}/src/Live2D*.cpp)
if (BUILD_PYTHON_MODULE)
add_library(${PROJECT_NAME} MODULE ${source_files})
else ()
add_library(${PROJECT_NAME} SHARED ${source_files})
endif ()

set_target_properties(${PROJECT_NAME} PROPERTIES
    PREFIX ""
    C_STANDARD 17
    C_STANDARD_REQUIRED ON
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

if(BUILD_PYTHON_MODULE)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        PREFIX ""
        SUFFIX ".pyd"
    )
endif()

target_include_directories(${PROJECT_NAME} PRIVATE
    ${Python3_INCLUDE_DIRS}
    ${PROJECT_SOURCE_DIR}/src/LApp
    # ${PROJECT_SOURCE_DIR}/third_party/CubismSdkForNative/Core/include
    # ${PROJECT_SOURCE_DIR}/third_party/CubismSdkForNative/Framework/src
    # # ${PROJECT_SOURCE_DIR}/third_party/spdlog/include
    # ${PROJECT_SOURCE_DIR}/third_party/glew/include
    # ${PROJECT_SOURCE_DIR}/third_party/glfw/include
    ${PROJECT_SOURCE_DIR}/third_party/stb
)

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
target_compile_definitions(${PROJECT_NAME} PRIVATE LIVE2D_EXPORTS /MP)
target_link_libraries(${PROJECT_NAME} PRIVATE
    debug -NODEFAULTLIB:libcmtd.lib
    optimized -NODEFAULTLIB:libcmt.lib
)
endif ()

target_link_libraries(${PROJECT_NAME} PRIVATE
    ${Python3_LIBRARIES}
    ${OPENGL_LIBRARIES}
    Boost::python
    LAppSrc
    Framework
    common
    # glfw
    # glew
)

if (NOT BUILD_PYTHON_MODULE)
add_executable(sample ${PROJECT_SOURCE_DIR}/src/main.cpp)
set_target_properties(sample PROPERTIES
    C_STANDARD 17
    C_STANDARD_REQUIRED ON
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

target_link_libraries(sample PRIVATE
    ${PROJECT_NAME}
    common
    glfw
    # spdlog::spdlog
)
endif ()

add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy ${Boost_S_LIBRARIES} ${PROJECT_BINARY_DIR}
)
