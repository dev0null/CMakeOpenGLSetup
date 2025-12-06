cmake_minimum_required(VERSION 3.31)
project(OGL_triangle)

# Set compiler configs
set(CMAKE_CXX_STANDARD 23)
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(CMAKE_CXX_SCAN_FOR_MODULES OFF)
endif()

# Set default build type
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

message("Using C compiler: " ${CMAKE_C_COMPILER})
message("Using CXX compiler: " ${CMAKE_CXX_COMPILER_ID})


if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Release>:Release>")
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(GLFW_BUILD_WAYLAND OFF CACHE BOOL "Build glfw on Wayland")
endif()

set(GLFW_INSTALL OFF CACHE BOOL "Install glfw" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "Compile glfw docs" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "Compile glfw examples" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "Compile glfw tests" FORCE)

add_subdirectory(thirdparty/glfw)
add_subdirectory(thirdparty/glad)
add_subdirectory(thirdparty/stb_image)

file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
add_executable("${CMAKE_PROJECT_NAME}")

target_compile_definitions("${CMAKE_PROJECT_NAME}" PRIVATE
    $<$<CONFIG:Debug>:DEBUG=1>
    $<$<CONFIG:Debug>:RESOURCES_PATH="${CMAKE_CURRENT_SOURCE_DIR}/resources/">

    $<$<CONFIG:Release>:RESOURCES_PATH="./resources/">
)

target_sources("${CMAKE_PROJECT_NAME}" PRIVATE ${SRC_FILES})
target_include_directories("${CMAKE_PROJECT_NAME}" PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include/")
target_link_libraries("${CMAKE_PROJECT_NAME}" PRIVATE
    glfw
    glad
    stb_image
    # Debug-only libs
    # $<$<CONFIG:Debug>lib_name>
)