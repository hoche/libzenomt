# GoogleTest Unit Tests

This directory contains GoogleTest-based unit tests for libzenomt.

## Building Tests

The tests are automatically built when `BUILD_TESTING` is enabled (default):

```bash
mkdir build
cd build
cmake ..
make
```

To disable tests:
```bash
cmake -DBUILD_TESTING=OFF ..
```

## Running Tests

### Using CTest (Recommended)
```bash
cd build
ctest
ctest --verbose  # For detailed output
ctest -R RunLoopTest  # Run specific test suite
```

### Running Directly
```bash
cd build/test
./zenomt_tests
./zenomt_tests --gtest_filter=RunLoopTest.*  # Run specific tests
```

## Test Coverage

### Core Components
- **RunLoop**: Basic timer scheduling, doLater, onEveryCycle, time functions
- **Timer**: Absolute/relative scheduling, recurrence, cancellation, rescheduling
- **Performer**: Async/sync performs, cross-thread execution (POSIX only)

### Utilities
- **Object**: Reference counting, retain/release, share_ref
- **Retainer**: Smart pointer operations, swap, move, inheritance
- **Hex**: Encoding/decoding, round-trip tests
- **URIParse**: URI parsing, query/fragment handling, percent decoding
- **Address**: IPv4/IPv6 handling, serialization, equality
- **Checksums**: in_cksum, CRC32 (little/big endian)
- **RateTracker**: Rate calculation, window expiry, sliding window

## Adding New Tests

1. Create a new test file: `test_<component>.cpp`
2. Add it to `TEST_SOURCES` in `test/CMakeLists.txt`
3. Follow the existing test structure:
   ```cpp
   #include <gtest/gtest.h>
   #include "zenomt/Component.hpp"
   
   TEST(ComponentTest, TestName) {
       // Test code
   }
   ```

## Notes

- Performer tests are only built on POSIX systems (not Windows)
- Some tests use timing which may be sensitive to system load
- Tests are designed to be deterministic and fast

