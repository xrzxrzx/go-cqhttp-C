/* 本文件代码来自于
 * https://blog.csdn.net/langeldep/article/details/6264058
 * https://blog.csdn.net/bladeandmaster88/article/details/54800287
 * 并稍加修改，并非本作者编写，仅仅是为了初学者能更快地进行机器人的编写，
 * 并非抄袭之意，如有侵权请联系我
 */
#pragma once

#define CODESIZE_MAX 1024

/*URL编码*/
char* urlencode(char url[]);

/*GBK转UTF-8*/
char* GBKtoUTF8(char* strGbk);

/*UTF-8转GBK*/
char* UTF8toGBK(char* strUtf8);
