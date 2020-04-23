#include "Lib/Sys/IniFile.hpp"
#include <filesystem>
#include <memory>
#include <fstream>

#include "gtest/gtest.h"

struct TestStruct
{
	int x;
	int y;
	int z;
};


TEST(IniFileCreate, CreateIfNotExists)
{
	std::filesystem::path filename = "CreateIniFile.ini";
	
	std::filesystem::remove(filename);
	ASSERT_FALSE(std::filesystem::exists(filename));
	
	IniFile file(filename.string(), "TestSection");
	EXPECT_TRUE(std::filesystem::exists(filename));
}

TEST(IniFileCreate, NoCreateIfExists)
{
	std::filesystem::path filename = "NoCreateIniFile.ini";
	std::string teststring = "FooFooBarBar";
	
	{
		std::fstream write(filename, std::ios_base::out | std::ios_base::trunc);
		write << teststring;
	}
	ASSERT_TRUE(std::filesystem::exists(filename));

	IniFile file(filename.string(), "TestSection");
	EXPECT_TRUE(std::filesystem::exists(filename));

	{
		//Ensure that file was not emptied
		std::fstream read(filename, std::ios_base::in);
		std::string instring;
		read >> instring;
		EXPECT_EQ(instring, teststring);
	}
}

class IniFileWriteTest : public ::testing::Test
{
protected:
	virtual void SetUp() override
	{
		inifile = std::make_unique<IniFile>("WriteTestIniFile.ini", "TestSection");
		ASSERT_TRUE(inifile);
	}
	
	void ExpectEmpty(const std::string& key)
	{
		char testbuffer[10] = { '\0' };
		EXPECT_EQ(0, inifile->getString(key, testbuffer, sizeof(testbuffer), ""));
		EXPECT_STREQ(testbuffer, "");
	}

	std::unique_ptr<IniFile> inifile;
};

TEST_F(IniFileWriteTest, WriteInt)
{
	std::string key = "IntTest";
	int value = 500;
	inifile->setInt(key, value);
	EXPECT_EQ(inifile->getInt(key, 0), value);
}

TEST_F(IniFileWriteTest, WriteString)
{
	std::string key = "StringTest";
	std::string source = "FooBar";
	std::string targetstring;
	targetstring.reserve(20);
	inifile->setString(key, source.c_str());
	inifile->getString(key, targetstring.data(), 20, "");
	EXPECT_EQ(source, source);
}

TEST_F(IniFileWriteTest, WriteStringNullptr)
{
	std::string key = "StringNullTest";
	std::string targetstring;
	targetstring.reserve(20);
	inifile->setString(key, nullptr);
	int read = inifile->getString(key, targetstring.data(), 20, "");
	EXPECT_EQ(0, read);
	EXPECT_TRUE(targetstring.empty());
}

TEST_F(IniFileWriteTest, WriteStruct)
{
	std::string key = "StructTest";
	TestStruct originalStruct = { 15, 20, 25 };
	TestStruct targetStruct = { 0, 0, 0 };

	inifile->setBinary(key, &originalStruct, sizeof(originalStruct));
	ASSERT_NE(0, inifile->getBinary(key, &targetStruct, sizeof(targetStruct)));
	EXPECT_EQ(originalStruct.x, targetStruct.x);
	EXPECT_EQ(originalStruct.y, targetStruct.y);
	EXPECT_EQ(originalStruct.z, targetStruct.z);
}

TEST_F(IniFileWriteTest, WriteStructNullptr)
{
	std::string key = "StructNullptrTest";
	TestStruct targetStruct = { 0, 0, 0 };

	inifile->setBinary(key, nullptr, 0);
	ASSERT_EQ(0, inifile->getBinary(key, &targetStruct, sizeof(targetStruct)));
}

TEST_F(IniFileWriteTest, DeleteInt)
{
	std::string key = "TestInt";
	inifile->setInt(key, 600);
	inifile->deleteValue(key);
	ExpectEmpty(key);
}

TEST_F(IniFileWriteTest, DeleteString)
{
	std::string key = "TestString";
	inifile->setString(key, "FooBar");
	inifile->deleteValue(key);
	ExpectEmpty(key);
}

TEST_F(IniFileWriteTest, DeleteStruct)
{
	std::string key = "TestStruct";
	TestStruct s = { 20, 21, 22 };
	inifile->setBinary(key, &s, sizeof(s));
	inifile->deleteValue(key);
	ExpectEmpty(key);
}

class ReadIniFileTest : public ::testing::Test
{
protected:
	virtual void SetUp() override
	{
		filename = "ReadTestIniFile.ini";
		makeTestIniFile();
		ASSERT_TRUE(std::filesystem::exists(filename));
		inifile = std::make_unique<const IniFile>(filename, "TestSection");
		ASSERT_TRUE(inifile);
	}

	void makeTestIniFile()
	{
		std::fstream fs(filename, std::ios_base::out | std::ios_base::trunc);
		fs << "[TestSection]" << std::endl;
		fs << "TestInt = 500" << std::endl;
		fs << "TestString = FooBar" << std::endl;
		fs << "TestBinary = 0F00000014000000190000003C" << std::endl; //Teststruct{x=15, y=20, z=25}
	}

	std::unique_ptr<const IniFile> inifile;
private:

	std::string filename;
};

TEST_F(ReadIniFileTest, ReadIntValid)
{
	int read = inifile->getInt("TestInt", 0);
	EXPECT_EQ(read, 500);
}

TEST_F(ReadIniFileTest, ReadIntMissing)
{
	int read = inifile->getInt("DoesNotExist", 600);
	EXPECT_EQ(read, 600);
}

TEST_F(ReadIniFileTest, ReadStringValid)
{
	char target[10] = { '\0' };
	int read = inifile->getString("TestString", target, sizeof(target), "");
	EXPECT_EQ(6, read);
	ASSERT_EQ(target[9], '\0');
	EXPECT_STREQ(target, "FooBar");
}

TEST_F(ReadIniFileTest, ReadStringTargetTooShort)
{
	char target[4] = { '\0' };
	inifile->getString("TestString", target, sizeof(target), "");
	ASSERT_EQ(target[3], '\0');
	EXPECT_STREQ(target, "Foo");
}

TEST_F(ReadIniFileTest, ReadStringTargetNullptr)
{
	int read = inifile->getString("TestString", nullptr, 0, "");
	EXPECT_EQ(read, 0);
}

TEST_F(ReadIniFileTest, ReadStringMissing)
{
	char target[10] = { '\0' };
	inifile->getString("KeyMissing", target, sizeof(target), "Default");
	ASSERT_EQ(target[9], '\0');
	EXPECT_STREQ(target, "Default");
}

TEST_F(ReadIniFileTest, ReadStringDefaultNullptr)
{
	char target[10] = { '\0' };
	inifile->getString("KeyMissing", target, sizeof(target), nullptr);
	ASSERT_EQ(target[9], '\0');
	EXPECT_STREQ(target, "");
}

TEST_F(ReadIniFileTest, ReadStringDefaultTooLong)
{
	char target[10] = { '\0' };
	inifile->getString("KeyMissing", target, sizeof(target), "TooLongTargetString");
	ASSERT_EQ(target[9], '\0');
	EXPECT_STREQ(target, "TooLongTa");
}
