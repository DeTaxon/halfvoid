.text
_start:
	push %rbp
	mov %rsp,%rbp
	sub $240,%rsp
	movaps %xmm7,-16(%rbp)
	movaps %xmm6,-32(%rbp)
	movaps %xmm5,-48(%rbp)
	movaps %xmm4,-64(%rbp)
	movaps %xmm3,-80(%rbp)
	movaps %xmm2,-96(%rbp)
	movaps %xmm1,-112(%rbp)
	movaps %xmm0,-128(%rbp)
	mov %r9,-136(%rbp)
	mov %r8,-144(%rbp)
	mov %rcx,-152(%rbp)
	mov %rdx,-160(%rbp)
	mov %rsi,-168(%rbp)
	mov %rdi,-176(%rbp)

	lea -176(%rbp),%rax
	mov %rax,-184(%rbp)

	lea 16(%rbp),%rax
	mov %rax,-192(%rbp)

	movl $48,-196(%rbp)
	movl $0,-200(%rbp)

	// - 208 int res
	// - 224 float res

	mov %rax,-240(%rbp)

	movq $0x8765432112345678,%r11
	lea -208(%rbp),%rdi
	lea -224(%rbp),%rsi
	lea -200(%rbp),%rdx
	movq $0x1234567887654321,%rcx
	call *%r11

	test $0,%rax
	jne tail_call

	mov -208(%rbp),%rax
	movaps -224(%rbp),%xmm0

	add $240,%rsp
	pop %rbp
	ret

tail_call:

	mov %rax,%r11

	movaps  -16(%rbp),%xmm7
	movaps  -32(%rbp),%xmm6
	movaps  -48(%rbp),%xmm5
	movaps  -64(%rbp),%xmm4
	movaps  -80(%rbp),%xmm3
	movaps  -96(%rbp),%xmm2
	movaps -112(%rbp),%xmm1
	movaps -128(%rbp),%xmm0
	mov -136(%rbp),%r9 
	mov -144(%rbp),%r8 
	mov -152(%rbp),%rcx
	mov -160(%rbp),%rdx
	mov -168(%rbp),%rsi
	mov -176(%rbp),%rdi

	mov -240(%rbp),%rax

	add $240,%rsp
	pop %rbp
	jmp *%r11
