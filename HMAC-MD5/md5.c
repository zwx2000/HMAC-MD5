//
//  md5.c
//  HMAC-MD5
//
//  Created by 赵文序 on 2020/11/16.
//

#include "md5.h"

static unsigned char * Padding_Message;    // 第一次填充后的信息（10...0)
static unsigned int  * Paded_Message;      // 第二次填充后的信息（末尾附加message_length）
static unsigned long block_num;            // 分块数目

// T表
const int T_TABLE[] = {
    0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee,
    0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501,
    0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be,
    0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821,
    0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa,
    0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8,
    0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed,
    0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a,
    0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c,
    0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70,
    0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05,
    0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665,
    0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039,
    0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1,
    0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1,
    0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391
};

// 各轮各次迭代运算(1...64)采用的左循环移位的位数s值
const int S_VALUE[] = {
    7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22,
    5,  9, 14, 20, 5,  9, 14, 20, 5,  9, 14, 20, 5,  9, 14, 20,
    4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23,
    6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21
};

// MD缓冲区初始向量IV（little-endian）
const unsigned int IV[4] = {
    0x67452301,
    0xEFCDAB89,
    0x98BADCFE,
    0x10325476
};

// 存储每一轮计算出的CV值
unsigned int CV[4] = {
    0x67452301,
    0xEFCDAB89,
    0x98BADCFE,
    0x10325476
};

// 1.填充信息
// message_len：消息的字节数
void Padding(char * origin_message, unsigned long message_len) {
    // 第一次填充
    // 512bit一组 即64byte一组
    block_num = message_len / 64 + (((message_len * 8) % 512) >= 448 ? 2 : 1);
    Padding_Message = (unsigned char *)malloc(block_num * 64);
    for(int i = 0; i < message_len; i ++) {
        Padding_Message[i] = origin_message[i];
    }
    for(unsigned long i = message_len; i < block_num * 64; i ++) {
        Padding_Message[i] = 0;
    }
    Padding_Message[message_len] = 0x80; //第一位填充为1

    Paded_Message = (unsigned int *)malloc(block_num * 16 * sizeof(unsigned int) / sizeof(char)); // 64/4 = 16
    char_to_int(Padding_Message, Paded_Message, block_num * 64);
    
    // 第二次填充 附加一个64位二进制表示的初始信息长度
    unsigned int front = ((message_len * 8) >> 32) & 0x00000000ffffffff;
    unsigned int behind = (message_len * 8) & 0x00000000ffffffff;
    Paded_Message[block_num * 16 - 2] = behind;
    Paded_Message[block_num * 16 - 1] = front;
    return;
}

// 2.分块
MD5_GROUP Block(char * origin_message, unsigned long message_length) {
    MD5_GROUP res;
    Padding(origin_message, message_length);
    initCV(CV);
    
    for (int i = 0; i < block_num; i++) {
        unsigned int tmp[4] = {0, 0, 0, 0};
        MD5_Compress(Paded_Message + i * 16, tmp);
        for (int j = 0; j < 4; j++) CV[j] += tmp[j];
    }

    for (int i = 0; i < 4; i++) res.element[i] = CV[i];

    free(Padding_Message);
    free(Paded_Message);
    return res;
}

// 模块二：
// 1.初始化CV
void initCV(unsigned int CV[4]) {
    for(int i = 0; i < 4; i ++) CV[i] = IV[i];
}
// 2.循环压缩函数
void MD5_Compress(int * group, unsigned int * res) {
    unsigned int curCV[4];
    unsigned int nextCV[4];
    for(int i = 0; i < 4; i ++) curCV[i] = CV[i];
    
    for(int i = 0; i < 4; i ++) {
        for(int j = 0; j < 16; j ++) {
            // 对a迭代：a = b + ((a + g(b, c, d) + X[k] + T[i]) <<< s)
            switch (i) {
                case 0: // 第一轮
                    nextCV[1] = curCV[1] + CLS((curCV[0] + F(curCV[1], curCV[2], curCV[3]) + group[j] + T_TABLE[j]), S_VALUE[j]);
                    break;
                case 1: // 第二轮
                    nextCV[1] = curCV[1] + CLS((curCV[0] + G(curCV[1], curCV[2], curCV[3]) + group[(1 + 5 * j) % 16] + T_TABLE[j + i * 16]), S_VALUE[j + i * 16]);
                    break;
                case 2: // 第三轮
                    nextCV[1] = curCV[1] + CLS((curCV[0] + H(curCV[1], curCV[2], curCV[3]) + group[(5 + 3 * j) % 16] + T_TABLE[j + i * 16]), S_VALUE[j + i * 16]);
                    break;
                case 3: // 第四轮
                    nextCV[1] = curCV[1] + CLS((curCV[0] + I(curCV[1], curCV[2], curCV[3]) + group[(7 * j) % 16] + T_TABLE[j + i * 16]), S_VALUE[j + i * 16]);
                    break;
                default:
                    break;
            }
            nextCV[2] = curCV[1];
            nextCV[3] = curCV[2];
            nextCV[0] = curCV[3];
            
            for(int i = 0; i < 4; i ++) curCV[i] = nextCV[i];
        }
    }
    
    // 将(A, B, C, D)置换为(B, C, D, A)

    for(int i = 0; i < 4; i ++) res[i] = curCV[i];
}

// int转为char
unsigned char * int_to_char(unsigned int * arr, unsigned char * res, unsigned long len) {
    for (int i = 0; i < len; i++)
    {
      res[i * 4 + 3] = (arr[i] >> 24) & 0x000000ff;
      res[i * 4 + 2] = (arr[i] >> 16) & 0x000000ff;
      res[i * 4 + 1] = (arr[i] >> 8) & 0x000000ff;
      res[i * 4] =  arr[i] & 0x000000ff;
    }
    return res;
}

// char转为int
unsigned int * char_to_int(unsigned char * str, unsigned int * res, unsigned long len) {
    for (int i = 0; i < len / 4; i++)
    {
      res[i] = (str[i * 4]) |
               (str[i * 4 + 1] << 8) |
               (str[i * 4 + 2] << 16) |
               (str[i * 4 + 3] << 24);
    }

    return res;
}
