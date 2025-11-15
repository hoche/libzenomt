#include <gtest/gtest.h>
#include "zenomt/Hex.hpp"

using namespace com::zenomt;

TEST(HexTest, Encode) {
	uint8_t data[] = {0, 1, 5, 4, 5};
	std::string encoded = Hex::encode(data, sizeof(data));
	EXPECT_EQ(encoded, "0001050405");
}

TEST(HexTest, DecodeValid) {
	std::vector<uint8_t> result;
	
	EXPECT_TRUE(Hex::decode("0001050405", result));
	EXPECT_EQ(result.size(), 5);
	EXPECT_EQ(result[0], 0);
	EXPECT_EQ(result[1], 1);
	EXPECT_EQ(result[2], 5);
	EXPECT_EQ(result[3], 4);
	EXPECT_EQ(result[4], 5);
}

TEST(HexTest, DecodeWithSpaces) {
	std::vector<uint8_t> result;
	
	EXPECT_TRUE(Hex::decode("00 01 02", result));
	EXPECT_EQ(result.size(), 3);
	EXPECT_EQ(result[0], 0);
	EXPECT_EQ(result[1], 1);
	EXPECT_EQ(result[2], 2);
}

TEST(HexTest, DecodeEmpty) {
	std::vector<uint8_t> result;
	
	EXPECT_TRUE(Hex::decode("", result));
	EXPECT_EQ(result.size(), 0);
}

TEST(HexTest, DecodeInvalid) {
	std::vector<uint8_t> result;
	
	EXPECT_FALSE(Hex::decode("f 1", result));
	EXPECT_FALSE(Hex::decode("1", result));
	EXPECT_FALSE(Hex::decode("0x33", result));
	EXPECT_FALSE(Hex::decode("fo", result));
}

TEST(HexTest, DecodeByte) {
	EXPECT_EQ(Hex::decodeByte("00"), 0x00);
	EXPECT_EQ(Hex::decodeByte("08"), 0x08);
	EXPECT_EQ(Hex::decodeByte("a0"), 0xa0);
	EXPECT_EQ(Hex::decodeByte("Ff"), 0xff);
	EXPECT_EQ(Hex::decodeByte("A0g"), 0xa0); // Stops at invalid char
}

TEST(HexTest, DecodeByteInvalid) {
	EXPECT_LT(Hex::decodeByte(""), 0);
	EXPECT_LT(Hex::decodeByte("0"), 0);
	EXPECT_LT(Hex::decodeByte("0g"), 0);
	EXPECT_LT(Hex::decodeByte("g0"), 0);
}

TEST(HexTest, RoundTrip) {
	uint8_t original[] = {0x00, 0xff, 0x12, 0xab, 0xcd};
	std::string encoded = Hex::encode(original, sizeof(original));
	
	std::vector<uint8_t> decoded;
	EXPECT_TRUE(Hex::decode(encoded.c_str(), decoded));
	
	EXPECT_EQ(decoded.size(), sizeof(original));
	for(size_t i = 0; i < sizeof(original); i++) {
		EXPECT_EQ(decoded[i], original[i]);
	}
}

