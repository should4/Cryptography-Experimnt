#include "../AES.h"
#include "../Utils.h"
#include "../BiPoly.h"
#include <iostream>
#include <string.h>

using namespace std;

void Test1()
{
    // 0x000102030405060708090a0b0c0d0e0f
    unsigned char testKey[] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                               0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f};

    // 0x00112233445566778899aabbccddeeff
    unsigned char plain[] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
                             0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff};

    unsigned char right[] = {0x69, 0xc4, 0xe0, 0xd8, 0x6a, 0x7b, 0x04, 0x30,
                             0xd8, 0xcd, 0xb7, 0x80, 0x70, 0xb4, 0xc5, 0x5a};

    // 测试 S盒字代替过程
    /*while (true)
    {
        int a;
        cin >> a;

        uint8_t b = AES::ByteTransform(a);

        cout << "a = " << int(a) << " b: " << int(b) << "inv(b): " << int(AES::InvByteTransform(b)) << endl;
    }*/

    // 字体换 -> 行移位 -> 列混合 测试
    /*uint8_t state[4][4];
    uint8_t out[16] = {};

    uint8_t i, j, round;

    for (i = 0; i < 4; i++)
    {
        for (j = 0; j < 4; j++)
        {
            state[i][j] = plain[i + 4 * j];
        }
    }

    AES::SubBytes(state);
    AES::ShiftRows(state);
    AES::MixColumns(state);
    AES::InvMixColumns(state);
    AES::InvShiftRows(state);
    AES::InvSubBytes(state);

    for (i = 0; i < 4; i++)
    {
        for (j = 0; j < 4; j++)
        {
            out[i + 4 * j] = state[i][j];
        }
    }
    for (int i = 0; i < 16; ++i)
    {
        printf("%x ", out[i]);
    }*/

    // 加密测试
    /*unsigned char w[16 * 11] = {};
    unsigned char out[16] = {};
    AES::KeyExpand(testKey, w);
    for (int i = 0; i < 44 * 11; ++i)
    {
        printf("%x ", w[i]);
        if (i % 16 == 0)
        {
            printf("\n");
        }
    }
    AES::DecriptBlock(right, out, w);
    for (int i = 0; i < 16; ++i)
    {
        printf("%x ", out[i]);
    }*/
}

void Test2()
{
    // 扩展欧几里得算法测试
    while (true)
    {
        int a, b;
        cout << "input a,b: ";
        cin >> a >> b;
        int x, y;
        int d = Utils::exgcd(a, b, x, y);

        cout << "d: " << d << " x: " << x << " y: " << y << endl;
    }
}

void Test3()
{
    // 在多项式上的扩展欧几里得算法测试
    // unsigned int a = 0x11B; // x^6+x^4+x+1
    // unsigned int b = 0x53;  // x^8+x^4+x^3+x+1

    // 多项式除法测试
    /*while (true)
    {
        unsigned int d, r;
        BiPoly::Div(a, b, d, r);
        cout << "d: " << d << " r: " << r << endl;
        cin >> a >> b;
    }*/

    // 多项式乘法测试
    /*while (true)
    {
        unsigned int a, b;
        cin >> a >> b;
        cout << "here " << a << " " << b << endl;
        unsigned int mul = BiPoly::Mul(a, b);
        cout << "mul: " << mul << " mod:" << BiPoly::Mod(mul, 0x11B) << endl;
    }*/

    // 多项式扩展欧几里得算法测试
    while (true)
    {
        unsigned int a, b;
        cin >> a >> b;
        unsigned int x, y;
        unsigned int d = BiPoly::exgcd(a, b, x, y);
        cout << "d: " << d << " x:" << BiPoly::Mod(x, 0x11B) << " y:" << BiPoly::Mod(y, 0x11B) << endl;
    }
}

/*void Test4()
{
    // 测试使用 AES 对英文字符串加密
    char *plain_text = "6bc1bee22e409f96e93d7e117393172a";
    char *key_r = "2b7e151628aed2a6abf7158809cf4f3c";
    unsigned int inlen = strlen(plain_text);
    unsigned int keylen = strlen(key_r);
    unsigned char *out = (unsigned char *)malloc(inlen * sizeof(unsigned char));
    unsigned char *in = Utils::Hexstr2CharStr(plain_text, inlen);
    unsigned char *key = Utils::Hexstr2CharStr(key_r, keylen);
    AES::EncryptECB(in, inlen, out, key, keylen);

    for (int i = 0; i < 16; ++i)
    {
        printf("%x ", out[i]);
    }
}*/

void encript_str(string plain_text, string key)
{
    if ((plain_text.size() % 16) != 0)
    {
        plain_text.append(16 - (plain_text.size() % 16), '!');
    }

    unsigned char *out = (unsigned char *)malloc(plain_text.size());

    AES::EncryptECB((unsigned char *)plain_text.c_str(), plain_text.size(), out, (unsigned char *)key.c_str(), key.size());
    cout << "plain_text: " << plain_text.c_str() << endl;
    printf("Cipher Text: 0x");
    for (int i = 0; i < plain_text.size(); ++i)
    {
        printf("%02X", out[i]);
    }
    printf("\n");
}

void decript_str(string cipher_text, string key)
{
    unsigned char *out = (unsigned char *)malloc(cipher_text.size() / 2);
    unsigned char *in = Utils::Hexstr2CharStr((char *)cipher_text.c_str(), cipher_text.size());

    int inlen = cipher_text.size() / 2;
    AES::DecryptECB(in, inlen, out, (unsigned char *)key.c_str(), key.size());

    cout << "Cipher Text: " << cipher_text << endl;
    printf("plain Text: ");
    for (int i = 0; i < inlen; ++i)
    {
        printf("%c", out[i]);
    }
    printf("\n");
}

int main()
{
    cout << "-------------------------AES 加密----------------------" << endl;
    // 密钥
    string key = "abcdefghijklmnop";
    // 加密
    string plain_text1 = "HelloWorldFriend";
    string plain_text2 = "Hello my friend,this is my cryptography experiment,I will show You!"; // 64位                                                       // 16 位密钥

    cout << "key: " << key << endl;

    encript_str(plain_text1, key);

    encript_str(plain_text2, key);

    cout << endl
         << "-------------------------AES 解密----------------------" << endl;
    // 解密
    string cipher_text2 = "6C343B85AD6C92ABDA711CFE5816AC3D";
    string cipher_text1 = "B07D8B859B815983FFDB52A8F0FE613B2F20BA3EF210524362264A6A4A2B9D7F9CE41A3B3C0057A3A995EAC8498CE9462D385CC741AE18A2BF87AB54A05CA7DEC5DDB6B60F43B565428A8650BD5359F9";

    cout << "key: " << key << endl;

    decript_str(cipher_text1, key);

    decript_str(cipher_text2, key);

    return 0;
}
// B07D8B859B81 5983FFDB52A8F0FE 613B2F20BA3EF210 524362264A6A4A2B 9D7F9CE41A3B3C00 57A3A995EAC8498C E9462D385CC741AE 18A2BF87AB54A05C A7DEC5DDB6B60F43 B565428A8650BD5359F9
// b07d8b859b81 5983ffdb52a8f0fe 613b2f20ba3ef210 524362264a6a4a2b 9d7f9ce41a3b3c0  57a3a995eac8498c e9462d385cc741ae 18a2bf87ab54a05c a7dec5ddb6b6f43  b565428a8650bd5359f9

// b07d8b859b815983ffdb52a8f0fe613b2f20ba3ef210524362264a6a4a2b9d7f9ce41a3b3c0057a3a995eac8498ce9462d385cc741ae18a2bf87ab54a05ca7dec5ddb6b6f43b565428a8650bd5359f9
// B07D8B859B815983FFDB52A8F0FE613B2F20BA3EF210524362264A6A4A2B9D7F9CE41A3B3C0057A3A995EAC8498CE9462D385CC741AE18A2BF87AB54A05CA7DEC5DDB6B60F43B565428A8650BD5359F9