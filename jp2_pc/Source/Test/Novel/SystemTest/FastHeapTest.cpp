#include "common.hpp"
#include "Lib/Sys/FastHeap.hpp"

#include "gtest/gtest.h"

TEST(CFastHeap, AllocateSingle)
{
	constexpr size_t elements = 1000;
	CFastHeap fh(sizeof(int) * elements);

	for (size_t i = 0; i < elements; i++) {
		auto* newint = new(fh) int;
		ASSERT_NE(newint, nullptr);
		
		*newint = 500; //Ensure we can write data to allocated memory
	}	
}

TEST(CFastHeap, AllocateArray)
{
	constexpr size_t arraySize = 10;
	constexpr size_t arrayCount = 100;
	CFastHeap fh(sizeof(int) * arraySize * arrayCount);
	
	for (size_t i = 0; i < arrayCount; i++)
	{
		auto* newintarray = new(fh) int[arraySize];
		ASSERT_NE(newintarray, nullptr);
		
		for (size_t j = 0; j < arraySize; j++)
			newintarray[j] = 500; //Ensure we can write data to allocated memory
	}
}
