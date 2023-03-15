##  Part A

rbx, rbp, r12,r13,r14,r15在准备阶段必须保存到栈中（callee-save registers）

基础汇编代码，简单，写就完了



##  Part B

顺序执行的处理器主要有五个阶段：取指Fetch，译码Decode，执行Execute，访存Memory，写回Write back

按照书上的内容填写seq-full.hcl即可



## Part C

类似Part B填写pipe-full.hcl即可，还可以在上面对分支预测等功能进行优化，不太关心硬件就不花时间了

通过循环展开，以及在访存的等待时间插入其他要执行的指令来进行时间优化，最后拿了50.9/60.0，差不多得了

首先把循环展开成步长为6的循环，然后对于剩下的不足6个的数据用条件代替循环，介于程序长度有限制不超过1kb，Rest3之前一次性读取并写入数据，从Rest3开始一次性读取三个数据（可能越界），然后通过条件判断依次写入数据

```assembly
##################################################################
# Do not modify this portion
# Function prologue.
# %rdi = src, %rsi = dst, %rdx = len
ncopy:

##################################################################
# You can modify this portion
	# Loop header
	xorq %rax,%rax		# count = 0;
	iaddq $-6,%rdx		# len <= 0?
	jl Rest			# if so, goto Rest:

SixSteps:	
	mrmovq (%rdi), %r8	# read val from src...
	mrmovq 8(%rdi), %r9	# read val from src...
	mrmovq 16(%rdi), %r10	# read val from src...
	mrmovq 24(%rdi), %r11	# read val from src...
	mrmovq 32(%rdi), %r12	# read val from src...
	mrmovq 40(%rdi), %r13	# read val from src...
	rmmovq %r8, (%rsi)
	rmmovq %r9, 8(%rsi)
	rmmovq %r10, 16(%rsi)
	rmmovq %r11, 24(%rsi)
	rmmovq %r12, 32(%rsi)
	rmmovq %r13, 40(%rsi)
	andq %r8, %r8		# val <= 0?
	jle Npos1
	iaddq $1, %rax		# count++
Npos1:
	andq %r9, %r9		# val <= 0?
	jle Npos2
	iaddq $1, %rax		# count++
Npos2:
	andq %r10, %r10		# val <= 0?
	jle Npos3
	iaddq $1, %rax		# count++
Npos3:
	andq %r11, %r11		# val <= 0?
	jle Npos4
	iaddq $1, %rax		# count++
Npos4:
	andq %r12, %r12		# val <= 0?
	jle Npos5
	iaddq $1, %rax		# count++
Npos5:
	andq %r13, %r13		# val <= 0?
	jle Npos6
	iaddq $1, %rax		# count++
Npos6:
	iaddq $48, %rdi		# src += 6 
	iaddq $48, %rsi		# dest += 6
	iaddq $-6, %rdx		# len >= 0?
	jge SixSteps		# if so, goto SixSteps:

Rest:
	iaddq $1 ,%rdx		# len == 0?
	jne Rest4
	mrmovq (%rdi), %r8	# read val from src...
	mrmovq 8(%rdi), %r9	# read val from src...
	mrmovq 16(%rdi), %r10	# read val from src...
	mrmovq 24(%rdi), %r11	# read val from src...
	mrmovq 32(%rdi), %r12	# read val from src...
	rmmovq %r8, (%rsi)
	rmmovq %r9, 8(%rsi)
	rmmovq %r10, 16(%rsi)
	rmmovq %r11, 24(%rsi)
	rmmovq %r12, 32(%rsi)
	andq %r8, %r8		# val <= 0?
	jle Rest5Npos1
	iaddq $1, %rax		# count++
Rest5Npos1:
	andq %r9, %r9		# val <= 0?
	jle Rest5Npos2
	iaddq $1, %rax		# count++
Rest5Npos2:
	andq %r10, %r10		# val <= 0?
	jle Rest5Npos3
	iaddq $1, %rax		# count++
Rest5Npos3:
	andq %r11, %r11		# val <= 0?
	jle Rest5Npos4
	iaddq $1, %rax		# count++
Rest5Npos4:
	andq %r12, %r12		# val <= 0?
	jle Rest5Npos5
	iaddq $1, %rax		# count++
Rest5Npos5:
	jmp Done
Rest4:
	iaddq $1 ,%rdx		# len == 0?
	jne Rest3
	mrmovq (%rdi), %r8	# read val from src...
	mrmovq 8(%rdi), %r9	# read val from src...
	mrmovq 16(%rdi), %r10	# read val from src...
	mrmovq 24(%rdi), %r11	# read val from src...
	rmmovq %r8, (%rsi)
	rmmovq %r9, 8(%rsi)
	rmmovq %r10, 16(%rsi)
	rmmovq %r11, 24(%rsi)
	andq %r8, %r8		# val <= 0?
	jle Rest4Npos1
	iaddq $1, %rax		# count++
Rest4Npos1:
	andq %r9, %r9		# val <= 0?
	jle Rest4Npos2
	iaddq $1, %rax		# count++
Rest4Npos2:
	andq %r10, %r10		# val <= 0?
	jle Rest4Npos3
	iaddq $1, %rax		# count++
Rest4Npos3:
	andq %r11, %r11		# val <= 0?
	jle Rest4Npos4
	iaddq $1, %rax		# count++
Rest4Npos4:
	jmp Done
Rest3:
	mrmovq (%rdi), %r8	# read val from src...
	mrmovq 8(%rdi), %r9	# read val from src...
	mrmovq 16(%rdi), %r10	# read val from src...
	iaddq $4 ,%rdx		# len == 0?
	je Done
	rmmovq %r8, (%rsi)
	andq %r8, %r8		# val <= 0?
	jle Rest3Npos1
	iaddq $1, %rax		# count++
Rest3Npos1:
	iaddq $-1 ,%rdx		# len == 0?
	je Done
	rmmovq %r9, 8(%rsi)
	andq %r9, %r9		# val <= 0?
	jle Rest3Npos2
	iaddq $1, %rax		# count++
Rest3Npos2:
	iaddq $-1 ,%rdx		# len == 0?
	je Done
	rmmovq %r10, 16(%rsi)
	andq %r10, %r10		# val <= 0?
	jle Done
	iaddq $1, %rax		# count++
	
##################################################################
# Do not modify the following section of code
# Function epilogue.
Done:
	ret
##################################################################
```

