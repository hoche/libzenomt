OPENSSL_INCLUDEDIR = -I/usr/local/openssl/include
OPENSSL_LIBDIR = -L/usr/local/openssl/lib

# CXXFLAGS = -Wall -pedantic -std=c++11 -g -I../include
# CXXFLAGS = -Os -Wall -pedantic -std=c++11 -fno-exceptions -I../include
CXXFLAGS = -Os -Wall -pedantic -std=c++11 -I../include

LIBRARY = ../libzenomt.a

ifndef WITHOUT_OPENSSL
# WS_EXAMPLES = testwebsock
endif

TESTS = tis testperform testchecksums testlist testaddress testhex testuriparse testratetracker testretainer
EXAMPLES = $(WS_EXAMPLES)

default: all
test-all: all
all: tests examples
tests: $(TESTS)
examples: $(EXAMPLES)

$(LIBRARY):
	@cd .. && $(MAKE)

tis: tis.o $(LIBRARY)
	rm -f $@
	$(CXX) -o $@ $+

testperform: testperform.o $(LIBRARY)
	rm -f $@
	$(CXX) -o $@ $+ -lpthread

testchecksums: testchecksums.o $(LIBRARY)
	rm -f $@
	$(CXX) -o $@ $+

testlist: testlist.o $(LIBRARY)
	rm -f $@
	$(CXX) -o $@ $+

testaddress: testaddress.o $(LIBRARY)
	rm -f $@
	$(CXX) -o $@ $+

testhex: testhex.o $(LIBRARY)
	rm -f $@
	$(CXX) -o $@ $+

testuriparse: testuriparse.o $(LIBRARY)
	rm -f $@
	$(CXX) -o $@ $+

testratetracker: testratetracker.o $(LIBRARY)
	rm -f $@
	$(CXX) -o $@ $+

testretainer: testretainer.o $(LIBRARY)
	rm -f $@
	$(CXX) -o $@ $+

# make ci: build all, but only run the automated tests.
ci: all
	./tis
	./testperform
	./testchecksums
	./testlist
	./testaddress
	./testhex
	./testuriparse
	./testratetracker
	./testretainer

clean:
	rm -f *.o $(TESTS) $(EXAMPLES)
