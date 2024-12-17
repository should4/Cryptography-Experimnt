#ifndef _BIPOLY_H_
#define _BIPOLY_H_

#include <iostream>
#include <vector>

class BiPoly
{
public:
    // 模二多项式使用扩展欧几里得算法
    static unsigned int exgcd(unsigned int a, unsigned int b, unsigned int &x, unsigned int &y);

    // 模二多项式 运算函数
    // 除法运算
    static void Div(unsigned int a, unsigned int b, unsigned int &d, unsigned int &r);
    static unsigned int Mod(unsigned int a, unsigned int b);
    static unsigned int Div(unsigned int a, unsigned int b);
    // 乘法运算
    static unsigned int Mul(unsigned int a, unsigned int b);
};

#endif