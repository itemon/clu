#ifndef MY_TEST_H
#define MY_TEST_H
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

enum CLuError {
  clu_err_no_err = 0,
  clu_err_index_not_found,
  clu_err_index_corupted,
  clu_err_no_mem,
  clu_err_param_is_nptr,
};

/**
 * testing api
 */ 
const char* clu_str_num(void);

// clucene handle proxy struct
struct _CLuceneIndexHandler {};
typedef struct _CLuceneIndexHandler CLuceneIndexHandler;

struct _CLuceneDocTag {
  char *name, *value;
  int flags;
};
typedef _CLuceneDocTag CLuceneDocTag;

struct _CLuceneDocConfig {
  CLuceneDocTag* tags;
  int tag_size;
};
typedef _CLuceneDocConfig CLuceneDocConfig;

// contructing index store handler
CLuceneIndexHandler* clu_get_index_handler(const char* index_store_dir, enum CLuError* err, bool startover);

/**
 * add files rooted at file to this index writer
 */
void clu_add_doc_to_index_handler(CLuceneIndexHandler* handler, const char* dir, CLuceneDocConfig* config);

void clu_add_custom_doc_to_index_handler(CLuceneIndexHandler* handler, CLuceneDocConfig* config);

/**
 * optimizie the index
 */
void clu_optimize_index_handler(CLuceneIndexHandler* handler);

// free index store handler
void clu_free_index_handler(CLuceneIndexHandler* index_handler);

// about search c-api
// search api
typedef struct _CLuceneSearchHandler {} CLuceneSearchHandler;
CLuceneSearchHandler* clu_get_searcher(const char* index_store_dir, enum CLuError *err);

void clu_free_searcher(CLuceneSearchHandler* search_handler);

typedef struct _CLuceneSearchResult {
  char* name;
  char* path;
} CLuceneSearchResult;

typedef struct _CLuceneSearchResults {
  CLuceneSearchResult* list;
  size_t len;
  size_t max_len; /*config max length of the result list*/
} CLuceneSearchResults;

void clu_list_all_terms(const char* index_store_dir);

/**
 * create search results 
 */
CLuceneSearchResults* clu_create_search_results(size_t list_size, size_t max_size, enum CLuError *err);

CLuceneSearchResults* clu_search(CLuceneSearchHandler* search_handler, CLuceneSearchResults* rlts, const char* query, enum CLuError *err);

void clu_free_search_results(CLuceneSearchResults* results);

#ifdef __cplusplus
}
#endif

void my_test_whisper();

#endif