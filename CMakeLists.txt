cmake_minimum_required(VERSION 3.10)

project(libzenomt VERSION 1.6.0)

find_package(OpenSSL)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# GoogleTest setup using FetchContent (C++11 compatible)
option(BUILD_TESTING "Build the testing tree" ON)
if(BUILD_TESTING)
	include(FetchContent)
	FetchContent_Declare(
		googletest
		GIT_REPOSITORY https://github.com/google/googletest.git
		GIT_TAG        v1.12.1  # C++11 compatible version
	)
	# For Windows: Prevent overriding the parent project's compiler/linker settings
	set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
	# GoogleTest v1.10.0 is compatible with C++11 and will inherit CMAKE_CXX_STANDARD from parent
	FetchContent_MakeAvailable(googletest)
endif()

add_library(zenomt STATIC
	src/Address.cpp
	src/Checksums.cpp
	src/Hex.cpp
	src/IndexSet.cpp
	src/Object.cpp
	src/RateTracker.cpp
	src/RunLoop.cpp
	src/SimpleWebSocket.cpp
	src/Timer.cpp
	src/URIParse.cpp
	src/WriteReceipt.cpp
)

if(OpenSSL_FOUND)
	target_sources(zenomt PRIVATE src/SimpleWebSocket_OpenSSL.cpp)
endif()

if(NOT WIN32)
	target_sources(zenomt PRIVATE
		src/EPollRunLoop.cpp
		src/Performer.cpp
		src/PosixStreamPlatformAdapter.cpp
		src/SelectRunLoop.cpp
	)
endif()

target_compile_options(zenomt PRIVATE -Os)

if(WIN32)
	target_compile_options(zenomt PRIVATE /permissive-)
	target_compile_definitions(zenomt PRIVATE WIN32_LEAN_AND_MEAN NOMINMAX)
endif()

target_include_directories(zenomt INTERFACE
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
	$<INSTALL_INTERFACE:include>)

if(OpenSSL_FOUND)
	target_link_libraries(zenomt PRIVATE OpenSSL::Crypto)
endif()

if(WIN32)
	target_link_libraries(zenomt PRIVATE ws2_32)
endif()

export (TARGETS zenomt NAMESPACE zenomt:: FILE zenomt-targets.cmake)

install (TARGETS zenomt
	EXPORT zenomt-targets
	LIBRARY DESTINATION lib
	INCLUDES DESTINATION include
)
install (DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/include/" DESTINATION include)
install (EXPORT zenomt-targets
	NAMESPACE zenomt::
	DESTINATION lib/cmake/zenomt
)

include (CMakePackageConfigHelpers)
write_basic_package_version_file (zenomt-config-version.cmake COMPATIBILITY SameMajorVersion)
configure_package_config_file (src/config.cmake.in zenomt-config.cmake
	INSTALL_DESTINATION lib/cmake/zenomt
)
install (FILES
	"${CMAKE_CURRENT_BINARY_DIR}/zenomt-config.cmake"
	"${CMAKE_CURRENT_BINARY_DIR}/zenomt-config-version.cmake"
	DESTINATION lib/cmake/zenomt
)

# Testing support
if(BUILD_TESTING)
	enable_testing()
	add_subdirectory(test)
endif()
