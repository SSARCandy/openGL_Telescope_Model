cmake_minimum_required(VERSION 3.0)

project( Telescope )

SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)

if (${CMAKE_SYSTEM_NAME} MATCHES "Emscripten")
    set(CMAKE_C_COMPILER "em++")
    include_directories(
        /home/ssarcandy/github/emsdk/upstream/emscripten/system/include
    )
endif ()

find_package(OpenGL REQUIRED)

if (NOT ${CMAKE_SYSTEM_NAME} MATCHES "Emscripten")
    find_package(GLEW REQUIRED)
    find_package(GLUT REQUIRED)
endif ()

include_directories(${OPENGL_INCLUDE_DIRS} ${GLUT_INCLUDE_DIRS})
include_directories(${PROJECT_SOURCE_DIR}/)
include_directories(${PROJECT_SOURCE_DIR}/lib)

add_executable( Telescope 
    "${PROJECT_SOURCE_DIR}/src/TypeDef.h" 
    "${PROJECT_SOURCE_DIR}/src/Drawer.h" 
    "${PROJECT_SOURCE_DIR}/src/Main.cpp" 
)


if (${CMAKE_SYSTEM_NAME} MATCHES "Emscripten")
    target_link_libraries(Telescope ${OPENGL_LIBRARIES})
  #  set_target_properties(Telescope PROPERTIES LINK_FLAGS "-o dist/engine.js -s USE_FREETYPE=1 -s DISABLE_EXCEPTION_CATCHING=0 -s DEMANGLE_SUPPORT=1 -s SAFE_HEAP=1 --bind -s WASM=1 -O2 -s LEGACY_GL_EMULATION=0  -s GL_UNSAFE_OPTS=0 -s ASSERTIONS=1 -s GL_ASSERTIONS=1 -s INVOKE_RUN=0  -std=c++11 -s USE_WEBGL2=1 -s FULL_ES3=1 -s USE_GLFW=3 -s OFFSCREENCANVAS_SUPPORT=1")
else ()
    target_link_libraries(Telescope ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} GLEW)
endif ()
