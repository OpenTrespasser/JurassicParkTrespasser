#include "gtest/gtest.h"

#include "Lib/Std/UTypes.hpp"

#include <vector>


void ApplyClut16
(
	uint16* pu2,		// Surface to apply clut to.
	const uint16* au2_clut,	// Clut table.
	int     i_width,	// Width of the surface in pixels.
	int     i_height,	// Height of the surface in pixels.
	int     i_stride,	// Stride of the surface in pixels.
	uint16  u2_mask		// Bit mask for alpha.
);


std::vector<uint16> CreateClut()
{
	std::vector<uint16> result(UINT16_MAX);
	for (uint16 i = 0; i < UINT16_MAX; i++)
		result[i] = UINT16_MAX - i;
	return result;
}

TEST(ApplyClut, ApplyClut1Px)
{
	uint16 px = 0x3FF0;
	auto clut = CreateClut();

	ApplyClut16(&px, clut.data(), 1, 1, 1, 0);
	EXPECT_EQ(px, 0xE00F);
}

TEST(ApplyClut, ApplyClut4Px)
{
	std::vector<uint16> pixels = { 0x3FF0, 0x3FF1, 0x3FF2, 0x3FF3 };
	std::vector<uint16> target = { 0xE00F, 0xE00E, 0xE00D, 0xE00C };
	auto clut = CreateClut();

	ApplyClut16(pixels.data(), clut.data(), 2, 2, 2, 0);
	EXPECT_EQ(pixels, target);
}

TEST(ApplyClut, ApplyClutRectangleWide)
{
	std::vector<uint16> pixels = { 0x3FF0, 0x3FF1, 0x3FF2, 0x3FF3, 0x3FF4, 0x3FF5, 0x3FF6, 0x3FF7 };
	std::vector<uint16> target = { 0xE00F, 0xE00E, 0xE00D, 0xE00C, 0xE00B, 0xE00A, 0xE009, 0xE008 };
	auto clut = CreateClut();

	ApplyClut16(pixels.data(), clut.data(), 4, 2, 4, 0);
	EXPECT_EQ(pixels, target);
}

TEST(ApplyClut, ApplyClutRectangleTall)
{
	std::vector<uint16> pixels = { 0x3FF0, 0x3FF1, 0x3FF2, 0x3FF3, 0x3FF4, 0x3FF5, 0x3FF6, 0x3FF7 };
	std::vector<uint16> target = { 0xE00F, 0xE00E, 0xE00D, 0xE00C, 0xE00B, 0xE00A, 0xE009, 0xE008 };
	auto clut = CreateClut();

	ApplyClut16(pixels.data(), clut.data(), 4, 2, 4, 0);
	EXPECT_EQ(pixels, target);
}
