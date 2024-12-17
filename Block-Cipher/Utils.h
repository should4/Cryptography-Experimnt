#ifndef _UTILS_H_
#define _UTILS_H_

#include <iostream>

class Utils
{
public:
    // 扩展欧几里得算法
    static int exgcd(int a, int b, int &x, int &y);

    // 求一个二进制数的最高位
    static unsigned int TopDigit(unsigned int x);

    // 每两位 HEX 字符串由一位 unsigned char 表示
    static unsigned char *Hexstr2CharStr(char *hex_str, int len);
    // static unsigned char *CharStr2Hexstr(char *hex_str, int len);
    static char HexCToChar(char c);
};

#endif