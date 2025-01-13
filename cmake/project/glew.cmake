set(GLEW_VERSION 2.2.0)
set(LIB_NAME glew)
set(${LIB_NAME}_URL "https://github.com/nigels-com/glew/releases/download/glew-${GLEW_VERSION}/glew-${GLEW_VERSION}.zip")
set(${LIB_NAME}_URL_HASH "sha256:a9046a913774395a095edcc0b0ac2d81c3aacca61787b39839b941e9be14e0d4")
set(${LIB_NAME}_DOWNLOAD_DIR ${PROJECT_SOURCE_DIR}/third_party)
set(${LIB_NAME}_DOWNLOAD_NAME ${LIB_NAME}.zip)
set(${LIB_NAME}_SOURCE_DIR ${${LIB_NAME}_DOWNLOAD_DIR}/${LIB_NAME})
set(${LIB_NAME}_CMAKE_ARGS "-DCMAKE_POSITION_INDEPENDENT_CODE=ON -DOpenGL_GL_PREFERENCE=GLVND -DBUILD_UTILS=OFF -DBUILD_SHARED_LIBS=OFF")
if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
set(${LIB_NAME}_BUILD_DIR ${${LIB_NAME}_SOURCE_DIR}/build2/linux)
set(${LIB_NAME}_INSTALL_DIR ${${LIB_NAME}_SOURCE_DIR}/install/linux)
elseif(${CMAKE_SYSTEM_NAME} STREQUAL "Windows" AND CMAKE_SIZEOF_VOID_P EQUAL 8)
set(${LIB_NAME}_BUILD_DIR ${${LIB_NAME}_SOURCE_DIR}/build2/windows)
set(${LIB_NAME}_INSTALL_DIR ${${LIB_NAME}_SOURCE_DIR}/install/windows)
set(${LIB_NAME}_CMAKE_ARGS "-A x64 ${${LIB_NAME}_CMAKE_ARGS}")
elseif(${CMAKE_SYSTEM_NAME} STREQUAL "Windows" AND CMAKE_SIZEOF_VOID_P EQUAL 4)
set(${LIB_NAME}_BUILD_DIR ${${LIB_NAME}_SOURCE_DIR}/build2/win32)
set(${LIB_NAME}_INSTALL_DIR ${${LIB_NAME}_SOURCE_DIR}/install/win32)
set(${LIB_NAME}_CMAKE_ARGS "-A Win32 ${${LIB_NAME}_CMAKE_ARGS}")
else()
message(FATAL_ERROR "Unsupported platform")
endif()

if (NOT EXISTS ${${LIB_NAME}_INSTALL_DIR})

execute_process(
    COMMAND ${CMAKE_COMMAND} -E env 
        PYTHONPATH=${PROJECT_SOURCE_DIR}
        ${Python3_EXECUTABLE} ${PROJECT_SOURCE_DIR}/scripts/compile.py
            --url ${${LIB_NAME}_URL}
            --url_hash ${${LIB_NAME}_URL_HASH}
            --download_dir ${${LIB_NAME}_DOWNLOAD_DIR}
            --download_name ${${LIB_NAME}_DOWNLOAD_NAME}
            --source_dir ${${LIB_NAME}_SOURCE_DIR}
            --build_dir ${${LIB_NAME}_BUILD_DIR}
            --install_dir ${${LIB_NAME}_INSTALL_DIR}
            --cmakelists_dir ${${LIB_NAME}_SOURCE_DIR}/build/cmake
            --cmake_args ${${LIB_NAME}_CMAKE_ARGS}
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
        RESULT_VARIABLE result
        COMMAND_ECHO STDOUT
)

if(result)
message(FATAL_ERROR "Failed to compile ${LIB_NAME}!")
endif()

endif()

set(GLEW_ROOT ${${LIB_NAME}_INSTALL_DIR})
find_package(GLEW REQUIRED)

message("GLEW_INCLUDE_DIRS: ${GLEW_INCLUDE_DIRS}")
message("GLEW_LIBRARIES: ${GLEW_LIBRARIES}")

target_link_libraries(${PROJECT_NAME} PRIVATE GLEW::glew_s)
