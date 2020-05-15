#include "RegToIni.hpp"
#include "RegInit.hpp"
#include <atlbase.h>
#include <vector>

class RegToIniConverter
{
public:
	RegToIniConverter(IniFile& inifile) : inifile(inifile), regkey() {}
	bool convert();
private:
	void transferBinary(const std::string& key, ULONG size);
	void transferInt(const std::string& key);
	void transferString(const std::string& key, ULONG size);
	IniFile& inifile;
	ATL::CRegKey regkey;
};

bool RegToIniConverter::convert()
{
	const std::string keys[] = {
		strDDDEVICE_NAME			,
		strDDDEVICE_DESCRIPTION		,
		strDDDEVICE_GUID			,
		strD3DDEVICE_NAME			,
		strD3DDEVICE_DESCRIPTION	,
		strD3DDEVICE_GUID			,
		strFLAG_FULLSCREEN          ,
		strFLAG_D3D                 ,
		strFLAG_SYSTEMMEM           ,
		strFLAG_REGINIT				,
		strSIZE_WIDTH               ,
		strSIZE_HEIGHT              ,
		strPARTITION_SUBDIVISION    ,
		strPARTITION_STUFFCHILDREN  ,
		strAUTOSETTINGS             ,
		REG_KEY_PID                 ,
		REG_KEY_DATA_DRIVE          ,
		REG_KEY_INSTALLED           ,
		REG_KEY_INSTALLED_DIR       ,
		REG_KEY_NOVIDEO             ,
		REG_KEY_AUDIO_LEVEL         ,
		REG_KEY_AUDIO_EFFECT        ,
		REG_KEY_AUDIO_AMBIENT       ,
		REG_KEY_AUDIO_VOICEOVER     ,
		REG_KEY_AUDIO_MUSIC         ,
		REG_KEY_AUDIO_SUBTITLES     ,
		REG_KEY_AUDIO_ENABLE		,
		REG_KEY_AUDIO_ENABLE3D		,
		REG_KEY_GAMMA               ,
		REG_KEY_DSOUND_IGNORE       ,
		REG_KEY_DDRAW_CERT_IGNORE   ,
		REG_KEY_DDRAW_HARD_IGNORE   ,
		REG_KEY_VIEWPORT_X          ,
		REG_KEY_VIEWPORT_Y          ,
		REG_KEY_RENDERING_QUALITY   ,
		REG_KEY_AUTOLOAD            ,
		REG_KEY_SAFEMODE            ,
		REG_KEY_KEYMAP              ,
		REG_KEY_GORE                ,
		REG_KEY_INVERTMOUSE         ,
		REG_KEY_AUTOSAVE            ,
		strD3D_FILTERCACHES         ,
		strD3D_DITHER               ,
		strVIDEOCARD_TYPE			,
		strVIDEOCARD_NAME			,
		strRECOMMENDEDTEXMAX        ,
		strTRIPLEBUFFER             ,
		strRESTORE_NVIDIA       	,
		strRESTORE_NVIDIAMIPMAPS	,
		strRESTORE_NVIDIASQUARE     ,
		strPAGEMANAGED              ,
		strD3D_TITLE                ,
		strZBUFFER_BITDEPTH			,
		strHARDWARE_WATER			,
		"SwapSpaceMb"				,
		"NoCopyright"				,
		"ShowProgressBar"
	};

	if (regkey.Open(HKEY_LOCAL_MACHINE, "Software\\DreamWorks Interactive\\Trespasser", KEY_READ) != ERROR_SUCCESS)
		return false;

	for (const auto& entry : keys)
	{
		DWORD type = 0;
		ULONG size = 0;
		auto status = regkey.QueryValue(entry.c_str(), &type, nullptr, &size);
		if (status == ERROR_FILE_NOT_FOUND) //key does not exist
			continue;
		if (status != ERROR_SUCCESS)
			continue;

		if (type == REG_BINARY)
			transferBinary(entry, size);
		else if (type == REG_DWORD)
			transferInt(entry);
		else if (type == REG_SZ)
			transferString(entry, size);
	}

	return true;
}

void RegToIniConverter::transferBinary(const std::string& key, ULONG size)
{
	std::vector<uint8_t> data;
	ULONG allocated = size;
	data.resize(size);

	if (regkey.QueryBinaryValue(key.c_str(), data.data(), &allocated) == ERROR_SUCCESS)
		inifile.setBinary(key, data.data(), size);
}

void RegToIniConverter::transferInt(const std::string& key)
{
	DWORD value = 0;
	if (regkey.QueryDWORDValue(key.c_str(), value) == ERROR_SUCCESS)
		inifile.setInt(key, value);
}

void RegToIniConverter::transferString(const std::string& key, ULONG size)
{
	std::string string;
	ULONG allocated = size + 1;
	string.resize(allocated);

	if (regkey.QueryStringValue(key.c_str(), string.data(), &allocated) == ERROR_SUCCESS)
		inifile.setString(key, string.c_str());
}

bool ConvertRegistryToIni(IniFile& inifile)
{
	return RegToIniConverter(inifile).convert();
}
