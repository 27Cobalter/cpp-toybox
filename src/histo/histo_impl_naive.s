	.file	"histo_impl_naive.cc"
	.text
	.align 2
	.p2align 4
	.globl	_ZN7MyHisto11Create_ImplILNS_6MethodE0EEEvPti
	.type	_ZN7MyHisto11Create_ImplILNS_6MethodE0EEEvPti, @function
_ZN7MyHisto11Create_ImplILNS_6MethodE0EEEvPti:
.LFB10529:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	pushq	%rbx
	.cfi_def_cfa_offset 24
	.cfi_offset 3, -24
	movslq	%edx, %rbp
	movq	%rsi, %rbx
	subq	$8, %rsp
	.cfi_def_cfa_offset 32
	movq	24(%rdi), %rdx
	movq	16(%rdi), %rcx
	salq	$2, %rdx
	testq	%rdx, %rdx
	jle	.L5
	movq	%rcx, %rdi
	xorl	%esi, %esi
	call	memset@PLT
	movq	%rax, %rcx
.L5:
	movq	%rbx, %rsi
	leaq	(%rbx,%rbp,2), %rdx
	testl	%ebp, %ebp
	je	.L8
	.p2align 4
	.p2align 3
.L6:
	movzwl	(%rsi), %eax
	addq	$2, %rsi
	incl	(%rcx,%rax,4)
	cmpq	%rdx, %rsi
	jne	.L6
.L8:
	addq	$8, %rsp
	.cfi_def_cfa_offset 24
	popq	%rbx
	.cfi_def_cfa_offset 16
	popq	%rbp
	.cfi_def_cfa_offset 8
	ret
	.cfi_endproc
.LFE10529:
	.size	_ZN7MyHisto11Create_ImplILNS_6MethodE0EEEvPti, .-_ZN7MyHisto11Create_ImplILNS_6MethodE0EEEvPti
	.align 2
	.p2align 4
	.globl	_ZN7MyHisto11Create_ImplILNS_6MethodE2EEEvPti
	.type	_ZN7MyHisto11Create_ImplILNS_6MethodE2EEEvPti, @function
_ZN7MyHisto11Create_ImplILNS_6MethodE2EEEvPti:
.LFB10554:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	pushq	%r12
	.cfi_offset 12, -24
	movl	%edx, %r12d
	pushq	%rbx
	movq	24(%rdi), %rdx
	movq	16(%rdi), %rcx
	andq	$-64, %rsp
	.cfi_offset 3, -32
	movq	%rsi, %rbx
	salq	$2, %rdx
	testq	%rdx, %rdx
	jle	.L12
	movq	%rcx, %rdi
	xorl	%esi, %esi
	call	memset@PLT
	movq	%rax, %rcx
.L12:
	cmpl	$31, %r12d
	jle	.L16
	shrl	$5, %r12d
	vpxor	%xmm5, %xmm5, %xmm5
	kxnorw	%k1, %k1, %k1
	leal	-1(%r12), %eax
	movq	%rbx, %rsi
	salq	$6, %rax
	leaq	64(%rbx,%rax), %rdx
	movl	$1, %eax
	vpbroadcastd	%eax, %zmm4
	.p2align 4
	.p2align 3
.L14:
	vmovdqu64	(%rsi), %zmm0
	kmovw	%k1, %k2
	addq	$64, %rsi
	kmovw	%k1, %k3
	kmovw	%k1, %k4
	kmovw	%k1, %k5
	vpunpcklwd	%zmm5, %zmm0, %zmm1
	vpunpckhwd	%zmm5, %zmm0, %zmm0
	vmovdqa32	%zmm1, %zmm3
	vpconflictd	%zmm1, %zmm1
	vpopcntd	%zmm1, %zmm1
	vpgatherdd	(%rcx,%zmm3,4), %zmm2{%k2}
	vpaddd	%zmm1, %zmm2, %zmm1
	vpaddd	%zmm4, %zmm1, %zmm1
	vpscatterdd	%zmm1, (%rcx,%zmm3,4){%k3}
	vmovdqa32	%zmm0, %zmm1
	vpconflictd	%zmm0, %zmm0
	vpopcntd	%zmm0, %zmm0
	vpgatherdd	(%rcx,%zmm1,4), %zmm2{%k4}
	vpaddd	%zmm2, %zmm0, %zmm0
	vpaddd	%zmm4, %zmm0, %zmm0
	vpscatterdd	%zmm0, (%rcx,%zmm1,4){%k5}
	cmpq	%rdx, %rsi
	jne	.L14
	vzeroupper
.L16:
	leaq	-16(%rbp), %rsp
	popq	%rbx
	popq	%r12
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE10554:
	.size	_ZN7MyHisto11Create_ImplILNS_6MethodE2EEEvPti, .-_ZN7MyHisto11Create_ImplILNS_6MethodE2EEEvPti
	.align 2
	.p2align 4
	.globl	_ZN7MyHisto11Create_ImplILNS_6MethodE3EEEvPti
	.type	_ZN7MyHisto11Create_ImplILNS_6MethodE3EEEvPti, @function
_ZN7MyHisto11Create_ImplILNS_6MethodE3EEEvPti:
.LFB10555:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	pushq	%r12
	.cfi_offset 12, -24
	movl	%edx, %r12d
	pushq	%rbx
	movq	24(%rdi), %rdx
	movq	16(%rdi), %rcx
	andq	$-64, %rsp
	.cfi_offset 3, -32
	movq	%rsi, %rbx
	salq	$2, %rdx
	testq	%rdx, %rdx
	jle	.L19
	movq	%rcx, %rdi
	xorl	%esi, %esi
	call	memset@PLT
	movq	%rax, %rcx
.L19:
	cmpl	$31, %r12d
	jle	.L23
	shrl	$5, %r12d
	vpxor	%xmm6, %xmm6, %xmm6
	kxnorw	%k1, %k1, %k1
	leal	-1(%r12), %eax
	movq	%rbx, %rsi
	salq	$6, %rax
	leaq	64(%rbx,%rax), %rdx
	movl	$1, %eax
	vpbroadcastd	%eax, %zmm5
	.p2align 4
	.p2align 3
.L21:
	vmovdqu64	(%rsi), %zmm0
	kmovw	%k1, %k2
	addq	$64, %rsi
	kmovw	%k1, %k3
	kmovw	%k1, %k4
	kmovw	%k1, %k5
	vpunpcklwd	%zmm6, %zmm0, %zmm1
	vpunpckhwd	%zmm6, %zmm0, %zmm0
	vmovdqa32	%zmm1, %zmm4
	vpconflictd	%zmm1, %zmm1
	vpopcntd	%zmm1, %zmm1
	vmovdqa32	%zmm0, %zmm3
	vpconflictd	%zmm0, %zmm0
	vpgatherdd	(%rcx,%zmm4,4), %zmm2{%k2}
	vpopcntd	%zmm0, %zmm0
	vpaddd	%zmm1, %zmm2, %zmm1
	vpaddd	%zmm5, %zmm1, %zmm1
	vpscatterdd	%zmm1, (%rcx,%zmm4,4){%k3}
	vpgatherdd	(%rcx,%zmm3,4), %zmm1{%k4}
	vpaddd	%zmm0, %zmm1, %zmm0
	vpaddd	%zmm5, %zmm0, %zmm0
	vpscatterdd	%zmm0, (%rcx,%zmm3,4){%k5}
	cmpq	%rdx, %rsi
	jne	.L21
	vzeroupper
.L23:
	leaq	-16(%rbp), %rsp
	popq	%rbx
	popq	%r12
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE10555:
	.size	_ZN7MyHisto11Create_ImplILNS_6MethodE3EEEvPti, .-_ZN7MyHisto11Create_ImplILNS_6MethodE3EEEvPti
	.align 2
	.p2align 4
	.globl	_ZN7MyHisto11Create_ImplILNS_6MethodE1EEEvPti
	.type	_ZN7MyHisto11Create_ImplILNS_6MethodE1EEEvPti, @function
_ZN7MyHisto11Create_ImplILNS_6MethodE1EEEvPti:
.LFB10556:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movl	%edx, %eax
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	pushq	%r15
	pushq	%r14
	pushq	%r13
	pushq	%r12
	pushq	%rbx
	.cfi_offset 15, -24
	.cfi_offset 14, -32
	.cfi_offset 13, -40
	.cfi_offset 12, -48
	.cfi_offset 3, -56
	movq	%rdi, %r12
	cltd
	andq	$-64, %rsp
	movq	%rsi, %r14
	subq	$64, %rsp
	movq	24(%rdi), %r13
	movq	40(%r12), %rcx
	movq	%rdi, 48(%rsp)
	movl	48(%rdi), %edi
	movq	%rcx, 56(%rsp)
	idivl	%edi
	movl	%eax, %r15d
	movl	%r13d, %eax
	cltd
	idivl	%edi
	leaq	0(,%rcx,4), %rdx
	movl	%eax, %ebx
	testq	%rdx, %rdx
	jle	.L26
	movq	32(%r12), %rdi
	xorl	%esi, %esi
	call	memset@PLT
	movl	48(%r12), %edi
.L26:
	movq	48(%rsp), %rax
	movq	16(%rax), %r12
	testl	%edi, %edi
	jle	.L72
	movslq	%r15d, %r11
	movslq	%r13d, %r13
	xorl	%ecx, %ecx
	leaq	(%r11,%r11), %r10
	xorl	%r9d, %r9d
	xorl	%r8d, %r8d
	leaq	(%r14,%r10), %rsi
.L32:
	testl	%r15d, %r15d
	jg	.L75
	incl	%r8d
	addq	%r11, %r9
	addq	%r13, %rcx
	addq	%r10, %rsi
	cmpl	%edi, %r8d
	jl	.L32
.L31:
	cmpl	$1, %edi
	jle	.L72
	movl	%ebx, %esi
	movq	%r13, %rax
	movq	%r13, %r14
	movl	$1, %r9d
	shrl	$4, %esi
	negq	%rax
	notq	%r14
	movq	%r13, %r10
	leal	-1(%rbx), %r11d
	leaq	0(,%rax,4), %r15
	leaq	0(,%r14,4), %rax
	movslq	%ebx, %r14
	salq	$6, %rsi
	movq	%rax, 56(%rsp)
	movl	%edi, %eax
	movq	%r13, %rdi
	movl	%r9d, %r13d
	movq	%r12, %r9
	movq	%r14, %r12
	.p2align 4
	.p2align 3
.L46:
	movq	%r10, %rcx
	xorl	%edx, %edx
	xorl	%r8d, %r8d
.L45:
	testl	%ebx, %ebx
	jg	.L76
	incl	%r8d
	addq	%r12, %rdx
	addq	%r12, %rcx
	cmpl	%eax, %r8d
	jl	.L45
	incl	%r13d
	addq	%rdi, %r10
	addq	%r15, 56(%rsp)
	cmpl	%eax, %r13d
	jl	.L46
.L77:
	vzeroupper
.L72:
	leaq	-40(%rbp), %rsp
	popq	%rbx
	popq	%r12
	popq	%r13
	popq	%r14
	popq	%r15
	popq	%rbp
	.cfi_remember_state
	.cfi_def_cfa 7, 8
	ret
	.p2align 4
	.p2align 3
.L76:
	.cfi_restore_state
	movq	%r15, 24(%rsp)
	movq	48(%rsp), %r15
	movl	%r13d, 44(%rsp)
	movq	%rdi, 32(%rsp)
	.p2align 4
	.p2align 3
.L43:
	cmpl	$6, %r11d
	jbe	.L35
	cmpq	$56, 56(%rsp)
	jbe	.L35
	cmpl	$14, %r11d
	jbe	.L47
	leaq	(%r9,%rdx,4), %rdi
	leaq	(%r9,%rcx,4), %r13
	xorl	%eax, %eax
	.p2align 4
	.p2align 3
.L37:
	vmovdqu32	(%rdi,%rax), %zmm0
	vpaddd	0(%r13,%rax), %zmm0, %zmm0
	vmovdqu32	%zmm0, (%rdi,%rax)
	addq	$64, %rax
	cmpq	%rax, %rsi
	jne	.L37
	movl	%ebx, %eax
	andl	$-16, %eax
	movl	%eax, %r13d
	cmpl	%ebx, %eax
	je	.L74
	movl	%ebx, %r14d
	subl	%eax, %r14d
	leal	-1(%r14), %edi
	cmpl	$6, %edi
	jbe	.L39
.L36:
	leaq	(%rdx,%r13), %rdi
	addq	%rcx, %r13
	leaq	(%r9,%rdi,4), %rdi
	vmovdqu	(%rdi), %ymm0
	vpaddd	(%r9,%r13,4), %ymm0, %ymm0
	vmovdqu	%ymm0, (%rdi)
	movl	%r14d, %edi
	andl	$-8, %edi
	addl	%edi, %eax
	andl	$7, %r14d
	je	.L74
.L39:
	movslq	%eax, %rdi
	leaq	(%rdi,%rdx), %r13
	addq	%rcx, %rdi
	movl	(%r9,%rdi,4), %edi
	addl	%edi, (%r9,%r13,4)
	leal	1(%rax), %edi
	cmpl	%edi, %ebx
	jle	.L74
	movslq	%edi, %rdi
	leaq	(%rdi,%rdx), %r13
	addq	%rcx, %rdi
	movl	(%r9,%rdi,4), %edi
	addl	%edi, (%r9,%r13,4)
	leal	2(%rax), %edi
	cmpl	%edi, %ebx
	jle	.L74
	movslq	%edi, %rdi
	leaq	(%rdx,%rdi), %r13
	addq	%rcx, %rdi
	movl	(%r9,%rdi,4), %edi
	addl	%edi, (%r9,%r13,4)
	leal	3(%rax), %edi
	cmpl	%edi, %ebx
	jle	.L74
	movslq	%edi, %rdi
	leaq	(%rdx,%rdi), %r13
	addq	%rcx, %rdi
	movl	(%r9,%rdi,4), %edi
	addl	%edi, (%r9,%r13,4)
	leal	4(%rax), %edi
	cmpl	%edi, %ebx
	jle	.L74
	movslq	%edi, %rdi
	leaq	(%rdx,%rdi), %r13
	addq	%rcx, %rdi
	movl	(%r9,%rdi,4), %edi
	addl	%edi, (%r9,%r13,4)
	leal	5(%rax), %edi
	cmpl	%edi, %ebx
	jle	.L74
	movslq	%edi, %rdi
	addl	$6, %eax
	leaq	(%rdx,%rdi), %r13
	addq	%rcx, %rdi
	movl	(%r9,%rdi,4), %edi
	addl	%edi, (%r9,%r13,4)
	cmpl	%eax, %ebx
	jle	.L74
	cltq
	leaq	(%rdx,%rax), %rdi
	addq	%rcx, %rax
	movl	(%r9,%rax,4), %eax
	addl	%eax, (%r9,%rdi,4)
.L74:
	addq	%r12, %rdx
.L42:
	movl	48(%r15), %eax
	incl	%r8d
	addq	%r12, %rcx
	cmpl	%eax, %r8d
	jl	.L43
	movl	44(%rsp), %r13d
	movq	32(%rsp), %rdi
	movq	%r15, 48(%rsp)
	movq	24(%rsp), %r15
	addq	%r15, 56(%rsp)
	incl	%r13d
	addq	%rdi, %r10
	cmpl	%eax, %r13d
	jl	.L46
	jmp	.L77
	.p2align 4
	.p2align 3
.L35:
	leaq	(%r9,%rdx,4), %rax
	addq	%r12, %rdx
	leaq	(%r9,%rdx,4), %r13
	.p2align 4
	.p2align 3
.L41:
	movl	(%rax,%r10,4), %edi
	addl	%edi, (%rax)
	addq	$4, %rax
	cmpq	%rax, %r13
	jne	.L41
	jmp	.L42
.L47:
	movl	%ebx, %r14d
	xorl	%eax, %eax
	xorl	%r13d, %r13d
	jmp	.L36
.L75:
	movq	48(%rsp), %r15
	.p2align 4
	.p2align 3
.L30:
	leaq	(%r14,%r9,2), %rdx
	.p2align 4
	.p2align 3
.L29:
	movzwl	(%rdx), %eax
	addq	$2, %rdx
	addq	%rcx, %rax
	incl	(%r12,%rax,4)
	cmpq	%rdx, %rsi
	jne	.L29
	movl	48(%r15), %edi
	incl	%r8d
	addq	%r11, %r9
	addq	%r13, %rcx
	addq	%r10, %rsi
	cmpl	%r8d, %edi
	jg	.L30
	movq	%r15, 48(%rsp)
	jmp	.L31
	.cfi_endproc
.LFE10556:
	.size	_ZN7MyHisto11Create_ImplILNS_6MethodE1EEEvPti, .-_ZN7MyHisto11Create_ImplILNS_6MethodE1EEEvPti
	.ident	"GCC: (GNU) 14.1.1 20240507"
	.section	.note.GNU-stack,"",@progbits
