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

#define EXPORT __attribute__((visibility("default")))

using namespace std;
using namespace lucene::document;
using namespace lucene::index;
using namespace lucene::analysis;
using namespace lucene::util;

EXPORT void my_test_whisper() {
  int64_t d = DateTools::stringToTime(_T("2012"));
  TCHAR* tmp = DateTools::getISOFormat(d);

#ifdef MYTEST_INC_FINGER
  cout << "finger here" << endl;
#endif

  string dat;
  int ret;
  char cur[3];

  wctomb(nullptr, 0);
  while (*tmp != '\0') {
    ret = wctomb(cur, *tmp);
    if (ret <= 0)
      break;
    dat.insert(dat.size(), cur, ret);
    tmp++;
  }

  cout << dat << endl;
}

extern "C" {
  EXPORT const char* clu_str_num() {
    return "1.0.0";
  }

  EXPORT CLuceneIndexHandler* clu_get_index_handler(const char* index_store_dir) {
    IndexWriter* index_writer;

    // test if index is exists already
    const bool has_index = IndexReader::indexExists(index_store_dir);
    if (has_index && IndexReader::isLocked(index_store_dir)) {
      IndexReader::unlock(index_store_dir);
    }

    LanguageBasedAnalyzer* an = _CLNEW LanguageBasedAnalyzer(_T("cjk"));
    // an.setLanguage(_T("cjk"));

    index_writer = _CLNEW IndexWriter(index_store_dir, an, !has_index);
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

  void _writing_index(IndexWriter* index_writer, Document* doc, const char* cur) {
    size_t len;
    len = strlen(cur);
    TCHAR wchr[len + 1];
    len = convert_multi_byte_to_wchar(cur, wchr);
    wchr[len] = '\0';

    doc->clear();

    doc->add( 
      *_CLNEW Field(_T("contents"), wchr, Field::STORE_YES | Field::INDEX_TOKENIZED)
    );

    doc->add(
      *_CLNEW Field(_T("lib"), _T("vue"), Field::STORE_YES)
    );

    index_writer->addDocument(doc);
  }

  EXPORT void clu_add_doc_to_index_handler(CLuceneIndexHandler* handler, const char* dir) {
    CAST_HANDLER(handler);
    vector<string> files;
    Misc::listFiles(dir, files, true);

    vector<string>::iterator leading = files.begin();
    char* cur;
    size_t len;
    Document doc;

    while (leading != files.end()) {
      cur = _read_file_content(leading->c_str());
      _writing_index(index_writer, &doc, cur);

      if (cur != 0)
        free(cur);
      ++leading;
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
}