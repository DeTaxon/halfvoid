.text
_start:
	push %rbp
	mov %rsp,%rbp
	sub $136,%rsp
	movq %xmm3,-8(%rbp)
	movq %xmm2,-16(%rbp)
	movq %xmm1,-24(%rbp)
	movq %xmm0,-32(%rbp)
	movq %r9,-40(%rbp)
	movq %r8,-48(%rbp)
	movq %rdx,-56(%rbp)
	movq %rcx,-64(%rbp)

	lea -64(%rbp),%rax
	mov %rax,-72(%rbp)

	lea 16(%rbp),%rax
	mov %rax,-80(%rbp)

	movl $0,-88(%rbp)

	// -96 int res
	// -104 float res
	// -136 , + 32 butes for shadow space call

	movq $0x8765432112345678,%r11
	lea -96(%rbp),%rcx
	lea -224(%rbp),%rdi
	lea -200(%rbp),%r8
	movq $0x1234567887654321,%r9
	call *%r11

	cmp $0,%rax
	jne tail_call

	mov -208(%rbp),%rax
	movaps -224(%rbp),%xmm0

	add $138,%rsp
	pop %rbp
	ret

tail_call:

	movq -8(%rbp),%xmm3
	movq -16(%rbp),%xmm2
	movq -24(%rbp),%xmm1
	movq -32(%rbp),%xmm0
	movq -40(%rbp),%r9
	movq -48(%rbp),%r8
	movq -56(%rbp),%rdx
	movq -64(%rbp),%rcx

	add $136,%rsp
	pop %rbp
	jmp *%r11
