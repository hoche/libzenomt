AR = ar
RANLIB = ranlib

OPENSSL_INCLUDEDIR = -I/usr/local/openssl/include
OPENSSL_LIBDIR = -L/usr/local/openssl/lib

# CXXFLAGS = -g -Wall -pedantic -std=c++11
# CXXFLAGS = -Os -Wall -pedantic -std=c++11 -fno-exceptions
CXXFLAGS = -Os -Wall -pedantic -std=c++11

UTILS = src/Checksums.o src/Hex.o src/IndexSet.o src/Object.o src/RateTracker.o src/Timer.o \
	src/Address.o src/WriteReceipt.o \
	src/EPollRunLoop.o src/Performer.o \
	src/RunLoop.o src/SelectRunLoop.o src/URIParse.o \
	src/PosixStreamPlatformAdapter.o src/SimpleWebSocket.o

ifndef WITHOUT_OPENSSL
UTILS_OPENSSL = src/SimpleWebSocket_OpenSSL.o
$(UTILS_OPENSSL): CPPFLAGS += $(OPENSSL_INCLUDEDIR)
endif

LIBOBJS = $(UTILS) $(UTILS_OPENSSL)

default: libzenomt.a

all: default test-all

libzenomt.a: $(LIBOBJS)
	rm -f $@
	$(AR) -r $@ $+
	$(RANLIB) $@

tests ci test-all examples: default
	@cd test && $(MAKE) $@

clean:
	rm -f $(LIBOBJS) libzenomt.a
	@cd test && $(MAKE) clean
