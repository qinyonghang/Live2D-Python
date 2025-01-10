
find_program(Python3_EXECUTABLE python NO_CMAKE_FIND_ROOT_PATH)
find_package(Python3 COMPONENTS Interpreter Development)

if(NOT Python3_FOUND)
    message(STATUS "Python not found in current environment. Searching in .conda directory.")

    if(EXISTS ${PROJECT_SOURCE_DIR}/.conda)
        message(STATUS "Found .conda directory at ${PROJECT_SOURCE_DIR}/.conda")

        if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
            find_program(PYTHON3_CONDA_EXECUTABLE 
                NAMES python.exe python3.exe
                PATHS ${PROJECT_SOURCE_DIR}/.conda
                NO_DEFAULT_PATH
            )
        elseif (CMAKE_SYSTEM_NAME STREQUAL "Linux")
            find_program(PYTHON3_CONDA_EXECUTABLE 
                NAMES python python3
                PATHS ${PROJECT_SOURCE_DIR}/.conda/bin
                NO_DEFAULT_PATH
            )
        endif()

        if(PYTHON3_CONDA_EXECUTABLE)
            message(STATUS "Found Python executable in .conda: ${PYTHON3_CONDA_EXECUTABLE}")
            set(Python3_EXECUTABLE ${PYTHON3_CONDA_EXECUTABLE} CACHE FILEPATH "Python executable" FORCE)
            find_package(Python3 COMPONENTS Interpreter Development REQUIRED)
        else()
            message(FATAL_ERROR "Python not found in current environment or .conda directory.")
        endif()
    else()
        message(FATAL_ERROR "Python not found in current environment and .conda directory does not exist.")
    endif()
else()
    message(STATUS "Python found in current environment: ${Python3_EXECUTABLE}")
endif()

message("Python3_EXECUTABLE=" ${Python3_EXECUTABLE})
message("Python3_INCLUDE_DIRS=" ${Python3_INCLUDE_DIRS})
message("Python3_LIBRARIES=" ${Python3_LIBRARIES})

target_include_directories(${PROJECT_NAME} PRIVATE ${Python3_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PRIVATE ${Python3_LIBRARIES})
