cmake_minimum_required(VERSION 3.2)
project(BscProject)

#Define directories
set(PROJECT_DIR ${CMAKE_SOURCE_DIR})
set(SOURCE_DIR ${PROJECT_DIR}/src)
set(LIB_DIR ${PROJECT_DIR}/lib)
set(BIN_DIR ${PROJECT_DIR}/bin)
set(RES_DIR ${PROJECT_DIR}/res)
set(SHADER_DIR ${PROJECT_DIR}/shaders)

#Clear bin/res directory
file(REMOVE_RECURSE ${BIN_DIR})

#Write executable to binary directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BIN_DIR})

#Compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m64 -std=c++11 -static-libgcc -static-libstdc++ -static -pthread -D M_PI_2=1.57079632679 -D M_PI=3.14159265359")

#Glob all source files for compilation
file(GLOB SOURCE_FILES ${SOURCE_DIR}/*.cpp)
add_executable(BscProject ${SOURCE_FILES})

#Copy resource files and shaders into binary directory
file(COPY ${SHADER_DIR} DESTINATION ${BIN_DIR})
file(COPY ${RES_DIR} DESTINATION ${BIN_DIR})

#############
# Libraries #
#############

#GLEW
include_directories(${LIB_DIR}/glew/include/)
file(GLOB GLEW_SOURCES ${LIB_DIR}/glew/src/*.cpp ${LIB_DIR}/glew/src/*.c)
add_library(GLEW STATIC ${GLEW_SOURCES})
target_link_libraries(BscProject GLEW)
add_definitions(-DGLEW_STATIC)

#OpenGL
find_package(OpenGL REQUIRED)
target_link_libraries(BscProject ${OPENGL_LIBRARIES})

#GLM
include_directories(${LIB_DIR}/glm/)

#Freetype
include_directories(${LIB_DIR}/freetype/include/)
add_subdirectory(${LIB_DIR}/freetype/)
target_link_libraries(BscProject freetype)

#GLFW
add_subdirectory(${LIB_DIR}/glfw/)
include_directories(${LIB_DIR}/glfw/include/)
target_link_libraries(BscProject glfw ${GLFW_LIBRARIES})

#AssImp
include_directories(${LIB_DIR}/assimp/include)
add_subdirectory(${LIB_DIR}/assimp/)
target_link_libraries(BscProject assimp)
