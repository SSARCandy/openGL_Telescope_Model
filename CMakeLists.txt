cmake_minimum_required(VERSION 3.0)

project( Telescope )

include_directories(${PROJECT_SOURCE_DIR}/lib)

add_executable( Telescope 
"${PROJECT_SOURCE_DIR}/src/Main.cpp" 
"${PROJECT_SOURCE_DIR}/lib/GLee.c" 
"${PROJECT_SOURCE_DIR}/lib/GLee.h" 
"${PROJECT_SOURCE_DIR}/lib/glFrame.h" 
"${PROJECT_SOURCE_DIR}/lib/glfrustum.h" 
"${PROJECT_SOURCE_DIR}/lib/glm.cpp" 
"${PROJECT_SOURCE_DIR}/lib/glm.h" 
"${PROJECT_SOURCE_DIR}/lib/gltools.cpp" 
"${PROJECT_SOURCE_DIR}/lib/gltools.h" 
"${PROJECT_SOURCE_DIR}/lib/math3d.cpp" 
"${PROJECT_SOURCE_DIR}/lib/math3d.h" 
"${PROJECT_SOURCE_DIR}/lib/stopwatch.h" 
"${PROJECT_SOURCE_DIR}/lib/TriangleMesh.cpp" 
"${PROJECT_SOURCE_DIR}/lib/TriangleMesh.h" 
"${PROJECT_SOURCE_DIR}/lib/VBOMesh.cpp" 
"${PROJECT_SOURCE_DIR}/lib/VBOMesh.h" 
"${PROJECT_SOURCE_DIR}/lib/wglext.h" 
)

#target_link_libraries( Telescope )