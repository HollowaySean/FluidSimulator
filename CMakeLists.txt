cmake_minimum_required(VERSION 3.0.0)
project(FluidSim VERSION 0.1.0)

include(CTest)
enable_testing()

# Set binary location
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../bin)

# GLFW and OpenGL options
find_package(OpenGL REQUIRED)
find_package(glfw3 3.3 REQUIRED)
find_package(GLEW REQUIRED)

# Build GUI configuration
file(GLOB sourceGUI
    "./src/*.cpp"
    "./lib/imgui/*.cpp"
)
add_executable(FluidSimGUI ./src/main/mainGUI.cpp ${sourceGUI})
target_include_directories(FluidSimGUI PRIVATE "./lib")
target_link_libraries(FluidSimGUI OpenGL::GL)
target_link_libraries(FluidSimGUI glfw)
target_link_libraries(FluidSimGUI GLEW)

# Build recording configuration
if(${CMAKE_BUILD_TYPE} STREQUAL "Record")
    file(GLOB sourceRecord
        "./src/*.cpp"
        "./lib/imgui/*.cpp"
        "./lib/EasyBMP/*.cpp"
    )
    add_executable(FluidSimRecord ./src/main/mainRecord.cpp ${sourceRecord})
    target_include_directories(FluidSimRecord PRIVATE "./lib")
    target_link_libraries(FluidSimRecord OpenGL::GL)
    target_link_libraries(FluidSimRecord glfw)
    target_link_libraries(FluidSimRecord GLEW)
endif()

# CPack options
set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
