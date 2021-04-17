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
#include <locale>

#include "mytest.h"
#include "jsmn.h"
#include "tool.h"

// #include "CJKAnalyzer.h"
#include "CLucene/analysis/LanguageBasedAnalyzer.h"
#include "CLucene/util/CLStreams.h"
#include "NGramAnalyzer.h"

using namespace std;
using namespace lucene::util;
using mytest::NGramAnalyzer;

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

inline char* make_copy_of(const char* src) {
	char* no_const_char = const_cast<char*>(src);
	char* heap_char = (char*)malloc(strlen(no_const_char) + 1);
	strcpy(heap_char, no_const_char);
	heap_char[strlen(heap_char)] = '\0';
	return heap_char;
}

inline void destroy_io_config(IOFileConfig* conf) {
	free(conf->inputFile);
	free(conf->outputFile);
	// free(conf);
}

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
				// char* input = const_cast<char*>(line.substr(pos + 1).c_str());
				// char* inputCopy = (char*)malloc(strlen(input) + 1);
				// strcpy(inputCopy, input);
				char* input = make_copy_of(line.substr(pos + 1).c_str());
				config->inputFile = input;
				// std::cout << input << std::endl;
				break;
			}
				

			case 'o': {
				char* output = make_copy_of(line.substr(pos + 1).c_str());
				config->outputFile = output;
				break;
			}
			   
			
			default:
				break;
			}
			
		}
	}

	std::cout << config->inputFile << ":" << config->outputFile << std::endl;
}

static const char *JSON_STRING =
    "{\"user\": \"johndoe\", \"admin\": false, \"uid\": 1000,\n  "
    "\"groups\": [\"users\", \"wheel\", \"audio\", \"video\"]}";

static int jsoneq(const char *json, jsmntok_t *tok, const char *s) {
  if (tok->type == JSMN_STRING && (int)strlen(s) == tok->end - tok->start &&
      strncmp(json + tok->start, s, tok->end - tok->start) == 0) {
    return 0;
  }
  return -1;
}

void testing_analyzer() {
	TCHAR sythesized_buf[9] = {
                0x9ec4,
                0x4f1f,
                0x20,
                0x73, 
                0x74, 
                0x6f, 
                0x72, 
                0x79,
                0x0,
            };
	StringReader reader(sythesized_buf);
	lucene::analysis::LanguageBasedAnalyzer an;
	an.setLanguage(_T("cjk"));
	lucene::analysis::Token token;

	lucene::analysis::TokenStream* stream = an.tokenStream(NULL, &reader);
	
	while (stream->next(&token)) {
    std::wcout << "offset: " << token.startOffset() << "->" << token.endOffset() << endl;
	}

	reader.reset(0);

	NGramAnalyzer gram_analyzer(2, 3);
	using TokenStream = lucene::analysis::TokenStream;
	using Token = lucene::analysis::Token;

	TokenStream* ts = gram_analyzer.tokenStream(nullptr, &reader);
	while (ts->next(&token)) {
		_tprintf(token.termBuffer());
		wcout << " token is " << token.termBuffer() << "-" << token.startOffset() << ":" << token.endOffset() << endl;
	}
}

int main( int32_t argc, char** argv ){
	//Dumper Debug
	#ifdef _MSC_VER
	#ifdef _DEBUG
		_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF | _CRTDBG_CHECK_ALWAYS_DF | _CRTDBG_CHECK_CRT_DF );
		_crtBreakAlloc=-1;
	#endif
	#endif

	my_test_whisper();

	jsmn_parser parser;
	jsmn_init(&parser);

	int r = jsmn_parse(&parser, JSON_STRING, strlen(JSON_STRING), NULL, 0);
	cout << "Need " << r << " tokens" << endl;

    jsmn_init(&parser);
	jsmntok_t t[r];
	r = jsmn_parse(&parser, JSON_STRING, strlen(JSON_STRING), t, r);

	// jsmntok_t tok = t[1];
	int tok_size = t[1].end - t[1].start;
	char tok_text[tok_size + 1];
	strncpy(tok_text, JSON_STRING + t[1].start, tok_size);
	tok_text[tok_size] = '\0';
	cout << "first token is " << tok_text << " size " << tok_size << endl;

	TCHAR sythesized_buf[9] = {
                0x9ec4,
                0x4f1f,
                0x20,
                0x73, 
                0x74, 
                0x6f, 
                0x72, 
                0x79,
                0x0,
            };

    cout << "before setting locale, previous locale is " << setlocale(LC_ALL, NULL) << endl;

    setlocale(LC_ALL, "");//zh_CN.UTF-8
	std::wcout.imbue(std::locale("zh_CN.UTF-8"));

	cout << "after setting local to user prefered, current local is " << setlocale(LC_ALL, NULL) << endl;
	char* mb = convert_wchar_to_mb(sythesized_buf);
	cout << "mb=" << mb << "; strlen=" << strlen(mb) << "---->" << count_mb_of_wchr(sythesized_buf) << endl;
	free(mb);

	char* index_dir = (char*)"/Users/huangwei/code/prjs/guazi/clu_idx/index";
	cout << clu_str_num() << endl;

    /**/CLuceneIndexHandler* h = clu_get_index_handler(index_dir, NULL, true);
    CLuceneDocTag tags[1] = {
      {.name = (char*)"lib_mod", .value = (char*)"vue", .flags = 0},
    };
    CLuceneDocConfig doc_config = {
      .tags = tags,
      .tag_size = 1
    };

	char* files_dir = (char*)"/Users/huangwei/code/prjs/guazi/clu_idx/files";

    clu_add_doc_to_index_handler(h, files_dir, &doc_config);
    clu_optimize_index_handler(h);
    clu_free_index_handler(h);

	// getStats(index_dir);

	clu_list_all_terms(index_dir);

    enum CLuError err;
	CLuceneSearchResults* srlts = clu_create_search_results(10, 10, &err);

	CLuceneSearchHandler* sh = clu_get_searcher(index_dir, &err);

#define SHOW_SEARCH_RLT(srlts) \
  cout << "result size = " << srlts->len << ":" << endl;\
  for (size_t i = 0; i < srlts->len; ++i) {\
	cout << "path: " << srlts->list[i].path << endl;\
	if (srlts->list[i].content) {\
	  cout << "matching content " << srlts->list[i].content << endl;\
	}\
	if (srlts->list[i].name) {\
	  cout << "matching name " << srlts->list[i].name << endl;\
	}\
	cout << "-----------" << endl;\
  }

	/*cout << "search for lib_mod:vue" << endl;
	srlts = clu_search(sh, srlts, "lib_mod:vue", &err);//contents:黄伟, OR lib_mod:vue,
	SHOW_SEARCH_RLT(srlts)*/

	// cout << "search results, size=" << srlts->len << "; results[0].name = " << srlts->list[0].path << endl;
	// cout << "search results done" << endl;

	// perform another search
	cout << "search for OpenGL" << endl;
	srlts = clu_search(sh, srlts, "ju", &err);//"contents:Safari"
	SHOW_SEARCH_RLT(srlts)
	
	// cout << "search for opengl, result size = " << srlts->len << ":";
	// if (srlts->len > 0) {
	//   cout << "result[0]=" << srlts->list[0].path;
	// }
	// cout << endl;

	clu_free_searcher(sh);
	clu_free_search_results(srlts);

	testing_analyzer();

	// testing highlighter

	uint64_t str = Misc::currentTimeMillis();

    /*
	
	IOFileConfig config;
	read_local_io_config(&config, __FILE__);
	std::cout << "input: " << config.inputFile << "; output: " << config.outputFile << std::endl;

	try{
		char files[250],ndx[250];

		if (strlen(config.inputFile) > 0 && strlen(config.outputFile) > 0) {
			// IndexFiles(config.inputFile, config.outputFile, true);
			strcpy(files, config.inputFile);
			strcpy(ndx, config.outputFile);
		} else {
			printf("Location of text files to be indexed[%s]: ", __FILE__);
			
			char* tmp = fgets(files,250,stdin);
			if ( tmp == NULL ) return 1;
			files[strlen(files)-1] = 0;
			
			printf("Location to store the clucene index: ");
			
			tmp = fgets(ndx,250,stdin);
			if ( tmp == NULL ) return 1;
			ndx[strlen(ndx)-1] = 0;
		}

		IndexFiles(files,ndx,true);
        getStats(ndx);
        SearchFiles(ndx);
        //DeleteFiles(ndx);

		destroy_io_config(&config);

    }catch(CLuceneError& err){
        printf("Error: %s\n", err.what());
    }catch(...){
        printf("Unknown error\n");
    }
*/

	_lucene_shutdown(); //clears all static memory
    //print lucenebase debug

	//Debugging techniques:
	//For msvc, use this for breaking on memory leaks: 
	//	_crtBreakAlloc
	//for linux, use valgrind

	printf ("\n\nTime taken: %d\n\n", (int32_t)(Misc::currentTimeMillis() - str));

	return 0;
}
