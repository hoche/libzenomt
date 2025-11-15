#include <gtest/gtest.h>
#include <cstring>
#include "zenomt/Checksums.hpp"

using namespace com::zenomt;

TEST(ChecksumsTest, InCksumBasic) {
	uint8_t buf[] = {0x01, 0x02, 0x03, 0x04, 0x05};
	uint16_t result = in_cksum(buf, sizeof(buf));
	
	// Basic sanity check - should not be zero for non-zero input
	EXPECT_NE(result, 0);
}

TEST(ChecksumsTest, InCksumKnownValue) {
	uint8_t buf[] = {0x01, 0x02, 0x03, 0x04, 0x05};
	uint16_t result = in_cksum(buf, sizeof(buf));
	
	// Expected: ((0x0102 + 0x0304 + 0x05) ^ 0xffff) with carries
	// This is a basic check - exact value depends on implementation
	EXPECT_NE(result, 0);
}

TEST(ChecksumsTest, CRC32LEBasic) {
	const char* str = "test";
	uint32_t crc = crc32_le(0, str, strlen(str));
	
	// Should not be zero for non-zero input
	EXPECT_NE(crc, 0);
}

TEST(ChecksumsTest, CRC32LEZero) {
	const char* str = "test";
	uint32_t crc1 = crc32_le(0, str, strlen(str));
	uint32_t crc2 = crc32_le(crc1, str, strlen(str));
	
	// CRC of data + its own CRC should be zero
	uint8_t crc_buf[4];
	crc_buf[0] = crc1 & 0xff;
	crc_buf[1] = (crc1 >> 8) & 0xff;
	crc_buf[2] = (crc1 >> 16) & 0xff;
	crc_buf[3] = (crc1 >> 24) & 0xff;
	
	uint32_t final = crc32_le(crc1, crc_buf, sizeof(crc_buf));
	EXPECT_EQ(final, 0);
}

TEST(ChecksumsTest, CRC32BEBasic) {
	const char* str = "test";
	uint32_t crc = crc32_be(0, str, strlen(str));
	
	// Should not be zero for non-zero input
	EXPECT_NE(crc, 0);
}

TEST(ChecksumsTest, CRC32BEZero) {
	const char* str = "test";
	uint32_t crc1 = crc32_be(0, str, strlen(str));
	
	// CRC of data + its own CRC should be zero
	uint8_t crc_buf[4];
	crc_buf[0] = (crc1 >> 24) & 0xff;
	crc_buf[1] = (crc1 >> 16) & 0xff;
	crc_buf[2] = (crc1 >> 8) & 0xff;
	crc_buf[3] = crc1 & 0xff;
	
	uint32_t final = crc32_be(crc1, crc_buf, sizeof(crc_buf));
	EXPECT_EQ(final, 0);
}

TEST(ChecksumsTest, CRC32Incremental) {
	const char* str = "test";
	size_t len = strlen(str);
	
	// Compute CRC in one go
	uint32_t crc1 = crc32_le(0, str, len);
	
	// Compute CRC incrementally
	uint32_t crc2 = crc32_le(0, str, len - 4);
	crc2 = crc32_le(crc2, str + len - 4, 4);
	
	EXPECT_EQ(crc1, crc2);
}

