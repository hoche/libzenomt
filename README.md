zenomt Networking Library
=========================
Lightweight C++11 utilities for event‑driven (especially networking) programs. Extracted from https://github.com/zenomt/rtmfp-cpp.

Features
--------
- Run loops (`SelectRunLoop`, `EPollRunLoop`) with timers and deferred tasks
- Cross‑thread task handoff (`Performer`)
- Stream platform adapter (`PosixStreamPlatformAdapter`) for sockets
- Simple HTTP parsing and WebSocket server (`SimpleWebSocket`)
- Utilities: `Object`/`Retainer`, `List`, `IndexSet`, `RateTracker`, `Checksums`, `Hex`, `URIParse`, `Address`, `Priority`, `WriteReceipt`

Documentation
-------------
- **[Complete Index](docs/index.md)** - Master documentation index

**Core Components:**
- **RunLoop:** [Basic](docs/runloop.md) | [Comprehensive](docs/runloop-comprehensive.md) - Event loop with interaction diagrams
- **Timer:** [Comprehensive](docs/timer-comprehensive.md) - Timer scheduling with interaction diagrams
- **Memory Management:** [Basic](docs/memory.md) | [Comprehensive](docs/memory-comprehensive.md) - Object and Retainer
- **Address:** [Comprehensive](docs/address-comprehensive.md) - IPv4/IPv6 address handling
- **Performer (Cross-Thread):** [Quick Ref](docs/performer-quick-reference.md) | [Comprehensive](docs/performer-comprehensive.md) | [Lifecycle](docs/performer-lifecycle-analysis.md) | [Bugs](docs/performer-bug-analysis.md)

**Streams and Protocols:**
- **Streams:** [Basic](docs/streams.md) | [Comprehensive](docs/streams-comprehensive.md) - HTTP/WebSocket with interaction diagrams

**Utilities:**
- **Utilities:** [Basic](docs/utilities.md) | [Comprehensive](docs/utilities-comprehensive.md) - Hex, URI, Checksums, RateTracker, IndexSet, List, WriteReceipt, Priority

**Analysis:**
- [Code Evaluation](docs/code-evaluation.md) - Comprehensive code review

This code is extracted, forked, and reorganized from the author's public
Github repository at
[`https://github.com/zenomt/rtmfp-cpp`](https://github.com/zenomt/rtmfp-cpp).

Building
--------
The WebSocket server code requires a SHA1 implementation. An implementation
using OpenSSL is in
[src/SimpleWebSocket_OpenSSL.cpp](src/SimpleWebSocket_OpenSSL.cpp). If you
don't have OpenSSL or don't want to use it, you can define Make variable
`WITHOUT_OPENSSL`. If OpenSSL's incude and library directories aren't in the
default paths for your compiler, you can set Make variables `OPENSSL_INCLUDEDIR`
and `OPENSSL_LIBDIR` with appropriate directives (see the [Makefile](Makefile)
for what they should look like). For example, to build the library and run
the tests with Homebrew supplying OpenSSL:

    $ make OPENSSL_INCLUDEDIR=-I`brew --prefix openssl@3`/include/ OPENSSL_LIBDIR=-L`brew --prefix openssl@3`/lib/ ci

A CMakeLists.txt file is included, suitable for `add_subdirectory()` or
`find_package()`. The CMakeLists.txt file only builds the library; it does
not build or execute any tests at this time.

To tell `cmake` where your OpenSSL is, and to build for an older version of Mac OS
(`10.12` in this example):

    $ mkdir build
    $ cd build
    $ cmake -DCMAKE_OSX_DEPLOYMENT_TARGET=10.12 -DOPENSSL_ROOT_DIR=`brew --prefix openssl@3` ..
    $ make

Quick Start
-----------

```cpp
#include <zenomt/RunLoops.hpp>
#include <zenomt/Timer.hpp>

using namespace com::zenomt;

int main() {
  PreferredRunLoop loop;
  loop.scheduleRel(0.5, 0, true)->action = Timer::makeAction([&](Time){
    // do something later
    loop.stop();
  });
  loop.run();
}
```

For a WebSocket server example and architecture diagrams, see the docs linked above.
