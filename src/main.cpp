// ConsoleApplication1.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <fstream>
#include <string>
//#include "elfutils.h"
#include "elfutils_wrap.h"

int main(int argc, char* argv[])
{
	uint8_t build_id_type = 0;
	char* filepath = nullptr;

	std::string type_opt = "--type=";
	std::string arg;
	for (int i = 1; i < argc; i++) 
	{
		arg = std::string(argv[i]);
		//printf("arg=%s\n", arg);
		if (arg.find(std::string("-")) == 0) {
			if (arg.find(type_opt) == 0) {
				arg = arg.substr(type_opt.length(), arg.length() - type_opt.length());
				if (arg == "bugly")
				{
					build_id_type = 1;
				}
				else if (arg == "breakpad")
				{
					build_id_type = 0;
				}
				else if (arg == "buildId")
				{
					build_id_type = 2;
				}
				else if (arg == "buildId2breakpad")
				{
					build_id_type = 3;
				}
				else if (arg == "machoId2breakpad")
				{
					build_id_type = 4;
				}
				else
				{
					printf("Error: unknown type %s", arg.c_str());
					exit(-1);
				}
			} 
			else 
			{
				printf("Error: unknown option %s", arg.c_str());
				exit(-1);
			}
		}
		else 
		{
			filepath = argv[i];
		}
	}

	if (filepath == NULL)
	{
		printf("Usage: elfuuid --type=[bugly/breakpad/buildId/buildId2breakpad/machoId2breakpad] <file>\n");
		return -1;
	}

	if (build_id_type == 3)
	{
		std::string buildId = filepath;
		std::string breakpad_uuid = ConvertBuildIdToBreakpadUUID(buildId);
		printf("%s\n", breakpad_uuid.c_str());
		return 0;
	} else if (build_id_type == 4) 
	{
		std::string buildId = filepath;
		std::string breakpad_uuid = ConvertMachoIdToBreakpadUUID(buildId);
		printf("%s\n", breakpad_uuid.c_str());
		return 0;
	}



	//std::string build_id = FindElfBuildID(filepath, build_id_type);
	//printf("%s\n", build_id.c_str());
	//printf("type=%d\n", build_id_type);
	char* build_id = CFindElfBuildID(filepath, build_id_type);
	printf("%s\n", build_id);
    free(build_id);
	return 0;
}