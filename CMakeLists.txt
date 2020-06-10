cmake_minimum_required(VERSION 3.0)

project( Telescope )

SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)

include_directories(${OPENGL_INCLUDE_DIRS} ${GLUT_INCLUDE_DIRS})
include_directories(${PROJECT_SOURCE_DIR}/)
include_directories(${PROJECT_SOURCE_DIR}/lib)

add_executable( Telescope 
    "${PROJECT_SOURCE_DIR}/src/TypeDef.h" 
    "${PROJECT_SOURCE_DIR}/src/Drawer.h" 
    "${PROJECT_SOURCE_DIR}/src/Main.cpp" 
)

target_link_libraries(Telescope ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} GLEW)