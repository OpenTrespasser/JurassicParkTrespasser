#pragma once

#include <string>

class IniFile
{
public:
	IniFile();
	IniFile(const std::string& filename, const std::string& section);

	void deleteValue(const std::string& key);
	int getInt(const std::string& key, int defaultvalue) const;
	void setInt(const std::string& key, int value);
	int getBinary(const std::string& key, void* buffer, size_t buffersize) const;
	void setBinary(const std::string& key, void* buffer, size_t buffersize);
	int getString(const std::string& key, char* buffer, size_t buffersize, const char* defaultvalue) const;
	void setString(const std::string& key, const char* string);
	
private:
	IniFile(const std::pair<std::string, std::string>& args);
	
	static std::string convertFileName(const std::string& filename);
	static std::pair<std::string, std::string> determineDefaultFile();
	void createFileIfNotExists();
	
	const std::string filepath;
	const std::string section;
};
