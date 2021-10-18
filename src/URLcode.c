/* 本文件代码来自于
 * https://blog.csdn.net/langeldep/article/details/6264058
 * https://blog.csdn.net/bladeandmaster88/article/details/54800287
 * 并稍加修改，并非本作者编写，仅仅是为了初学者能更快地进行机器人的编写，
 * 并非抄袭之意，如有侵权请联系我
 */
#include "URLcode.h"
#include <malloc.h>
#include <stdio.h>
#include <string.h>
#include <wchar.h>
#include <windows.h>

static int hex2dec(char c);
static char dec2hex(short c);

int hex2dec(char c) {
  if ('0' <= c && c <= '9')
    return c - '0';
  else if ('a' <= c && c <= 'f')
    return c - 'a' + 10;
  else if ('A' <= c && c <= 'F')
    return c - 'A' + 10;
  else
    return -1;
}

char dec2hex(short c) {
  if (0 <= c && c <= 9)
    return c + '0';
  else if (10 <= c && c <= 15)
    return c + 'A' - 10;
  else
    return -1;
}

/*URL编码 */
char *urlencode(char url[]) {
  int i;
  int len = strlen(url);
  int res_len = 0;
  char *res = (char *)malloc(CODESIZE_MAX + 1);
  if (!res)
    return NULL;
  memset(res, 0, 1025);
  for (i = 0; i < len; ++i) {
    char c = url[i];
    if (('0' <= c && c <= '9') || ('a' <= c && c <= 'z') ||
        ('A' <= c && c <= 'Z') || c == '/' || c == '.')
      res[res_len++] = c;
    else {
      int j = (short)c;
      if (j < 0)
        j += 256;
      int i1, i0;
      i1 = j / 16;
      i0 = j - i1 * 16;
      res[res_len++] = '%';
      res[res_len++] = dec2hex(i1);
      res[res_len++] = dec2hex(i0);
    }
  }
  res[res_len] = '\0';
  return res;
}

/*GBK转UTF-8*/
char *GBKtoUTF8(char *strGbk) {
  // gbk转unicode
  int len = MultiByteToWideChar(CP_ACP, 0, strGbk, -1, NULL, 0);
  wchar_t *strUnicode = (wchar_t *)malloc(sizeof(wchar_t) * len + 2);
  if (!strUnicode)
    return NULL;
  wmemset(strUnicode, 0, len);
  MultiByteToWideChar(CP_ACP, 0, strGbk, -1, strUnicode, len);

  // unicode转UTF-8
  len = WideCharToMultiByte(CP_UTF8, 0, strUnicode, -1, NULL, 0, NULL, NULL);
  char *strUtf8 = (char *)malloc(sizeof(char) * len + 1);
  if (!strUtf8)
    return NULL;
  memset(strUtf8, 0, len + 1);
  WideCharToMultiByte(CP_UTF8, 0, strUnicode, -1, strUtf8, len, NULL, NULL);

  char *strTemp = (char *)malloc(sizeof(char) * strlen(strUtf8) + 1);
  if (!strTemp)
    return NULL;
  strcpy(strTemp, strUtf8); //此时的strTemp是UTF-8编码
  free(strUnicode);
  free(strUtf8);
  return strTemp;
}

/*UTF-8转GBK*/
char *UTF8toGBK(char *strUtf8) {
  // UTF-8转unicode
  int len = MultiByteToWideChar(CP_UTF8, 0, strUtf8, -1, NULL, 0);
  wchar_t *strUnicode = (wchar_t *)malloc(sizeof(wchar_t) * len + 2); // len = 2
  if (!strUnicode)
    return NULL;
  wmemset(strUnicode, 0, len);
  MultiByteToWideChar(CP_UTF8, 0, strUtf8, -1, strUnicode, len);

  // unicode转gbk
  len = WideCharToMultiByte(CP_ACP, 0, strUnicode, -1, NULL, 0, NULL, NULL);
  char *strGbk =
      (char *)malloc(len + 1); // len=3 本来为2，但是char*后面自动加上了\0
  if (!strGbk)
    return NULL;
  memset(strGbk, 0, len + 1);
  WideCharToMultiByte(CP_ACP, 0, strUnicode, -1, strGbk, len, NULL, NULL);

  char *strTemp = (char *)malloc(sizeof(char) * strlen(strGbk) +
                                 1); //此时的strTemp是GBK编码
  if (!strTemp)
    return NULL;
  strcpy(strTemp, strGbk);
  free(strUnicode);
  free(strGbk);
  return strTemp;
}

/*快捷编码*/
char *encode(char *str) {
  char *t1, t2;
  t1 = GBKtoUTF8(str);
  t2 = urlencode(t1);
  free(t1);
  return t2;
}
