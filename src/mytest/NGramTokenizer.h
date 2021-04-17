#ifndef _N_GRAM_TOKENIZER_
#define _N_GRAM_TOKENIZER_

#include "CLucene/_ApiHeader.h"
#include "CLucene/analysis/AnalysisHeader.h"
#include "CLucene/util/StringBuffer.h"
#include "CLucene/util/_FastCharStream.h"
#include "CLucene/util/CLStreams.h"

using lucene::analysis::Token;
using lucene::util::FastCharStream;

namespace mytest {
  class CLUCENE_CONTRIBS_EXPORT NGramTokenizer : public lucene::analysis::Tokenizer {
    private:
      int32_t min_gram_size, max_gram_size;
      // offset to point position
    //   int32_t offset, dataLen, bufferIndex;

    //   int32_t ngram_sess_begin, ngram_sess_len, ngram_cur, ngram_cur_len;
    //   TCHAR ngram_buffer[LUCENE_MAX_WORD_LEN];

      TCHAR buffer[LUCENE_MAX_WORD_LEN+1];
	//   const TCHAR* ioBuffer;

      FastCharStream* rd;
      int32_t rdPos;

    public:
      Token* next(Token* token);
      void reset(CL_NS(util)::Reader* _input);
      void close();

      ~NGramTokenizer();
      NGramTokenizer(int32_t min_gram_size_, int32_t max_gram_size_, lucene::util::BufferedReader* _input);

    //   friend class lucene::analysis::Tokenizer;

    inline int readChar();
  };
}

#endif