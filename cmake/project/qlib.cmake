
set(LIB_NAME qlib)
set(${LIB_NAME}_URL "https://github.com/qinyonghang/qLib/archive/refs/tags/v1.2.tar.gz")
set(${LIB_NAME}_URL_HASH "sha256:a1429482ee4c223b6e216d1b025431ec51f0116e1f583d50405188d6d61adcbb")
set(${LIB_NAME}_DOWNLOAD_DIR ${PROJECT_SOURCE_DIR}/third_party)
set(${LIB_NAME}_DOWNLOAD_NAME ${LIB_NAME}.tar.gz)
set(${LIB_NAME}_SOURCE_DIR ${${LIB_NAME}_DOWNLOAD_DIR}/${LIB_NAME})
if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
set(${LIB_NAME}_BUILD_DIR ${${LIB_NAME}_SOURCE_DIR}/build2/linux)
set(${LIB_NAME}_INSTALL_DIR ${${LIB_NAME}_SOURCE_DIR}/install/linux)
elseif(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
set(${LIB_NAME}_BUILD_DIR ${${LIB_NAME}_SOURCE_DIR}/build2/windows)
set(${LIB_NAME}_INSTALL_DIR ${${LIB_NAME}_SOURCE_DIR}/install/windows)
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
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
        RESULT_VARIABLE result
        COMMAND_ECHO STDOUT
)

if(result)
message(FATAL_ERROR "Failed to compile ${LIB_NAME}!")
endif()

endif()

cmake_policy(SET CMP0144 NEW)
set(QLIB_ROOT ${${LIB_NAME}_INSTALL_DIR})
find_package(qlib REQUIRED)

message("QLIB_LIBRARIES: ${QLIB_LIBRARIES}")

target_link_libraries(${PROJECT_NAME} PUBLIC ${QLIB_LIBRARIES})
