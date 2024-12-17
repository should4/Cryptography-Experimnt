#ifndef _AES_H_
#define _AES_H_

typedef unsigned char uint8_t;

class AES
{
public:
    // 使用电码本工作模式进行加解密
    static void EncryptECB(const uint8_t *in, unsigned int inlen, uint8_t *out, const uint8_t *key, unsigned int keylen);
    static void DecryptECB(const uint8_t *in, unsigned int inlen, uint8_t *out, const uint8_t *key, unsigned int keylen);

    // 按字块 加密 | 解密
    // roundKeys : 代表轮密钥
    static void EncriptBlock(const uint8_t *in, uint8_t *out, const uint8_t *roundKeys);
    static void DecriptBlock(const uint8_t *in, uint8_t *out, const uint8_t *roundKeys);

    // S盒 计算生成 多项式求逆 + 仿射变换
    static uint8_t ByteTransform(uint8_t a);
    static uint8_t InvByteTransform(uint8_t a);

    // 字代替总过程
    static void SubBytes(uint8_t state[4][4]);
    static void InvSubBytes(uint8_t state[4][4]);
    // 行移位
    static void ShiftRows(uint8_t state[4][4]);
    static void InvShiftRows(uint8_t state[4][4]);
    // 列混合
    static void MixColumns(uint8_t state[4][4]);
    static void InvMixColumns(uint8_t state[4][4]);
    // XOR 轮密钥
    static void XorRoundKey(uint8_t state[4][4], const uint8_t key[]);

    // 密钥扩展：轮密钥计算
    static void KeyExpand(const uint8_t key[], uint8_t w[]);        // 密钥扩展
    static void RotWord(uint8_t *a);                                // 字循环
    static void SubWord(uint8_t *a);                                // 字节代换
    static void XorWords(uint8_t *a, const uint8_t *b, uint8_t *c); // 将每字与轮常数抑或并保存在 c 中

private:
    const unsigned int block_bytes = 16; // 分组长度
    unsigned int Nr = 10;                // 轮数
    unsigned int Nk = 4;                 // 密钥字数（密钥Ki中每个字W[4i],W[4i+1],W[4i+2],W[4i+3]为32位比特字，包含4个字节）
};

// S 盒的仿射变换矩阵： B = ZA^(-1) + C
// Z 矩阵
static const uint8_t s_box_z_matrix[8] = {
    0b11110001,
    0b11100011,
    0b11000111,
    0b10001111,
    0b00011111,
    0b00111110,
    0b01111100,
    0b11111000,
};
// C 矩阵
static const uint8_t s_box_c_matrix = 0b01100011;

/// circulant MDS matrix 列混合矩阵
static const uint8_t CMDS[4][4] = {
    {2, 3, 1, 1},
    {1, 2, 3, 1},
    {1, 1, 2, 3},
    {3, 1, 1, 2},
};

// Inverse circulant MDS matrix 列混合逆矩阵
static const uint8_t INV_CMDS[4][4] = {
    {14, 11, 13, 9},
    {9, 14, 11, 13},
    {13, 9, 14, 11},
    {11, 13, 9, 14},
};

// 轮常数
static const uint8_t RCON[10][4] = {
    {0x1, 0x0, 0x0, 0x0},
    {0x2, 0x0, 0x0, 0x0},
    {0x4, 0x0, 0x0, 0x0},
    {0x8, 0x0, 0x0, 0x0},
    {0x10, 0x0, 0x0, 0x0},
    {0x20, 0x0, 0x0, 0x0},
    {0x40, 0x0, 0x0, 0x0},
    {0x80, 0x0, 0x0, 0x0},
    {0x1B, 0x0, 0x0, 0x0},
    {0x36, 0x0, 0x0, 0x0},
};
#endif
