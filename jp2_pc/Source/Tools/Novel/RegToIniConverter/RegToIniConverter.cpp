#include "Lib/Sys/RegToIni.hpp"

int main(int argc, char** argv)
{
	std::string targetfile = argc >= 2 ? argv[1] : "OpenTrespasser.ini";

	IniFile inifile(targetfile, "OpenTrespasser");
	bool outcome = ConvertRegistryToIni(inifile);

	return outcome ? EXIT_SUCCESS : EXIT_FAILURE;
}
