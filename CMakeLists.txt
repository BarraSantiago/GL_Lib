cmake_minimum_required(VERSION 3.10)
project(glLib)

# GL lib
file(GLOB_RECURSE GL_LIB_SOURCES "GL_Lib/src/*.cpp")
file(GLOB_RECURSE GL_LIB_SOURCES_C "GL_Lib/src/*.c")

find_package(OpenGL REQUIRED)
find_package(glfw3 3.3 REQUIRED)
find_package(glm CONFIG REQUIRED)

include_directories(GL_Lib/src GL_Lib/src/glad/include)
add_executable(glLib ${GL_LIB_SOURCES} ${GL_LIB_SOURCES_C})

if (CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    target_compile_options(glLib PRIVATE -fPIC)
endif()

target_link_libraries(glLib PRIVATE ${OPENGL_LIBRARIES} glfw glm::glm m)

target_include_directories(your_target_name PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/GL_Lib/src
)