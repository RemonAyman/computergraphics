cmake_minimum_required(VERSION 3.20)
set(CMAKE_POLICY_VERSION_MINIMUM 3.5)

project(GraphicsAlgorithms)

set(CMAKE_CXX_STANDARD 17)

include(FetchContent)
FetchContent_Declare(
  freeglut
  GIT_REPOSITORY https://github.com/FreeGLUTProject/FreeGLUT.git
  GIT_TAG master
)

set(FREEGLUT_BUILD_SHARED_LIBS ON CACHE BOOL "" FORCE)
set(FREEGLUT_BUILD_STATIC_LIBS OFF CACHE BOOL "" FORCE)
set(FREEGLUT_GLES OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(freeglut)

find_package(OpenGL REQUIRED)

file(GLOB SOURCES "src/*.cpp")
list(FILTER SOURCES EXCLUDE REGEX "test_storage\\.cpp$")
file(GLOB HEADERS "src/*.h")

add_executable(GraphicsAlgorithms ${SOURCES} ${HEADERS})

target_link_libraries(GraphicsAlgorithms PRIVATE OpenGL::GL OpenGL::GLU freeglut comdlg32)
target_include_directories(GraphicsAlgorithms PRIVATE src)

# Copy DLL to executable directory
add_custom_command(TARGET GraphicsAlgorithms POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "$<TARGET_FILE:freeglut>"
    "$<TARGET_FILE_DIR:GraphicsAlgorithms>"
)
