#include <iostream>
#include <stdio.h>
#include "CLucene.h"

size_t convert_multi_byte_to_wchar(const char* src, TCHAR* dest);
char* convert_wchar_to_mb(TCHAR* wchr, char* result = nullptr);
size_t count_mb_of_wchr(TCHAR* wchr);