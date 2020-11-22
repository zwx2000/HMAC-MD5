# Information Security Assignment 3. 期中大作业

**个人信息**

|  姓名  |   学号   |
| :----: | :------: |
| 赵文序 | 18342137 |

**实验环境**

- 操作系统：macOS Catalina 10.15.7
- IDE：Xcode Version 12.2
- 实现语言：C

**INDEX**

[TOC]

## Topic Requirement

- 完成一个`HMAC-MD5`算法的程序设计和实现，包括
  - MD5算法原理概述；HMAC算法原理概述；总体结构设计；模块分解；数据结构设计；C语言源代码；编译运行结果；验证用例

## 一. MD5算法原理概述

> MD5即Message-Digest Algorithm 5 (信息-摘要算法5)，是广泛使用的<u>Hash算法</u>，用于确保信息传输的**完整性**和**一致性**。

- MD5使用`little-endian`(小端模式)，输入任意不定长度信息，以`512-bit`进行分组，生成4个`32-bit`数据，最后这四个`32-bit`数据分组级联后输出固定`128-bit`的信息摘要（散列值）。

### MD5算法基本过程

#### 1. 填充

**首先需要对输入信息(K)进行填充(Padding)**，使其位长对<u>512</u>求余的结果为<u>448</u>，信息位长扩展为N*512+448

填充方法：

1. 在信息后填充`一个1`和`若干个0`，直到满足上述条件(`K' % 512 = 448`)时停止
2. 在完成步骤1得到的结果末尾添加一个以64位二进制表示的填充**前**信息的长度(Message length) , `K mod 2^64`

![](/Users/zhaowenxu/Desktop/HMACMD5/tianchong.png)

此时处理过后==**信息的位长 = N*512 + 448 + 64**==，恰好为512的整数倍，便于之后的处理。

#### 2. 分块

将处理过后的信息以==**512-bits**==为大小进行分组，分组结果记为`Y(0) Y(1) ... Y(q) ... Y(L-1)`

![](/Users/zhaowenxu/Desktop/HMACMD5/group.png)

#### 3. 缓冲区初始化

- 4个字节(32bit)的4个寄存器构成向量(MD缓冲区)`(A,B,C,D)`

- 采用==**小端存储**==(little-endian)的存储结构，即**低位字节**排放在内存的**低地址端**，**高位字节**排放在内存的**高地址端**。

  - 例如：

  ![](/Users/zhaowenxu/Desktop/HMACMD5/chushihua.png)

PS:16进制初值作为MD缓冲区的初始向量`IV`

#### 4. 循环压缩

> 一个MD5运算以**512-bit消息分组**为单位，每一分组 Y~q~ (q = 0, 1 ,..., L-1) 经过**4个**循环的压缩算法，主循环**4轮**，每一轮需进行**16次操作**，4轮循环共有**64次迭代运算**。

**主要算法流程：**

1. 单轮运算过程：

   第一分组 Y~0~ 的初始向量 CV 即为16进制初值，从第二分组 Y~1~ 开始，输入向量 CV~1~ 即为上一分组的运算结果，H~MD5~ 从 CV 输入128位，从消息分组输入512位，压缩运算后输出128位，作为用于下一轮输入的CV值，表示为：

   - $CV_0 = IV$
   - $CV_i = H_{MD5}(CV_{i-1}, Y_{i-1}), i = 1, ..., L$
   - 输出结果hash值：$MD=CV_L$

2. 每轮循环固定不同的生成函数F，G，H，I，结合指定的T表元素`T[]`和消息分组的不同部分`X[]`做16次迭代运算，总共经过4轮循环后输出128位结果。

   ---

   生成函数g(也称轮函数)是一个32位非线性逻辑函数，具体定义如下表所示：

   | 轮次 |  Function g  |      g(b, c, d)      |
   | :--: | :----------: | :------------------: |
   |  1   | $F(b, c, d)$ | `(b ⋀ c) ⋁ (~b ⋀ d)` |
   |  2   | $G(b, c, d)$ | `(b ⋀ d) ⋁ (c ⋀ ~d)` |
   |  3   | $H(b, c, d)$ |     `b ⨁ c ⨁ d`      |
   |  4   | $I(b, c, d)$ |    `c ⨁ (b ⋁ ~d)`    |

   每轮循环的T表元素(16个)及消息分组部分(16个)关系如下表所示：

   | 轮次 | T表元素`T[]` | 第i次迭代使用的消息分组`X[k]`(j = i - 1) |
   | :--: | :----------: | :--------------------------------------: |
   |  1   |  T[1...16]   |                 `k = j`                  |
   |  2   |  T[17...32]  |          `k = (1 + 5j) mod 16`           |
   |  3   |  T[33...48]  |          `k = (5 + 3j) mod 16`           |
   |  4   |  T[49...64]  |             `k = 7j mod 16`              |

   T表生成函数：$T[i] = int(2^{32} * |sin(i)|)$

   - int 取整函数，sin 正弦函数，以 i 作为弧度输入

   各轮各次迭代运算(1...64)采用的左循环移位的位数s值：

   - `s[ 1…16] = { 7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22 }`
   - `s[17…32] = { 5, 9, 14, 20, 5, 9, 14, 20, 5, 9, 14, 20, 5, 9, 14, 20 }`
   - `s[33…48] = { 4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23 }`
   - `s[49…64] = { 6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21 }`

   注：每次循环使用相同的迭代逻辑和4*16次运算的预设参数表

   **MD5压缩函数H~MD5~在每轮循环中的一次迭代运算逻辑**：

   1. 对A迭代：`a = b + ((a + g(b, c, d) + X[k] + T[i]) <<< s)` 

   2. 缓冲区`(A, B, C, D)`循环置换为`(B, C, D, A)`

      说明：

      - `a, b, c, d`为MD缓冲区(A, B, C, D)的各个寄存器的当前值
      - `g`：轮函数F，G，H，I中的一个
      - `<<<s` : 将32位输入循环左移 (CLS) s 位。（移位时不丢失移位前原范围的位，而是将它们作为另一端的补入位）
      - `X[k]` : 当前处理消息分组的第 k 个 (k = 0…15) 32位字。如果消息M按32-bit编址，即为M~q∗16+k~ 

      - `T[i] `: T 表的第 i 个元素，32位字；T 表总共有64个元素，也
        称为加法常数。
      - `+`: 模2^32^加法

   每轮循环中的一次迭代运算逻辑示意图：

   ![](/Users/zhaowenxu/Desktop/HMACMD5/lun.png)

#### 5. 得出结果

将经过4轮循环压缩运算后生成的a, b, c, d级联128位的散列值输出，得到最终结果。

## 二. HMAC算法原理概述

> HMAC(**keyed-hash message authentication code**)是一种基于Hash函数和密钥进行消息认证的方法，该算法就是通过提供一个Hash函数，一个key，一个字符串来计算出一个值。通信双方约定好key之后，双方各自使用这个算法交换算法结果来保证双方之前交互过的消息来自对方并且没有被篡改

**HMAC算法结构**

![](/Users/zhaowenxu/Desktop/HMACMD5/截屏2020-11-16 下午3.19.16.png)

| 参数 |             意义             |
| :--: | :--------------------------: |
|  M   |    Message input to HMAC     |
|  H   |    Embedded hash function    |
|  b   | Length (bits) of input block |
|  k   | The secret key, $|k|\leq b$  |
|  n   |     Length of hash code      |
| ipad |     00110110 重复 b/8 次     |
| opad |     01011010 重复 b/8 次     |

HMAC~k~ = H( (K^+^ ⨁ opad) || H( (K^+^ ⨁ ipad) || M ) ) 

- 对共享密钥k左边补0，生成一个b位的数据块K^+^
- K^+^与ipad作XOR，生成b位的S~i~
- 对(S~i~ || M)进行hash压缩（例如MD5），得到H(S~i~ || M)
- K^+^与opad作XOR，生成b位的S~o~a
- 对S~o~ || H(S~i~ || M)进行hash压缩（例如MD5），得到HMAC~k~ = H(S~o~ || H(S~i~ || M))

## 三. 总体结构设计

本次需要实现的是一个`HMAC-MD5`算法，即将MD5作为HMAC的hash函数。

**文件目录结构**：

```c
.
|____hmac.c				// 定义hmac算法用到的变量并实现相关函数
|____md5.h				// 定义md5算法相关的函数
|____hmac.h				// 定义hmac算法相关的函数
|____main.c				// 测试运行文件
|____md5.c				// 定义md5算法用到的变量并实现相关函数
```

## 四. 模块分解

根据本次实验的要求，大致分为两大模块**，MD5算法**以及**HMAC算法**

- MD5算法：

  - Part1:

    - 填充信息

      ```c
      void Padding(const unsigned char * origin_message, const unsigned long message_len);
      ```

    - 分块

      ```c
      MD5_GROUP Block(unsigned char * origin_message, unsigned long message_length);
      ```

  - Part2:

    - 初始化CV

      ```c
      void initCV(void);
      ```

    - 循环压缩

      ```c
      void MD5_Compress(const int * group, unsigned int * res);
      ```

    - int类型转换为char类型

      ```c
      void int_to_char(const unsigned int * arr, unsigned char * res, unsigned long len);
      ```

    - char类型转换为int类型

      ```c
      void char_to_int(const unsigned char * str, unsigned int * res, unsigned long len);
      ```

- HMAC算法：

  - 对共享密钥k左边补0，生成一个b位数据块K^+^

    ```c
    void generate_K_plus(void);
    ```

  - 生成S~i~(K^+^ ⨁ ipad)

    ```c
    void generate_Si(void);
    ```

  - 生成S~o~(K^+^ ⨁ opad)

    ```c
    void generate_So(void);
    ```

  - hash压缩（利用MD5）

    ```c
    MD5_GROUP Hash(unsigned char *S, unsigned long S_len, unsigned char *M, unsigned long M_len);
    ```

## 五. 数据结构设计

- MD5

  - 定义循环压缩函数及循环左移函数：

    ```
    // 循环压缩函数(轮函数)定义
    #define F(b, c, d) ((b & c) | (~b & d))        // 第一轮
    #define G(b, c, d) ((b & d) | (c & ~d))        // 第二轮
    #define H(b, c, d) (b ^ c ^ d)                 // 第三轮
    #define I(b, c, d) (c ^ (b | ~d))              // 第四轮
    
    // 循环左移函数
    #define CLS(x, s) ((x << s) | (x >> (32 - s)))
    ```

  - 定义MD5的分块结构体`MD5_GROUP`，512bit为一组，由4个32位寄存器构成

    ```c
    typedef struct {
        unsigned int element[4];
    } MD5_GROUP;
    ```

  - T表

    ```c
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
    ```

  - 各轮各次迭代运算(1...64)采用的左循环移位的位数s值

    ```c
    const int S_VALUE[] = {
        7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22,
        5,  9, 14, 20, 5,  9, 14, 20, 5,  9, 14, 20, 5,  9, 14, 20,
        4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23,
        6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21
    };
    ```

  - MD缓冲区初始化向量

    ```c
    const unsigned int IV[4] = {
        0x67452301,
        0xEFCDAB89,
        0x98BADCFE,
        0x10325476
    };
    ```

  - CV值

    ```c
    unsigned int CV[4];
    ```

- HMAC

  - 定义ipad与opad

    ```c
    #define ipad 0x36 // 00110110 重复 b/8 次
    #define opad 0x5c // 01011100 重复 b/8 次
    ```

  - 变量

    ```c
    unsigned char * M; 			// Message input to HMAC
    char * k; 							// The secret key, |k| <= b
    char * k_plus; 					// 对共享密钥k左边补0，生成一个b位的数据块K+
    unsigned long b = 0;    // Length (bits) of input block
    unsigned long key_len;  // length of key
    char * Si;							// 存储Si
    char * So;							// 存储So
    ```



## 六. C语言源代码

github仓库链接：[HMAC-MD5](https://github.com/zwx2000/HMAC-MD5)

## 七. 编译运行结果

HMAC-MD5输入信息：

```
HMAC-MD5
```

公钥：

```
key
```

==输出哈希值==（私钥）：

```
33db9f9da9320e7d402b5c5f451f25c6
```

![](/Users/zhaowenxu/Desktop/HMACMD5/截屏2020-11-18 下午9.25.36.png)

## 八. 验证用例

验证时采用网站[1024程序员开发工具箱](https://1024tools.com/hmac)进行在线验证

==验证结果==如下：

![](/Users/zhaowenxu/Desktop/HMACMD5/截屏2020-11-18 下午9.19.53.png)

经比对验证生成结果一致。
