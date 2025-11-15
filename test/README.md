zenomt Library Tests and Examples
=================================
This directory contains unit tests for the library, as well as samples/examples.

Samples, Examples, and Manual Tests
-----------------------------------
These programs all answer brief usage info with the `-h` option. For more information on
what's going on in each, check the source.

(none)

Unit Tests
----------
These programs are run on `make ci`.

* [`tis`](tis.cpp): Initially and primarily to exercise `IndexSet`, but also
  exercises `List`, `Timer`, and `SelectRunLoop`. Should be decomposed and assertions
  added to detect failures automatically rather than require human inspection.
* [`testperform`](testperform.cpp): Exercises `Performer`, `Timer`, `SelectRunLoop`, and
  multithreaded synchronization. Also requires human inspection.
* [`testchecksums`](testchecksums.cpp): Test the Internet Checksum and CRC32 implementations.
* [`testlist`](testlist.cpp): Test `List`.
* [`testaddress`](testaddress.cpp): Test `Address`.
* [`testhex`](testhex.cpp): Test hex encode/decode functions.
* [`testuriparse`](testuriparse.cpp): Test URI parser.
