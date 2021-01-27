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
#include "CLucene/highlighter/Highlighter.h"
#include "CLucene/highlighter/QueryScorer.h"
#include "CLucene/highlighter/SimpleFragmenter.h"

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
using namespace lucene::search::highlight;

EXPORT void my_test_whisper() {
#ifdef MYTEST_INC_FINGER
  cout << "MYTTEST_INC_FINDER flag on" << endl;
#endif
}

extern "C" {
  // declaring prototype of json parsing function
  void json_array_token(Document* doc, const char* json, jsmntok_t* all_tok, jsmntok_t* tok, size_t* i);
  void json_object_token(Document* doc, const char* json, jsmntok_t* all_tok, jsmntok_t* tok, size_t* i);

  #define CHECK_AND_RTN_NO_MEM(ptr) \
    if (unlikely(!ptr)) {\
      *err = clu_err_no_mem;\
      return nullptr;\
    }

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

  #define BUILD_INDEX(key_val, wchr_key, wchr_val) \
    int flag = Field::STORE_YES | Field::INDEX_TOKENIZED;\
    if (strcmp(key_val, "category") == 0 \
        || strcmp(key_val, "path") == 0 \
        || strcmp(key_val, "type") == 0 \
        || strcmp(key_val, "sub_type") == 0 \
        || strcmp(key_val, "parent") == 0 \
        || strcmp(key_val, "namespace") == 0) {\
      flag = Field::STORE_YES | Field::INDEX_UNTOKENIZED;\
    }\
    doc->add(* _CLNEW Field(wchr_key, wchr_val, flag));

  /**
   * param {i} specify the index of current tok
   * param {tok} the current token 
   */
  void json_array_token(Document* doc, const char* json, jsmntok_t* all_toks, jsmntok_t* tok, size_t* i) {
    if (tok->type == JSMN_ARRAY && tok->size > 0) {
      // string array value share common key
      // the key token is back by 2 steps
      size_t len;

      // due to scope problem
      // char* spe_key_val;
      // TCHAR* spe_wchr_key;
      jsmntok_t* key_tok;
      if (*i > 0) {
        key_tok = &all_toks[*i - 1];
      }
      // BUILD_TOKEN_VALUE(key, json, all_toks[*i - 1])
      // CVT_CHR_TO_WCHAR(key_val, key)
      // spe_key_val = key_val;
      // spe_wchr_key = wchr_key;

      for (size_t j = 0; j < tok->size; ++j) {
        *i += 1;
        jsmntok_t tok = all_toks[*i];
        switch (tok.type) {
        case JSMN_OBJECT: {
          if (tok.size > 0) {
            json_object_token(doc, json, all_toks, &tok, i);
          }
          break;
        }
          
        case JSMN_STRING: {
          jsmntok_t real_key_tok = *key_tok;
          BUILD_TOKEN_VALUE(key, json, real_key_tok)
          CVT_CHR_TO_WCHAR(key_val, key)

          BUILD_TOKEN_VALUE(val, json, tok)
          CVT_CHR_TO_WCHAR(val_val, val)

          wcout << "[json array token]key: " << wchr_key << ":" << wchr_val << endl;
          BUILD_INDEX(key_val, wchr_key, wchr_val)
          
          break;
        }

        default: {
          break;
        }
          
        }
      }
    }
  }

  void json_object_token(Document* doc, const char* json, jsmntok_t* all_toks, jsmntok_t* tok, size_t* i) {
    if (tok->type == JSMN_OBJECT && tok->size > 0) {
      size_t ack_remaining = tok->size;
      size_t len;

      while (ack_remaining > 0) {
        *i += 1;
        BUILD_TOKEN_VALUE(key, json, all_toks[*i])
        CVT_CHR_TO_WCHAR(key_val, key)
        *i += 1;

        switch (all_toks[*i].type)
        {
        case JSMN_ARRAY: {
          json_array_token(doc, json, all_toks, &all_toks[*i], i);
          break;
        }

        case JSMN_OBJECT: {
          json_object_token(doc, json, all_toks, &all_toks[*i], i);
          break;
        }
        
        default: {
          BUILD_TOKEN_VALUE(val, json, all_toks[*i])
          CVT_CHR_TO_WCHAR(val_val, val)
          wcout << "[json_object_token] key:" << wchr_key << ":" << wchr_val << endl;
          BUILD_INDEX(key_val, wchr_key, wchr_val)
          break;
        }
        }
        
        ack_remaining--;
      }
    }
  }

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
        size_t i = 0;
        switch (travel_token.type) {
          case JSMN_ARRAY: {
            // cout << "category " << index_flag_of_pros["category"] << endl;
            json_array_token(doc, cur, doc_tokens, &travel_token, &i);
            break;
          }
          case JSMN_OBJECT: {
            json_object_token(doc, cur, doc_tokens, &travel_token, &i);
            break;
          }
          default: {
            // primitive type is not supposed to be legal here
            // drop that data
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

  EXPORT CLuceneSearchResults* clu_create_search_results(size_t list_size, size_t max_size, enum CLuError *err) {
    CLuceneSearchResults* rlts = (CLuceneSearchResults*)malloc(sizeof(CLuceneSearchResults));
    CHECK_AND_RTN_NO_MEM(rlts)
    rlts->len = list_size;

    CLuceneSearchResult* list = (CLuceneSearchResult*)malloc(sizeof(CLuceneSearchResult) * list_size);
    CHECK_AND_RTN_NO_MEM(list)

    for (size_t i = 0; i < list_size; ++i) {
      list[i].path = nullptr;
      list[i].name = nullptr;
    }
    rlts->list = list;
    rlts->max_len = max_size;

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

    size_t len;
    CVT_CHR_TO_WCHAR(query, qry)

    // const TCHAR* inquiry = wchr_qry;
    LanguageBasedAnalyzer an;
    an.setLanguage(_T("cjk"));

    QueryParser parser(_T("contents"), &an);
    Query* q = parser.parse(wchr_qry);
    
    Hits* h = s->search(q);
    len = h->length();

    const TCHAR* p;

    // char* helleworld = "the quick fox jumps over lazy dog";
    Term term(_T("contents"), _T("function"));
    TermQuery term_query(&term);
    QueryScorer scorer(&term_query);//, s->getReader(), _T("name")
    SimpleFragmenter frag;
    Highlighter hl(&scorer);
    hl.setTextFragmenter(&frag);

    // allocate memory for result stucture
    // we'll try to reuse memory for frequently search request
    // scene
    if (rlts == NULL) {
      *err = clu_err_param_is_nptr;
      return nullptr;
    }

    // if max_len was configed, just use it
    if (rlts->max_len > 0) {
      len = rlts->max_len >= len ? len : rlts->max_len;
    }

    CLuceneSearchResult *items = rlts->list;
    if (unlikely(!items)) {
      *err = clu_err_param_is_nptr;
      return nullptr;
    }
    /*if (!items) {
      items = (CLuceneSearchResult*)malloc(sizeof(CLuceneSearchResult) * len);
    } else {
      if (len > rlts->len) {
        // reallocate memory
        items = (CLuceneSearchResult*)realloc(rlts->list, len);
      }
    }*/

  #define DECODE_CHR_FROM_TCHR() \
    size_t p_size = count_mb_of_wchr((TCHAR*)p);\
    if (!prop) {\
      prop = (char*)calloc(1, p_size + 1);\
      if (!prop) {\
        *err = clu_err_no_mem;\
        break;\
      }\
    } else {\
      prop = (char*)realloc(prop, strlen(prop) + 1);\
    }\
    convert_wchar_to_mb((TCHAR*)p, prop);

    char* prop;
    for (size_t i = 0; i < len; i++) {
      Document& doc = h->doc(i);

      p = doc.get(_T("path"));
      prop = items[i].path;
      DECODE_CHR_FROM_TCHR()
      items[i].path = prop;

      p = doc.get(_T("contents"));
      if (p) {
        cout << "using highlighting...";
        TCHAR* hl_rlt = hl.getBestFragment(&an, _T("contents"), p);
        // _tprintf(hl_rlt);
        cout << hl_rlt;
        cout << endl;
      }
      items[i].name = 0;

      prop = nullptr;
    }

    rlts->len = len;
    rlts->list = items;

    _CLLDELETE(h);
    _CLLDELETE(q);
    
    return rlts;
  }
}