/*------------------------------------------------------------------------------
* Copyright (C) 2003-2006 Ben van Klinken and the CLucene Team
* 
* Distributable under the terms of either the Apache License (Version 2.0) or 
* the GNU Lesser General Public License, as specified in the COPYING file.
------------------------------------------------------------------------------*/
#include "CLucene/StdHeader.h"
#include "CLucene/_clucene-config.h"

#include "CLucene.h"
#include "CLucene/util/Misc.h"

//test for memory leaks:
#ifdef _MSC_VER
#ifdef _DEBUG
	#define _CRTDBG_MAP_ALLOC
	#include <stdlib.h>
	#include <crtdbg.h>
#endif
#endif

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string.h>
#include <libgen.h>
#include <sstream>

using namespace std;
using namespace lucene::util;

//void DeleteFiles(const char* dir);
void IndexFiles(const char* path, const char* target, const bool clearIndex);
void SearchFiles(const char* index);
void getStats(const char* directory);

// additional read file config
struct _IOFileConfig {
	char* inputFile;
	char* outputFile;
};
typedef struct _IOFileConfig IOFileConfig;

enum pos_state_of_config {
	pos_state_init,
	pos_state_key,
	pos_state_value,
};

void read_local_io_config(IOFileConfig* config, char* initDir) {
	char* dir = dirname(initDir);
	char* config_file = strcat(dir, "/io_config");
	std::cout << config_file << std::endl;

	std::ifstream ifs(config_file, std::ios::binary);
	if (!ifs.is_open()) {
		perror("can not open config file");
		return;
	}

	std::string line;
	size_t pos;
	char* buf;
	while (std::getline(ifs, line)) {
		// std::cout << "line " << line << std::endl;
		if (std::string::npos != (pos = line.find("=", 0))) {
			switch (line[0])
			{
			case 'i': {
				char* input = const_cast<char*>(line.substr(pos + 1).c_str());
				char* inputCopy = (char*)malloc(strlen(input) + 1);
				strcpy(inputCopy, input);
				config->inputFile = input;
				// std::cout << input << std::endl;
				break;
			}
				

			case 'o':
			   config->outputFile = const_cast<char*>(line.substr(pos + 1).c_str());
				break;
			
			default:
				break;
			}
			
		}
	}

	std::cout << config->inputFile << ":" << config->outputFile << std::endl;
}

int main( int32_t argc, char** argv ){
	//Dumper Debug
	#ifdef _MSC_VER
	#ifdef _DEBUG
		_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF | _CRTDBG_CHECK_ALWAYS_DF | _CRTDBG_CHECK_CRT_DF );
		_crtBreakAlloc=-1;
	#endif
	#endif

	uint64_t str = Misc::currentTimeMillis();

	IOFileConfig config;
	read_local_io_config(&config, __FILE__);
	std::cout << "input: " << config.inputFile << std::endl;

	try{

    	printf("Location of text files to be indexed[%s]: ", __FILE__);
    	char files[250];
		char* tmp = fgets(files,250,stdin);
		if ( tmp == NULL ) return 1;
		files[strlen(files)-1] = 0;
		
		printf("Location to store the clucene index: ");
		char ndx[250];
		tmp = fgets(ndx,250,stdin);
		if ( tmp == NULL ) return 1;
		ndx[strlen(ndx)-1] = 0;

		IndexFiles(files,ndx,true);
        getStats(ndx);
        SearchFiles(ndx);
        //DeleteFiles(ndx);

    }catch(CLuceneError& err){
        printf("Error: %s\n", err.what());
    }catch(...){
        printf("Unknown error\n");
    }

	_lucene_shutdown(); //clears all static memory
    //print lucenebase debug

	//Debugging techniques:
	//For msvc, use this for breaking on memory leaks: 
	//	_crtBreakAlloc
	//for linux, use valgrind

	printf ("\n\nTime taken: %d\n\n", (int32_t)(Misc::currentTimeMillis() - str));
	return 0;
}
