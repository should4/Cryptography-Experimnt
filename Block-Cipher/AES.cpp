#include "AES.h"
#include "BiPoly.h"

#include <string.h> // memcpy

void AES::EncryptECB(const uint8_t *in, unsigned int inlen, uint8_t *out, const uint8_t *key, unsigned int keylen)
{
    // [1] 进行密钥扩展
    unsigned char w[16 * 11] = {};
    AES::KeyExpand(key, w);

    // [2] 对原始输入数据进行分块处理
    // 此处不做填充处理
    if (inlen % 16 != 0 || keylen % 16 != 0)
    {
        out = nullptr;
        printf("length error!\n");
        return;
    }

    unsigned int block_num = inlen / 16;
    for (int i = 0; i < block_num; ++i)
    {
        EncriptBlock(in + 16 * i, out + 16 * i, w);
    }
}

void AES::DecryptECB(const uint8_t *in, unsigned int inlen, uint8_t *out, const uint8_t *key, unsigned int keylen)
{
    // [1] 进行密钥扩展
    unsigned char w[16 * 11] = {};
    AES::KeyExpand(key, w);

    // [2] 对原始输入数据进行分块处理
    // 此处不做填充处理
    if (inlen % 16 != 0 || keylen % 16 != 0)
    {
        out = nullptr;
        printf("length error!\n");
        return;
    }

    // [3] 采用 ECB 分块独立加密
    // 此处可以采用多线程的模式处理，利用多处理器的冰箱能力计算
    unsigned int block_num = inlen / 16;
    for (int i = 0; i < block_num; ++i)
    {
        DecriptBlock(in + 16 * i, out + 16 * i, w);
    }
}

void AES::EncriptBlock(const uint8_t *in, uint8_t *out, const uint8_t *roundKeys)
{
    uint8_t state[4][4];
    int round;

    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            state[i][j] = in[i + 4 * j];
        }
    }

    XorRoundKey(state, roundKeys);

    for (round = 1; round < 10; ++round)
    {
        SubBytes(state);
        ShiftRows(state);
        MixColumns(state);
        XorRoundKey(state, roundKeys + 16 * round);
    }

    // 第 10 轮加密不进行列混淆
    SubBytes(state);
    ShiftRows(state);
    XorRoundKey(state, roundKeys + 16 * round);

    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            out[i + 4 * j] = state[i][j];
        }
    }
}

void AES::DecriptBlock(const uint8_t *in, uint8_t *out, const uint8_t *roundKeys)
{
    uint8_t state[4][4];
    int round = 10;
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            state[i][j] = in[i + 4 * j];
        }
    }

    // 第 10 轮加密不进行列混淆
    XorRoundKey(state, roundKeys + 16 * round--);
    InvShiftRows(state);
    InvSubBytes(state);

    for (; round > 0; --round)
    {
        XorRoundKey(state, roundKeys + 16 * round);
        InvMixColumns(state);
        InvShiftRows(state);
        InvSubBytes(state);
    }

    XorRoundKey(state, roundKeys);

    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            out[i + 4 * j] = state[i][j];
        }
    }
}

void AES::SubBytes(uint8_t state[4][4])
{
    for (int i = 0; i < 4; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            state[i][j] = ByteTransform(state[i][j]);
        }
    }
}

void AES::InvSubBytes(uint8_t state[4][4])
{
    for (int i = 0; i < 4; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            state[i][j] = InvByteTransform(state[i][j]);
        }
    }
}

uint8_t AES::ByteTransform(uint8_t a)
{
    // [1] 多项式求逆
    unsigned x, y;
    // 使用扩展欧几里得算法求逆
    BiPoly::exgcd((unsigned int)a, 0x11B, x, y);
    a = (uint8_t)(BiPoly::Mod(x, 0x11B));

    // [2] 仿射变换
    uint8_t b = 0;
    for (int i = 0; i < 8; ++i)
    {
        uint8_t tmp = s_box_z_matrix[i] & a;
        for (int j = 0; j < 8; ++j)
        {
            b ^= ((tmp >> j) & 0x01) << i; // 对应位均不为 0
        }
        // 加上 c 矩阵
        b ^= (s_box_c_matrix & (0x01 << i));
    }
    return b;
}

uint8_t AES::InvByteTransform(uint8_t a)
{
    // [1] 首先进行放射逆运算
    uint8_t tmp[8] = {0};

    for (uint8_t i = 0; i < 8; i++)
    {
        tmp[i] = (((a >> ((i + 2) % 8)) & 0x1) ^ ((a >> ((i + 5) % 8)) & 0x1) ^ ((a >> ((i + 7) % 8)) & 0x1) ^ ((0x05 >> i) & 0x1)) << i;
    }
    tmp[0] = tmp[0] + tmp[1] + tmp[2] + tmp[3] + tmp[4] + tmp[5] + tmp[6] + tmp[7];
    a = tmp[0];

    // [2] 接着求多项式的逆
    unsigned x, y;
    BiPoly::exgcd((unsigned int)a, 0x11B, x, y);
    a = (uint8_t)(BiPoly::Mod(x, 0x11B));

    return a;
}

void AES::ShiftRows(uint8_t state[4][4])
{
    // 第 i行 左移 i次
    for (int i = 0; i < 4; ++i)
    {
        uint8_t tmp[4] = {};
        for (int j = 0; j < 4; ++j)
        {

            tmp[j] = state[i][(j + i) % 4];
        }
        memcpy(state[i], tmp, 4);
    }
}

void AES::InvShiftRows(uint8_t state[4][4])
{
    // 第 i行 右移 i次
    uint8_t tmp[16] = {};
    for (int i = 0; i < 4; ++i)
    {
        uint8_t tmp[4] = {};
        for (int j = 0; j < 4; ++j)
        {
            tmp[j] = state[i][(4 + j - i) % 4];
        }
        memcpy(state[i], tmp, 4);
    }
}

void AES::MixColumns(uint8_t state[4][4])
{
    uint8_t tmp[4][4] = {};

    // 矩阵乘法（多项式乘法）
    for (int i = 0; i < 4; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            tmp[i][j] = 0;
            for (int k = 0; k < 4; ++k)
            {
                tmp[i][j] ^= BiPoly::Mod(BiPoly::Mul((unsigned int)CMDS[i][k], state[k][j]), 0x11B);
            }
        }
    }

    for (int i = 0; i < 4; ++i)
    {
        memcpy(state[i], tmp[i], 4);
    }
}

void AES::InvMixColumns(uint8_t state[4][4])
{
    int8_t tmp[4][4] = {};

    // 矩阵乘法（多项式乘法）
    for (int i = 0; i < 4; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            tmp[i][j] = 0;
            for (int k = 0; k < 4; ++k)
            {
                tmp[i][j] ^= BiPoly::Mod(BiPoly::Mul((unsigned int)INV_CMDS[i][k], state[k][j]), 0x11B);
            }
        }
    }

    for (int i = 0; i < 4; ++i)
    {
        memcpy(state[i], tmp[i], 4);
    }
}

void AES::XorRoundKey(uint8_t state[4][4], const uint8_t key[])
{
    for (int i = 0; i < 4; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            state[i][j] = state[i][j] ^ key[4 * j + i];
        }
    }
}

void AES::KeyExpand(const uint8_t key[], uint8_t w[])
{
    // [1] 初始化轮密钥
    for (int i = 0; i < 16; ++i)
    {
        w[i] = key[i];
    }

    // 如果i是4的倍数，那么第i列由如下等式确定： W[i]=W[i-4]⨁T(W[i-1])
    // 2.如果 i 不是4的倍数，那么第i列由如下等式确定：W[i]=W[i-4]⨁W[i-1]
    // 注意：此处的 i = index / 4
    int index = 16; // 迭代初始位置
    uint8_t tmp[4]; // 每一字临时变量
    while (index < 4 * 44)
    {
        // tmp = W[i-1]
        for (int i = 0; i < 4; ++i)
        {
            tmp[i] = w[index - 4 + i];
        }

        // tmp = T(W[i-1])
        if (index / 4 % 4 == 0)
        {
            RotWord(tmp);
            SubWord(tmp);
            XorWords(tmp, RCON[(index / 16) - 1], tmp);
        }

        // W[i]=W[i-4]⨁T(W[i-1]) | W[i]=W[i-4]⨁W[i-1]
        for (int i = 0; i < 4; ++i)
        {
            w[index + i] = w[index - 16 + i] ^ tmp[i];
        }
        index += 4;
    }
}

void AES::RotWord(uint8_t *a)
{
    // 左移一字节
    uint8_t tmp[4];
    for (int i = 0; i < 4; ++i)
    {
        tmp[i] = a[(i + 1) % 4];
    }
    memcpy(a, tmp, 4);
}

void AES::SubWord(uint8_t *a)
{
    // 对字循环的结果使用S盒进行字节代换
    for (int i = 0; i < 4; ++i)
    {
        a[i] = ByteTransform(a[i]);
    }
}

void AES::XorWords(uint8_t *a, const uint8_t *b, uint8_t *c)
{
    // 轮常量异或
    for (int i = 0; i < 4; ++i)
    {
        c[i] = a[i] ^ b[i];
    }
}
