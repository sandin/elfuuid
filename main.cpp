// ConsoleApplication1.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
//#include "StatsFileReader.h"
//#include "Ue4StatsFileReader.h"
#include "elfutils.h"
#include <fstream>

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
		printf("Usage: elfuuid --type=[bugly/breakpad] <file>");
		return -1;
	}

	std::string build_id = FindElfBuildID(filepath, build_id_type);
	printf("%s\n", build_id.c_str());
	return 0;
}


