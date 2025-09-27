#include <ctype.h>
#include <string.h>

char *
trim_string (char *str)
{
  if (!str || !*str)
    return str;			// 处理空指针或空字符串

  // 跳过前导空白字符
  char *start = str;
  while (isspace ((unsigned char) *start))
    start++;

  // 处理全空白字符串的情况
  if (!*start)
    {
      *str = '\0';
      return str;
    }

  // 找到字符串末尾并跳过尾随空白字符
  char *end = str + strlen (str) - 1;
  while (end > start && isspace ((unsigned char) *end))
    end--;

  // 计算修剪后的长度并移动内容
  size_t len = end - start + 1;
  if (start != str)
    {
      memmove (str, start, len);
    }

  // 添加字符串终止符
  str[len] = '\0';

  return str;
}
