GLOBAL _cli
GLOBAL _sti
GLOBAL picMasterMask
GLOBAL picSlaveMask
GLOBAL haltcpu
GLOBAL _hlt

GLOBAL _irq00Handler
GLOBAL _irq01Handler
GLOBAL _irq02Handler
GLOBAL _irq03Handler
GLOBAL _irq04Handler
GLOBAL _irq05Handler
GLOBAL _exception0Handler
GLOBAL _exception6Handler
GLOBAL _systemHandler
GLOBAL _force_irq00

GLOBAL _getRAX
GLOBAL _getRBX
GLOBAL _getRCX
GLOBAL _getRDX
GLOBAL _getRBP
GLOBAL _getRSP
GLOBAL _getRDI
GLOBAL _getRSI
GLOBAL _getR8
GLOBAL _getR9
GLOBAL _getR10
GLOBAL _getR11
GLOBAL _getR12
GLOBAL _getR13
GLOBAL _getR14
GLOBAL _getR15
GLOBAL _setRSP
GLOBAL _setRDI

EXTERN irqDispatcher
EXTERN exceptionDispatcher
EXTERN SysCallDispatcher
EXTERN short_term_scheduling

SECTION .text


%macro pushState 0
	push rax
	push rbx
	push rcx
	push rdx
	push rbp
	push rdi
	push rsi
	push r8
	push r9
	push r10
	push r11
	push r12
	push r13
	push r14
	push r15
	mov rax, 15*8    ; RAX = Posición original del stackpointer
%endmacro

%macro popState 0
	pop r15
	pop r14
	pop r13
	pop r12
	pop r11
	pop r10
	pop r9
	pop r8
	pop rsi
	pop rdi
	pop rbp
	pop rdx
	pop rcx
	pop rbx
	pop rax
%endmacro

%macro irqHandlerMaster 1
	pushState

	mov rdi, %1 ; pasaje de parametro
	call irqDispatcher

	; signal pic EOI (End of Interrupt)
	mov al, 20h
	out 20h, al

	popState
	iretq
%endmacro

%macro sysHandlerMaster 0
	pushState
	call SysCallDispatcher
	popState
	iretq
%endmacro

%macro exceptionHandlerMaster 1
	; RSP = RIP que fallo
	; RSP + 3*8 = RSP original

	pushState				; Guardo valores asi no los pierdo llamando a funcion

	mov rsi, rsp		; RSI = stack pointer (apunta al R15 que pushee)
	add rsi, rax 		; RSI = DIRECCION DE RETORNO A LA FUNCION QUE FALLO

	mov rdx, rsp		; RDX = Stack pointer
	add rdx, rax		; RDX = RSI = DIRECCION DE RETORNO A LA FUNCION QUE FALLO
	add rdx, 3*8 		; RDX = DIRECCION DE STACK DE LA FUNCION QUE FALLO

									;Llamo a exceptionDispatcher(ID, dir de retorno original, dir de stack original)
	mov rdi, %1
	call exceptionDispatcher

	popState				; Dejo todo como estaba

	iretq
%endmacro

_hlt:
	sti
	hlt
	ret

_cli:
	cli
	ret

_sti:
	sti
	ret

picMasterMask:
	push rbp
    mov rbp, rsp
    mov ax, di
    out	21h,al
    pop rbp
    retn

picSlaveMask:
		push    rbp
    mov   rbp, rsp
    mov   ax, di  ; ax = mascara de 16 bits
    out	 	0A1h,al
    pop   rbp
    retn

;int 80 (RAX = ID, RBX = p1, RCX = p2, RDX = p3)
_systemHandler:
	sysHandlerMaster

;8254 Timer (Timer Tick)
_irq00Handler:
	pushState

	mov rdi, 0 ; pasaje de parametro
	call irqDispatcher

	mov rdi, rsp
	call short_term_scheduling
	mov rsp, rax

	; signal pic EOI (End of Interrupt)
	mov al, 20h
	out 20h, al

	popState
	iretq

;Keyboard
_irq01Handler:
	irqHandlerMaster 1

;Cascade pic never called
_irq02Handler:
	irqHandlerMaster 2

;Serial Port 2 and 4
_irq03Handler:
	irqHandlerMaster 3

;Serial Port 1 and 3
_irq04Handler:
	irqHandlerMaster 4

;USB
_irq05Handler:
	irqHandlerMaster 5


;Zero Division Exception
	_exception0Handler:
		exceptionHandlerMaster 0

;Invalid Operation Code Exception
	_exception6Handler:
		exceptionHandlerMaster 6

haltcpu:
	cli
	hlt
	ret

_getRAX:
	ret

_getRBX:
	mov rax, rbx
	ret

_getRCX:
	mov rax, rcx
	ret

_getRDX:
	mov rax, rdx
	ret

_getRBP:
	mov rax, rbp
	ret

_getRSP:
	mov rax, rsp
	ret

_getRDI:
	mov rax, rdi
	ret

_getRSI:
	mov rax, rsi
	ret

_getR8:
	mov rax, r8
	ret

_getR9:
	mov rax, r9
	ret

_getR10:
	mov rax, r10
	ret

_getR11:
		mov rax, r11
		ret
_getR12:
		mov rax, r12
		ret
_getR13:
		mov rax, r13
		ret
_getR14:
		mov rax, r14
		ret
_getR15:
		mov rax, r15
		ret

_setRSP:
		mov rsp, rdi
		ret

_setRDI:
		mov rdi, rdi
		ret

_force_irq00:
	int 20h
	ret


SECTION .bss
	aux resq 1
