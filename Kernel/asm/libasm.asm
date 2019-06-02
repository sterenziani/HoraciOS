GLOBAL RestoreSP
GLOBAL RestoreIP

GLOBAL cpuVendor
GLOBAL getSeconds
GLOBAL getMinutes
GLOBAL getHours
GLOBAL readKey
GLOBAL outb
GLOBAL inb
GLOBAL beepon
GLOBAL beepoff
GLOBAL mutex_acquire
GLOBAL mask_interruptions
GLOBAL unmask_interruptions

EXTERN picMasterMask

section .bss
	RestoreSP: resq 1
	RestoreIP: resq 1

section .text
cpuVendor:
	push rbp
	mov rbp, rsp
	push rbx
	mov rax, 0
	cpuid
	mov [rdi], ebx
	mov [rdi + 4], edx
	mov [rdi + 8], ecx
	mov byte [rdi+13], 0
	mov rax, rdi
	pop rbx
	mov rsp, rbp
	pop rbp
	ret

getSeconds:
	push rbp
	mov rbp, rsp
	mov rax, 0
	mov al, 0
	out 70h, al
	in al, 71h
	mov rsp, rbp
	pop rbp
	ret

getMinutes:
	push rbp
	mov rbp, rsp
	mov rax, 0
	cli
	mov al, 2
	out 70h, al
	in al, 71h
	sti
	mov rsp, rbp
	pop rbp
	ret

getHours:
	push rbp
	mov rbp, rsp
	mov rax, 0
	mov al, 4
	out 70h, al
	in al, 71h
	mov rsp, rbp
	pop rbp
	ret

outb:
	mov rax, rsi
	mov rdx, rdi
	out dx, al
	ret

inb:
	mov rax, 0
	mov rdx, rdi
	in al, dx
	ret

readKey:
	mov rax,0
	in al, 60h
	ret

beepon:
		;Set frequency
	mov al, 0xB6
	out 43h, al
	mov ax, 2000
	out 42h, al

		;Play sound
	in al, 61h
	mov al, 03h
	out 61h, al
	ret

beepoff:
	in al, 61h
	mov al, 0
	out 61h, al
	ret

mutex_acquire:
	push rbp
	mov rbp, rsp

	mov eax, 1					; rdi is first argument, a pointer to the current value of mutex
	xchg eax, [rdi]    	; [rdi] = 1, EAX = previous value of mutex
											; return old value of mutex, mutex is now 1

	mov rsp, rbp
	pop rbp
	ret

mask_interruptions:
	push rdx
	mov rdx, 0xFC
	call picMasterMask
	pop rdx
	ret

unmask_interruptions:
	push rdx
	mov rdx, 0xFE
	call picMasterMask
	pop rdx
	ret
