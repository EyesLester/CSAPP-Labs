## 一些符号表说明

| 符号名            | 内存位置 | 含义             |
| ----------------- | -------- | ---------------- |
| num_input_strings | 0x603760 | 输入字符串的个数 |
| input_strings     | 0x603780 | 输入的字符串     |



## main

每个phase前调用read_line读取一行字符串，存到`%rdi`里（由strings_not_equal函数推测得出），再调用phase的函数



## string_length函数

```
%rdx = %rdi

do

%rdx = %rdx + 1 // %rdx所存地址+1
%eax = %edx - %edi// %rdx当前所存地址 - %rdi所存的字符串起始地址

while (%rdx) != 0 // %rdx所存地址指向的字符为非0
```



## strings_not_equal函数

给定两个字符串的起始地址`%rdi`和`%rsi`，当它们不同时返回`%rax=1`，否则返回`%rax=0`



## phase_1

给`%esi`赋值为0x402400，然后在`strings_not_equal`中比较`%rdi`和`%rsi`，结果为0（不相等）时不爆炸。

```
Breakpoint 1, 0x0000000000400ee4 in phase_1 ()
(gdb) print (char *) 0x402400
$1 = 0x402400 "Border relations with Canada have never been better."
```

所以phase_1的密码是**Border relations with Canada have never been better.**



## phase_2

```assembly
0000000000400efc <phase_2>:
  400efc:	55                   	push   %rbp
  400efd:	53                   	push   %rbx
  400efe:	48 83 ec 28          	sub    $0x28,%rsp	# 栈扩大40字节
  400f02:	48 89 e6             	mov    %rsp,%rsi	# 栈顶指针存入%rsi(值设为a), %rsi = a
  400f05:	e8 52 05 00 00       	call   40145c <read_six_numbers> # 读取六个数字
  400f0a:	83 3c 24 01          	cmpl   $0x1,(%rsp) 
  400f0e:	74 20                	je     400f30 <phase_2+0x34>
  400f10:	e8 25 05 00 00       	call   40143a <explode_bomb> # if M(a) != 1 then 爆炸
  400f15:	eb 19                	jmp    400f30 <phase_2+0x34>
  400f17:	8b 43 fc             	mov    -0x4(%rbx),%eax	# %eax = M(%rbx - 4)
  400f1a:	01 c0                	add    %eax,%eax		# %eax = 2*M(%rbx - 4)
  400f1c:	39 03                	cmp    %eax,(%rbx)		# M(%rbx) - 2*M(%rbx - 4)
  400f1e:	74 05                	je     400f25 <phase_2+0x29>
  400f20:	e8 15 05 00 00       	call   40143a <explode_bomb> # if not 0 then 爆炸
  400f25:	48 83 c3 04          	add    $0x4,%rbx # %rbx += 4
  400f29:	48 39 eb             	cmp    %rbp,%rbx
  400f2c:	75 e9                	jne    400f17 <phase_2+0x1b>
  400f2e:	eb 0c                	jmp    400f3c <phase_2+0x40> # if %rbx == %rbp 结束
  400f30:	48 8d 5c 24 04       	lea    0x4(%rsp),%rbx	# %rbx = a + 4
  400f35:	48 8d 6c 24 18       	lea    0x18(%rsp),%rbp	# %rbp = a + 24
  400f3a:	eb db                	jmp    400f17 <phase_2+0x1b>
  400f3c:	48 83 c4 28          	add    $0x28,%rsp # 栈缩小40字节
  400f40:	5b                   	pop    %rbx
  400f41:	5d                   	pop    %rbp
  400f42:	c3                   	ret    
```

它的逻辑可以转成如下伪代码

```pseudocode
const a = rsp
if M(a) == 1 then explode_bomb()
rbx = a + 4
rbp = a + 24
do {
	eax = rbx - 4
	if (M(rbx) - 2*M(rbx - 4)) != 0 then explode_bomb()
	rbx += 4
} while rbx != rbp
```

即密码为六个整型数字，第一个为1，后一个是前一个的两倍

所以phase_2的密码是**1 2 4 8 16 32**



### 附read_six_numbers

```assembly
000000000040145c <read_six_numbers>:
  40145c:	48 83 ec 18          	sub    $0x18,%rsp		# 栈扩大24字节
  401460:	48 89 f2             	mov    %rsi,%rdx		# %rdx = a
  401463:	48 8d 4e 04          	lea    0x4(%rsi),%rcx	# %rcx = a + 4
  401467:	48 8d 46 14          	lea    0x14(%rsi),%rax	# %rax = a + 20
  40146b:	48 89 44 24 08       	mov    %rax,0x8(%rsp)	# M(%rsp+8) = a + 20
  401470:	48 8d 46 10          	lea    0x10(%rsi),%rax	# %rax = a + 16
  401474:	48 89 04 24          	mov    %rax,(%rsp)		# M(%rsp) = a + 16
  401478:	4c 8d 4e 0c          	lea    0xc(%rsi),%r9	# %r9 = a + 12
  40147c:	4c 8d 46 08          	lea    0x8(%rsi),%r8	# %r8 = a + 8
  401480:	be c3 25 40 00       	mov    $0x4025c3,%esi	# M($0x4025c3)=="%d %d %d %d %d %d"
  401485:	b8 00 00 00 00       	mov    $0x0,%eax
  40148a:	e8 61 f7 ff ff       	call   400bf0 <__isoc99_sscanf@plt> # 返回成功赋值的字段个数
  40148f:	83 f8 05             	cmp    $0x5,%eax
  401492:	7f 05                	jg     401499 <read_six_numbers+0x3d> # 读取到六个数字则不爆炸
  401494:	e8 a1 ff ff ff       	call   40143a <explode_bomb>
  401499:	48 83 c4 18          	add    $0x18,%rsp		# 栈缩小24字节，
  40149d:	c3                   	ret    
```

除了`%rsi`和`%rdi`，read_six_numbers向sscanf函数传入了以下六个参数：

M(%rsp+8) = a + 20

M(%rsp) = a + 16

%r9 = a + 12

%r8 = a + 8

%rcx = a + 4

%rdx = a

应该是sscanf解析得到的6个int型整数结果的地址。

所以read_six_numbers的作用是输入参数`%rdi=输入字符串地址`和`%rsi=a`，从字符串解析得到6个int型整数，结果存到内存地址为a到a+20的位置。



## phase_3

```assembly
0000000000400f43 <phase_3>:
  400f43:	48 83 ec 18          	sub    $0x18,%rsp
  400f47:	48 8d 4c 24 0c       	lea    0xc(%rsp),%rcx	# %rcx = %rsp + 12 该地址的内容设为a2
  400f4c:	48 8d 54 24 08       	lea    0x8(%rsp),%rdx	# %rdx = %rsp + 8 该地址的内容设为a1
  400f51:	be cf 25 40 00       	mov    $0x4025cf,%esi	# M(0x4025cf) == "%d %d"
  400f56:	b8 00 00 00 00       	mov    $0x0,%eax
  400f5b:	e8 90 fc ff ff       	call   400bf0 <__isoc99_sscanf@plt>
  400f60:	83 f8 01             	cmp    $0x1,%eax
  400f63:	7f 05                	jg     400f6a <phase_3+0x27> # sscanf解析得到不够2个数字则爆炸
  400f65:	e8 d0 04 00 00       	call   40143a <explode_bomb> # 不爆炸则%eax = 2
  400f6a:	83 7c 24 08 07       	cmpl   $0x7,0x8(%rsp)	# M(%rsp + 8) - 7
  400f6f:	77 3c                	ja     400fad <phase_3+0x6a> # if a1 > 7 then 爆炸（无符号数）
  400f71:	8b 44 24 08          	mov    0x8(%rsp),%eax	# %eax = a1
  400f75:	ff 24 c5 70 24 40 00 	jmp    *0x402470(,%rax,8) # (a1 * 8 + 0x402470)
  400f7c:	b8 cf 00 00 00       	mov    $0xcf,%eax				# %eax = 207
  400f81:	eb 3b                	jmp    400fbe <phase_3+0x7b>
  400f83:	b8 c3 02 00 00       	mov    $0x2c3,%eax				# %eax = 707
  400f88:	eb 34                	jmp    400fbe <phase_3+0x7b>
  400f8a:	b8 00 01 00 00       	mov    $0x100,%eax				# %eax = 256
  400f8f:	eb 2d                	jmp    400fbe <phase_3+0x7b>
  400f91:	b8 85 01 00 00       	mov    $0x185,%eax				# %eax = 389
  400f96:	eb 26                	jmp    400fbe <phase_3+0x7b>
  400f98:	b8 ce 00 00 00       	mov    $0xce,%eax				# %eax = 206
  400f9d:	eb 1f                	jmp    400fbe <phase_3+0x7b>
  400f9f:	b8 aa 02 00 00       	mov    $0x2aa,%eax				# %eax = 682
  400fa4:	eb 18                	jmp    400fbe <phase_3+0x7b>
  400fa6:	b8 47 01 00 00       	mov    $0x147,%eax				# %eax = 327
  400fab:	eb 11                	jmp    400fbe <phase_3+0x7b>
  400fad:	e8 88 04 00 00       	call   40143a <explode_bomb>
  400fb2:	b8 00 00 00 00       	mov    $0x0,%eax
  400fb7:	eb 05                	jmp    400fbe <phase_3+0x7b>
  400fb9:	b8 37 01 00 00       	mov    $0x137,%eax				# %eax = 311
  400fbe:	3b 44 24 0c          	cmp    0xc(%rsp),%eax			# %eax - M(%rsp + 12)
  400fc2:	74 05                	je     400fc9 <phase_3+0x86>	# %eax == a2则结束
  400fc4:	e8 71 04 00 00       	call   40143a <explode_bomb>
  400fc9:	48 83 c4 18          	add    $0x18,%rsp
  400fcd:	c3                   	ret    
```

通过sscanf解析到两个整型数字a1,a2存到`M(%rsp + 8)`和`M(%rsp + 12)`

为了不爆炸，首先要保证 0 <= a1 < = 7，然后程序会跳转到`M(a1 * 8 + 0x402470)`的位置，然后跳转到`0x400fbe`判定

通过gdb检查得到：

|  a1  |    0     |    1     |    2     |    3     |    4     |    5     |    6     |    7     |
| :--: | :------: | :------: | :------: | :------: | :------: | :------: | :------: | :------: |
| 地址 | 0x402470 | 0x402478 | 0x402480 | 0x402488 | 0x402490 | 0x402498 | 0x4024a0 | 0x4024a8 |
| 内容 | 0x400f7c | 0x400fb9 | 0x400f83 | 0x400f8a | 0x400f91 | 0x400f98 | 0x400f9f | 0x400fa6 |
| %eax |   207    |   311    |   707    |   256    |   389    |   206    |   682    |   327    |

当`%eax` == a2时结束，所以上面任意一对都是正确的密码，如 **0 207** 



## phase_4

```assembly
000000000040100c <phase_4>:
  40100c:	48 83 ec 18          	sub    $0x18,%rsp
  401010:	48 8d 4c 24 0c       	lea    0xc(%rsp),%rcx	# %rcx = %rsp + 12 该地址的内容设为a2
  401015:	48 8d 54 24 08       	lea    0x8(%rsp),%rdx	# %rdx = %rsp + 8 该地址的内容设为a1
  40101a:	be cf 25 40 00       	mov    $0x4025cf,%esi	# M(0x4025cf) == "%d %d"
  40101f:	b8 00 00 00 00       	mov    $0x0,%eax
  401024:	e8 c7 fb ff ff       	call   400bf0 <__isoc99_sscanf@plt>
  401029:	83 f8 02             	cmp    $0x2,%eax
  40102c:	75 07                	jne    401035 <phase_4+0x29>	# sscanf没解析到两个数字就爆炸
  40102e:	83 7c 24 08 0e       	cmpl   $0xe,0x8(%rsp)			# a1 - 14(无符号数)
  401033:	76 05                	jbe    40103a <phase_4+0x2e>	# a1 <= 14时不爆炸
  401035:	e8 00 04 00 00       	call   40143a <explode_bomb>
  40103a:	ba 0e 00 00 00       	mov    $0xe,%edx				# %edx = 14
  40103f:	be 00 00 00 00       	mov    $0x0,%esi				# %esi = 0
  401044:	8b 7c 24 08          	mov    0x8(%rsp),%edi			# %edi = a1
  401048:	e8 81 ff ff ff       	call   400fce <func4>			# 传上面三个参数给func4
  40104d:	85 c0                	test   %eax,%eax
  40104f:	75 07                	jne    401058 <phase_4+0x4c>	# %eax不是0就爆炸
  401051:	83 7c 24 0c 00       	cmpl   $0x0,0xc(%rsp)			# a2 - 0(无符号数)
  401056:	74 05                	je     40105d <phase_4+0x51>	# a2 == 0则结束
  401058:	e8 dd 03 00 00       	call   40143a <explode_bomb>
  40105d:	48 83 c4 18          	add    $0x18,%rsp
  401061:	c3                   	ret    
```

分析得知要让`%eax`等于0，且a2等于0时结束

由func4得知当a1为7时`%eax`等于0

所以phase_4的密码是 **7 0**



### 附func4

```assembly
0000000000400fce <func4>:
  400fce:	48 83 ec 08          	sub    $0x8,%rsp
  400fd2:	89 d0                	mov    %edx,%eax			# %eax = %edx
  400fd4:	29 f0                	sub    %esi,%eax			# %eax = %edx - %esi
  400fd6:	89 c1                	mov    %eax,%ecx			# %ecx = %edx - %esi
  400fd8:	c1 e9 1f             	shr    $0x1f,%ecx			# %ecx = (%edx - %esi) >> 31
  400fdb:	01 c8                	add    %ecx,%eax			# %eax = %edx - %esi + 																					((%edx - %esi) >> 31)
  400fdd:	d1 f8                	sar    %eax					# %eax = (%edx - %esi + 																			((%edx - %esi) >> 31))>>1
  400fdf:	8d 0c 30             	lea    (%rax,%rsi,1),%ecx	# %ecx = %rax + %rsi
  400fe2:	39 f9                	cmp    %edi,%ecx			# %ecx - a1
  400fe4:	7e 0c                	jle    400ff2 <func4+0x24>	# %ecx <= a1 则跳转到400ff2
  400fe6:	8d 51 ff             	lea    -0x1(%rcx),%edx
  400fe9:	e8 e0 ff ff ff       	call   400fce <func4>
  400fee:	01 c0                	add    %eax,%eax
  400ff0:	eb 15                	jmp    401007 <func4+0x39>	
  400ff2:	b8 00 00 00 00       	mov    $0x0,%eax			# %eax = 0
  400ff7:	39 f9                	cmp    %edi,%ecx			# %ecx - a1
  400ff9:	7d 0c                	jge    401007 <func4+0x39>	# %ecx >= a1 则结束
  400ffb:	8d 71 01             	lea    0x1(%rcx),%esi
  400ffe:	e8 cb ff ff ff       	call   400fce <func4>
  401003:	8d 44 00 01          	lea    0x1(%rax,%rax,1),%eax
  401007:	48 83 c4 08          	add    $0x8,%rsp
  40100b:	c3                   	ret    
```

它的逻辑可以转成如下伪代码

```pseudocode
func4(const edi = a1, esi = 0, edx = 14){
    eax = (edx - esi + ((edx - esi) >> 31)) >> 1
    ecx = rax + rsi
    if (ecx <= a1) then {
    	eax = 0
    	if (ecx >= a1) then return;
    	else {......}//不用管
    } else {
    ......//不用管
    }
}
```

带入得到`%eax` = 7，`%ecx` = 7，当a1 == `%ecx` == 7时，func4安全结束，且返回值`%eax`为0 



## phase_5

`%fs:0x28`作为stack guard，如果它没有被改变，则认为栈没有溢出。

movzbl指令把目标操作数的低8位用源操作数的的低8位替代，高24位用0替代。

movb指令把目标操作数的低8位用源操作数的的低8位替代。

```assembly
0000000000401062 <phase_5>:
  401062:	53                   	push   %rbx
  401063:	48 83 ec 20          	sub    $0x20,%rsp
  401067:	48 89 fb             	mov    %rdi,%rbx				# %rbx = 字符串地址%rdi
  40106a:	64 48 8b 04 25 28 00 	mov    %fs:0x28,%rax			# %rax = %fs:0x28
  401071:	00 00 
  401073:	48 89 44 24 18       	mov    %rax,0x18(%rsp)			# M(%rsp + 24) = %fs:0x28
  401078:	31 c0                	xor    %eax,%eax				# %eax = 0
  
  40107a:	e8 9c 02 00 00       	call   40131b <string_length>	# 得到字符串长度
  40107f:	83 f8 06             	cmp    $0x6,%eax				
  401082:	74 4e                	je     4010d2 <phase_5+0x70>	# 字符串长度为6时跳转到4010d2
  401084:	e8 b1 03 00 00       	call   40143a <explode_bomb>	# 字符串长度不为6就炸
  401089:	eb 47                	jmp    4010d2 <phase_5+0x70>
  
  40108b:	0f b6 0c 03          	movzbl (%rbx,%rax,1),%ecx # %ecx = M(%rbx + %rax)movzbl
  40108f:	88 0c 24             	mov    %cl,(%rsp)				# M(%rsp) = %cl
  401092:	48 8b 14 24          	mov    (%rsp),%rdx				# %rdx = M(%rsp)
  401096:	83 e2 0f             	and    $0xf,%edx				# %edx &= 0xf
  401099:	0f b6 92 b0 24 40 00 	movzbl 0x4024b0(%rdx),%edx # %edx = M(0x4024b0 + %rdx)movzbl
  4010a0:	88 54 04 10          	mov    %dl,0x10(%rsp,%rax,1)	# M(%rsp+%rax+16) = %dl
  4010a4:	48 83 c0 01          	add    $0x1,%rax				# %rax += 1
  4010a8:	48 83 f8 06          	cmp    $0x6,%rax
  4010ac:	75 dd                	jne    40108b <phase_5+0x29>	# 如果%rax!=6，则跳转到40108b
  4010ae:	c6 44 24 16 00       	movb   $0x0,0x16(%rsp)			# M(%rsp+22)低8位 = 0
  4010b3:	be 5e 24 40 00       	mov    $0x40245e,%esi			# %esi = 0x40245e
  4010b8:	48 8d 7c 24 10       	lea    0x10(%rsp),%rdi			# %rdi = %rsp + 16
  4010bd:	e8 76 02 00 00       	call   401338 <strings_not_equal> # 比较%rdi和%rsi指向的字符串
  4010c2:	85 c0                	test   %eax,%eax
  4010c4:	74 13                	je     4010d9 <phase_5+0x77>	# 字符串相同跳转到4010d9
  4010c6:	e8 6f 03 00 00       	call   40143a <explode_bomb>	# 字符串不同就爆炸
  4010cb:	0f 1f 44 00 00       	nopl   0x0(%rax,%rax,1)
  4010d0:	eb 07                	jmp    4010d9 <phase_5+0x77>
 
  4010d2:	b8 00 00 00 00       	mov    $0x0,%eax				# %eax = 0
  4010d7:	eb b2                	jmp    40108b <phase_5+0x29>	# 跳转到40108b
  
  4010d9:	48 8b 44 24 18       	mov    0x18(%rsp),%rax			# %rax = M(%rsp + 24)
  4010de:	64 48 33 04 25 28 00 	xor    %fs:0x28,%rax			# 比较%rax和%fs:0x28
  4010e5:	00 00 
  4010e7:	74 05                	je     4010ee <phase_5+0x8c>	# 相等则结束
  4010e9:	e8 42 fa ff ff       	call   400b30 <__stack_chk_fail@plt> # 不相等则报错栈溢出
  4010ee:	48 83 c4 20          	add    $0x20,%rsp
  4010f2:	5b                   	pop    %rbx
  4010f3:	c3                   	ret    
```

它的逻辑可以转成如下伪代码（忽略栈溢出判断过程）

```pseudocode
rdi = 输入字符串起始地址a
rbx = rdi
rax = string_length(rdi)
if eax != 6 then explode_bomb()
eax = 0
do {
    ecx = M(rbx + rax) // 替换低8位，高24位补零
    M(rsp) = el // el是ecx的低八位，即M(rbx + rax)的低8位
    rdx = M(rsp) // M(rbx + rax)的64位，无用？
    edx &= 0xf // 无用？
    edx = M(0x4024b0 + rdx) // 替换低8位，高24位补零
    M(rsp + rax + 16) = dl
    rax += 1
} while rax!=6
M(rsp + 22) = 0 // 替换低8位
esi = 0x40245e // M(0x40245e) == "flyers"
rdi = rsp + 16
if string(rdi) != string(rsi) then explode_bomb()
return
```

循环体将被执行6次，rax是循环计数器

第t次循环的作用：

```pseudocode
M(rsp + t + 16) = M(0x4024b0 + M(a + t)的低8位)的低8位
```

M(rsp + t + 16) = M(0x4024b0 + 输入字符串的第t个字符的ascii码)

gdb查询得到：

​	M(0x40245e) == "flyers"

​	M(0x4024b0) = "maduiersnfotvbylSo you think you can stop the bomb with ctrl-c, do you?"

| 序号 | 字符 | 相对0x4024b0的位移量（输入字符的8进制ASCII码） |
| :--: | :--: | :--------------------------------------------: |
|  0   |  f   |                    9(\011)                     |
|  1   |  l   |                    15(\017)                    |
|  2   |  y   |                    14(\016)                    |
|  3   |  e   |                     5(\05)                     |
|  4   |  r   |                     6(\06)                     |
|  5   |  s   |                     7(\07)                     |

所以phase_5的密码是ASCII字符序列 **9 15 14 5 6 7**，对应C语言转义字符序列 **\011\017\016\05\06\07**



## phase_6

先画出代码块的状态转移图再逐块分析

```assembly
00000000004010f4 <phase_6>:
  4010f4:	41 56                	push   %r14
  4010f6:	41 55                	push   %r13
  4010f8:	41 54                	push   %r12
  4010fa:	55                   	push   %rbp
  4010fb:	53                   	push   %rbx
  4010fc:	48 83 ec 50          	sub    $0x50,%rsp
  401100:	49 89 e5             	mov    %rsp,%r13		# %r13 = %rsp
  401103:	48 89 e6             	mov    %rsp,%rsi		# %rsi = %rsp
  401106:	e8 51 03 00 00       	call   40145c <read_six_numbers> # 解析六个数字存到%rsp~%rsp+20
  40110b:	49 89 e6             	mov    %rsp,%r14		# %r14 = 第一个数字的地址
  40110e:	41 bc 00 00 00 00    	mov    $0x0,%r12d		# %r12d(低32位) = 0

.L1:
  401114:	4c 89 ed             	mov    %r13,%rbp		# %rbp = %r13
  401117:	41 8b 45 00          	mov    0x0(%r13),%eax	# %eax = M(%r13)
  40111b:	83 e8 01             	sub    $0x1,%eax		# %eax -= 1
  40111e:	83 f8 05             	cmp    $0x5,%eax
  401121:	76 05                	jbe    401128 <phase_6+0x34>	# 如果0 <= %eax <= 5则不爆炸
  401123:	e8 12 03 00 00       	call   40143a <explode_bomb>	
  401128:	41 83 c4 01          	add    $0x1,%r12d		# %r12d += 1
  40112c:	41 83 fc 06          	cmp    $0x6,%r12d
  401130:	74 21                	je     401153 <phase_6+0x5f> # 如果 %r12d == 6 则跳转到.L3
  401132:	44 89 e3             	mov    %r12d,%ebx		# %ebx = %r12d
.L2
  401135:	48 63 c3             	movslq %ebx,%rax		# %rax = %ebx 高位补0
  401138:	8b 04 84             	mov    (%rsp,%rax,4),%eax	# %eax = M(%rsp + 4*%rax)
  40113b:	39 45 00             	cmp    %eax,0x0(%rbp)
  40113e:	75 05                	jne    401145 <phase_6+0x51> # 如果M(%rbp) != %eax则不爆炸
  401140:	e8 f5 02 00 00       	call   40143a <explode_bomb>
  401145:	83 c3 01             	add    $0x1,%ebx		# %ebx += 1
  401148:	83 fb 05             	cmp    $0x5,%ebx
  40114b:	7e e8                	jle    401135 <phase_6+0x41> # 如果%ebx <= 5则跳转.L2
  40114d:	49 83 c5 04          	add    $0x4,%r13		# %r13 += 4
  401151:	eb c1                	jmp    401114 <phase_6+0x20> # 跳转.L1

.L3:
  401153:	48 8d 74 24 18       	lea    0x18(%rsp),%rsi	# %rsi = %rsp + 24
  401158:	4c 89 f0             	mov    %r14,%rax		# %rax = %r14
  40115b:	b9 07 00 00 00       	mov    $0x7,%ecx		# %ecx = 7
.L4
  401160:	89 ca                	mov    %ecx,%edx		# %edx = %ecx
  401162:	2b 10                	sub    (%rax),%edx		# %edx -= M(%rax)
  401164:	89 10                	mov    %edx,(%rax)		# M(%rax) = %edx
  401166:	48 83 c0 04          	add    $0x4,%rax		# %rax += 4
  40116a:	48 39 f0             	cmp    %rsi,%rax
  40116d:	75 f1                	jne    401160 <phase_6+0x6c> # 如果%rax != %rsi则跳转.L4
  40116f:	be 00 00 00 00       	mov    $0x0,%esi		# %esi = 0
  401174:	eb 21                	jmp    401197 <phase_6+0xa3> # 跳转到.L7
.L5  
  401176:	48 8b 52 08          	mov    0x8(%rdx),%rdx	# %rdx = M(%rdx + 8)
  40117a:	83 c0 01             	add    $0x1,%eax		# %eax += 1
  40117d:	39 c8                	cmp    %ecx,%eax
  40117f:	75 f5                	jne    401176 <phase_6+0x82> # 如果%eax != %ecx跳转到.L5
  401181:	eb 05                	jmp    401188 <phase_6+0x94> # 跳转到.L6
.L6_0  
  401183:	ba d0 32 60 00       	mov    $0x6032d0,%edx	# %edx =  0x6032d0
.L6
  401188:	48 89 54 74 20       	mov    %rdx,0x20(%rsp,%rsi,2) # M(%rsp+2*%rsi+32) = %rdx
  40118d:	48 83 c6 04          	add    $0x4,%rsi			# %rsi += 4
  401191:	48 83 fe 18          	cmp    $0x18,%rsi
  401195:	74 14                	je     4011ab <phase_6+0xb7> # 如果%rsi == 24跳转到.L8
.L7  
  401197:	8b 0c 34             	mov    (%rsp,%rsi,1),%ecx	# %ecx = M(%rsp + %rsi)
  40119a:	83 f9 01             	cmp    $0x1,%ecx
  40119d:	7e e4                	jle    401183 <phase_6+0x8f> # 如果%ecx <= 1跳转到.L6_0
  40119f:	b8 01 00 00 00       	mov    $0x1,%eax			# %eax = 1
  4011a4:	ba d0 32 60 00       	mov    $0x6032d0,%edx		# %edx = 0x6032d0
  4011a9:	eb cb                	jmp    401176 <phase_6+0x82> # 跳转到.L5 
.L8
  4011ab:	48 8b 5c 24 20       	mov    0x20(%rsp),%rbx		# %rbx = M(%rsp + 32)
  4011b0:	48 8d 44 24 28       	lea    0x28(%rsp),%rax		# %rax = %rsp + 40
  4011b5:	48 8d 74 24 50       	lea    0x50(%rsp),%rsi		# %rsi = %rsp + 80
  4011ba:	48 89 d9             	mov    %rbx,%rcx			# %rcx = %rbx
.L9  
  4011bd:	48 8b 10             	mov    (%rax),%rdx			# %rdx = M(%rax)
  4011c0:	48 89 51 08          	mov    %rdx,0x8(%rcx)		# M(%rcx + 8) = %rdx
  4011c4:	48 83 c0 08          	add    $0x8,%rax			# %rax += 8
  4011c8:	48 39 f0             	cmp    %rsi,%rax
  4011cb:	74 05                	je     4011d2 <phase_6+0xde># 如果%rax == %rsi跳转到.L10
  4011cd:	48 89 d1             	mov    %rdx,%rcx			# %rcx = %rdx
  4011d0:	eb eb                	jmp    4011bd <phase_6+0xc9># 跳转到.L9
.L10  
  4011d2:	48 c7 42 08 00 00 00 	movq   $0x0,0x8(%rdx)		# M(%rdx + 8) = 0(8字节)
  4011d9:	00 
  4011da:	bd 05 00 00 00       	mov    $0x5,%ebp			# %ebp = 5
  
.L0:
  4011df:	48 8b 43 08          	mov    0x8(%rbx),%rax		# %rax = M(%rbx + 8)
  4011e3:	8b 00                	mov    (%rax),%eax			# %eax = M(%rax)
  4011e5:	39 03                	cmp    %eax,(%rbx)
  4011e7:	7d 05                	jge    4011ee <phase_6+0xfa> # 如果M(%rbx) >= %eax就不爆炸
  4011e9:	e8 4c 02 00 00       	call   40143a <explode_bomb>
  4011ee:	48 8b 5b 08          	mov    0x8(%rbx),%rbx		# %rbx = M(%rbx + 8)
  4011f2:	83 ed 01             	sub    $0x1,%ebp			# %ebp -= 1
  4011f5:	75 e8                	jne    4011df <phase_6+0xeb> # 如果%ebp != 0就跳.L0
  4011f7:	48 83 c4 50          	add    $0x50,%rsp
  4011fb:	5b                   	pop    %rbx
  4011fc:	5d                   	pop    %rbp
  4011fd:	41 5c                	pop    %r12
  4011ff:	41 5d                	pop    %r13
  401201:	41 5e                	pop    %r14
  401203:	c3                   	ret    
```

设输入的六个数字分别为x1 x2 x3 x4 x5 x6



.L3之前的逻辑如下

```pseudocode
r13 = rsp
rsi = rsp
M(rsp)~M(rsp+20)为输入的6个数字
r14 = rsp
r12d = 0
//.L1
do {
	rbp = r13
	if not (0 <= M(r13) - 1 <= 5) then explode_bomb()
	r12d += 1
	if r12d == 6 then break
	ebx = r12d
	//.L2
	do {
		if M(rbp) == M(rsp + 4*ebx) then explode_bomb()
		ebx += 1
	} while ebx <= 5
	r13 += 4
} while True
//.L3
```

为了这一部分不爆炸，1 <= xi  <= 6，且每个数字xi都各不相同



.L3, .L4的逻辑如下

```pseudocode
//.L3
rsi = rsp + 24
rax = r14 // 应等于rsp的值
ecx = 7
//.L4
do {
    M(rax) = ecx - M(rax)
    rax += 4
} while rax != rsi
esi = 0
//.L7
```

这一部分使得内存中每个数字 x~i~ = 7 - x~i~



.L7, .L6_0, .L6, .L5的逻辑如下

```pseudocode
//.L7
do {
	ecx = M(rsp + rsi)	// ecx是内存中的数字x,取值范围是[1,6]（注意原始的x已经被修改成7-x）
	if ecx <= 1 then {	// x == 1
		//.L6_0
		edx =  0x6032d0
	} else {			// 当x > 1 时，循环(x-1)次，每次迭代rdx = M(rdx + 8)
		eax = 1
		edx = 0x6032d0
		//.L5
		do {
			rdx = M(rdx + 8)
			eax += 1
		} while eax != ecx
	}
	//.L6
	M(rsp + 2*rsi + 32) = rdx
	rsi += 4
} while rsi != 24
//.L8
```
gdb查询得到rdx = M(rdx + 8)的迭代结果，发现规律rdx = 0x6032d0 + 迭代次数*0x10

| rdx迭代0次 | rdx迭代1次 | rdx迭代2次 | rdx迭代3次 | rdx迭代4次 | rdx迭代5次 |
| ---------- | ---------- | ---------- | ---------- | ---------- | ---------- |
| 0x6032d0   | 0x6032e0   | 0x6032f0   | 0x603300   | 0x603310   | 0x603320   |

所以M(rsp + 2*rsi + 32) = rdx结果如下

| M(rsp + 32)       | M(rsp + 40)       | M(rsp + 48)       | M(rsp + 56)       | M(rsp + 64)       | M(rsp + 72)       |
| ----------------- | ----------------- | ----------------- | ----------------- | ----------------- | ----------------- |
| rdx迭代(6-x~1~)次 | rdx迭代(6-x~2~)次 | rdx迭代(6-x~3~)次 | rdx迭代(6-x~4~)次 | rdx迭代(6-x~5~)次 | rdx迭代(6-x~6~)次 |

把上面6个8字节长的新数字定义为y1 y2 y3 y4 y5 y6

y~i~ = 0x6032d0 + (6 - x~i~)*0x10



.L8, .L9的逻辑如下

```pseudocode
//.L8
rbx = M(rsp + 32) // rbx = y_1
rax = rsp + 40
rsi = rsp + 80
rcx = rbx // = y_1
//.L9
do { 
	rdx = M(rax) // 此时rdx = y_(i+1), rcx = y_i
	M(rcx + 8) = rdx // M(y_i + 8) = y_(i+1)
	rax += 8
	if rax == rsi then break
	rcx = rdx
} while True
//.L10
```

这一段在每次迭代中使得M(y~i~ + 8) = y~i+1~，i 从1到5

M(0x6032d8 + (6 - x~i~) \* 0x10)  = 0x6032d0 + (6 - x~i+1~) \* 0x10

即他们被重新赋值为0x6032d0到0x603320

最开始的时候，内存是这样的:

| M(0x6032d8) | M(0x6032e8) | M(0x6032f8) | M(0x603308) | M(0x603318) | M(0x603328) |
| ----------- | ----------- | ----------- | ----------- | ----------- | ----------- |
| 0x6032e0    | 0x6032f0    | 0x603300    | 0x603310    | 0x603320    | 0           |



.L10, .L0的逻辑如下

```pseudocode
//.L10
// rbx = y_1
M(rdx + 8) = 0(8字节) // 通过前面可知此时rdx == M(rsp + 72) == y_6，所以是M(y_6 + 8) = 0
ebp = 5 
//.L0
do {
	// 设此时rbx = a
	rax = M(rbx + 8) // rax = M(a + 8)
	eax = M(rax)     // eax = M(M(a + 8))低4字节！！
	if M(rbx) - eax < 0 then explode_bomb() // M(a) >= M(M(a + 8))则继续（低4字节比较！！）
	rbx = M(rbx + 8)	// rbx = M(a + 8)
	ebp -= 1
} while ebp != 0
//OVER!
```

顺利循环5次即可成功结束

每次循环末尾，rbx更新为rbx = M(rbx + 8)

炸弹判断条件是M(rbx) >= M(M(rbx + 8)) 低4字节比较

rbx初始值为y~1~，然后依次变成y~2~, y~3~, y~4~, y~5~, y~6~，变成y~6~之后直接结束

所以M(y~1~)  > M(y~2~)   > M(y~3~)   > M(y~4~)   > M(y~5~) > M(y~6~) 

查gdb得到

| M(0x6032d0) | M(0x6032e0) | M(0x6032f0) | M(0x603300)     | M(0x603310) | M(0x603320) |
| ----------- | ----------- | ----------- | --------------- | ----------- | ----------- |
| 0x10000014c | 0x2000000a8 | 0x30000039c | 0x4000002b**0** | 0x5000001dd | 0x6000001bb |

对于低4字节，M(0x6032f0) > M(0x603300) > M(0x603310) > M(0x603320) > M(0x6032d0) > M(0x6032e0)

所以y~1~到y~6~为以上顺序

x~1~到 x~6~ 依次为**4 3 2 1 6 5**
