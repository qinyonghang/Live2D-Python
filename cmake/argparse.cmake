set(LIB_NAME argparse)
set(${LIB_NAME}_URL "https://github.com/p-ranav/argparse/archive/refs/tags/v3.1.tar.gz")
set(${LIB_NAME}_URL_HASH "sha256:d01733552ca4a18ab501ae8b8be878131baa32e89090fafdeef018ebfa4c6e46")
set(${LIB_NAME}_DOWNLOAD_DIR ${PROJECT_SOURCE_DIR}/third_party)
set(${LIB_NAME}_DOWNLOAD_NAME ${LIB_NAME}.tar.gz)
set(${LIB_NAME}_SOURCE_DIR ${${LIB_NAME}_DOWNLOAD_DIR}/${LIB_NAME})

if(NOT EXISTS ${${LIB_NAME}_SOURCE_DIR})

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
message(FATAL_ERROR "Failed to download ${LIB_NAME}!")
endif()

endif()

set(${LIB_NAME}_INCLUDE_DIR ${${LIB_NAME}_SOURCE_DIR}/include)
message("${LIB_NAME}_INCLUDE_DIR=${${LIB_NAME}_INCLUDE_DIR}")

target_include_directories(sample PRIVATE ${${LIB_NAME}_INCLUDE_DIR})
