#include "common.hpp"
#include "Lib/Groff/FileIO.hpp"

#include "gtest/gtest.h"

TEST(CFileIO, Destructor)
{
	//Create several empty objects which are removed immediately
	//This shall happen without errors
	for (size_t i = 0; i < 5; i++) {
		CFileIO object;
	}
}
