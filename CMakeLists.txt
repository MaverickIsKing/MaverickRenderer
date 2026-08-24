cmake_minimum_required(VERSION 3.15)
project(MaverickRenderer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# project source files
file(GLOB_RECURSE APP_SOURCES CONFIGURE_DEPENDS
	"${CMAKE_SOURCE_DIR}/source/*.cpp"
	"${CMAKE_SOURCE_DIR}/source/*.h"
	"${CMAKE_SOURCE_DIR}/source/*.hpp"
)

add_executable(MaverickRenderer ${APP_SOURCES})

# additional header files include directories
target_include_directories(MaverickRenderer PRIVATE
	"${CMAKE_SOURCE_DIR}/minifb/include"
	"${CMAKE_SOURCE_DIR}/glm"
)

# adding minifb to project build
add_subdirectory(minifb)

# link libraries
target_link_libraries(MaverickRenderer PRIVATE minifb)
if (WIN32)
target_link_libraries(MaverickRenderer PRIVATE
	minifb
	opengl32
	winmm
	kernel32
	user32
	gdi32
	winspool
	shell32
	ole32
	oleaut32
	uuid
	comdlg32
	advapi32)
endif()