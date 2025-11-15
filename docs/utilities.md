Utilities Reference
===================

Containers

- List<T>
  - Handle‑based doubly‑linked list with a sentinel; O(1) insert/remove by handle.
  - API: `append`, `prepend`, `addBefore/After`, `remove`, `first/last`, `valuesDo`, `safeValuesDo`.
  - Variant `SumList<T>` tracks a running `sum()` via a size functor.

- IndexSet
  - Manages disjoint index ranges with operations to add/remove/iterate.
  - Query: `size`, `countRanges`, `contains`, `lowestIndex`, `highestIndex`, `firstRange`, `lastRange`.
  - Iterate: `extentsDo(from,to)` and `indicesDo(eachIndex)`.

Time and Scheduling

- RateTracker
  - Sliding‑window rate estimation over `windowPeriod` seconds.
  - Methods: `update(count, now)`, `getRate(now)`, `setWindowPeriod`, `reset`.

- WriteReceipt / WriteReceiptChain
  - Track start/deadline windows for message transmission and completion.
  - Fields: `startBy`, `finishBy`, `retransmit`, `parent`; state queries and `onFinished` callback.
  - Chain: `WriteReceiptChain::append()` to set parent linkage; `expire()` to clamp deadlines or clear.

Networking and Parsing

- Address (com::zenomt::rtmfp)
  - Unified IPv4/IPv6 socket address with encoding/decoding, ordering, and presentation.
  - Methods: `setFamily`, `setPort`, `getIPAddress`, `encode/decode`, `toPresentation`, `setFromPresentation`, `lookup` (POSIX).

- URIParse
  - RFC 3986 parsing helpers and transformations.
  - Methods: `transformRelativeReference`, `mergedRelativePath`, `removeDotSegments`, `split`, `percentDecode`, `lowercase`; component fields exposed.

Algorithms and Helpers

- Checksums
  - CRC‑32: `crc32_le`, `crc32_be` incremental; Internet checksum `in_cksum`.

- Hex
  - Hex dump and conversion: `encode`, `decode`, `print`, `dump`, `decodeDigit`, `decodeByte`.

- Priority
  - `enum Priority` with 8 precedence levels and aliases from `PRI_BACKGROUND` to `PRI_FLASHOVERRIDE`.

Snippets

Iterate IndexSet ranges

```cpp
IndexSet set;
set.add(0, 99);
set.add(200, 299);
set.extentsDo([](uintmax_t a, uintmax_t b){ /* [a,b] */ return true; });
```

Track send window with WriteReceipt

```cpp
auto receipt = std::make_shared<WriteReceipt>(loop.getCurrentTime(), /*startWithin*/0.02, /*finishWithin*/0.2);
receipt->onFinished = [](bool abandoned){ /* observe outcome */ };
```

Compute CRC32 (little‑endian)

```cpp
uint32_t crc = crc32_le(data, len); // initializes with 0xFFFFFFFF internally
crc ^= 0xFFFFFFFF; // typical finalization
```


