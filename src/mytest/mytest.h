#ifndef MY_TEST_H
#define MY_TEST_H
#pragma once

#ifdef __cplusplus
extern "C" {
#endif
/**
 * testing api
 */ 
const char* clu_str_num(void);

// clucene handle proxy struct
struct _CLuceneIndexHandler {};
typedef struct _CLuceneIndexHandler CLuceneIndexHandler;

struct _CLuceneDocTag {
  char *name, *value;
};
typedef _CLuceneDocTag CLuceneDocTag;

struct _CLuceneDocConfig {
  CLuceneDocTag* tags;
  int tag_size;
};
typedef _CLuceneDocConfig CLuceneDocConfig;

// contructing index store handler
CLuceneIndexHandler* clu_get_index_handler(const char* index_store_dir);

/**
 * add files rooted at file to this index writer
 */
void clu_add_doc_to_index_handler(CLuceneIndexHandler* handler, const char* dir, CLuceneDocConfig* config);

/**
 * optimizie the index
 */
void clu_optimize_index_handler(CLuceneIndexHandler* handler);

// free index store handler
void clu_free_index_handler(CLuceneIndexHandler* index_handler);

#ifdef __cplusplus
}
#endif

void my_test_whisper();

#endif