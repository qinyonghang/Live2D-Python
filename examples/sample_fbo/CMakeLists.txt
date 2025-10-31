
get_filename_component(TARGET_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)

message(STATUS "Build ${TARGET_NAME}:")
add_executable(${TARGET_NAME} main.cpp)

set_target_properties(${TARGET_NAME} PROPERTIES
    C_STANDARD 17
    C_STANDARD_REQUIRED ON
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)

target_link_libraries(${TARGET_NAME} PRIVATE pylive2d)

if(MSVC)
    target_compile_options(${TARGET_NAME} PRIVATE /utf-8)
endif()

FetchContent_Declare(
  glfw
  GIT_REPOSITORY https://github.com/glfw/glfw.git
  GIT_TAG        3.4
)

FetchContent_MakeAvailable(glfw)

target_link_libraries(${TARGET_NAME} PRIVATE glfw)
