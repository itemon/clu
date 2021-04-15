#ifndef _N_GRAM_TOKENIZER_
#define _N_GRAM_TOKENIZER_

#include "CLucene/_ApiHeader.h"
#include "CLucene/analysis/AnalysisHeader.h"

using lucene::analysis::Token;

namespace mytest {
  class CLUCENE_CONTRIBS_EXPORT NGramTokenizer : public lucene::analysis::Tokenizer {
    private:
      int32_t min_gram_size, max_gram_size, dataLen, bufferIndex;
      // offset to point position
      int32_t offset;

      int32_t ngram_sess_begin, ngram_sess_len, ngram_cur, ngram_cur_len;
      TCHAR ngram_buffer[LUCENE_MAX_WORD_LEN];

      TCHAR buffer[LUCENE_MAX_WORD_LEN+1];
	  const TCHAR* ioBuffer;

    public:
      Token* next(Token* token);
      void reset(CL_NS(util)::Reader* _input);
      void close();

      ~NGramTokenizer();
      NGramTokenizer(int32_t min_gram_size_, int32_t max_gram_size_, lucene::util::Reader* _input);

    //   friend class lucene::analysis::Tokenizer;
  };
}

#endif