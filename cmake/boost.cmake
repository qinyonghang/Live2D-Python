include(FetchContent)
include(ExternalProject)

set (Boost_ROOT ${PROJECT_SOURCE_DIR}/third_party/boost)

if(NOT EXISTS ${Boost_ROOT})
FetchContent_Declare(
    Boost
    URL https://archives.boost.io/release/1.80.0/source/boost_1_80_0.tar.gz
    URL_HASH SHA256=4b2136f98bdd1f5857f1c3dea9ac2018effe65286cf251534b6ae20cc45e1847
    DOWNLOAD_DIR ${PROJECT_SOURCE_DIR}/third_party/
    SOURCE_DIR ${PROJECT_SOURCE_DIR}/third_party/boost
)

FetchContent_MakeAvailable(Boost)
endif()

if(CMAKE_HOST_SYSTEM_NAME STREQUAL "Windows")
    set(Boost_BOOTSTRAP_SCRIPT "bootstrap.bat")
    set(Boost_B2_EXECUTABLE "b2.exe")
else()
    set(Boost_BOOTSTRAP_SCRIPT "bootstrap.sh")
    set(Boost_B2_EXECUTABLE "b2")
endif()

# string(TOLOWER ${CMAKE_BUILD_TYPE} Boost_BUILD_TYPE)
# ExternalProject_Add(
#     boost
#     PREFIX ${PROJECT_BINARY_DIR}/boost
#     SOURCE_DIR ${Boost_ROOT}
#     BUILD_IN_SOURCE 1
#     CONFIGURE_COMMAND ${PROJECT_SOURCE_DIR}/third_party/boost/${Boost_BOOTSTRAP_SCRIPT}
#     COMMAND ${CMAKE_COMMAND} -E env python ${PROJECT_SOURCE_DIR}/scripts/replace.py "${PROJECT_SOURCE_DIR}/third_party/boost/tools/build/src/tools/msvc.jam"
#     BUILD_COMMAND ${PROJECT_SOURCE_DIR}/third_party/boost/${Boost_B2_EXECUTABLE} --with-python link=shared address-model=64 variant=${Boost_BUILD_TYPE}
#     INSTALL_COMMAND ""
# )

set(Boost_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/third_party/boost)
set(Boost_LIBRARY_DIR ${PROJECT_SOURCE_DIR}/third_party/boost/stage/lib)
set(Boost_LIBRARIES Boost::python)

add_library(Boost::python SHARED IMPORTED)
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set (Boost_S_LIBRARIES ${Boost_LIBRARY_DIR}/libboost_python312.so)
    set_target_properties(Boost::python PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES ${Boost_INCLUDE_DIR}
        IMPORTED_LOCATION ${Boost_LIBRARY_DIR}/libboost_python312.so
    )
elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        set (Boost_DEBUG_TYPE gd-)
    endif()
    set (Boost_S_LIBRARIES ${Boost_LIBRARY_DIR}/boost_python312-vc143-mt-${Boost_DEBUG_TYPE}x64-1_80.dll)
    set_target_properties(Boost::python PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES ${Boost_INCLUDE_DIR}
        IMPORTED_IMPLIB ${Boost_LIBRARY_DIR}/boost_python312-vc143-mt-${Boost_DEBUG_TYPE}x64-1_80.lib
    )
endif()

add_dependencies(Boost::python boost)

message("Boost_ROOT: ${Boost_ROOT}")
message("Boost_INCLUDE_DIR: ${Boost_INCLUDE_DIR}")
message("Boost_LIBRARY_DIR: ${Boost_LIBRARY_DIR}")
message("Boost_LIBRARIES: ${Boost_LIBRARIES}")
