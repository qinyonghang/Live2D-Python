set(LIB_NAME boost)
set(${LIB_NAME}_URL "https://archives.boost.io/release/1.80.0/source/boost_1_80_0.tar.gz")
set(${LIB_NAME}_URL_HASH "sha256:4b2136f98bdd1f5857f1c3dea9ac2018effe65286cf251534b6ae20cc45e1847")
set(${LIB_NAME}_DOWNLOAD_DIR ${PROJECT_SOURCE_DIR}/third_party)
set(${LIB_NAME}_DOWNLOAD_NAME ${LIB_NAME}.tar.gz)
set(${LIB_NAME}_SOURCE_DIR ${${LIB_NAME}_DOWNLOAD_DIR}/${LIB_NAME})
if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
set(${LIB_NAME}_INSTALL_DIR ${${LIB_NAME}_SOURCE_DIR}/install2/linux)
elseif(${CMAKE_SYSTEM_NAME} STREQUAL "Windows" AND CMAKE_SIZEOF_VOID_P EQUAL 8)
set(${LIB_NAME}_ADDRESS_MODE "address-model=64")
set(${LIB_NAME}_INSTALL_DIR ${${LIB_NAME}_SOURCE_DIR}/install2/windows)
elseif(${CMAKE_SYSTEM_NAME} STREQUAL "Windows" AND CMAKE_SIZEOF_VOID_P EQUAL 4)
set(${LIB_NAME}_INSTALL_DIR ${${LIB_NAME}_SOURCE_DIR}/install2/win32)
set(${LIB_NAME}_ADDRESS_MODE "address-model=32")
else()
message(FATAL_ERROR "Unsupported system")
endif()

if (NOT EXISTS ${${LIB_NAME}_INSTALL_DIR})

if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
set(b2 "./b2")
set(bootstrap "sh bootstrap.sh")
elseif (CMAKE_SYSTEM_NAME STREQUAL "Windows")
set(b2 "b2.exe")
set(bootstrap "bootstrap.bat")
endif()

execute_process(
    COMMAND ${CMAKE_COMMAND} -E env 
        PYTHONPATH=${PROJECT_SOURCE_DIR}
        ${Python3_EXECUTABLE} ${PROJECT_SOURCE_DIR}/scripts/compile.py
            --url ${${LIB_NAME}_URL}
            --url_hash ${${LIB_NAME}_URL_HASH}
            --download_dir ${${LIB_NAME}_DOWNLOAD_DIR}
            --download_name ${${LIB_NAME}_DOWNLOAD_NAME}
            --source_dir ${${LIB_NAME}_SOURCE_DIR}
            --install_dir ${${LIB_NAME}_INSTALL_DIR}
            --custom_compile "${Python3_EXECUTABLE} ${PROJECT_SOURCE_DIR}/scripts/replace.py ${${LIB_NAME}_SOURCE_DIR}/libs/python/src/numpy/dtype.cpp \"reinterpret_cast<PyArray_Descr*>(ptr())->elsize\" \"0\"" "${Python3_EXECUTABLE} ${PROJECT_SOURCE_DIR}/scripts/replace.py ${${LIB_NAME}_SOURCE_DIR}/tools/build/src/tools/msvc.jam \"(14.3)\" \"(14.[34])\"" "${bootstrap}" "${b2} install ${${LIB_NAME}_ADDRESS_MODE} --with-python --link=static --prefix=${${LIB_NAME}_INSTALL_DIR}"
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
        RESULT_VARIABLE result
        COMMAND_ECHO STDOUT
)

if(result)
message(FATAL_ERROR "Failed to compile ${LIB_NAME}!")
endif()

endif()

cmake_policy(SET CMP0144 NEW)
cmake_policy(SET CMP0167 OLD)
set(Boost_ROOT ${${LIB_NAME}_INSTALL_DIR})
find_package(Boost REQUIRED COMPONENTS python)

message("Boost_INCLUDE_DIRS=${Boost_INCLUDE_DIRS}")
message("Boost_LIBRARIES=${Boost_LIBRARIES}")
