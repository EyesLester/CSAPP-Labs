/* 
 * CS:APP Data Lab 
 * 
 * <Please put your name and userid here>
 * 
 * bits.c - Source file with your solutions to the Lab.
 *          This is the file you will hand in to your instructor.
 *
 * WARNING: Do not include the <stdio.h> header; it confuses the dlc
 * compiler. You can still use printf for debugging without including
 * <stdio.h>, although you might get a compiler warning. In general,
 * it's not good practice to ignore compiler warnings, but in this
 * case it's OK.  
 */

#if 0
/*
 * Instructions to Students:
 *
 * STEP 1: Read the following instructions carefully.
 */

You will provide your solution to the Data Lab by
editing the collection of functions in this source file.

INTEGER CODING RULES:
 
  Replace the "return" statement in each function with one
  or more lines of C code that implements the function. Your code 
  must conform to the following style:
 
  int Funct(arg1, arg2, ...) {
      /* brief description of how your implementation works */
      int var1 = Expr1;
      ...
      int varM = ExprM;

      varJ = ExprJ;
      ...
      varN = ExprN;
      return ExprR;
  }

  Each "Expr" is an expression using ONLY the following:
  1. Integer constants 0 through 255 (0xFF), inclusive. You are
      not allowed to use big constants such as 0xffffffff.
  2. Function arguments and local variables (no global variables).
  3. Unary integer operations ! ~
  4. Binary integer operations & ^ | + << >>
    
  Some of the problems restrict the set of allowed operators even further.
  Each "Expr" may consist of multiple operators. You are not restricted to
  one operator per line.

  You are expressly forbidden to:
  1. Use any control constructs such as if, do, while, for, switch, etc.
  2. Define or use any macros.
  3. Define any additional functions in this file.
  4. Call any functions.
  5. Use any other operations, such as &&, ||, -, or ?:
  6. Use any form of casting.
  7. Use any data type other than int.  This implies that you
     cannot use arrays, structs, or unions.

 
  You may assume that your machine:
  1. Uses 2s complement, 32-bit representations of integers.
  2. Performs right shifts arithmetically.
  3. Has unpredictable behavior when shifting if the shift amount
     is less than 0 or greater than 31.


EXAMPLES OF ACCEPTABLE CODING STYLE:
  /*
   * pow2plus1 - returns 2^x + 1, where 0 <= x <= 31
   */
  int pow2plus1(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     return (1 << x) + 1;
  }

  /*
   * pow2plus4 - returns 2^x + 4, where 0 <= x <= 31
   */
  int pow2plus4(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     int result = (1 << x);
     result += 4;
     return result;
  }

FLOATING POINT CODING RULES

For the problems that require you to implement floating-point operations,
the coding rules are less strict.  You are allowed to use looping and
conditional control.  You are allowed to use both ints and unsigneds.
You can use arbitrary integer and unsigned constants. You can use any arithmetic,
logical, or comparison operations on int or unsigned data.

You are expressly forbidden to:
  1. Define or use any macros.
  2. Define any additional functions in this file.
  3. Call any functions.
  4. Use any form of casting.
  5. Use any data type other than int or unsigned.  This means that you
     cannot use arrays, structs, or unions.
  6. Use any floating point data types, operations, or constants.


NOTES:
  1. Use the dlc (data lab checker) compiler (described in the handout) to 
     check the legality of your solutions.
  2. Each function has a maximum number of operations (integer, logical,
     or comparison) that you are allowed to use for your implementation
     of the function.  The max operator count is checked by dlc.
     Note that assignment ('=') is not counted; you may use as many of
     these as you want without penalty.
  3. Use the btest test harness to check your functions for correctness.
  4. Use the BDD checker to formally verify your functions
  5. The maximum number of ops for each function is given in the
     header comment for each function. If there are any inconsistencies 
     between the maximum ops in the writeup and in this file, consider
     this file the authoritative source.

/*
 * STEP 2: Modify the following functions according the coding rules.
 * 
 *   IMPORTANT. TO AVOID GRADING SURPRISES:
 *   1. Use the dlc compiler to check that your solutions conform
 *      to the coding rules.
 *   2. Use the BDD checker to formally verify that your solutions produce 
 *      the correct answers.
 */


#endif
//1
/* 
 * bitXor - x^y using only ~ and & 
 *   Example: bitXor(4, 5) = 1
 *   Legal ops: ~ &
 *   Max ops: 14
 *   Rating: 1
 */
int bitXor(int x, int y) {
  int same1 = x & y;
  int same0 = ~x & ~y;
  int same0ordiff = ~same1;
  int same1ordiff = ~same0;
  int diff = same0ordiff & same1ordiff;
  return diff;
}
/* 
 * tmin - return minimum two's complement integer 
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 4
 *   Rating: 1
 */
int tmin(void) {
  return 1 << 31;
}
//2
/*
 * isTmax - returns 1 if x is the maximum, two's complement number,
 *     and 0 otherwise 
 *   Legal ops: ! ~ & ^ | +
 *   Max ops: 10
 *   Rating: 1
 */
int isTmax(int x) {
  int tmin = 1 << 31;
  int tmax = ~tmin;
  int nottmax = x ^ tmax;
  int res = !nottmax;
  return res;
}
/* 
 * allOddBits - return 1 if all odd-numbered bits in word set to 1
 *   where bits are numbered from 0 (least significant) to 31 (most significant)
 *   Examples allOddBits(0xFFFFFFFD) = 0, allOddBits(0xAAAAAAAA) = 1
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 12
 *   Rating: 2
 */
int allOddBits(int x) {
  // 用奇数位为1偶数位为0的掩码与上x，再判断结果是否与掩码本身一样即可
  int allOdd1_0 = 0xAA;
  int allOdd1_1 = allOdd1_0 << 8;
  int allOdd1_2 = allOdd1_1 << 8;
  int allOdd1_3 = allOdd1_2 << 8;
  int allOdd1 = allOdd1_0 + allOdd1_1 + allOdd1_2 + allOdd1_3;
  int aftermask = x & allOdd1;
  int notres = aftermask + ~allOdd1 + 1;
  return !notres;
}
/* 
 * negate - return -x 
 *   Example: negate(1) = -1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 5
 *   Rating: 2
 */
int negate(int x) {
  return ~x + 1;
}
//3
/* 
 * isAsciiDigit - return 1 if 0x30 <= x <= 0x39 (ASCII codes for characters '0' to '9')
 *   Example: isAsciiDigit(0x35) = 1.
 *            isAsciiDigit(0x3a) = 0.
 *            isAsciiDigit(0x05) = 0.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 15
 *   Rating: 3
 */
int isAsciiDigit(int x) {
  // 判断减最小值和被最大值减的正负号是否为非负
  int tmp1 = x + ~0x30 + 1;
  int tmp2 = 0x39 + ~x + 1;
  int res1 = tmp1 >> 31;
  int res2 = tmp2 >> 31;
  int notres = res1|res2;
  return !notres;
}
/* 
 * conditional - same as x ? y : z 
 *   Example: conditional(2,4,5) = 4
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 16
 *   Rating: 3
 */
int conditional(int x, int y, int z) {
  // 判断条件x由0/1转为-1/0，恰好-1的补码形式是全为1，再掩码即可
  int notx = !x;
  int mask = notx + ~1 + 1;
  int resy = y & mask;
  int resz = z & ~mask;
  int res = resy | resz;
  return res;
}
/* 
 * isLessOrEqual - if x <= y  then return 1, else return 0 
 *   Example: isLessOrEqual(4,5) = 1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 24
 *   Rating: 3
 */
int isLessOrEqual(int x, int y) {
  // 有符号数的位移是算术位移！！！！！！！！！
  // 正负符号相同的情况下相减判断正负，不同的情况下直接判断正负
  int res;
  int diffSign = (x ^ y) >> 31;
  // 符号不同时，diffSign==1，x的符号为0则x>y，即return 0，x的符号为1则x<y，即return 1
  res = diffSign & !!(x >> 31);
  // 符号相同时，diffSign==0，y-x符号位为0则y>=x，即return 1，y-x符号位为1则y<x，即return 0，
  int yMinusx = y + ~x + 1;
  int flag = !(yMinusx >> 31);
  res = res | ((!diffSign) & flag);
  return res;
}
//4
/* 
 * logicalNeg - implement the ! operator, using all of 
 *              the legal operators except !
 *   Examples: logicalNeg(3) = 0, logicalNeg(0) = 1
 *   Legal ops: ~ & ^ | + << >>
 *   Max ops: 12  
 *   Rating: 4 
 */
int logicalNeg(int x) {
  // 取反后仅0变为-1（位全为1），再二分法按位与
  // 最佳方法：利用0以及最小值补码的相反数为自身，符号位相或，仅0的结果为0
  x = ~x;
  int xlow16 = x >> 16;
  int x16 = x & xlow16;
  int xlow8 = x16 >> 8;
  int x8 = x16 & xlow8;
  int xlow4 = x8 >> 4;
  int x4 = x8 & xlow4;
  int xlow2 = x4 >> 2;
  int x2 = x4 & xlow2;
  int xlow1 = x2 >> 1;
  int x1 = x2 & xlow1;
  int res = x1 & 0x1;
  return res;
}
/* howManyBits - return the minimum number of bits required to represent x in
 *             two's complement
 *  Examples: howManyBits(12) = 5
 *            howManyBits(298) = 10
 *            howManyBits(-5) = 4
 *            howManyBits(0)  = 1
 *            howManyBits(-1) = 1
 *            howManyBits(0x80000000) = 32
 *  Legal ops: ! ~ & ^ | + << >>
 *  Max ops: 90
 *  Rating: 4
 */
int howManyBits(int x) {//共77ops
/* 题目分析
  1. 正数的最高有效位数+1即为结果（有效位范围1~32）
  2. 0的结果是1
  3. 负数一般来说可以先转换成其相反数，再求得最高有效位+1即为结果
  4. 但绝对值恰好为2的幂次的负数，结果应正好为相反数的最高有效位，不需要+1
*/
/* 解决思路
  1. 先把负数x转成正数，非负数x不变，得到|x|(注意int的最小值min仍是负数)
  2. 二分法寻找|x|的最高有效位，具体来说，查看高n/2位是否全为0，再决定继续查找高n/2位或低n/2位，不全为0时res += n/2
  3. 二分法剩余1位时，res直接加上它（当x==0时，res此时恰好等于0）
  4. x符号位为0时，res += 1（当x==0时，res此时恰好等于1）
  5. x符号位为1时，如果|x| << (32-res)为min，说明x恰好为2的幂次的负数，res不变，否则res += 1
*/
  // 初始准备[13ops]
  int res = 0;
  int min = 1 << 31;
  int sflag = x & min; // 符号位
  int smask = sflag >> 31; 
  int xabs = (x & ~smask) + ((~x + 1) & smask); // 对符号位为1的负数求其“相反数”（注意一定范围的最小值的“相反数”仍为负数，需要特殊考虑）
  int t = xabs;// t为迭代中的01串
  int maskhigh16 = (0xff << 8) + 0xff;
  int maskhigh8 = 0xff;
  int maskhigh4 = 0xf;
  int maskhigh2 = 0x3;
  int maskhigh1 = 0x1;
  int minus1 = (~1)+1;
  //判断剩余串的高16位是否为0[11ops]
  int high16 = (t >> 16) & maskhigh16;// 仅high16需要掩码删除潜在的前导1，其他的highx有效位前全为0
  int low16 = t & maskhigh16;
  int flag16 = (!high16) + minus1;
  res = res + (16 & flag16);
  t = (low16 & ~flag16) + (high16 & flag16);
  // 判断剩余串的高8位是否为0[10ops]
  int high8 = t >> 8;
  int low8 = t & maskhigh8;
  int flag8 = (!high8) + minus1;
  res = res + (8 & flag8);
  t = (low8 & ~flag8) + (high8 & flag8);
  // 判断剩余串的高4位是否为0[10ops]
  int high4 = t >> 4;
  int low4 = t & maskhigh4;
  int flag4 = (!high4) + minus1;
  res = res + (4 & flag4);
  t = (low4 & ~flag4) + (high4 & flag4);
  // 判断剩余串的高2位是否为0[10ops]
  int high2 = t >> 2;
  int low2 = t & maskhigh2;
  int flag2 = (!high2) + minus1;
  res = res + (2 & flag2);
  t = (low2 & ~flag2) + (high2 & flag2);
  // 判断剩余串的高1位是否为0[10ops]
  int high1 = t >> 1;
  int low1 = t & maskhigh1;
  int flag1 = (!high1) + minus1;
  res = res + (1 & flag1);
  t = (low1 & ~flag1) + (high1 & flag1);
  // 加上剩余位[1ops]
  res = res + t;
  // 加上补码符号位（除了-pow(2,n)以外）[12ops]
  int special1 = !!((x << (32 + ~res + 1)) ^ min);
  res = res + (special1 & smask) + (1 & ~smask);
  return res;
} 
//float
/* 
 * floatScale2 - Return bit-level equivalent of expression 2*f for
 *   floating point argument f.
 *   Both the argument and result are passed as unsigned int's, but
 *   they are to be interpreted as the bit-level representation of
 *   single-precision floating point values.
 *   When argument is NaN, return argument
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Rating: 4
 */
unsigned floatScale2(unsigned uf) {
  return 2;
}
/* 
 * floatFloat2Int - Return bit-level equivalent of expression (int) f
 *   for floating point argument f.
 *   Argument is passed as unsigned int, but
 *   it is to be interpreted as the bit-level representation of a
 *   single-precision floating point value.
 *   Anything out of range (including NaN and infinity) should return
 *   0x80000000u.
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Rating: 4
 */
int floatFloat2Int(unsigned uf) {
  return 2;
}
/* 
 * floatPower2 - Return bit-level equivalent of the expression 2.0^x
 *   (2.0 raised to the power x) for any 32-bit integer x.
 *
 *   The unsigned value that is returned should have the identical bit
 *   representation as the single-precision floating-point number 2.0^x.
 *   If the result is too small to be represented as a denorm, return
 *   0. If too large, return +INF.
 * 
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. Also if, while 
 *   Max ops: 30 
 *   Rating: 4
 */
unsigned floatPower2(int x) {
    return 2;
}
