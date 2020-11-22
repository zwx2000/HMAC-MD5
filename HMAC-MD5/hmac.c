//
//  hmac.c
//  HMAC-MD5
//
//  Created by 赵文序 on 2020/11/16.
//

#include "hmac.h"

#define BLOCKSIZE 64

char * M; // Message input to HMAC
char * k; // The secret key, |k| <= b
char * k_plus; // 对共享密钥k左边补0，生成一个b位的数据块K+
unsigned long b = 0; // Length (bits) of input block
unsigned long key_len = 0; // length of input key
char * Si;
char * So;

// 对共享密钥k左边补0，生成一个b位的数据块K+
void generate_K_plus() {
    k_plus = (char *)malloc(64);
    // 右边补位0
    for (unsigned long i = 0; i < key_len; i ++) k_plus[i] = k[i];
    for (unsigned long i = key_len; i < 64; i ++) k_plus[i] = 0;
}

// K+与ipad作XOR，生成b位的Si
void generate_Si() {
    Si = (char *)malloc(64);
    for (unsigned long i = 0; i < 64; i ++)
      Si[i] = k_plus[i] ^ ipad;
}

// K+与opad作XOR，生成b位的So
void generate_So() {
    So = (char *)malloc(64);
    for (unsigned long i = 0; i < 64; i ++)
      So[i] = k_plus[i] ^ opad;
}

void generateKPlus()
{
  k_plus = (char *)malloc(BLOCKSIZE);

  // 右边补位0
  for (unsigned long i = 0; i < key_len; i++)
  {
    k_plus[i] = k[i];
  }

  for (unsigned long i = key_len; i < BLOCKSIZE; i++)
  {
    k_plus[i] = 0;
  }

  // 顺带获取 Si 和 So
  Si = (char *)malloc(BLOCKSIZE);
  So = (char *)malloc(BLOCKSIZE);

  for (unsigned long i = 0; i < BLOCKSIZE; i++)
  {
    Si[i] = k_plus[i] ^ ipad;
    So[i] = k_plus[i] ^ opad;
  }
}

// hash函数（利用MD5实现）
MD5_GROUP Hash(unsigned char * S, unsigned long S_len, unsigned char * M, unsigned long M_len) {
    unsigned char SM[M_len + S_len + 1];
    SM[S_len + M_len] = 0;

    // 第一段
    for (unsigned long i = 0; i < S_len; i++) SM[i] = S[i];

    // 第二段
    for (unsigned long i = 0; i < M_len; i++) SM[i + S_len] = M[i];
    
    MD5_GROUP res = Block(SM, S_len + M_len);
    return res;
}
