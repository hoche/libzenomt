#include <gtest/gtest.h>
#include <cstring>
#include "zenomt/Address.hpp"

using namespace com::zenomt;
using namespace com::zenomt::rtmfp;

TEST(AddressTest, IPv4Basic) {
	Address addr;
	uint8_t ipv4[] = {192, 168, 1, 1};
	
	EXPECT_TRUE(addr.setFamily(AF_INET));
	EXPECT_TRUE(addr.setIPAddress(ipv4, sizeof(ipv4)));
	EXPECT_EQ(addr.getFamily(), AF_INET);
	
	uint8_t result[4];
	size_t len = addr.getIPAddress(result);
	EXPECT_EQ(len, 4);
	EXPECT_EQ(memcmp(result, ipv4, 4), 0);
}

TEST(AddressTest, IPv6Basic) {
	Address addr;
	uint8_t ipv6[16] = {0x20, 0x01, 0x0d, 0xb8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1};
	
	EXPECT_TRUE(addr.setFamily(AF_INET6));
	EXPECT_TRUE(addr.setIPAddress(ipv6, sizeof(ipv6)));
	EXPECT_EQ(addr.getFamily(), AF_INET6);
}

TEST(AddressTest, Port) {
	Address addr;
	uint16_t port = 8080;
	
	// setPort() only works if family is set
	addr.setFamily(AF_INET);
	addr.setPort(port);
	EXPECT_EQ(addr.getPort(), port);
}

TEST(AddressTest, SerializeIPv4) {
	Address addr;
	uint8_t ipv4[] = {192, 168, 1, 1};
	uint16_t port = 8080;
	
	addr.setFamily(AF_INET);
	addr.setIPAddress(ipv4, sizeof(ipv4));
	addr.setPort(port);
	
	uint8_t buffer[16];
	size_t len = addr.encode(buffer);
	EXPECT_GT(len, 0);
}

TEST(AddressTest, SerializeIPv6) {
	Address addr;
	uint8_t ipv6[16] = {0x20, 0x01, 0x0d, 0xb8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1};
	uint16_t port = 8080;
	
	addr.setFamily(AF_INET6);
	addr.setIPAddress(ipv6, sizeof(ipv6));
	addr.setPort(port);
	
	uint8_t buffer[32];
	size_t len = addr.encode(buffer);
	EXPECT_GT(len, 0);
}

TEST(AddressTest, Equality) {
	Address addr1, addr2;
	uint8_t ipv4[] = {192, 168, 1, 1};
	
	addr1.setFamily(AF_INET);
	addr1.setIPAddress(ipv4, sizeof(ipv4));
	addr1.setPort(8080);
	
	addr2.setFamily(AF_INET);
	addr2.setIPAddress(ipv4, sizeof(ipv4));
	addr2.setPort(8080);
	
	EXPECT_EQ(addr1, addr2);
}

TEST(AddressTest, Inequality) {
	Address addr1, addr2;
	uint8_t ipv4_1[] = {192, 168, 1, 1};
	uint8_t ipv4_2[] = {192, 168, 1, 2};
	
	addr1.setFamily(AF_INET);
	addr1.setIPAddress(ipv4_1, sizeof(ipv4_1));
	addr1.setPort(8080);
	
	addr2.setFamily(AF_INET);
	addr2.setIPAddress(ipv4_2, sizeof(ipv4_2));
	addr2.setPort(8080);
	
	EXPECT_FALSE(addr1 == addr2);
}

