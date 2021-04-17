#include <iostream>

#include "CLucene/_ApiHeader.h"
#include "NGramAnalyzer.h"
#include "NGramTokenizer.h"
#include "CLucene/analysis/Analyzers.h"

using lucene::analysis::TokenStream;

namespace mytest {
  TokenStream* NGramAnalyzer::tokenStream(const TCHAR* fieldName, CL_NS(util)::Reader* reader) {
    return _CLNEW mytest::NGramTokenizer(min_gram_size, max_gram_size, reader->__asBufferedReader());
  }

  NGramAnalyzer::NGramAnalyzer(int32_t min_gram_size_, int32_t max_gram_size_) : 
    min_gram_size(min_gram_size_), max_gram_size(max_gram_size_), Analyzer() {
    // customer construction
  }

  NGramAnalyzer::~NGramAnalyzer() {
    std::wcout << "ngram destroied" << std::endl;
  }
}