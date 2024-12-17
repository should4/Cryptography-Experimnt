#include "BiPoly.h"
#include "Utils.h"
#include <iostream>

unsigned int BiPoly::exgcd(unsigned int a, unsigned int b, unsigned int &x, unsigned &y)
{
    // std::cout << "a: " << a << " b: " << b << std::endl;
    // std::cout << "x: " << x << " y: " << y << std::endl;

    if (b == 0)
    {
        x = 1;
        y = 0;
        return a;
    }
    unsigned int d = exgcd(b, Mod(a, b), x, y);
    unsigned int temp = y;
    y = x ^ (Mul(Div(a, b), y));
    x = temp;

    // std::cout << "x: " << Mod(x, 0x11B) << " y: " << Mod(y, 0x11B) << std::endl;
    return d;
}

void BiPoly::Div(unsigned int a, unsigned int b, unsigned int &d, unsigned int &r)
{
    d = 0;
    r = a;

    while (Utils::TopDigit(r) >= Utils::TopDigit(b))
    {
        int diff = Utils::TopDigit(r) - Utils::TopDigit(b);
        d ^= (1 << diff);
        r ^= (b << diff);
        // std::cout << "d: " << d << " r: " << r << std::endl;
    }
}

unsigned int BiPoly::Mod(unsigned int a, unsigned int b)
{
    unsigned int d, r;
    Div(a, b, d, r);
    return r;
}

unsigned int BiPoly::Div(unsigned int a, unsigned int b)
{
    unsigned int d, r;
    Div(a, b, d, r);
    return d;
}

unsigned int BiPoly::Mul(unsigned int a, unsigned int b)
{
    unsigned int sum = 0;
    int digit = 0;
    unsigned int temp = b;
    while (temp != 0)
    {
        if ((temp & 1) != 0)
        {
            sum ^= (a << digit);
        }
        temp = temp >> 1;
        digit++;
    }
    // std::cout << "a: " << a << " b: " << b << " sum:" << sum << std::endl;
    return sum;
}
