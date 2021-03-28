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
    offset = 3;
  }

  void NGramTokenizer::close() {

  }

  void NGramTokenizer::reset(CL_NS(util)::Reader* _input) {

  }

  Token* NGramTokenizer::next(Token* token) {
    if (offset > 0) {
      offset--;
      buffer[0] = (TCHAR)65;
      buffer[1] = (TCHAR)66;
      buffer[2] = 0;
      token->set(buffer, 0, 2);
      return token;
    }
    return nullptr;
  }
}