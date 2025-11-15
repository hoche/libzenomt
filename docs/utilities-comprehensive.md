# Utilities: Comprehensive Documentation

**Date:** 2024

## Overview

This document covers all utility components in libzenomt: Hex encoding/decoding, URI parsing, checksums, rate tracking, index sets, lists, write receipts, and priority levels.

---

## Table of Contents

1. [Hex Encoding/Decoding](#hex-encodingdecoding)
2. [URI Parsing](#uri-parsing)
3. [Checksums](#checksums)
4. [RateTracker](#ratetracker)
5. [IndexSet](#indexset)
6. [List](#list)
7. [WriteReceipt](#writereceipt)
8. [IssuerWriteReceipt](#issuerwritereceipt)
9. [Priority](#priority)
10. [Algorithm Helpers](#algorithm-helpers)

---

## Hex Encoding/Decoding

### Overview

`Hex` provides utilities for encoding binary data to hexadecimal strings and decoding hex strings back to binary data, with support for debugging output.

### API Reference

#### Encoding

```cpp
static std::string encode(const void *bytes, size_t len);
static std::string encode(const void *bytes, const void *limit);
static std::string encode(const std::vector<uint8_t> &bytes);
```

**Returns:** Uppercase hexadecimal string (e.g., "00FF1234")

**Example:**
```cpp
uint8_t data[] = {0x00, 0xFF, 0x12, 0x34};
std::string hex = Hex::encode(data, 4);
// hex == "00FF1234"
```

#### Decoding

```cpp
static bool decode(const char *hex, std::vector<uint8_t> &dst);
```

**Parameters:**
- `hex`: Hex string (may contain spaces, case-insensitive)
- `dst`: Output vector (cleared and populated)

**Returns:** `true` if successful, `false` on error

**Example:**
```cpp
std::vector<uint8_t> data;
if(Hex::decode("00 FF 12 34", data)) {
    // data == {0x00, 0xFF, 0x12, 0x34}
}
```

#### Byte Decoding

```cpp
static int decodeByte(const char *hex);
```

**Returns:** Decoded byte (0-255) or -1 on error

**Behavior:**
- Decodes exactly 2 hex digits
- Stops at first invalid character
- Case-insensitive

**Example:**
```cpp
int byte = Hex::decodeByte("FF");  // Returns 255
int byte2 = Hex::decodeByte("A0"); // Returns 160
int byte3 = Hex::decodeByte("G0"); // Returns -1 (error)
```

#### Digit Decoding

```cpp
static int decodeDigit(char d);
```

**Returns:** 0-15 for valid hex digit, -1 for invalid

**Example:**
```cpp
int d1 = Hex::decodeDigit('A');  // Returns 10
int d2 = Hex::decodeDigit('5');  // Returns 5
int d3 = Hex::decodeDigit('G');  // Returns -1
```

#### Debugging Output

```cpp
static void print(const char *msg, const void *bytes, size_t len);
static void dump(const char *msg, const void *bytes, size_t len, bool nl = true);
```

**Behavior:**
- `print()`: Single-line hex output
- `dump()`: Multi-line hex dump with offsets

**Example:**
```cpp
uint8_t data[16] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
Hex::print("Data", data, 16);
// Output: Data: 000102030405060708090A0B0C0D0E0F

Hex::dump("Data", data, 16);
// Output:
// Data:
// 0000: 00 01 02 03 04 05 06 07  08 09 0A 0B 0C 0D 0E 0F
```

### Usage Patterns

#### Round-Trip Encoding/Decoding

```cpp
uint8_t original[] = {0x12, 0x34, 0x56, 0x78};
std::string encoded = Hex::encode(original, 4);

std::vector<uint8_t> decoded;
Hex::decode(encoded, decoded);
// decoded == original
```

#### Parsing Hex Strings with Spaces

```cpp
std::vector<uint8_t> data;
Hex::decode("00 FF AB CD", data);  // Spaces are ignored
// data == {0x00, 0xFF, 0xAB, 0xCD}
```

#### Error Handling

```cpp
std::vector<uint8_t> data;
if(!Hex::decode("invalid", data)) {
    // Handle error
}
```

---

## URI Parsing

### Overview

`URIParse` provides RFC 3986-compliant URI parsing and manipulation utilities, including component extraction, relative reference resolution, and percent decoding.

### URI Structure

```
┌─────────────────────────────────────────┐
│  URI Components                         │
│  scheme://user:pass@host:port/path?q#f  │
│  └─┬─┘ └──────┬──────┘ └─┬─┘ └─┬─┘└┬┘ │
│  scheme   authority    path  q  f   │
│            └─┬─┘                      │
│          userinfo@hostinfo            │
└─────────────────────────────────────────┘
```

### API Reference

#### Construction and Parsing

```cpp
URIParse();
URIParse(const std::string &uri_);
void parse(const std::string &uri_);
```

**Behavior:**
- Default constructor: Creates empty URI
- Constructor with string: Parses immediately
- `parse()`: Re-parses URI, populates all components

#### Component Access

All components are public string members:

```cpp
std::string uri;              // Full URI
std::string scheme;           // Scheme (lowercase)
std::string canonicalScheme;  // Canonical scheme
std::string authority;        // Authority (userinfo@hostinfo)
std::string host;             // Hostname/IP
std::string port;             // Port number
std::string path;             // Path component
std::string query;            // Query string
std::string fragment;        // Fragment
std::string origin;           // scheme://authority
```

#### Relative Reference Resolution

```cpp
std::string transformRelativeReference(const std::string &relativeUri) const;
```

**Behavior:** Resolves relative URI against base URI (RFC 3986 §5.2.1)

**Example:**
```cpp
URIParse base("http://example.com/foo/bar");
std::string resolved = base.transformRelativeReference("../baz");
// resolved == "http://example.com/foo/baz"
```

#### Path Merging

```cpp
std::string mergedRelativePath(const std::string &relativePath) const;
```

**Behavior:** Merges relative path with base path (RFC 3986 §5.2.3)

#### Dot Segment Removal

```cpp
static std::string removeDotSegments(const std::string &path);
```

**Behavior:** Removes `.` and `..` segments from path (RFC 3986 §5.2.4)

**Example:**
```cpp
std::string path = "/a/b/../c/./d";
std::string cleaned = URIParse::removeDotSegments(path);
// cleaned == "/a/c/d"
```

#### String Splitting

```cpp
static std::vector<std::string> split(const std::string &str, char sep, size_t maxParts = 0);
static std::vector<std::string> split(const std::string &str, const std::string &seps, size_t maxParts = 0);
```

**Parameters:**
- `str`: String to split
- `sep`/`seps`: Separator(s)
- `maxParts`: Maximum parts (0 = unlimited)

**Example:**
```cpp
auto parts = URIParse::split("a:b:c", ':', 2);
// parts == {"a", "b:c"}

auto parts2 = URIParse::split("a,b;c", ",;");
// parts2 == {"a", "b", "c"}
```

#### Percent Decoding

```cpp
static std::string percentDecode(const std::string &str);
static std::string safePercentDecode(const std::string &str);
```

**Behavior:**
- `percentDecode()`: Returns empty string on error
- `safePercentDecode()`: Returns original string on error

**Example:**
```cpp
std::string decoded = URIParse::percentDecode("hello%20world");
// decoded == "hello world"

std::string safe = URIParse::safePercentDecode("hello%2Gworld");
// safe == "hello%2Gworld" (error, returns original)
```

#### Case Conversion

```cpp
static std::string lowercase(const std::string &str);
```

**Behavior:** Converts ASCII string to lowercase (not Unicode-aware)

### Usage Patterns

#### Parse URI

```cpp
URIParse uri("http://user:pass@example.com:8080/path?query=value#fragment");
// uri.scheme == "http"
// uri.host == "example.com"
// uri.port == "8080"
// uri.path == "/path"
// uri.query == "query=value"
// uri.fragment == "fragment"
```

#### Resolve Relative URI

```cpp
URIParse base("http://example.com/base/path");
std::string resolved = base.transformRelativeReference("../other");
// resolved == "http://example.com/base/other"
```

#### Extract Components

```cpp
URIParse uri("rtmfp://host:1935/path");
if(uri.scheme == "rtmfp" && uri.port == "1935") {
    // Handle RTMFP connection
}
```

---

## Checksums

### Overview

Provides CRC-32 (little-endian and big-endian) and Internet checksum algorithms for data integrity verification.

### API Reference

#### CRC-32 Little-Endian

```cpp
uint32_t crc32_le(uint32_t crc, const void *buf, size_t len);
uint32_t crc32_le(const void *buf, size_t len);
```

**Parameters:**
- `crc`: Previous CRC value (0xFFFFFFFF for initial)
- `buf`: Data buffer
- `len`: Data length

**Returns:** CRC-32 value (typically inverted for final result)

**Incremental Usage:**
```cpp
uint32_t crc = 0xFFFFFFFF;
crc = crc32_le(crc, data1, len1);
crc = crc32_le(crc, data2, len2);
crc ^= 0xFFFFFFFF;  // Finalize
```

**Single Call:**
```cpp
uint32_t crc = crc32_le(data, len);
crc ^= 0xFFFFFFFF;  // Finalize
```

#### CRC-32 Big-Endian

```cpp
uint32_t crc32_be(uint32_t crc, const void *buf, size_t len);
uint32_t crc32_be(const void *buf, size_t len);
```

**Behavior:** Same as little-endian but uses big-endian byte order

#### Internet Checksum

```cpp
uint16_t in_cksum(const void *buf, size_t len);
```

**Returns:** 16-bit Internet checksum (used in IP, TCP, UDP headers)

**Behavior:**
- One's complement sum
- Handles byte swapping automatically
- Returns value ready for header insertion

### Usage Patterns

#### Verify Data Integrity

```cpp
uint32_t crc = crc32_le(data, len);
crc ^= 0xFFFFFFFF;

// Later, verify
uint32_t verify = crc32_le(data, len);
verify ^= 0xFFFFFFFF;
if(crc == verify) {
    // Data intact
}
```

#### Incremental CRC

```cpp
uint32_t crc = 0xFFFFFFFF;
crc = crc32_le(crc, header, headerLen);
crc = crc32_le(crc, payload, payloadLen);
crc ^= 0xFFFFFFFF;  // Final CRC
```

#### Network Checksum

```cpp
uint16_t checksum = in_cksum(packet, packetLen);
// Use in IP/TCP/UDP header
```

---

## RateTracker

### Overview

`RateTracker` estimates data transfer rate using a sliding window approach, useful for bandwidth monitoring and rate limiting.

### Architecture

```mermaid
flowchart LR
    A[addBytes] --> B[Sliding Window]
    B --> C[getRate]
    C --> D[Bytes/Second]
    
    E[Time] --> B
    F[Window Period] --> B
```

### API Reference

#### Construction

```cpp
RateTracker(Duration windowPeriod = 1.0);
```

**Parameters:**
- `windowPeriod`: Time window in seconds (default: 1.0)

#### Updating

```cpp
void update(size_t count, Time now);
```

**Parameters:**
- `count`: Number of bytes/items
- `now`: Current time

**Behavior:**
- Adds count to window
- Removes entries older than window period
- Maintains sliding window

#### Querying Rate

```cpp
double getRate(Time now) const;
```

**Returns:** Rate in bytes/second (or items/second)

**Calculation:**
- Sums all entries within window period
- Divides by window period

#### Configuration

```cpp
void setWindowPeriod(Duration windowPeriod);
Time getWindowPeriod() const;
```

**Behavior:**
- `setWindowPeriod()`: Changes window size
- `getWindowPeriod()`: Returns current window period

#### Reset

```cpp
void reset();
```

**Behavior:** Clears all data, resets to initial state

### Usage Patterns

#### Monitor Transfer Rate

```cpp
RateTracker tracker(1.0);  // 1 second window

// During transfer
tracker.update(bytesTransferred, loop.getCurrentTime());
double rate = tracker.getRate(loop.getCurrentTime());
// rate == bytes per second
```

#### Bandwidth Throttling

```cpp
RateTracker tracker(1.0);
const double maxRate = 1000000.0;  // 1 MB/s

while(transferring) {
    double currentRate = tracker.getRate(now);
    if(currentRate < maxRate) {
        sendMoreData();
        tracker.update(bytesSent, now);
    } else {
        wait();
    }
}
```

---

## IndexSet

### Overview

`IndexSet` manages disjoint ranges of indices efficiently, useful for tracking received data segments, available ranges, or sparse index sets.

### Architecture

```mermaid
flowchart LR
    A[IndexSet] --> B[Range 1: 0-99]
    A --> C[Range 2: 200-299]
    A --> D[Range 3: 500-599]
    
    B --> E[Operations]
    C --> E
    D --> E
    
    E --> F[add/remove/contains]
```

### API Reference

#### Construction

```cpp
IndexSet();
IndexSet(const IndexSet &other);
```

#### Querying

```cpp
uintmax_t size() const;              // Total indices
size_t countRanges() const;           // Number of ranges
bool empty() const;                   // Is empty
bool contains(uintmax_t anIndex) const;
uintmax_t lowestIndex() const;        // Smallest index
uintmax_t highestIndex() const;       // Largest index
Range firstRange() const;             // First range
Range lastRange() const;              // Last range
```

#### Adding

```cpp
void add(uintmax_t fromIndex, uintmax_t toIndex);
void add(uintmax_t anIndex);  // Single index
void add(const IndexSet& other);
```

**Behavior:**
- Merges contiguous ranges
- Maintains disjoint property

#### Removing

```cpp
void remove(uintmax_t fromIndex, uintmax_t toIndex);
void remove(uintmax_t anIndex);
void remove(const IndexSet& other);
```

**Behavior:**
- Splits ranges if needed
- Maintains disjoint property

#### Iteration

```cpp
bool extentsDo(const std::function<bool(uintmax_t from, uintmax_t to)> &each_f) const;
bool indicesDo(const std::function<bool(uintmax_t eachIndex)> &each_f) const;
```

**Behavior:**
- `extentsDo()`: Iterates over ranges [from, to]
- `indicesDo()`: Iterates over each individual index
- Returns `false` to stop iteration

#### Clear

```cpp
void clear();
```

### Range Structure

```cpp
struct Range {
    uintmax_t start;
    uintmax_t end;
    
    uintmax_t size() const;
    bool intersects(uintmax_t from, uintmax_t to) const;
    bool contains(uintmax_t anIndex) const;
    void extend(uintmax_t from, uintmax_t to);
};
```

### Usage Patterns

#### Track Received Segments

```cpp
IndexSet received;

// Receive segment 0-99
received.add(0, 99);

// Receive segment 200-299
received.add(200, 299);

// Check if complete
if(received.contains(0) && received.contains(99) && 
   received.contains(200) && received.contains(299)) {
    // Segments received
}
```

#### Find Missing Ranges

```cpp
IndexSet received;
received.add(0, 99);
received.add(200, 299);

IndexSet all;
all.add(0, 999);

all.remove(received);
// all now contains missing ranges
```

#### Iterate Over Ranges

```cpp
IndexSet set;
set.add(0, 99);
set.add(200, 299);

set.extentsDo([](uintmax_t from, uintmax_t to) {
    std::cout << "Range: [" << from << ", " << to << "]" << std::endl;
    return true;  // Continue
});
```

---

## List

### Overview

`List<T>` is a handle-based doubly-linked list with O(1) insertion and removal. Uses integer handles instead of iterators, making it safe to modify during iteration.

### Architecture

```mermaid
flowchart LR
    A[List] --> B[Sentinel Node]
    B --> C[Node 1]
    C --> D[Node 2]
    D --> E[Node 3]
    E --> B
    
    F[Free List] --> G[Unused Nodes]
```

### API Reference

#### Construction

```cpp
List(const T& blank = T());
```

**Parameters:**
- `blank`: Default value for new nodes

#### Navigation

```cpp
long first() const;
long last() const;
long next(long name) const;
long prev(long name) const;
```

**Returns:** Handle (long) or `SENTINEL` (0) if not found

#### Access

```cpp
T& at(long name);
const T& at(long name) const;
T& firstValue();
T& lastValue();
```

**Throws:** `std::out_of_range` if handle invalid (or aborts if no exceptions)

#### Insertion

```cpp
long append(const T& val);
long prepend(const T& val);
long addAfter(long name);
long addAfter(const T& val, long name);
long addBefore(long name);
long addBefore(const T& val, long name);
```

**Returns:** Handle to new node

#### Removal

```cpp
bool remove(long name);
bool removeFirst();
bool removeLast();
void clear();
```

**Returns:** `true` if removed, `false` if not found

#### Querying

```cpp
bool empty() const;
size_t size() const;
bool has(long name) const;
long find(const T& val) const;
```

#### Movement

```cpp
bool moveNameToHead(long name);
bool moveNameToTail(long name);
bool rotateNameToHead(long name);
bool rotateNameToTail(long name);
```

#### Iteration

```cpp
bool valuesDo(const std::function<bool(T& value)> &pred);
bool safeValuesDo(const std::function<bool(T& value)> &pred);
```

**Behavior:**
- `valuesDo()`: Iterates, can modify list (use handles)
- `safeValuesDo()`: Copies list first, safe for modifications

### SumList

```cpp
template <class T>
class SumList : public List<T> {
    SumList(const Size_f size_f, const T& blank = T());
    size_t sum() const;
};
```

**Behavior:**
- Tracks sum of values via size function
- Automatically updates sum on add/remove

### Usage Patterns

#### Basic List Operations

```cpp
List<int> list;

long h1 = list.append(10);
long h2 = list.append(20);
long h3 = list.append(30);

// Iterate
for(long h = list.first(); h != List<int>::SENTINEL; h = list.next(h)) {
    std::cout << list.at(h) << std::endl;
}
```

#### Safe Iteration with Modification

```cpp
List<int> list;
// ... populate ...

list.safeValuesDo([](int& value) {
    if(value < 0) {
        // Can safely modify original list here
        return false;  // Stop iteration
    }
    return true;
});
```

#### SumList for Tracking Sizes

```cpp
SumList<std::string> list([](const std::string& s) { return s.size(); });

list.append("hello");   // sum = 5
list.append("world");   // sum = 10
size_t total = list.sum();  // 10
```

---

## WriteReceipt

### Overview

`WriteReceipt` tracks message transmission deadlines and completion status, supporting retransmission control and dependent message chains.

### Architecture

```mermaid
flowchart TD
    A[WriteReceipt] --> B[startBy]
    A --> C[finishBy]
    A --> D[retransmit]
    A --> E[parent]
    A --> F[onFinished]
    
    G[WriteReceiptChain] --> H[Chain of Receipts]
    H --> I[Parent Linkage]
```

### API Reference

#### Construction

```cpp
WriteReceipt(Time origin, Duration startWithin, Duration finishWithin);
```

**Parameters:**
- `origin`: Creation time
- `startWithin`: Deadline to start transmission
- `finishWithin`: Deadline to finish transmission

**Effects:**
- `startBy = origin + startWithin`
- `finishBy = origin + finishWithin`
- `retransmit = true` (default)

#### Deadlines

```cpp
Time startBy;   // Must start by this time
Time finishBy;  // Must finish by this time
```

#### Control

```cpp
void abandon();
void abandonIfNeeded(Time now);
void setStartWithin(Duration age);
void setFinishWithin(Duration age);
```

**Behavior:**
- `abandon()`: Immediately abandon message
- `abandonIfNeeded()`: Abandon if past deadline
- `setStartWithin()`: Update start deadline
- `setFinishWithin()`: Update finish deadline

#### Status

```cpp
Time createdAt() const;
bool isAbandoned() const;
bool isStarted() const;
bool isDelivered() const;
bool isFinished() const;
```

#### Retransmission

```cpp
bool retransmit;  // Default: true
```

**Behavior:**
- `true`: Fragments can be retransmitted
- `false`: Best-effort, send once only

#### Parent Chain

```cpp
std::shared_ptr<WriteReceipt> parent;
```

**Behavior:**
- If parent is abandoned, this receipt is abandoned
- Useful for dependent messages (e.g., video frames)

#### Callback

```cpp
std::function<void(bool wasAbandoned)> onFinished;
```

**Called when:** Message is finished (delivered or abandoned)

### WriteReceiptChain

```cpp
class WriteReceiptChain {
    void append(std::shared_ptr<WriteReceipt> receipt);
    void expire(Time deadline);
    void expire(Time startDeadline, Time finishDeadline);
};
```

**Behavior:**
- `append()`: Chains receipts (sets parent linkage)
- `expire()`: Updates deadlines and clears chain

### Usage Patterns

#### Basic Write Receipt

```cpp
Time now = loop.getCurrentTime();
auto receipt = std::make_shared<WriteReceipt>(now, 0.02, 0.2);
receipt->onFinished = [](bool abandoned) {
    if(abandoned) {
        // Handle timeout
    } else {
        // Handle success
    }
};
```

#### Dependent Messages

```cpp
WriteReceiptChain chain;

auto frame1 = std::make_shared<WriteReceipt>(now, 0.02, 0.2);
chain.append(frame1);

auto frame2 = std::make_shared<WriteReceipt>(now, 0.02, 0.2);
chain.append(frame2);  // frame2->parent = frame1

// If frame1 is abandoned, frame2 is also abandoned
```

#### Deadline Checking

```cpp
auto receipt = std::make_shared<WriteReceipt>(now, 0.02, 0.2);

// Periodically check
receipt->abandonIfNeeded(loop.getCurrentTime());
if(receipt->isAbandoned()) {
    // Handle timeout
}
```

---

## IssuerWriteReceipt

### Overview

`IssuerWriteReceipt` is a specialized subclass of `WriteReceipt` designed for protocol layers that need to track internal reference counting. It provides automatic use count management for tracking when all parts of a message (fragments, segments) have been delivered.

**Purpose:**
- Internal use count tracking for protocol layers
- Automatic completion when all fragments are delivered
- Simplifies multi-fragment message tracking
- Designed for sender-side (issuer) tracking

### Use Count Semantics

```mermaid
sequenceDiagram
    participant App as Application
    participant IWR as IssuerWriteReceipt
    participant Protocol as Protocol Layer

    App->>IWR: new IssuerWriteReceipt(...)
    Note over IWR: m_useCount = 0
    
    Protocol->>IWR: useCountUp() (fragment 1 sent)
    Note over IWR: m_useCount = 1
    
    Protocol->>IWR: useCountUp() (fragment 2 sent)
    Note over IWR: m_useCount = 2
    
    Protocol->>IWR: useCountUp() (fragment 3 sent)
    Note over IWR: m_useCount = 3
    
    Protocol->>IWR: useCountDown() (fragment 1 ACKed)
    Note over IWR: m_useCount = 2
    
    Protocol->>IWR: useCountDown() (fragment 2 ACKed)
    Note over IWR: m_useCount = 1
    
    Protocol->>IWR: useCountDown() (fragment 3 ACKed)
    Note over IWR: m_useCount = 0
    IWR->>IWR: isFinished() -> true
    IWR->>App: onFinished(false) - delivered
```

**Behavior:**
- Use count represents outstanding operations (fragments in flight)
- Increment when starting a new operation
- Decrement when operation completes
- Automatically calls `onFinished(false)` when count reaches 0

### API Reference

#### Construction

```cpp
IssuerWriteReceipt(Time origin, Duration startWithin, Duration finishWithin);
```

**Inherits from:** `WriteReceipt`

**Parameters:** Same as `WriteReceipt`

**Initial State:**
- `m_useCount = 0`
- `m_started = false`
- All other fields same as `WriteReceipt`

#### Use Count Management

```cpp
void useCountUp();
void useCountDown();
```

**Behavior:**
- `useCountUp()`: Increments internal use count. Call when starting a new fragment/segment transmission.
- `useCountDown()`: Decrements internal use count. Call when fragment/segment is acknowledged. If count reaches 0, automatically calls `onFinished(false)` and clears `onFinished` callback.

**Thread Safety:** Not thread-safe. Use from single protocol thread.

#### Starting Transmission

```cpp
void start();
```

**Behavior:**
- Marks receipt as started (`m_started = true`)
- Should be called when first fragment begins transmission
- Used by `isStarted()` and deadline checking

### When to Use

Use `IssuerWriteReceipt` when:
- Protocol layer fragments messages into multiple parts
- Need to track when all fragments are acknowledged
- Want automatic completion callback when all parts delivered
- Building sender-side (issuer) protocol logic

**Typical Use Cases:**
- RTMFP fragment tracking
- Custom protocol with segmentation
- Any protocol that splits messages into multiple transmissions

### Comparison with WriteReceipt

| Feature | WriteReceipt | IssuerWriteReceipt |
|---------|--------------|-------------------|
| Use count | Manual (external tracking) | Automatic (useCountUp/Down) |
| Completion | Manual `onFinished` call | Automatic when count reaches 0 |
| Intended use | External tracking | Internal protocol layer |
| `m_useCount` | Not directly accessible | Protected, managed internally |
| Fragment tracking | Manual | Built-in |

### Usage Pattern

#### Multi-Fragment Message

```cpp
// Protocol layer sending a message in 3 fragments
Time now = loop.getCurrentTime();
auto receipt = std::make_shared<IssuerWriteReceipt>(now, 0.02, 0.2);
receipt->onFinished = [](bool abandoned) {
    if(abandoned) {
        std::cout << "Message abandoned" << std::endl;
    } else {
        std::cout << "All fragments delivered" << std::endl;
    }
};

// Send fragment 1
receipt->useCountUp();
receipt->start();  // Mark as started
sendFragment(fragment1, [receipt](bool success) {
    if(success) {
        receipt->useCountDown();  // Fragment 1 ACKed
    } else {
        receipt->abandon();  // Fragment failed
    }
});

// Send fragment 2
receipt->useCountUp();
sendFragment(fragment2, [receipt](bool success) {
    if(success) {
        receipt->useCountDown();  // Fragment 2 ACKed
    } else {
        receipt->abandon();
    }
});

// Send fragment 3
receipt->useCountUp();
sendFragment(fragment3, [receipt](bool success) {
    if(success) {
        receipt->useCountDown();  // Fragment 3 ACKed, receipt finishes
        // onFinished(false) called automatically
    } else {
        receipt->abandon();
    }
});
```

#### With Retransmission

```cpp
auto receipt = std::make_shared<IssuerWriteReceipt>(now, 0.02, 0.2);
receipt->retransmit = true;  // Allow retransmission

// Send fragment
receipt->useCountUp();
sendFragment(fragment, [receipt](bool success) {
    if(success) {
        receipt->useCountDown();
    } else if(receipt->retransmit) {
        // Retransmit logic handled by protocol
        // Don't call useCountDown until ACKed
    }
});
```

### Design Pattern: Issue/Receive

The `IssuerWriteReceipt` implements the **issue/receive pattern**:

1. **Issue**: Protocol layer issues fragments, incrementing use count
2. **Track**: Each fragment tracked separately
3. **Receive**: Acknowledgments decrement use count
4. **Complete**: When count reaches 0, message is complete

This pattern is common in reliable transport protocols where messages are fragmented for transmission.

### Best Practices

1. **Match up/down calls**: Each `useCountUp()` must have corresponding `useCountDown()`
2. **Call `start()` early**: Mark receipt as started when first fragment begins
3. **Handle errors**: Call `abandon()` if transmission fails
4. **Single thread**: Use from single protocol thread (not thread-safe)
5. **Check state**: Use `isFinished()` to check completion status

### Common Pitfalls

1. **Mismatched counts**: Forgetting `useCountDown()` causes receipt to never finish
2. **Double decrement**: Calling `useCountDown()` more than `useCountUp()` causes incorrect state
3. **Thread safety**: Using from multiple threads causes race conditions
4. **Not calling `start()`**: Receipt won't be marked as started, affecting deadline checks
5. **Abandoning with count > 0**: Should decrement count or handle abandoned state properly

### Example: Protocol Layer Integration

```cpp
class MyProtocolLayer {
    void sendMessage(const std::vector<uint8_t> &message) {
        Time now = m_loop.getCurrentTime();
        auto receipt = std::make_shared<IssuerWriteReceipt>(now, 0.02, 0.2);
        receipt->onFinished = [this](bool abandoned) {
            if(!abandoned) {
                onMessageDelivered();
            }
        };
        
        // Fragment message
        size_t fragmentSize = 1024;
        for(size_t i = 0; i < message.size(); i += fragmentSize) {
            size_t len = std::min(fragmentSize, message.size() - i);
            auto fragment = std::vector<uint8_t>(message.begin() + i, 
                                                  message.begin() + i + len);
            
            receipt->useCountUp();
            if(i == 0) {
                receipt->start();
            }
            
            sendFragment(fragment, [receipt](bool success) {
                if(success) {
                    receipt->useCountDown();
                } else {
                    receipt->abandon();
                }
            });
        }
    }
};
```

---

## Priority

### Overview

`Priority` enum provides 8 precedence levels for message prioritization, with named aliases for common use cases.

### Priority Levels

```cpp
enum Priority {
    PRI_0 = 0, PRI_1, PRI_2, PRI_3, PRI_4, PRI_5, PRI_6, PRI_7,
    
    PRI_LOWEST        = PRI_0,
    PRI_HIGHEST       = PRI_7,
    
    PRI_BACKGROUND    = PRI_0,
    PRI_BULK          = PRI_1,
    PRI_DATA          = PRI_2,
    PRI_ROUTINE       = PRI_3,
    PRI_PRIORITY      = PRI_4,  // Time Critical starts here
    PRI_IMMEDIATE     = PRI_5,
    PRI_FLASH         = PRI_6,
    PRI_FLASHOVERRIDE = PRI_7
};
```

### Usage

```cpp
Priority p = PRI_ROUTINE;
if(isUrgent) {
    p = PRI_IMMEDIATE;
}
```

---

## Algorithm Helpers

### collect()

```cpp
template <class OutputItemType, class UnaryOperation, class Iterable>
std::vector<OutputItemType> collect(const UnaryOperation &fn, const Iterable &iterable);
```

**Behavior:** Maps function over iterable, returns vector of results

**Example:**
```cpp
std::vector<int> numbers = {1, 2, 3, 4};
auto doubled = collect<int>([](int x) { return x * 2; }, numbers);
// doubled == {2, 4, 6, 8}
```

---

## Summary

**Key Components:**

1. **Hex**: Binary ↔ hexadecimal conversion
2. **URIParse**: RFC 3986 URI parsing and manipulation
3. **Checksums**: CRC-32 and Internet checksum
4. **RateTracker**: Sliding window rate estimation
5. **IndexSet**: Disjoint range management
6. **List**: Handle-based doubly-linked list
7. **WriteReceipt**: Message transmission tracking
8. **Priority**: 8-level priority system
9. **Algorithm**: Generic helper functions

**Common Use Cases:**

- **Hex**: Debugging, data serialization
- **URIParse**: Protocol parsing, URL manipulation
- **Checksums**: Data integrity verification
- **RateTracker**: Bandwidth monitoring, throttling
- **IndexSet**: Segment tracking, sparse sets
- **List**: Safe iteration with modification
- **WriteReceipt**: Reliable transmission tracking
- **Priority**: Message prioritization

