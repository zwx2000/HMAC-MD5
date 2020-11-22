//
//  md5.h
//  HMAC-MD5
//
//  Created by 赵文序 on 2020/11/16.
//

#ifndef md5_h
#define md5_h

#include <stdlib.h>
#include <stdio.h>

// 循环压缩函数(轮函数)定义
#define F(b, c, d) ((b & c) | (~b & d))        // 第一轮
#define G(b, c, d) ((b & d) | (c & ~d))        // 第二轮
#define H(b, c, d) (b ^ c ^ d)                 // 第三轮
#define I(b, c, d) (c ^ (b | ~d))              // 第四轮

/*循环左移函数
     1 2 3
     2 3 0  左移  1  位
     0 0 1  右移 3-1 位
     2 3 1
 */
#define CLS(x, s) ((x << s) | (x >> (32 - s)))

// MD5分组：4个32位的寄存器 512bit
typedef struct {
    unsigned int element[4];
} MD5_GROUP;

// 模块一：
// 1.填充信息
void Padding(char * origin_message, unsigned long message_len);
// 2.分块
MD5_GROUP Block(char * origin_message, unsigned long message_length);

// 模块二：
// 1.初始化CV
void initCV(unsigned int CV[4]);
// 1.循环压缩函数
void MD5_Compress(int * group, unsigned int * res);
// int转为char
unsigned char * int_to_char(unsigned int * arr, unsigned char * res, unsigned long len);
// char转为int
unsigned int * char_to_int(unsigned char * str, unsigned int * res, unsigned long len);

#endif /* md5_h */
