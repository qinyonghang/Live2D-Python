
set(LIB_NAME CubismSdkForNative)
set(${LIB_NAME}_URL "https://cubism.live2d.com/sdk-native/bin/CubismSdkForNative-5-r.1.zip")
set(${LIB_NAME}_URL_HASH "sha256:073f623fc3fb06c192e68bb5287fdb9dab1952164c2a956dec6d4b45fd342ffb")
set(${LIB_NAME}_DOWNLOAD_DIR ${PROJECT_SOURCE_DIR}/third_party)
set(${LIB_NAME}_DOWNLOAD_NAME ${LIB_NAME})
set(${LIB_NAME}_SOURCE_DIR ${${LIB_NAME}_DOWNLOAD_DIR}/${LIB_NAME})

if (NOT EXISTS ${${LIB_NAME}_SOURCE_DIR})

execute_process(
    COMMAND ${CMAKE_COMMAND} -E env 
        PYTHONPATH=${PROJECT_SOURCE_DIR}
        ${Python3_EXECUTABLE} ${PROJECT_SOURCE_DIR}/scripts/compile.py
            --url ${${LIB_NAME}_URL}
            --url_hash ${${LIB_NAME}_URL_HASH}
            --download_dir ${${LIB_NAME}_DOWNLOAD_DIR}
            --download_name ${${LIB_NAME}_DOWNLOAD_NAME}
            --source_dir ${${LIB_NAME}_SOURCE_DIR}
            --skip_compile
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
        RESULT_VARIABLE result
        COMMAND_ECHO STDOUT
)

if(result)
message(FATAL_ERROR "Failed to compile ${LIB_NAME}!")
endif()

endif()

# Add Cubism Core.
set(core_dir ${${LIB_NAME}_SOURCE_DIR}/Core)
# Import as static library.
add_library(Live2DCubismCore STATIC IMPORTED)
# Find library path.
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
set_target_properties(Live2DCubismCore
    PROPERTIES
    IMPORTED_LOCATION ${core_dir}/lib/linux/x86_64/libLive2DCubismCore.a
    INTERFACE_INCLUDE_DIRECTORIES ${core_dir}/include
)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
set(CORE_LIB_SUFFIX ${core_dir}/lib/windows/x86_64/${MSVC_TOOLSET_VERSION})
elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
set(CORE_LIB_SUFFIX ${core_dir}/lib/windows/x86/${MSVC_TOOLSET_VERSION})
endif()

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
endif()

# file(GLOB_RECURSE source_files ${${LIB_NAME}_SOURCE_DIR}/Framework/src/*.cpp)
# file(GLOB_RECURSE header_files ${${LIB_NAME}_SOURCE_DIR}/Framework/src/*.hpp)
set(source_files "")
set(header_files "")

file(GLOB_RECURSE all_source_files ${${LIB_NAME}_SOURCE_DIR}/Framework/src/*.cpp)
file(GLOB_RECURSE all_header_files ${${LIB_NAME}_SOURCE_DIR}/Framework/src/*.hpp)

foreach(source_file IN LISTS all_source_files)
    if(NOT source_file MATCHES ".*Rendering.*")
        list(APPEND source_files ${source_file})
    endif()
endforeach()
list(APPEND source_files
    ${${LIB_NAME}_SOURCE_DIR}/Framework/src/Rendering/CubismRenderer.cpp
    ${${LIB_NAME}_SOURCE_DIR}/Framework/src/Rendering/CubismRenderer.hpp
    ${${LIB_NAME}_SOURCE_DIR}/Framework/src/Rendering/CubismClippingManager.hpp
    ${${LIB_NAME}_SOURCE_DIR}/Framework/src/Rendering/CubismClippingManager.tpp
    ${${LIB_NAME}_SOURCE_DIR}/Framework/src/Rendering/OpenGL/CubismOffscreenSurface_OpenGLES2.cpp
    ${${LIB_NAME}_SOURCE_DIR}/Framework/src/Rendering/OpenGL/CubismOffscreenSurface_OpenGLES2.hpp
    ${${LIB_NAME}_SOURCE_DIR}/Framework/src/Rendering/OpenGL/CubismShader_OpenGLES2.cpp
    ${${LIB_NAME}_SOURCE_DIR}/Framework/src/Rendering/OpenGL/CubismShader_OpenGLES2.hpp
    ${${LIB_NAME}_SOURCE_DIR}/Framework/src/Rendering/OpenGL/CubismRenderer_OpenGLES2.cpp
    ${${LIB_NAME}_SOURCE_DIR}/Framework/src/Rendering/OpenGL/CubismRenderer_OpenGLES2.hpp
)

foreach(header_file IN LISTS all_header_files)
    if(NOT header_file MATCHES ".*Rendering.*")
        list(APPEND header_files ${header_file})
    endif()
endforeach()
add_library(live2d STATIC ${source_files} ${header_files})

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    target_compile_definitions(live2d PUBLIC CSM_TARGET_LINUX_GL)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    target_compile_definitions(live2d PUBLIC CSM_TARGET_WIN_GL)
endif()

target_include_directories(live2d PUBLIC ${${LIB_NAME}_SOURCE_DIR}/Framework/src)
target_link_libraries(live2d PUBLIC Live2DCubismCore GLEW::glew_s)
