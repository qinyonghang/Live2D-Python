set(LIB_NAME glfw3)
set(${LIB_NAME}_URL "https://github.com/glfw/glfw/releases/download/3.4/glfw-3.4.zip")
set(${LIB_NAME}_URL_HASH "sha256:b5ec004b2712fd08e8861dc271428f048775200a2df719ccf575143ba749a3e9")
set(${LIB_NAME}_DOWNLOAD_DIR ${PROJECT_SOURCE_DIR}/third_party)
set(${LIB_NAME}_SOURCE_DIR ${${LIB_NAME}_DOWNLOAD_DIR}/${LIB_NAME})
if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
set(${LIB_NAME}_BUILD_DIR ${${LIB_NAME}_SOURCE_DIR}/build2/linux)
set(${LIB_NAME}_INSTALL_DIR ${${LIB_NAME}_SOURCE_DIR}/install/linux)
elseif(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
set(${LIB_NAME}_BUILD_DIR ${${LIB_NAME}_SOURCE_DIR}/build2/windows)
set(${LIB_NAME}_INSTALL_DIR ${${LIB_NAME}_SOURCE_DIR}/install/windows)
endif()

if(NOT EXISTS ${${LIB_NAME}_INSTALL_DIR})

execute_process(
    COMMAND ${CMAKE_COMMAND} -E env 
        PYTHONPATH=${PROJECT_SOURCE_DIR}
        ${Python3_EXECUTABLE} ${PROJECT_SOURCE_DIR}/scripts/compile.py
            --url ${${LIB_NAME}_URL}
            --url_hash ${${LIB_NAME}_URL_HASH}
            --download_dir ${${LIB_NAME}_DOWNLOAD_DIR}
            --download_name ${LIB_NAME}
            --source_dir ${${LIB_NAME}_SOURCE_DIR}
            --build_dir ${${LIB_NAME}_BUILD_DIR}
            --install_dir ${${LIB_NAME}_INSTALL_DIR}
            --cmake_args " -DGLFW_BUILD_WAYLAND=OFF"
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
        RESULT_VARIABLE result
        COMMAND_ECHO STDOUT
)

if(result)
message(FATAL_ERROR "Failed to compile ${LIB_NAME}!")
endif()

endif()

cmake_policy(SET CMP0144 NEW)
find_package(${LIB_NAME} REQUIRED PATHS ${${LIB_NAME}_INSTALL_DIR} NO_DEFAULT_PATH)

target_link_libraries(sample PRIVATE glfw)
