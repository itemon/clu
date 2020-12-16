#include "tool.h"

size_t convert_multi_byte_to_wchar(const char* src, TCHAR* dest) {
    size_t look_len = strlen(src);
    if (look_len == 0) {
        return 0;
    }

    if (dest == nullptr) {
        return 0;
    }

    const char* src_end = src + look_len;
    TCHAR chr;
    int chr_cnt;
    size_t c = 0;
    while (true) {
        chr_cnt = mbtowc(&chr, src, src_end - src);
        if (chr_cnt == -1) {
            break;
        }
        dest[c++] = chr;
        src += chr_cnt;
    }
    return c;
}

char* convert_wchar_to_mb(TCHAR* wchr) {
  std::string dat;
  int ret;
  char cur[3];

  wctomb(nullptr, 0);
  while (*wchr != '\0') {
    ret = wctomb(cur, *wchr);
    if (ret <= 0)
      break;
    dat.insert(dat.size(), cur, ret);
    wchr++;
  }

  char* result = (char*)malloc(dat.size() + 1);
  strncpy(result, dat.c_str(), dat.size());
  result[dat.size()] = '\0';

  return result;
}