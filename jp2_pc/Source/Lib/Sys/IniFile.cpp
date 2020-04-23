#include "IniFile.hpp"

#include "Windows.h"

#include <fstream>
#include <filesystem>

IniFile::IniFile() : IniFile(determineDefaultFile())
{
}

IniFile::IniFile(const std::pair<std::string, std::string>& args)
	: IniFile(args.first, args.second)
{
}

IniFile::IniFile(const std::string& filename, const std::string& section)
	: filepath(convertFileName(filename)), section(section)
{
	createFileIfNotExists();
}

void IniFile::deleteValue(const std::string& key)
{
	//There is no direct way to delete an entry without parsing the file ourselves
	setString(key, "");
}

int IniFile::getInt(const std::string& key, int defaultvalue) const
{
	return GetPrivateProfileInt(section.c_str(), key.c_str(), defaultvalue, filepath.c_str());
}

void IniFile::setInt(const std::string& key, int value)
{
	//There is no WritePrivateProfileInt
	setString(key, std::to_string(value).c_str());
}

int IniFile::getBinary(const std::string& key, void* buffer, size_t buffersize) const
{
	if (!GetPrivateProfileStruct(section.c_str(), key.c_str(), buffer, buffersize, filepath.c_str()))
		return 0;
	return buffersize;
}

void IniFile::setBinary(const std::string& key, void* buffer, size_t buffersize)
{
	WritePrivateProfileStruct(section.c_str(), key.c_str(), buffer, buffersize, filepath.c_str());
}

int IniFile::getString(const std::string& key, char* buffer, size_t buffersize, const char* defaultvalue) const
{
	return GetPrivateProfileString(section.c_str(), key.c_str(), defaultvalue, buffer, buffersize, filepath.c_str());
}

void IniFile::setString(const std::string& key, const char* string)
{
	WritePrivateProfileString(section.c_str(), key.c_str(), string, filepath.c_str());
}

std::string IniFile::convertFileName(const std::string& filename)
{
	//The Get/WritePrivateProfile functions needs the file name as a path.
	//If it is a pure file name, the file will be assumed to be at C:\Windows
	//A path like .\foo.ini would suffice
	//To be safe, we use the full absolute path
	return std::filesystem::absolute(filename).string();
}

std::pair<std::string, std::string> IniFile::determineDefaultFile()
{
	if (std::filesystem::exists("tpass.ini"))
		return std::pair("tpass.ini", "Settings");
	else
		return std::pair("OpenTrespasser.ini", "OpenTrespasser");
}

void IniFile::createFileIfNotExists()
{
	if (!std::filesystem::exists(filepath))
		std::fstream file(filepath, std::ios_base::out | std::ios_base::trunc | std::ios_base::_Noreplace);
}
