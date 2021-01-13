#include <iostream>
#include <cstdlib>
#include <cstring>

#include "CLucene/StdHeader.h"
#include "CLucene/_clucene-config.h"

#include "CLucene.h"
#include "CLucene/util/CLStreams.h"
#include "CLucene/util/dirent.h"
#include "CLucene/config/repl_tchar.h"
#include "CLucene/util/Misc.h"
#include "CLucene/util/StringBuffer.h"

#include "CLucene/analysis/LanguageBasedAnalyzer.h"

#include "mytest.h"
#include "tool.h"
#include "jsmn.h"

#define EXPORT __attribute__((visibility("default")))

using namespace std;
using namespace lucene::document;
using namespace lucene::index;
using namespace lucene::analysis;
using namespace lucene::analysis::standard;
using namespace lucene::util;
using namespace lucene::search;

EXPORT void my_test_whisper() {
#ifdef MYTEST_INC_FINGER
  cout << "MYTTEST_INC_FINDER flag on" << endl;
#endif
}

extern "C" {
  EXPORT const char* clu_str_num() {
    return _CL_VERSION;
  }

  EXPORT CLuceneIndexHandler* clu_get_index_handler(const char* index_store_dir, enum CLuError* err, bool startover) {
    IndexWriter* index_writer;

    // test if index is exists already
    bool has_index = IndexReader::indexExists(index_store_dir);
    if (has_index && IndexReader::isLocked(index_store_dir)) {
      IndexReader::unlock(index_store_dir);
    }

    LanguageBasedAnalyzer* an = _CLNEW LanguageBasedAnalyzer(_T("cjk"));
    // StandardAnalyzer* san = _CLNEW StandardAnalyzer();

    index_writer = _CLNEW IndexWriter(index_store_dir, an, startover || !has_index);
    index_writer->setMaxFieldLength(0x7FFFFFFFL);
    index_writer->setUseCompoundFile(false);

    return reinterpret_cast<CLuceneIndexHandler*>(index_writer);
  }

  #define CAST_HANDLER(handler) \
    IndexWriter* index_writer = reinterpret_cast<IndexWriter*>(handler);

  char* _read_file_content(const char* file) {
    FILE* file_handler = fopen(file, "rb");
    if (!file_handler) {
      return NULL;
    }

    if (fseek(file_handler, 0L, SEEK_END) != 0) {
      return NULL;
    }

    const long content_length = ftell(file_handler);
    if (fseek(file_handler, 0L, SEEK_SET) != 0) {
      return NULL;
    }

    char* content = (char*)malloc(content_length + 1);
    if (!content) {
      return NULL;
    }

    size_t n = fread(content, 1, content_length, file_handler);
    fclose(file_handler);

    if (n != content_length) {
      return NULL;
    }
    content[content_length] = '\0';
    return content;
  }

/*#define CONVERT_CHR_TO_WCHAR(VAR_SUFFIX) \
  len = strlen(entity);\
  TCHAR entity_wchr_##VAR_SUFFIX[len + 1];\
  len = convert_multi_byte_to_wchar(entity, entity_wchr_##VAR_SUFFIX);\
  entity_wchr_##VAR_SUFFIX[len] = '\0';*/

#define CVT_CHR_TO_WCHAR(entity, VAR_SUFFIX) \
  len = strlen(entity);\
  TCHAR wchr_##VAR_SUFFIX[len + 1];\
  len = convert_multi_byte_to_wchar(entity, wchr_##VAR_SUFFIX);\
  wchr_##VAR_SUFFIX[len] = '\0';

  static int jsoneq(const char *json, jsmntok_t *tok, const char *s) {
    if (tok->type == JSMN_STRING && (int)strlen(s) == tok->end - tok->start &&
        strncmp(json + tok->start, s, tok->end - tok->start) == 0) {
      return 0;
    }
    return -1;
  }

  // build char* value of travel_token
  // the name of the char* value is named by name param
  // using pattern name_val
  #define BUILD_TOKEN_VALUE(name, str, travel_token) \
    char name##_val[travel_token.end - travel_token.start + 1]; \
    strncpy(name##_val, str + travel_token.start, travel_token.end - travel_token.start); \
    name##_val[travel_token.end - travel_token.start] = '\0';

  // writing index to doc
  void _writing_index(Document* doc, const char* cur, CLuceneDocConfig* config, bool is_json) {
    size_t len;

    if (cur) {
      if (is_json) {
        jsmn_parser parser;
        jsmn_init(&parser);

        size_t cur_len = strlen(cur);
        int token_needs = jsmn_parse(&parser, cur, cur_len, NULL, 0);
        if (token_needs <= 0) {
          goto proceed_final_prop;
          return;
        }
        jsmntok_t doc_tokens[token_needs];
        jsmn_init(&parser);

        token_needs = jsmn_parse(&parser, cur, cur_len, doc_tokens, token_needs);
        jsmntok_t travel_token = doc_tokens[0];
        switch (travel_token.type) {
          case JSMN_ARRAY: {
            break;
          }
          case JSMN_OBJECT: {
            // top level object was considered as single doc
            // int doc_type, doc_sub_type;
            for (size_t i = 1; i < token_needs; ++i) {
              if (jsoneq(cur, &doc_tokens[i], "attr_doc") == 0) {
                travel_token = doc_tokens[i + 1];
                if (travel_token.type == JSMN_OBJECT && travel_token.size > 0) {
                  BUILD_TOKEN_VALUE(doc, cur, travel_token)
                  for (size_t j = 0; j < travel_token.size; ++j) {
                    if (jsoneq(cur, &doc_tokens[i + 1 + j], "type") == 0) {
                      // doc type is
                      BUILD_TOKEN_VALUE(typ, cur, doc_tokens[i + 1 + j + 1])
                      // doc_type = atoi(typ_val);

                      /*len = strlen(typ_val);
                      TCHAR wide_doc_type[len + 1];
                      STRCPY_AtoT(wide_doc_type, typ_val, len);
                      wide_doc_type[len] = '\0';*/

                      CVT_CHR_TO_WCHAR(typ_val, doc_type);

                      doc->add(*_CLNEW Field(_T("doc_type"), 
                        wchr_doc_type, 
                        Field::STORE_YES | Field::INDEX_UNTOKENIZED));
                      
                    } else if (jsoneq(cur, &doc_tokens[i + 1 + j], "sub_type") == 0) {
                      BUILD_TOKEN_VALUE(sub_typ, cur, doc_tokens[i + 1 + j + 1])
                      // doc_sub_type = atoi(sub_typ_val);

                      CVT_CHR_TO_WCHAR(sub_typ_val, doc_sub_type)
                      doc->add(*_CLNEW Field(_T("doc_sub_type"), 
                        wchr_doc_sub_type, 
                        Field::STORE_YES | Field::INDEX_UNTOKENIZED));
                    } else if (jsoneq(cur, &doc_tokens[i + 1 + j], "parent") == 0) {
                      // has parent class
                      BUILD_TOKEN_VALUE(parent, cur, doc_tokens[i + 1 + j + 1])
                      CVT_CHR_TO_WCHAR(parent_val, parent)
                      
                      doc->add(* _CLNEW Field(_T("doc_parent"), wchr_parent, Field::STORE_YES | Field::INDEX_UNTOKENIZED));
                    } else if (jsoneq(cur, &doc_tokens[i + 1 + j], "namespace") == 0) {
                      BUILD_TOKEN_VALUE(namespace_type, cur, doc_tokens[i + 1 + j])
                      CVT_CHR_TO_WCHAR(namespace_type_val, namespace_type)

                      doc->add(* _CLNEW Field(_T("doc_namespace"), wchr_namespace_type, Field::STORE_YES | Field::INDEX_UNTOKENIZED));
                    }
                  }
                }
                ++i;
                break;
              }
            }
            break;
          }
          default: {
            break;
          }
        }
      } else {
        CVT_CHR_TO_WCHAR(cur, content)
        doc->add( 
          *_CLNEW Field(_T("contents"), wchr_content, Field::STORE_YES | Field::INDEX_TOKENIZED)
        );
      }
    }

  proceed_final_prop:
    if (config != NULL && config->tag_size > 0) {
      char* entity;
      int flags;
      for (int i = 0; i < config->tag_size; ++i) {
        entity = config->tags[i].name;
        CVT_CHR_TO_WCHAR(entity, name)

        entity = config->tags[i].value;
        CVT_CHR_TO_WCHAR(entity, value)

        flags = config->tags[i].flags;
        if (flags == 0) {
          flags = Field::STORE_YES | Field::INDEX_UNTOKENIZED;
        }

        doc->add(
          *_CLNEW Field(wchr_name, wchr_value, flags)
        );
      }
    }
  }

  EXPORT void clu_add_doc_to_index_handler(CLuceneIndexHandler* handler, const char* dir, CLuceneDocConfig* config) {
    CAST_HANDLER(handler);
    vector<string> files;
    Misc::listFiles(dir, files, true);

    vector<string>::iterator leading = files.begin();
    char* cur;
    size_t len;
    Document doc;
    const char* path;

    while (leading != files.end()) {
      path = leading->c_str();
      cur = _read_file_content(path);

      doc.clear();

      len = strlen(path);
      TCHAR wchr_path[len+1];
      len = convert_multi_byte_to_wchar(path, wchr_path);
      wchr_path[len] = '\0';
      doc.add(*_CLNEW Field(_T("path"), wchr_path, Field::STORE_YES | Field::INDEX_UNTOKENIZED));

      bool is_index_file = str_end_with(path, ".index.json");
      _writing_index(&doc, cur, config, is_index_file);
      index_writer->addDocument(&doc);

      if (cur != 0)
        free(cur);
      ++leading;
    }
  }

  EXPORT void add_custom_doc_to_index_handler(CLuceneIndexHandler* handler, CLuceneDocConfig* config) {
    CAST_HANDLER(handler);
    if (config && config->tag_size > 0) {
      Document doc;
      _writing_index(&doc, NULL, config, false);
      index_writer->addDocument(&doc);
    }
  }

  EXPORT void clu_optimize_index_handler(CLuceneIndexHandler* handler) {
    CAST_HANDLER(handler);
    index_writer->setUseCompoundFile(true);
    index_writer->optimize();
  }

  EXPORT void clu_free_index_handler(CLuceneIndexHandler* handler) {
    CAST_HANDLER(handler);
    if (index_writer->getAnalyzer())
      _CLLDELETE(index_writer->getAnalyzer());
    index_writer->close();
    _CLLDELETE(index_writer);
  }

  EXPORT CLuceneSearchHandler* clu_get_searcher(const char* index_store_dir, enum CLuError *err) {
    const bool has_index = IndexReader::indexExists(index_store_dir);
    if (!has_index) {
      *err = clu_err_index_not_found;
      return NULL;
    }
    IndexReader* r = IndexReader::open(index_store_dir);
    IndexSearcher* s = _CLNEW IndexSearcher(r);

    return reinterpret_cast<CLuceneSearchHandler*>(s);
  }

  EXPORT void clu_free_searcher(CLuceneSearchHandler* search_handler) {
    IndexSearcher* s = reinterpret_cast<IndexSearcher*>(search_handler);
    IndexReader* r = s->getReader();
    s->close();
    r->close();

    _CLLDELETE(s);
    _CLLDELETE(r);
  }

  EXPORT void clu_list_all_terms(const char* index_store_dir) {
    IndexReader* r = IndexReader::open(index_store_dir);
    printf("Statistics for %s\n", index_store_dir);
    printf("==================================\n");

    printf("Max Docs: %d\n", r->maxDoc() );
    printf("Num Docs: %d\n", r->numDocs() );

    int64_t ver = r->getCurrentVersion(index_store_dir);
    _tprintf(_T("Current Version: %f\n"), (float_t)ver );

    TermEnum* te = r->terms();
    int32_t nterms;
    for (nterms = 0; te->next() == true; nterms++) {
              /* empty */
              const TCHAR* chr = te->term(true)->text();
        // _tprintf(_T("term %s\n"),
            // chr);
        std::wcout << chr << std::endl;
      }
    printf("Term count: %d\n\n", nterms );
    _CLLDELETE(te);

    r->close();
    _CLLDELETE(r);
  }

  EXPORT void clu_free_search_results(CLuceneSearchResults* results) {
    size_t i = 0;
    while (i < results->len) {
      CLuceneSearchResult r = results->list[i];
      // std::cout << "start to free here " << r.path << std::endl;
      if (r.name != NULL)
        free(r.name);
      if (r.path != NULL)
        free(r.path);
      ++i;
    }
    free(results->list);
    free(results);
  }

  EXPORT CLuceneSearchResults* clu_create_search_results(size_t list_size, enum CLuError *err) {
    CLuceneSearchResults* rlts = (CLuceneSearchResults*)malloc(sizeof(CLuceneSearchResults));
    rlts->len = list_size;
    CLuceneSearchResult* list = (CLuceneSearchResult*)malloc(sizeof(CLuceneSearchResult) * list_size);
    rlts->list = list;
    return rlts;
  }

  /**
   * on searching for index file, we separate it by masks 
   * 1 global function
   * 11 global variable
   * 12 global macro varaible
   * 13 global macro function
   * 
   * 2 class
   * 21 class method
   * 22 class variable
   * 23 class stitic method
   * 24 class static variable
   * 
   * on distinguish doc type
   * 0 api
   * 1 tutorial
   */

  EXPORT CLuceneSearchResults* clu_search(CLuceneSearchHandler* handler, CLuceneSearchResults* rlts, const char* query, enum CLuError *err) {
    IndexSearcher* s = reinterpret_cast<IndexSearcher*>(handler);
    using QueryParser = lucene::queryParser::QueryParser;

    // size_t len = strlen(query);
    // TCHAR wchr_query[len+1];
    // len = convert_multi_byte_to_wchar(query, wchr_query);
    // wchr_query[len] = '\0';

    size_t len;
    CVT_CHR_TO_WCHAR(query, qry)

    const TCHAR* inquiry = wchr_qry;
    LanguageBasedAnalyzer an;
    an.setLanguage(_T("cjk"));

    QueryParser parser(_T("contents"), &an);
    Query* q = parser.parse(inquiry);
    
    Hits* h = s->search(q);
    len = h->length();

    const TCHAR* p;

    // allocate memory for result stucture
    // we'll try to reuse memory for frequently search request
    // scene
    if (rlts == NULL) {
      rlts = (CLuceneSearchResults*)malloc(sizeof(CLuceneSearchResults));
      rlts->list = NULL;
    }

    CLuceneSearchResult *items = rlts->list;
    if (!items) {
      items = (CLuceneSearchResult*)malloc(sizeof(CLuceneSearchResult) * len);
    } else {
      // std::cout << "debug brk" << rlts->list << std::endl;
      if (len > rlts->len) {
        // reallocate memory
        items = (CLuceneSearchResult*)realloc(rlts->list, len);
      }
    }

    for (size_t i = 0; i < len; i++) {
      Document& doc = h->doc(i);
      p = doc.get(_T("path"));
      // _tprintf(p);

      // item = (CLuceneSearchResult*)malloc(sizeof(CLuceneSearchResult));
      // item->path = convert_wchar_to_mb((TCHAR*)p);
      // items[i] = *item;
      items[i].path = convert_wchar_to_mb((TCHAR*)p);
      items[i].name = 0;
    }

    rlts->len = len;
    rlts->list = items;

    _CLLDELETE(h);
    _CLLDELETE(q);
    
    return rlts;
  }
}