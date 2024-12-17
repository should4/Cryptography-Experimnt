#include "Utils.h"

int Utils::exgcd(int a, int b, int &x, int &y)
{
    if (b == 0)
    {
        x = 1;
        y = 0;
        return a; // 到达递归边界开始向上一层返回
    }
    int d = exgcd(b, a % b, x, y);
    int temp = y;
    y = x - (a / b) * y;
    x = temp;
    return d;
}

unsigned int Utils::TopDigit(unsigned int x)
{
    unsigned int top = 0;
    while (x != 0)
    {
        x = (x >> 1);
        top++;
    }
    return top;
}

unsigned char *Utils::Hexstr2CharStr(char *hex_str, int len)
{
    if (len % 2 != 0)
    {
        printf("Hex Transform Error: len error !\n");
        return nullptr;
    }
    unsigned char *res = (unsigned char *)malloc(len / 2 * sizeof(unsigned char));
    for (int i = 0, j = 0; i < len;)
    {
        unsigned char c = (HexCToChar(hex_str[i]) << 4) + HexCToChar(hex_str[i + 1]);
        res[j] = c;
        i = i + 2;
        j++;
    }
    return res;
}



char Utils::HexCToChar(char c)
{
    if (c >= '0' && c <= '9')
    {
        c = c - '0';
    }
    else if (c >= 'a' && c <= 'f')
    {
        c = 10 + c - 'a';
    }
    else if (c >= 'A' && c <= 'F')
    {
        c = 10 + c - 'A';
    }

    return c;
}