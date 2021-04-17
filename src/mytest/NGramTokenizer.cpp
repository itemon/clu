// #include "CLucene/_ApiHeader.h"
#include "NGramTokenizer.h"
#include "CLucene/util/CLStreams.h"
#include <iostream>

namespace mytest {
  NGramTokenizer::~NGramTokenizer() {
    wcout << "NGramTokenizer destroied" << endl;
  }

  NGramTokenizer::NGramTokenizer(int32_t min_gram_size_, int32_t max_gram_size_, lucene::util::Reader* _input) : 
    min_gram_size(min_gram_size_), 
    max_gram_size(max_gram_size_), 
    dataLen(0), 
    bufferIndex(0),
    ioBuffer(NULL),
    offset(0),

    ngram_sess_begin(0),
    ngram_sess_len(0),
    ngram_cur(0),
    ngram_cur_len(0),

    Tokenizer(_input) {
    // customizing costruction
    // offset = 3;
  }

  void NGramTokenizer::close() {
  }

  void NGramTokenizer::reset(CL_NS(util)::Reader* _input) {
  }   

  Token* NGramTokenizer::next(Token* token) {

    if (ngram_sess_begin >= 0 && ngram_sess_len > 0) {
      // ngram_buffer[0] = buffer[ngram_cur];
      // ngram_buffer[1] = '\0';
      // token->set(ngram_buffer, ngram_cur, ngram_cur + 1);

      if (ngram_cur_len <= ngram_sess_len) {
        // token->set(buffer, ngram_sess_begin + ngram_cur, ngram_sess_begin + ngram_cur + ngram_cur_len);
        
        int32_t copy_len;
        for (int32_t from = ngram_cur, to = ngram_cur + ngram_cur_len; from < to; ++from) {
          ngram_buffer[copy_len++] = buffer[from];
        }
        ngram_buffer[copy_len] = '\0';
        // token->set(ngram_buffer, 0, copy_len);
        token->set(ngram_buffer, ngram_cur + ngram_sess_begin, ngram_sess_begin + ngram_cur + ngram_cur_len);

        ++ngram_cur;
        // 0 1 2 3 4 5 6 7 8
        //     2 3 4

        if (ngram_sess_begin + ngram_cur + ngram_cur_len > ngram_sess_begin + ngram_sess_len) {
          ngram_cur = 0;
          ++ngram_cur_len;
        }

        return token;
      } else {
        ngram_sess_begin = 0;
        ngram_sess_len = 0;
      }
    }

    int32_t length = 0;
    int32_t start = offset;

    TCHAR c;

    while (true) {
      if (bufferIndex > dataLen - 1) {
        dataLen = input->read(ioBuffer, 1, LUCENE_IO_BUFFER_SIZE);
        if (dataLen == -1) {
          dataLen = 0;
        }
        bufferIndex = 0;
      }

      if (dataLen <= 0) {
        if (length > 0)
          break;
        else
          return NULL;
      }

      ++offset;
      c = ioBuffer[bufferIndex++];
      if (_istspace(c)) {
        if (length == 0) {
          start = offset - 1;
        }
        break;
      } else {
        buffer[length++] = c;
        if (length >= LUCENE_MAX_WORD_LEN) {
          break;
        }
      }
      // _tprintf(_T(">>>%s\n"), &c);
    }

    buffer[length] = '\0';
    token->set(buffer, start, start + length);

    // start using ngram
    if (length > 0 && length >= min_gram_size) {
      ngram_sess_begin = start;
      ngram_sess_len = length;
      ngram_cur = 0;
      ngram_cur_len = min_gram_size;
    }
    
    // if (offset > 0) {
    //   offset--;
    //   buffer[0] = (TCHAR)65;
    //   buffer[1] = (TCHAR)66;
    //   buffer[2] = 0;
    //   token->set(buffer, 0, 2);
    //   return token;
    // }
    return token;
  }
}