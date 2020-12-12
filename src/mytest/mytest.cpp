
#include <iostream>
#include <cstdlib>
#include <cstring>
#include "CLucene.h"
#include "mytest.h"

#define EXPORT __attribute__((visibility("default")))

using namespace std;
using namespace lucene::document;

EXPORT void my_test_whisper() {
  int64_t d = DateTools::stringToTime(_T("2012"));
  TCHAR* tmp = DateTools::getISOFormat(d);
  /*wcout << tmp << endl;

  char r[1024];
  int ret = wctomb(r, *tmp);
  cout << "op chars " << ret << endl;

  puts(r);

  TCHAR myname[3] = {
        0x9ec4,
        0x4f1f,
        0x0,
    };

  tmp = myname;*/

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

  cout << "testing it right now in whispter" << endl;
}

extern "C" {
  EXPORT const char* clu_str_num() {
    return "not implemented yet";
  }
}