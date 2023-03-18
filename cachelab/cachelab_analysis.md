## Part A

命令行参数由Command name，Option，Option argument 以及Operands组成。
Command name是程序的名称。操作对象Operands又称Non-option argument。

没什么难点，分支循环写就完了



## Part B

利用分块矩阵乘法的思想进行分块转置，从而充分利用Spatial Locality

参数是s = 5, E = 1, b = 5，则有2^5^=32个的cache set，对应的每个cache line的block包含2^5^=32个字节

相当于每个cacheline可以装下8个int型

输入样例如下

- 32 × 32: 8 points if m < 300, 0 points if m > 600 

- 64 × 64: 8 points if m < 1, 300, 0 points if m > 2, 000 

- 61 × 67: 10 points if m < 2, 000, 0 points if m > 3, 000

最后还是去看了大神的思路，总结就是要解决同一分块中的set号冲突以及AB分块对应位置的set号冲突

32 × 32的直接用8 × 8的分块就过了，miss数287

64 × 64的用8 × 8的分块，在每个分块中先把A的上半部分**转置移动**到B的上半部分（**竖的**），然后把A的左下部分**转置移动**到B的右上部分（**横的**），同时把B的右上部分**移动**到B的左下部分（**横的**），最后把A的右下部分**转置移动**到B的右下部分（**横的**），最后miss数1179（还可以对对角块进行进一步优化）

61 × 67的暴力试出用17 × 4的分块过了，miss数1848
