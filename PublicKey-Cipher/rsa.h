#ifndef _RSA_H_
#define _RSA_H_

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <limits.h>

#define ACCURACY 20 // 素数测试的准确度，使用Solovay-Strassen算法，通常20次测试对于大多数大素数已足够

#define FACTOR_DIGITS 100     // 生成质因数时使用的数字位数
#define EXPONENT_MAX RAND_MAX // 最大指数，使用RAND_MAX作为最大值
#define BUF_SIZE 1024         // 缓冲区大小

#define BIGNUM_CAPACITY 20 // 大数的最大容量

#define RADIX 4294967296UL     // 基数，2^32
#define HALFRADIX 2147483648UL // 基数的一半，2^31

#define MAX(a, b) ((a) > (b) ? (a) : (b)) // 宏定义，返回两个数中的最大值

typedef unsigned int word; // 定义一个word类型，使用无符号整数

// 定义大数结构体，包含长度、容量和数据
typedef struct _bignum
{
    int length;   // 大数的长度
    int capacity; // 大数的容量
    word *data;   // 大数的数据
} bignum;

// 大数的加法操作
void bignum_iadd(bignum *source, bignum *add);           // source += add
void bignum_add(bignum *result, bignum *b1, bignum *b2); // result = b1 + b2

// 大数的减法操作
void bignum_isubtract(bignum *source, bignum *add);           // source -= add
void bignum_subtract(bignum *result, bignum *b1, bignum *b2); // result = b1 - b2

// 大数的乘法操作
void bignum_imultiply(bignum *source, bignum *add);           // source *= add
void bignum_multiply(bignum *result, bignum *b1, bignum *b2); // result = b1 * b2

// 大数的除法操作
void bignum_idivide(bignum *source, bignum *div);                      // source /= div
void bignum_idivider(bignum *source, bignum *div, bignum *remainder);  // source /= div，并将余数存入remainder
void bignum_remainder(bignum *source, bignum *div, bignum *remainder); // 计算余数，source % div
void bignum_imodulate(bignum *source, bignum *modulus);                // source %= modulus

// 大数的除法，返回商和余数
void bignum_divide(bignum *quotient, bignum *remainder, bignum *b1, bignum *b2); // b1 / b2，将商和余数分别存入quotient和remainder

// 素数检测相关函数
int solovayPrime(int a, bignum *n);                    // 使用Solovay-Strassen算法检测n是否为素数
int probablePrime(bignum *n, int k);                   // 使用k次测试判断n是否为素数
void randPrime(int numDigits, bignum *result);         // 生成一个numDigits位的随机质数
void randExponent(bignum *phi, int n, bignum *result); // 生成一个随机指数
int readFile(FILE *fd, char **buffer, int bytes);      // 读取文件内容到buffer中，返回读取的字节数

// 单个数的加密与解密
void encode(bignum *m, bignum *e, bignum *n, bignum *result); // 对消息m进行加密，使用公钥(e, n)，结果存入result
void decode(bignum *c, bignum *d, bignum *n, bignum *result); // 对密文c进行解密，使用私钥(d, n)，结果存入result

// 消息的加密与解密（按块处理）
bignum *encodeMessage(int len, int bytes, char *message, bignum *exponent, bignum *modulus);   // 将消息分块进行加密
int *decodeMessage(int len, int bytes, bignum *cryptogram, bignum *exponent, bignum *modulus); // 解密加密的消息

/**
 * 保存一些常用的大整数（0 - 10），避免重复创建。
 * 用法示例：NUMS[5] = bignum("5")，即NUMS数组中存储了0到10的数字。
 */
word DATA0[1] = {0};   // 定义大整数0
word DATA1[1] = {1};   // 定义大整数1
word DATA2[1] = {2};   // 定义大整数2
word DATA3[1] = {3};   // 定义大整数3
word DATA4[1] = {4};   // 定义大整数4
word DATA5[1] = {5};   // 定义大整数5
word DATA6[1] = {6};   // 定义大整数6
word DATA7[1] = {7};   // 定义大整数7
word DATA8[1] = {8};   // 定义大整数8
word DATA9[1] = {9};   // 定义大整数9
word DATA10[1] = {10}; // 定义大整数10

// 将数字0到10存储为bignum类型的常量数组
bignum NUMS[11] = {{1, 1, DATA0}, {1, 1, DATA1}, {1, 1, DATA2}, {1, 1, DATA3}, {1, 1, DATA4}, {1, 1, DATA5}, {1, 1, DATA6}, {1, 1, DATA7}, {1, 1, DATA8}, {1, 1, DATA9}, {1, 1, DATA10}};

#endif