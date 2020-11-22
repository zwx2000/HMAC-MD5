//
//  main.c
//  HMAC-MD5
//
//  Created by 赵文序 on 2020/11/16.
//

#include <stdio.h>
#include <stdlib.h>
#include "md5.h"
#include "hmac.h"

#define BLOCKSIZE 64

int main(int argc, const char * argv[]) {
    extern unsigned char * Padding_Message;    // 第一次填充后的信息（10...0)
    extern unsigned int  * Paded_Message;      // 第二次填充后的信息（末尾附加message_length）
    extern unsigned long block_num;            // 分块数目
    extern char * M;                           // 输入HMAC的信息
    extern char * k;                           // 密钥
    extern char * k_plus;                      // 对共享密钥k左边补0，生成一个b位的数据块K+
    extern unsigned long b;                    // Length (bits) of input block
    extern unsigned long key_len;              // length of key
    extern unsigned char * Si;                 // K+ ^ ipad
    extern unsigned char * So;                 // K+ ^ opad
    
    M = (char *)malloc(15);
    k = (char *)malloc(4);
    M = "HMAC-MD5";
    k = "key";
    key_len = 4;
    b = 8;
    printf("raw information:\n\"%s\"\n\n", M);
    printf("key:\n\"%s\"\n\n", k);
    
    generate_K_plus();
    
    generate_Si();
    MD5_GROUP res1 = Hash(Si, BLOCKSIZE, M, b);
    unsigned char res1_str[16];
    int_to_char(res1.element, res1_str, 4);
//    printf("H(Si||M)：\n");
//    for (int i = 0; i < 4 * 4; i++) printf("%02x", res1_str[i]);
//    printf("\n\n");
    
    generate_So();
    MD5_GROUP res2 = Hash(So, BLOCKSIZE, res1_str, 16);
    unsigned char res2_str[16];
    int_to_char(res2.element, res2_str, 4);
    printf("HMAC_K：\n");
    for (int i = 0; i < 4 * 4; i++) printf("%02x", res2_str[i]);
    printf("\n\n");

    return 0;
}
