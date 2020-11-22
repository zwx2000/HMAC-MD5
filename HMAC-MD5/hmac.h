//
//  hmac.h
//  HMAC-MD5
//
//  Created by 赵文序 on 2020/11/16.
//

#ifndef hmac_h
#define hmac_h

#include <stdio.h>
#include "md5.h"

#define ipad 0x36 // 00110110 重复 b/8 次
#define opad 0x5c // 01011100 重复 b/8 次

void generateKPlus(void);

// 对共享密钥k左边补0，生成一个b位的数据块K+
void generate_K_plus(void);
// K+与ipad作XOR，生成b位的Si
void generate_Si(void);
// K+与opad作XOR，生成b位的So
void generate_So(void);
// hash函数（利用MD5实现）
MD5_GROUP Hash(unsigned char *S, unsigned long S_len, unsigned char *M, unsigned long M_len);

//void freeAll(void);
#endif /* hmac_h */
