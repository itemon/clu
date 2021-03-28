#include "CLucene/_ApiHeader.h"
#include "NGramTokenizer.h"
#include <iostream>

namespace mytest {
  NGramTokenizer::~NGramTokenizer() {
    wcout << "NGramTokenizer destroied" << endl;
  }

  NGramTokenizer::NGramTokenizer(int32_t min_gram_size_, int32_t max_gram_size_, lucene::util::Reader* input) : 
    min_gram_size(min_gram_size_), max_gram_size(max_gram_size_), Tokenizer(input) {
    // customizing costruction
  }

  void NGramTokenizer::close() {

  }

  void NGramTokenizer::reset(CL_NS(util)::Reader* _input) {

  }

  Token* next(Token* token) {
    return nullptr;
  }
}