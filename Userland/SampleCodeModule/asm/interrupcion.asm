GLOBAL SysCallDispatcher
section .text

SysCallDispatcher:
	push rbp
	mov rbp, rsp
	int 80h;
	mov rsp, rbp
	pop rbp
	ret;

_throwInvalidopCode:
	rsm
	ret
