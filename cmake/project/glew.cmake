set(GLEW_VERSION 2.2.0)
set(LIB_NAME glew)
set(${LIB_NAME}_URL "https://github.com/nigels-com/glew/releases/download/glew-${GLEW_VERSION}/glew-${GLEW_VERSION}.zip")
set(${LIB_NAME}_URL_HASH "sha256:a9046a913774395a095edcc0b0ac2d81c3aacca61787b39839b941e9be14e0d4")
set(${LIB_NAME}_DOWNLOAD_DIR ${PROJECT_SOURCE_DIR}/third_party)
set(${LIB_NAME}_DOWNLOAD_NAME ${LIB_NAME}.zip)
set(${LIB_NAME}_SOURCE_DIR ${${LIB_NAME}_DOWNLOAD_DIR}/${LIB_NAME})
if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
set(${LIB_NAME}_BUILD_DIR ${${LIB_NAME}_SOURCE_DIR}/build2/linux)
set(${LIB_NAME}_INSTALL_DIR ${${LIB_NAME}_SOURCE_DIR}/install/linux)
elseif(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
set(${LIB_NAME}_BUILD_DIR ${${LIB_NAME}_SOURCE_DIR}/build2/windows)
set(${LIB_NAME}_INSTALL_DIR ${${LIB_NAME}_SOURCE_DIR}/install/windows)
endif()

if (NOT EXISTS ${${LIB_NAME}_INSTALL_DIR})

set(cmake_args "-DCMAKE_POSITION_INDEPENDENT_CODE=ON -DOpenGL_GL_PREFERENCE=GLVND -DBUILD_UTILS=OFF -DBUILD_SHARED_LIBS=OFF")

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
            --cmake_args ${cmake_args}
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
