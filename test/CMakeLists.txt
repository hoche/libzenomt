cmake_minimum_required(VERSION 3.10)

# GoogleTest is fetched in the main CMakeLists.txt
# Test executable
set(TEST_SOURCES
	test_runloop.cpp
	test_timer.cpp
	test_object.cpp
	test_retainer.cpp
	test_hex.cpp
	test_uriparse.cpp
	test_address.cpp
	test_checksums.cpp
	test_ratetracker.cpp
)

# Only build Performer tests on non-Windows (requires POSIX)
if(NOT WIN32)
	list(APPEND TEST_SOURCES
		test_performer.cpp
		test_performer_posix.cpp
	)
endif()

add_executable(zenomt_tests ${TEST_SOURCES})

target_link_libraries(zenomt_tests
	PRIVATE
	zenomt
	gtest
	gtest_main
	gmock
)

# Link pthread on POSIX systems
if(NOT WIN32)
	target_link_libraries(zenomt_tests PRIVATE pthread)
endif()

target_include_directories(zenomt_tests PRIVATE
	${CMAKE_SOURCE_DIR}/include
)

# Add test to CTest
include(GoogleTest)
gtest_discover_tests(zenomt_tests)

