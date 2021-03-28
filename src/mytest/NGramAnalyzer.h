#ifndef _NGRAM_ANALYZER
#define _NGRAM_ANALYZER

#include "CLucene/_ApiHeader.h"
#include "CLucene/analysis/AnalysisHeader.h"

using lucene::analysis::TokenStream;

namespace mytest {
  class CLUCENE_CONTRIBS_EXPORT NGramAnalyzer : public lucene::analysis::Analyzer {
    private:
      int32_t min_gram_size, max_gram_size;

    public:
      TokenStream* tokenStream(const TCHAR* fieldName, CL_NS(util)::Reader* reader);
      ~NGramAnalyzer();
      NGramAnalyzer(int32_t min_gram_size_, int32_t max_gram_size_);
  };
}


#endif