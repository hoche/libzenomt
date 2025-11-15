#include <gtest/gtest.h>
#include "zenomt/URIParse.hpp"

using namespace com::zenomt;

TEST(URIParseTest, BasicParse) {
	auto u = URIParse::parse("rtmfp://foo:1935/bar");
	
	EXPECT_EQ(u.scheme, "rtmfp");
	EXPECT_EQ(u.host, "foo");
	EXPECT_EQ(u.port, "1935");
	EXPECT_EQ(u.path, "/bar");
	EXPECT_EQ(u.effectivePort, "1935");
}

TEST(URIParseTest, IPv6) {
	auto u = URIParse::parse("rtmfp://[2001:db8::1]:1234");
	
	EXPECT_EQ(u.host, "2001:db8::1");
	EXPECT_EQ(u.port, "1234");
	EXPECT_EQ(u.hostinfo, "[2001:db8::1]:1234");
}

TEST(URIParseTest, DefaultPort) {
	auto u = URIParse::parse("rtmfp://foo");
	EXPECT_EQ(u.effectivePort, "1935");
}

TEST(URIParseTest, HTTPSDefaultPort) {
	auto u = URIParse::parse("https://example.com");
	EXPECT_EQ(u.effectivePort, "443");
}

TEST(URIParseTest, HTTPDefaultPort) {
	auto u = URIParse::parse("http://example.com");
	EXPECT_EQ(u.effectivePort, "80");
}

TEST(URIParseTest, QueryAndFragment) {
	auto u = URIParse::parse("rtmfp://foo/bar?query=baz#fragment");
	
	EXPECT_EQ(u.path, "/bar");
	EXPECT_EQ(u.query, "query=baz");
	EXPECT_EQ(u.fragment, "fragment");
}

TEST(URIParseTest, UserInfo) {
	auto u = URIParse::parse("rtmfp://user:pass@foo:1936");
	
	EXPECT_EQ(u.userinfo, "user:pass");
	EXPECT_EQ(u.user, "user");
	EXPECT_EQ(u.password, "pass");
	EXPECT_EQ(u.port, "1936");
}

TEST(URIParseTest, Split) {
	auto parts = URIParse::split("part1:part2:part3", ":");
	EXPECT_EQ(parts.size(), 3);
	EXPECT_EQ(parts[0], "part1");
	EXPECT_EQ(parts[1], "part2");
	EXPECT_EQ(parts[2], "part3");
}

TEST(URIParseTest, SplitWithLimit) {
	auto parts = URIParse::split("part1:part2:part3", ":", 2);
	EXPECT_EQ(parts.size(), 2);
	EXPECT_EQ(parts[0], "part1");
	EXPECT_EQ(parts[1], "part2:part3");
}

TEST(URIParseTest, PercentDecode) {
	EXPECT_EQ(URIParse::percentDecode("hi%20there"), "hi there");
	EXPECT_EQ(URIParse::percentDecode("hi%20%20there"), "hi  there");
	EXPECT_EQ(URIParse::percentDecode("%20"), " ");
}

TEST(URIParseTest, PercentDecodeInvalid) {
	EXPECT_TRUE(URIParse::percentDecode("hi%20%2there").empty());
	EXPECT_TRUE(URIParse::percentDecode("hi%00there").empty());
	EXPECT_TRUE(URIParse::percentDecode("%").empty());
}

TEST(URIParseTest, SafePercentDecode) {
	EXPECT_EQ(URIParse::safePercentDecode("hi%20%2there"), "hi%20%2there");
	EXPECT_EQ(URIParse::safePercentDecode("hi%20there"), "hi there");
}

TEST(URIParseTest, Lowercase) {
	EXPECT_EQ(URIParse::lowercase("HELLO"), "hello");
	EXPECT_EQ(URIParse::lowercase("Hello"), "hello");
	EXPECT_EQ(URIParse::lowercase("hello"), "hello");
}

