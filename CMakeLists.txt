cmake_minimum_required (VERSION 2.6)

project (MinOpHash++)

#set( CMAKE_CONFIGURATION_TYPES "Debug" "Release" )

set( CMAKE_VERBOSE_MAKEFILE on )

set( CMAKE_C_STANDARD 99 )
set( CMAKE_C_EXTENSIONS OFF )
set( CMAKE_CXX_STANDARD 17 )
set( CMAKE_CXX_EXTENSIONS OFF )


add_compile_options( -Wall -Wextra -Wconversion )
add_link_options( LINKER:--gc-sections )

if( CMAKE_BUILD_TYPE MATCHES "Deb" )
	add_compile_options( -g2 -Og )
else()
	add_compile_options( -g0 )
	add_link_options( -s )
endif()


file( GLOB_RECURSE MYPROJECT_SRC "src/*.cpp" "src/*.c" )

add_executable(MinOpHash++ ${MYPROJECT_SRC})
target_link_libraries(MinOpHash++ jsoncpp)
