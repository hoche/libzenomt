# libzenomt: Complete Documentation Index

**Date:** 2024

## Overview

libzenomt is a lightweight C++11 foundation library for event-driven networking applications. This index provides quick access to all documentation.

---

## Core Components

### RunLoop System
- **[Basic Guide](runloop.md)** - Quick introduction
- **[Comprehensive Guide](runloop-comprehensive.md)** - Full documentation with interaction diagrams
  - Event loop architecture
  - SelectRunLoop vs EPollRunLoop
  - Descriptor management
  - Time management
  - Integration with timers and deferred tasks

### Timer System
- **[Comprehensive Guide](timer-comprehensive.md)** - Full documentation with interaction diagrams
  - Timer scheduling and firing
  - Recurrence and catch-up behavior
  - TimerList management
  - Rescheduling and cancellation

### Memory Management
- **[Basic Guide](memory.md)** - Quick introduction
- **[Comprehensive Guide](memory-comprehensive.md)** - Full documentation
  - Object reference counting
  - Retainer smart pointer
  - share_ref bridge to std::shared_ptr
  - Inheritance and polymorphism

### Address Handling
- **[Comprehensive Guide](address-comprehensive.md)** - Full documentation
  - IPv4 and IPv6 support
  - Encoding/decoding formats
  - Family conversion
  - DNS lookup
  - Presentation string format

### Cross-Thread Execution
- **[Quick Reference](performer-quick-reference.md)** - One-page cheat sheet
- **[Comprehensive Guide](performer-comprehensive.md)** - Full documentation with interaction diagrams
- **[Lifecycle Analysis](performer-lifecycle-analysis.md)** - Threading and destruction behavior
- **[Bug Analysis](performer-bug-analysis.md)** - Known issues and edge cases

---

## Streams and Protocols

### Streams System
- **[Basic Guide](streams.md)** - Quick introduction
- **[Comprehensive Guide](streams-comprehensive.md)** - Full documentation with interaction diagrams
  - IStreamPlatformAdapter interface
  - PosixStreamPlatformAdapter implementation
  - SimpleHttpStream HTTP parsing
  - SimpleWebSocket server implementation

---

## Utilities

### Utilities
- **[Basic Guide](utilities.md)** - Quick reference
- **[Comprehensive Guide](utilities-comprehensive.md)** - Full documentation
  - Hex encoding/decoding
  - URI parsing (RFC 3986)
  - Checksums (CRC-32, Internet checksum)
  - RateTracker
  - IndexSet
  - List (handle-based)
  - WriteReceipt
  - Priority levels
  - Algorithm helpers

---

## Analysis and Evaluation

### Code Analysis
- **[Code Evaluation](code-evaluation.md)** - Comprehensive code review
  - Strengths and weaknesses
  - Issues and concerns
  - Recommendations by priority

### Performer Analysis
- **[Lifecycle Analysis](performer-lifecycle-analysis.md)** - Threading and lifecycle behavior
- **[Bug Analysis](performer-bug-analysis.md)** - Critical bugs and fixes

---

## Quick Start Guides

### Getting Started
- **[Overview](overview.md)** - Library overview and architecture
- **[README](../README.md)** - Project README with build instructions

### Component Quick References
- **[Performer Quick Reference](performer-quick-reference.md)** - One-page Performer guide
- **[RunLoop Basic Guide](runloop.md)** - RunLoop introduction
- **[Memory Basic Guide](memory.md)** - Memory management introduction
- **[Streams Basic Guide](streams.md)** - Streams introduction
- **[Utilities Basic Guide](utilities.md)** - Utilities reference

---

## Documentation by Component

### Core Runtime
| Component | Documentation |
|-----------|---------------|
| RunLoop | [Basic](runloop.md), [Comprehensive](runloop-comprehensive.md) |
| Timer | [Comprehensive](timer-comprehensive.md) |
| Performer | [Quick Ref](performer-quick-reference.md), [Comprehensive](performer-comprehensive.md), [Lifecycle](performer-lifecycle-analysis.md), [Bugs](performer-bug-analysis.md) |
| Object/Retainer | [Basic](memory.md), [Comprehensive](memory-comprehensive.md) |

### Networking
| Component | Documentation |
|-----------|---------------|
| Address | [Comprehensive](address-comprehensive.md) |
| Streams | [Basic](streams.md), [Comprehensive](streams-comprehensive.md) |
| HeaderBodyStream | [Streams Comprehensive](streams-comprehensive.md#headerbodystream) |
| HTTP/WebSocket | [Streams Comprehensive](streams-comprehensive.md) |

### Utilities
| Component | Documentation |
|-----------|---------------|
| Hex | [Utilities Comprehensive](utilities-comprehensive.md#hex-encodingdecoding) |
| URIParse | [Utilities Comprehensive](utilities-comprehensive.md#uri-parsing) |
| Checksums | [Utilities Comprehensive](utilities-comprehensive.md#checksums) |
| RateTracker | [Utilities Comprehensive](utilities-comprehensive.md#ratetracker) |
| IndexSet | [Utilities Comprehensive](utilities-comprehensive.md#indexset) |
| List | [Utilities Comprehensive](utilities-comprehensive.md#list) |
| WriteReceipt | [Utilities Comprehensive](utilities-comprehensive.md#writereceipt) |
| IssuerWriteReceipt | [Utilities Comprehensive](utilities-comprehensive.md#issuerwritereceipt) |
| Priority | [Utilities Comprehensive](utilities-comprehensive.md#priority) |

---

## Documentation Features

All comprehensive documentation includes:

- ✅ **Architecture diagrams** - Component structure and relationships
- ✅ **Interaction diagrams** - Mermaid sequence diagrams showing execution flow
- ✅ **API reference** - Complete function signatures and behavior
- ✅ **Usage patterns** - Common use cases with code examples
- ✅ **Best practices** - Recommended approaches
- ✅ **Common pitfalls** - Mistakes to avoid with fixes
- ✅ **Performance notes** - Overhead and optimization tips
- ✅ **Thread safety** - Thread safety guarantees per operation

---

## Getting Help

### Finding Documentation

1. **By component name**: Use the component table above
2. **By task**: Check the Quick Start Guides
3. **By problem**: See Analysis documents for known issues
4. **By API**: Check comprehensive guides for full API reference

### Documentation Structure

- **Basic guides**: Quick introductions, minimal examples
- **Comprehensive guides**: Full documentation with diagrams and patterns
- **Analysis documents**: Deep dives into behavior and issues
- **Quick references**: One-page cheat sheets

---

## Component Coverage

### Fully Documented ✅
- RunLoop (basic + comprehensive)
- Timer (comprehensive)
- Performer (quick ref + comprehensive + analysis)
- Memory/Object/Retainer (basic + comprehensive)
- Address (comprehensive)
- Streams (basic + comprehensive)
- Utilities (basic + comprehensive)

### Implementation Files
- EPollRunLoop - Covered in RunLoop comprehensive
- SelectRunLoop - Covered in RunLoop comprehensive
- PosixStreamPlatformAdapter - Covered in Streams comprehensive
- SimpleWebSocket - Covered in Streams comprehensive
- All utility implementations - Covered in Utilities comprehensive

---

## Examples

### Minimal Example

```cpp
#include <zenomt/RunLoops.hpp>
#include <zenomt/Timer.hpp>

using namespace com::zenomt;

int main() {
    PreferredRunLoop loop;
    loop.scheduleRel(0.5, 0, true)->action = Timer::makeAction([&](Time){
        loop.stop();
    });
    loop.run();
}
```

### WebSocket Server Example

See [Streams Comprehensive Guide](streams-comprehensive.md#usage-patterns)

### Cross-Thread Example

See [Performer Comprehensive Guide](performer-comprehensive.md#usage-patterns)

---

## Related Documentation

- **[README](../README.md)** - Project overview and build instructions
- **[Code Evaluation](code-evaluation.md)** - Code quality assessment
- **[Overview](overview.md)** - High-level architecture

---

## Documentation Status

| Component | Status | Documentation |
|-----------|--------|---------------|
| RunLoop | ✅ Complete | Basic + Comprehensive |
| Timer | ✅ Complete | Comprehensive |
| Performer | ✅ Complete | Quick Ref + Comprehensive + Analysis |
| Memory | ✅ Complete | Basic + Comprehensive |
| Address | ✅ Complete | Comprehensive |
| Streams | ✅ Complete | Basic + Comprehensive |
| Utilities | ✅ Complete | Basic + Comprehensive |
| EPollRunLoop | ✅ Complete | Covered in RunLoop |
| SelectRunLoop | ✅ Complete | Covered in RunLoop |
| PosixStreamPlatformAdapter | ✅ Complete | Covered in Streams |
| SimpleWebSocket | ✅ Complete | Covered in Streams |
| Hex | ✅ Complete | Covered in Utilities |
| URIParse | ✅ Complete | Covered in Utilities |
| Checksums | ✅ Complete | Covered in Utilities |
| RateTracker | ✅ Complete | Covered in Utilities |
| IndexSet | ✅ Complete | Covered in Utilities |
| List | ✅ Complete | Covered in Utilities |
| WriteReceipt | ✅ Complete | Covered in Utilities |
| Priority | ✅ Complete | Covered in Utilities |
| Algorithm | ✅ Complete | Covered in Utilities |

**All components are fully documented!**

