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

size_t count_mb_of_wchr(TCHAR* wchr) {
  if (!wchr)
    return 0;

  int ret;
  size_t c = 0;
  char cur[3];

  wctomb(nullptr, 0);
  while (*wchr != '\0') {
    ret = wctomb(cur, *wchr);
    if (ret <= 0)
      break;
    c += ret;
    wchr++;
  }

  return c;
}

char* convert_wchar_to_mb(TCHAR* wchr, char* result) {
  std::string dat;
  int ret;
  char cur[3];

  if (wchr == NULL) {
    return NULL;
  }

  wctomb(nullptr, 0);
  while (*wchr != '\0') {
    ret = wctomb(cur, *wchr);
    if (ret <= 0)
      break;
    dat.insert(dat.size(), cur, ret);
    wchr++;
  }

  size_t copy_len;
  if (!result) {
    copy_len = dat.size();
    result = (char*)malloc(copy_len + 1);
  } else {
    copy_len = strlen(result);
  }
  strncpy(result, dat.c_str(), copy_len);
  result[copy_len] = '\0';

  return result;
}

bool str_end_with(const char* str, const char* sub) {
  if (!str || !sub) {
    return false;
  }

  size_t big_len = strlen(str), small_len = strlen(sub);
  if (small_len > big_len) {
    return false;
  }

  size_t c = small_len - 1;
  --big_len;
  while (c > 0) {
    if (sub[c] != str[big_len]) {
      return false;
    }
    --c;
    --big_len;
  }

  return true;
}