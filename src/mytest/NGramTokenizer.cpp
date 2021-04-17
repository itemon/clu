// #include "CLucene/_ApiHeader.h"
#include "NGramTokenizer.h"
#include <iostream>

namespace mytest {
  NGramTokenizer::~NGramTokenizer() {
    wcout << "NGramTokenizer destroied" << endl;
  }

  NGramTokenizer::NGramTokenizer(int32_t min_gram_size_, int32_t max_gram_size_, lucene::util::BufferedReader* _input) : 
    min_gram_size(min_gram_size_), 
    max_gram_size(max_gram_size_), 

    // dataLen(0), 
    // bufferIndex(0),
    // ioBuffer(NULL),
    // offset(0),

    // ngram_sess_begin(0),
    // ngram_sess_len(0),
    // ngram_cur(0),
    // ngram_cur_len(0),

    rd(_CLNEW FastCharStream(_input)),
    rdPos(0),

    Tokenizer(_input) {
    // customizing costruction
    // offset = 3;
  }

  void NGramTokenizer::close() {
  }

  void NGramTokenizer::reset(CL_NS(util)::Reader* _input) {
  }   

  Token* NGramTokenizer::next(Token* token) {
    return token;
  }

  inline int NGramTokenizer::readChar() {
    return 0;
  }

}