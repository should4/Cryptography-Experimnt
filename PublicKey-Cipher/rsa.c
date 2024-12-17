#include "rsa.h"

bignum *bignum_init()
{
    bignum *b = malloc(sizeof(bignum));
    b->length = 0;
    b->capacity = BIGNUM_CAPACITY;
    b->data = calloc(BIGNUM_CAPACITY, sizeof(word));
    return b;
}

void bignum_deinit(bignum *b)
{
    free(b->data);
    free(b);
}

int bignum_iszero(bignum *b)
{
    return b->length == 0 || (b->length == 1 && b->data[0] == 0);
}

int bignum_isnonzero(bignum *b)
{
    return !bignum_iszero(b);
}

void bignum_copy(bignum *source, bignum *dest)
{
    dest->length = source->length;
    if (source->capacity > dest->capacity)
    {
        dest->capacity = source->capacity;
        dest->data = realloc(dest->data, dest->capacity * sizeof(word));
    }
    memcpy(dest->data, source->data, dest->length * sizeof(word));
}

void bignum_fromstring(bignum *b, char *string)
{
    int i, len = 0;
    // 计算字符串长度
    while (string[len] != '\0')
        len++;
    for (i = 0; i < len; i++)
    {
        if (i != 0)
            bignum_imultiply(b, &NUMS[10]); // 基于十进制的乘法
        bignum_iadd(b, &NUMS[string[i] - '0']);
    }
}

void bignum_fromint(bignum *b, unsigned int num)
{
    b->length = 1;
    if (b->capacity < b->length)
    {
        b->capacity = b->length;
        b->data = realloc(b->data, b->capacity * sizeof(word));
    }
    b->data[0] = num;
}

void bignum_print(bignum *b)
{
    int cap = 100, len = 0, i;
    char *buffer = malloc(cap * sizeof(char));
    bignum *copy = bignum_init(), *remainder = bignum_init();
    if (b->length == 0 || bignum_iszero(b))
        printf("0");
    else
    {
        bignum_copy(b, copy);
        while (bignum_isnonzero(copy))
        {
            bignum_idivider(copy, &NUMS[10], remainder);
            buffer[len++] = remainder->data[0];
            if (len >= cap)
            {
                cap *= 2;
                buffer = realloc(buffer, cap * sizeof(char));
            }
        }
        for (i = len - 1; i >= 0; i--)
            printf("%d", buffer[i]);
    }
    bignum_deinit(copy);
    bignum_deinit(remainder);
    free(buffer);
}

int bignum_equal(bignum *b1, bignum *b2)
{
    int i;
    if (bignum_iszero(b1) && bignum_iszero(b2))
        return 1;
    else if (bignum_iszero(b1))
        return 0;
    else if (bignum_iszero(b2))
        return 0;
    else if (b1->length != b2->length)
        return 0;
    for (i = b1->length - 1; i >= 0; i--)
    {
        if (b1->data[i] != b2->data[i])
            return 0;
    }
    return 1;
}

int bignum_greater(bignum *b1, bignum *b2)
{
    int i;
    if (bignum_iszero(b1) && bignum_iszero(b2))
        return 0;
    else if (bignum_iszero(b1))
        return 0;
    else if (bignum_iszero(b2))
        return 1;
    else if (b1->length != b2->length)
        return b1->length > b2->length;
    for (i = b1->length - 1; i >= 0; i--)
    {
        if (b1->data[i] != b2->data[i])
            return b1->data[i] > b2->data[i];
    }
    return 0;
}

int bignum_less(bignum *b1, bignum *b2)
{
    int i;
    if (bignum_iszero(b1) && bignum_iszero(b2))
        return 0;
    else if (bignum_iszero(b1))
        return 1;
    else if (bignum_iszero(b2))
        return 0;
    else if (b1->length != b2->length)
        return b1->length < b2->length;
    for (i = b1->length - 1; i >= 0; i--)
    {
        if (b1->data[i] != b2->data[i])
            return b1->data[i] < b2->data[i];
    }
    return 0;
}

int bignum_geq(bignum *b1, bignum *b2)
{
    return !bignum_less(b1, b2);
}

int bignum_leq(bignum *b1, bignum *b2)
{
    return !bignum_greater(b1, b2);
}

void bignum_iadd(bignum *source, bignum *add)
{
    bignum *temp = bignum_init();
    bignum_add(temp, source, add);
    bignum_copy(temp, source);
    bignum_deinit(temp);
}

void bignum_add(bignum *result, bignum *b1, bignum *b2)
{
    word sum, carry = 0;
    int i, n = MAX(b1->length, b2->length);
    if (n + 1 > result->capacity)
    {
        result->capacity = n + 1;
        result->data = realloc(result->data, result->capacity * sizeof(word));
    }
    for (i = 0; i < n; i++)
    {
        sum = carry;
        if (i < b1->length)
            sum += b1->data[i];
        if (i < b2->length)
            sum += b2->data[i];
        result->data[i] = sum;

        if (i < b1->length)
        {
            if (sum < b1->data[i])
                carry = 1;
            else
                carry = 0;
        }
        else
        {
            if (sum < b2->data[i])
                carry = 1; /* Result must have wrapped 2^32 so carry bit is 1 */
            else
                carry = 0;
        }
    }
    if (carry == 1)
    {
        result->length = n + 1;
        result->data[n] = 1;
    }
    else
    {
        result->length = n;
    }
}

void bignum_isubtract(bignum *source, bignum *sub)
{
    bignum *temp = bignum_init();
    bignum_subtract(temp, source, sub);
    bignum_copy(temp, source);
    bignum_deinit(temp);
}

void bignum_subtract(bignum *result, bignum *b1, bignum *b2)
{
    int length = 0, i;
    word carry = 0, diff, temp;
    if (b1->length > result->capacity)
    {
        result->capacity = b1->length;
        result->data = realloc(result->data, result->capacity * sizeof(word));
    }
    for (i = 0; i < b1->length; i++)
    {
        temp = carry;
        if (i < b2->length)
            temp = temp + b2->data[i]; // 基于 RADIX 的模运算
        diff = b1->data[i] - temp;
        if (temp > b1->data[i])
            carry = 1;
        else
        {
            if (temp == 0 && b2->data[i] == 0xffffffff)
            {
                carry = 1;
            }
            else
            {
                carry = 0;
            }
        }
        result->data[i] = diff;
        if (result->data[i] != 0)
            length = i + 1;
    }
    result->length = length;
}

void bignum_imultiply(bignum *source, bignum *mult)
{
    bignum *temp = bignum_init();
    bignum_multiply(temp, source, mult);
    bignum_copy(temp, source);
    bignum_deinit(temp);
}

void bignum_multiply(bignum *result, bignum *b1, bignum *b2)
{
    int i, j, k;
    word carry, temp;
    unsigned long long int prod;

    // 如果结果的容量不足以存储乘积的大小，则重新分配内存
    if (b1->length + b2->length > result->capacity)
    {
        result->capacity = b1->length + b2->length;
        result->data = realloc(result->data, result->capacity * sizeof(word));
    }

    // 初始化结果数组，将所有数据位设为 0
    for (i = 0; i < b1->length + b2->length; i++)
        result->data[i] = 0;

    // 进行大数乘法操作
    for (i = 0; i < b1->length; i++)
    {
        for (j = 0; j < b2->length; j++)
        {
            // 计算当前位的乘积并加上进位
            prod = (b1->data[i] * (unsigned long long int)b2->data[j]) + (unsigned long long int)(result->data[i + j]);
            carry = (word)(prod / RADIX); // 计算进位部分

            // 如果有进位，则继续处理进位，直到没有进位
            k = 1;
            while (carry > 0)
            {
                temp = result->data[i + j + k] + carry; // 加上进位
                if (temp < result->data[i + j + k])     // 如果发生溢出，进位为 1
                    carry = 1;
                else
                    carry = 0;                  // 否则进位为 0
                result->data[i + j + k] = temp; // 更新结果
                k++;                            // 继续处理更高位的进位
            }

            // 将当前位的乘积加到结果的对应位置
            prod = (result->data[i + j] + b1->data[i] * (unsigned long long int)b2->data[j]) % RADIX;
            result->data[i + j] = prod; // 更新当前位
        }
    }

    // 检查结果的最高位是否为零，如果是，则需要减少结果的有效长度
    if (b1->length + b2->length > 0 && result->data[b1->length + b2->length - 1] == 0)
        result->length = b1->length + b2->length - 1; // 如果最高位为 0，则减小结果的长度
    else
        result->length = b1->length + b2->length; // 否则长度为乘法操作的最大位数
}

void bignum_idivide(bignum *source, bignum *div)
{
    bignum *q = bignum_init(), *r = bignum_init();
    bignum_divide(q, r, source, div);
    bignum_copy(q, source);
    bignum_deinit(q);
    bignum_deinit(r);
}

void bignum_idivider(bignum *source, bignum *div, bignum *remainder)
{
    bignum *q = bignum_init(), *r = bignum_init();
    bignum_divide(q, r, source, div);
    bignum_copy(q, source);
    bignum_copy(r, remainder);
    bignum_deinit(q);
    bignum_deinit(r);
}

void bignum_remainder(bignum *source, bignum *div, bignum *remainder)
{
    bignum *q = bignum_init();
    bignum_divide(q, remainder, source, div);
    bignum_deinit(q);
}

void bignum_imodulate(bignum *source, bignum *modulus)
{
    bignum *q = bignum_init(), *r = bignum_init();
    bignum_divide(q, r, source, modulus);
    bignum_copy(r, source);
    bignum_deinit(q);
    bignum_deinit(r);
}

void bignum_divide(bignum *quotient, bignum *remainder, bignum *b1, bignum *b2)
{
    bignum *b2copy = bignum_init(), *b1copy = bignum_init(); // 创建b1和b2的副本，用于运算
    bignum *temp = bignum_init(), *temp2 = bignum_init(), *temp3 = bignum_init();
    bignum *quottemp = bignum_init();
    word carry = 0;
    int n, m, i, j, length = 0;
    unsigned long long factor = 1;
    unsigned long long gquot, gtemp, grem;

    // 如果b1小于b2，则商为0，余数为b1
    if (bignum_less(b1, b2))
    {
        quotient->length = 0;
        bignum_copy(b1, remainder); // 余数为b1
    }
    // 如果b1为零，则商为0，余数为0
    else if (bignum_iszero(b1))
    {
        quotient->length = 0;
        bignum_fromint(remainder, 0); // 余数为0
    }
    // 如果b2只有一个limb（位数为1），可以进行简单的除法
    else if (b2->length == 1)
    {
        // 如果quotient容量小于b1的长度，则重新分配内存
        if (quotient->capacity < b1->length)
        {
            quotient->capacity = b1->length;
            quotient->data = realloc(quotient->data, quotient->capacity * sizeof(word));
        }

        // 从b1的最高位开始，进行除法操作
        for (i = b1->length - 1; i >= 0; i--)
        {
            gtemp = carry * RADIX + b1->data[i]; // 计算当前位和carry的乘积
            gquot = gtemp / b2->data[0];         // 当前商
            quotient->data[i] = gquot;           // 保存商的当前位
            if (quotient->data[i] != 0 && length == 0)
                length = i + 1;          // 记录商的长度
            carry = gtemp % b2->data[0]; // 计算余数
        }
        bignum_fromint(remainder, carry); // 设置余数
        quotient->length = length;        // 设置商的长度
    }
    else
    {
        // 执行长除法（除数大于一个limb）
        n = b1->length + 1; // 商的最大长度
        m = b2->length;     // 除数的长度

        // 如果quotient的容量不够，重新分配内存
        if (quotient->capacity < n - m)
        {
            quotient->capacity = n - m;
            quotient->data = realloc(quotient->data, (n - m) * sizeof(word));
        }

        // 复制b1和b2，以便在运算中不会修改原始数据
        bignum_copy(b1, b1copy);
        bignum_copy(b2, b2copy);

        // 归一化：将除数乘以2，直到最高有效位大于等于HALFRADIX
        while (b2copy->data[b2copy->length - 1] < HALFRADIX)
        {
            factor *= 2;
            bignum_imultiply(b2copy, &NUMS[2]); // 将b2copy乘以2
        }

        // 如果因子大于1，则将被除数b1copy也乘以该因子
        if (factor > 1)
        {
            bignum_fromint(temp, factor);
            bignum_imultiply(b1copy, temp);
        }

        // 如果被除数的长度小于商的长度，需要添加一个零
        if (b1copy->length != n)
        {
            b1copy->length++;
            if (b1copy->length > b1copy->capacity)
            {
                b1copy->capacity = b1copy->length;
                b1copy->data = realloc(b1copy->data, b1copy->capacity * sizeof(word));
            }
            b1copy->data[n - 1] = 0;
        }

        // 使用长除法计算商
        for (i = n - m - 1; i >= 0; i--)
        {
            gtemp = RADIX * b1copy->data[i + m] + b1copy->data[i + m - 1]; // 当前的除法部分
            gquot = gtemp / b2copy->data[m - 1];                           // 当前商
            if (gquot >= RADIX)
                gquot = UINT_MAX;               // 防止溢出
            grem = gtemp % b2copy->data[m - 1]; // 余数
            // 通过比较商和余数来调整商，避免过大
            while (grem < RADIX && gquot * b2copy->data[m - 2] > RADIX * grem + b1copy->data[i + m - 2])
            {
                gquot--;                     // 如果不符合条件，减小商
                grem += b2copy->data[m - 1]; // 增加余数
            }

            // 处理商并保存
            quottemp->data[0] = gquot % RADIX;
            quottemp->data[1] = (gquot / RADIX);
            if (quottemp->data[1] != 0)
                quottemp->length = 2;
            else
                quottemp->length = 1;

            // 计算临时乘积
            bignum_multiply(temp2, b2copy, quottemp);
            if (m + 1 > temp3->capacity)
            {
                temp3->capacity = m + 1;
                temp3->data = realloc(temp3->data, temp3->capacity * sizeof(word));
            }
            temp3->length = 0;

            // 复制当前位数据到temp3
            for (j = 0; j <= m; j++)
            {
                temp3->data[j] = b1copy->data[i + j];
                if (temp3->data[j] != 0)
                    temp3->length = j + 1;
            }

            // 如果临时结果大于当前值，需要减去除数
            if (bignum_less(temp3, temp2))
            {
                bignum_iadd(temp3, b2copy); // 调整
                gquot--;                    // 商减小
            }

            // 执行减法操作
            bignum_isubtract(temp3, temp2);
            for (j = 0; j < temp3->length; j++)
                b1copy->data[i + j] = temp3->data[j]; // 更新b1copy
            for (j = temp3->length; j <= m; j++)
                b1copy->data[i + j] = 0; // 清零多余位

            quotient->data[i] = gquot; // 设置商
            if (quotient->data[i] != 0)
                quotient->length = i; // 更新商的长度
        }

        // 最终确定商的长度
        if (quotient->data[b1->length - b2->length] == 0)
            quotient->length = b1->length - b2->length;
        else
            quotient->length = b1->length - b2->length + 1;

        // 根据因子进行最终余数的计算
        carry = 0;
        for (i = b1copy->length - 1; i >= 0; i--)
        {
            gtemp = carry * RADIX + b1copy->data[i]; // 计算余数
            b1copy->data[i] = gtemp / factor;        // 更新b1copy的值
            if (b1copy->data[i] != 0 && length == 0)
                length = i + 1;     // 记录余数的长度
            carry = gtemp % factor; // 更新carry
        }
        b1copy->length = length;
        bignum_copy(b1copy, remainder); // 设置最终余数
    }

    bignum_deinit(temp);
    bignum_deinit(temp2);
    bignum_deinit(temp3);
    bignum_deinit(b1copy);
    bignum_deinit(b2copy);
    bignum_deinit(quottemp);
}

void bignum_modpow(bignum *base, bignum *exponent, bignum *modulus, bignum *result)
{
    bignum *a = bignum_init(), *b = bignum_init(), *c = bignum_init();
    bignum *discard = bignum_init(), *remainder = bignum_init();
    bignum_copy(base, a);
    bignum_copy(exponent, b);
    bignum_copy(modulus, c);
    bignum_fromint(result, 1);
    while (bignum_greater(b, &NUMS[0]))
    {
        if (b->data[0] & 1)
        {
            bignum_imultiply(result, a);
            bignum_imodulate(result, c);
        }
        bignum_idivide(b, &NUMS[2]);
        bignum_copy(a, discard);
        bignum_imultiply(a, discard);
        bignum_imodulate(a, c);
    }
    bignum_deinit(a);
    bignum_deinit(b);
    bignum_deinit(c);
    bignum_deinit(discard);
    bignum_deinit(remainder);
}

void bignum_gcd(bignum *b1, bignum *b2, bignum *result)
{
    bignum *a = bignum_init(), *b = bignum_init(), *remainder = bignum_init();
    bignum *temp = bignum_init(), *discard = bignum_init();
    bignum_copy(b1, a);
    bignum_copy(b2, b);
    while (!bignum_equal(b, &NUMS[0]))
    {
        bignum_copy(b, temp);
        bignum_imodulate(a, b);
        bignum_copy(a, b);
        bignum_copy(temp, a);
    }
    bignum_copy(a, result);
    bignum_deinit(a);
    bignum_deinit(b);
    bignum_deinit(remainder);
    bignum_deinit(temp);
    bignum_deinit(discard);
}

void bignum_inverse(bignum *a, bignum *m, bignum *result)
{
    bignum *remprev = bignum_init(), *rem = bignum_init();
    bignum *auxprev = bignum_init(), *aux = bignum_init();
    bignum *rcur = bignum_init(), *qcur = bignum_init(), *acur = bignum_init();

    bignum_copy(m, remprev);
    bignum_copy(a, rem);
    bignum_fromint(auxprev, 0);
    bignum_fromint(aux, 1);
    while (bignum_greater(rem, &NUMS[1]))
    {
        bignum_divide(qcur, rcur, remprev, rem);
        bignum_subtract(acur, m, qcur);
        bignum_imultiply(acur, aux);
        bignum_iadd(acur, auxprev);
        bignum_imodulate(acur, m);

        bignum_copy(rem, remprev);
        bignum_copy(aux, auxprev);
        bignum_copy(rcur, rem);
        bignum_copy(acur, aux);
    }

    bignum_copy(acur, result);

    bignum_deinit(remprev);
    bignum_deinit(rem);
    bignum_deinit(auxprev);
    bignum_deinit(aux);
    bignum_deinit(rcur);
    bignum_deinit(qcur);
    bignum_deinit(acur);
}

int bignum_jacobi(bignum *ac, bignum *nc)
{
    bignum *remainder = bignum_init(), *twos = bignum_init();
    bignum *temp = bignum_init(), *a = bignum_init(), *n = bignum_init();
    int mult = 1, result = 0;

    bignum_copy(ac, a);
    bignum_copy(nc, n);

    // Jacobi符号的计算循环，直到a小于或等于1，或者a与n相等
    while (bignum_greater(a, &NUMS[1]) && !bignum_equal(a, n))
    {
        // 计算 a mod n
        bignum_imodulate(a, n);

        // 如果a <= 1 或者 a == n，结束循环
        if (bignum_leq(a, &NUMS[1]) || bignum_equal(a, n))
            break;

        // 初始化twos为0，开始从a中提取因子2
        bignum_fromint(twos, 0);

        // 因数2的提取：将a不断除以2，直到a不再是2的倍数
        while (a->data[0] % 2 == 0)
        {
            bignum_iadd(twos, &NUMS[1]); // twos加1
            bignum_idivide(a, &NUMS[2]); // a除以2
        }

        // 计算是否需要翻转符号
        if (bignum_greater(twos, &NUMS[0]) && twos->data[0] % 2 == 1)
        {
            // n mod 8的余数
            bignum_remainder(n, &NUMS[8], remainder);
            // 如果余数不是1也不是7，则翻转符号
            if (!bignum_equal(remainder, &NUMS[1]) && !bignum_equal(remainder, &NUMS[7]))
            {
                mult *= -1; // 翻转符号
            }
        }

        // 如果a <= 1 或者 a == n，结束循环
        if (bignum_leq(a, &NUMS[1]) || bignum_equal(a, n))
            break;

        // 计算n mod 4 和 a mod 4的余数
        bignum_remainder(n, &NUMS[4], remainder);
        bignum_remainder(a, &NUMS[4], temp);

        // 如果余数都不是1，则翻转符号
        if (!bignum_equal(remainder, &NUMS[1]) && !bignum_equal(temp, &NUMS[1]))
            mult *= -1;

        // 更新a和n的值，用于下一轮计算
        bignum_copy(a, temp);
        bignum_copy(n, a);
        bignum_copy(temp, n);
    }

    if (bignum_equal(a, &NUMS[1]))
        result = mult;
    else
        result = 0;

    // 释放分配的内存
    bignum_deinit(remainder);
    bignum_deinit(twos);
    bignum_deinit(temp);
    bignum_deinit(a);
    bignum_deinit(n);

    return result;
}

int solovayPrime(int a, bignum *n)
{
    bignum *ab = bignum_init(), *res = bignum_init(), *pow = bignum_init();
    bignum *modpow = bignum_init();
    int x, result;

    bignum_fromint(ab, a);
    x = bignum_jacobi(ab, n);
    if (x == -1)
        bignum_subtract(res, n, &NUMS[1]);
    else
        bignum_fromint(res, x);
    bignum_copy(n, pow);
    bignum_isubtract(pow, &NUMS[1]);
    bignum_idivide(pow, &NUMS[2]);
    bignum_modpow(ab, pow, n, modpow);

    result = !bignum_equal(res, &NUMS[0]) && bignum_equal(modpow, res);
    bignum_deinit(ab);
    bignum_deinit(res);
    bignum_deinit(pow);
    bignum_deinit(modpow);
    return result;
}

int probablePrime(bignum *n, int k)
{
    if (bignum_equal(n, &NUMS[2]))
        return 1;
    else if (n->data[0] % 2 == 0 || bignum_equal(n, &NUMS[1]))
        return 0;
    while (k-- > 0)
    {
        if (n->length <= 1)
        {
            if (!solovayPrime(rand() % (n->data[0] - 2) + 2, n))
                return 0;
        }
        else
        {
            int wit = rand() % (RAND_MAX - 2) + 2;
            if (!solovayPrime(wit, n))
                return 0;
        }
    }
    return 1;
}

void randPrime(int numDigits, bignum *result)
{
    char *string = malloc((numDigits + 1) * sizeof(char));
    int i;
    string[0] = (rand() % 9) + '1';
    string[numDigits - 1] = (rand() % 5) * 2 + '1';
    for (i = 1; i < numDigits - 1; i++)
        string[i] = (rand() % 10) + '0';
    string[numDigits] = '\0';
    bignum_fromstring(result, string);
    while (1)
    {
        if (probablePrime(result, ACCURACY))
        {
            free(string);
            return;
        }
        bignum_iadd(result, &NUMS[2]);
    }
}

void randExponent(bignum *phi, int n, bignum *result)
{
    bignum *gcd = bignum_init();
    int e = rand() % n;
    while (1)
    {
        bignum_fromint(result, e);
        bignum_gcd(result, phi, gcd);
        if (bignum_equal(gcd, &NUMS[1]))
        {
            bignum_deinit(gcd);
            return;
        }
        e = (e + 1) % n;
        if (e <= 2)
            e = 3;
    }
}

int readFile(FILE *fd, char **buffer, int bytes)
{
    int len = 0, cap = BUF_SIZE, r;
    char buf[BUF_SIZE];
    *buffer = malloc(BUF_SIZE * sizeof(char));
    while ((r = fread(buf, sizeof(char), BUF_SIZE, fd)) > 0)
    {
        if (len + r >= cap)
        {
            cap *= 2;
            *buffer = realloc(*buffer, cap);
        }
        memcpy(&(*buffer)[len], buf, r);
        len += r;
    }

    if (len + bytes - len % bytes > cap)
        *buffer = realloc(*buffer, len + bytes - len % bytes);
    do
    {
        (*buffer)[len] = '\0';
        len++;
    } while (len % bytes != 0);
    return len;
}

void encode(bignum *m, bignum *e, bignum *n, bignum *result)
{
    bignum_modpow(m, e, n, result);
}

void decode(bignum *c, bignum *d, bignum *n, bignum *result)
{
    bignum_modpow(c, d, n, result);
}

bignum *encodeMessage(int len, int bytes, char *message, bignum *exponent, bignum *modulus)
{

    int i, j;
    bignum *encoded = calloc(len / bytes, sizeof(bignum));
    bignum *num128 = bignum_init(), *num128pow = bignum_init();
    bignum *x = bignum_init(), *current = bignum_init();
    bignum_fromint(num128, 128);
    bignum_fromint(num128pow, 1);
    for (i = 0; i < len; i += bytes)
    {
        bignum_fromint(x, 0);
        bignum_fromint(num128pow, 1);
        // buffer[0] + buffer[1]*128 + buffer[2]*128^2
        for (j = 0; j < bytes; j++)
        {
            bignum_fromint(current, message[i + j]);
            bignum_imultiply(current, num128pow);
            bignum_iadd(x, current); // x += buffer[i + j] * (1 << (7 * j))
            bignum_imultiply(num128pow, num128);
        }
        encode(x, exponent, modulus, &encoded[i / bytes]);
#ifndef NOPRINT
        bignum_print(&encoded[i / bytes]);
        printf(" ");
#endif
    }
    return encoded;
}

int *decodeMessage(int len, int bytes, bignum *cryptogram, bignum *exponent, bignum *modulus)
{
    int *decoded = malloc(len * bytes * sizeof(int));
    int i, j;
    bignum *x = bignum_init(), *remainder = bignum_init();
    bignum *num128 = bignum_init();
    bignum_fromint(num128, 128);
    for (i = 0; i < len; i++)
    {
        decode(&cryptogram[i], exponent, modulus, x);
        for (j = 0; j < bytes; j++)
        {
            bignum_idivider(x, num128, remainder);
            if (remainder->length == 0)
                decoded[i * bytes + j] = (char)0;
            else
                decoded[i * bytes + j] = (char)(remainder->data[0]);
#ifndef NOPRINT
            printf("%c", (char)(decoded[i * bytes + j]));
#endif
        }
    }
    return decoded;
}

int main(void)
{
    int i, bytes, len;
    bignum *p = bignum_init(), *q = bignum_init(), *n = bignum_init();
    bignum *phi = bignum_init(), *e = bignum_init(), *d = bignum_init();
    bignum *bbytes = bignum_init(), *shift = bignum_init();
    bignum *temp1 = bignum_init(), *temp2 = bignum_init();

    bignum *encoded;
    int *decoded;
    char *buffer;
    FILE *f;

    srand(time(NULL)); // 初始化随机数种子

    randPrime(FACTOR_DIGITS, p); // 生成第一个质数 p
    printf("生成第一个素数 p, p = ");
    bignum_print(p);
    printf(" ... ");
    getchar(); // 等待用户输入

    randPrime(FACTOR_DIGITS, q); // 生成第二个质数 q
    printf("生成第二个素数 q, q = ");
    bignum_print(q);
    printf(" ... ");
    getchar(); // 等待用户输入

    bignum_multiply(n, p, q); // 计算模数 n = p * q
    printf("计算 n, n = pq = ");
    bignum_print(n);
    printf(" ... ");
    getchar(); // 等待用户输入

    bignum_subtract(temp1, p, &NUMS[1]); // 计算 p - 1
    bignum_subtract(temp2, q, &NUMS[1]); // 计算 q - 1
    bignum_multiply(phi, temp1, temp2);  // 计算欧拉函数 phi = (p - 1) * (q - 1)
    printf("计算欧拉函数 phi = (p - 1) * (q - 1) = ");
    bignum_print(phi);
    printf(" ... ");
    getchar(); // 等待用户输入

    randExponent(phi, EXPONENT_MAX, e); // 选择公钥指数 e
    printf("随机选择一个公钥指数, e = ");
    bignum_print(e);
    printf("\n公钥为 (");
    bignum_print(e);
    printf(", ");
    bignum_print(n);
    printf(") ... ");
    getchar(); // 等待用户输入

    bignum_inverse(e, phi, d); // 计算私钥指数 d
    printf("计算私钥指数, d = ");
    bignum_print(d);
    printf("\n私钥为 (");
    bignum_print(d);
    printf(", ");
    bignum_print(n);
    printf(") ... ");
    getchar(); // 等待用户输入

    /* 计算每次加密时可以编码的最大字节数 */
    bytes = -1;
    bignum_fromint(shift, 1 << 7); // 每个字符7位
    bignum_fromint(bbytes, 1);     // 初始化 bbytes
    while (bignum_less(bbytes, n)) // 当 bbytes 小于 n 时
    {
        bignum_imultiply(bbytes, shift); // 每次移位一字节
        bytes++;                         // 增加字节数
    }

    printf("打开文件 \"text.txt\"\n");
    f = fopen("text.txt", "r"); // 打开文件 "text.txt" 进行读取
    if (f == NULL)
    {
        printf("打开文件失败 \"text.txt\"！\n");
        return EXIT_FAILURE; // 文件打开失败，退出程序
    }
    len = readFile(f, &buffer, bytes); // 读取文件内容，len是读取的字节数

    printf("读取文件 \"text.txt\" 成功, %d 字节. 加密字节长度为 %d 字节 ... ", len, bytes);
    getchar(); // 等待用户输入
    printf("\n");
    encoded = encodeMessage(len, bytes, buffer, e, n); // 对文件内容进行加密
    printf("\n\n加密文件完成 ... ");
    getchar(); // 等待用户输入

    printf("开始解密文件 ... ");
    getchar(); // 等待用户输入
    printf("\n");
    decoded = decodeMessage(len / bytes, bytes, encoded, d, n); // 解密加密的消息
    printf("\n\n完成解密！");

    /* 释放之前分配的内存 */
    for (i = 0; i < len / bytes; i++)
        free(encoded[i].data); // 释放加密后的数据
    free(encoded);             // 释放加密后的数据指针
    free(decoded);             // 释放解密后的数据
    free(buffer);              // 释放文件缓冲区
    bignum_deinit(p);          // 释放 p
    bignum_deinit(q);          // 释放 q
    bignum_deinit(n);          // 释放 n
    bignum_deinit(phi);        // 释放 phi
    bignum_deinit(e);          // 释放 e
    bignum_deinit(d);          // 释放 d
    bignum_deinit(bbytes);     // 释放 bbytes
    bignum_deinit(shift);      // 释放 shift
    bignum_deinit(temp1);      // 释放 temp1
    bignum_deinit(temp2);      // 释放 temp2
    fclose(f);                 // 关闭文件

    return EXIT_SUCCESS; // 程序正常结束
}
