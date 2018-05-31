EXTERN HookEnabled:DB
EXTERN ArgTble:DB
EXTERN HookTable:DQ

EXTERN KiSystemCall64Ptr:DQ
EXTERN KiServiceCopyEndPtr:DQ

USERMD_STACK_GS = 10h
KERNEL_STACK_GS = 1A8h

MAX_SYSCALL_INDEX = 1000h

.CODE

; *********************************************************
;
; Determine if the specific syscall should be hooked
;
; if (SyscallHookEnabled[EAX & 0xFFF] == TRUE)
;     jmp KiSystemCall64_Emulate
; else (fall-through)
;     jmp KiSystemCall64
;
; *********************************************************


;�����������Զ����ϵͳ�������,R3��ĺ�������R0��ĺ�����ͨ���������ת
;syscallָ��ִ�к�����
SyscallEntryPoint PROC
    ;cli                                    ; Disable interrupts
    swapgs                                  ; ��GS�Ĵ���ָ��KPCR�ṹ (�û���ָ��TEB�ṹ��������ʶ�̵߳���Ϣ�����ں˲��KPCR�ṹ��������ʶ����������Ϣ)
    mov         gs:[USERMD_STACK_GS], rsp   ; �����û���Ķ�ջָ�뵽KPCR��UserRsp��Ա��
    cmp         rax, MAX_SYSCALL_INDEX      ; Is the index larger than the array size?
    jge         KiSystemCall64              ;

    lea         rsp, offset HookEnabled     ; RSP = &SyscallHookEnabled
    cmp         byte ptr [rsp + rax], 0     ; Is hooking enabled for this index?
    jne         KiSystemCall64_Emulate      ; NE = index is hooked

SyscallEntryPoint ENDP

; *********************************************************
;
; Return to the original NTOSKRNL syscall handler
; (Restore all old registers first)
;
; *********************************************************
KiSystemCall64 PROC
	mov         rsp, gs:[USERMD_STACK_GS]   ; Usermode RSP
	swapgs                                  ; Switch to usermode GS
	jmp         [KiSystemCall64Ptr]         ; Jump back to the old syscall handler
KiSystemCall64 ENDP

; *********************************************************
;
; Emulated routine executed directly after a SYSCALL
; (See: MSR_LSTAR)
;
; *********************************************************
KiSystemCall64_Emulate PROC
    ; NOTE:
    ; First 2 lines are included in SyscallEntryPoint

    mov         rsp, gs:[KERNEL_STACK_GS]   ; ����ջ�Ĵ�������ΪKPCR�ṹ��RspBase��Ա�������ں˶�ջ
    push        2Bh                         ; �ڶ�ջ�б���ssѡ����
    push        qword ptr gs:[10h]          ; �ڶ�ջ�б����û���ջָ��
    push        r11                         ; �ڶ�ջ�б���ԭʼ��RFLAGES�Ĵ���
    push        33h                         ; �ڶ�ջ�б���64λ��CSѡ����
    push        rcx                         ; �ڶ�ջ�б��淵�ص�ַ
    mov         rcx, r10                    ; ����һ��������ֵ��ֵ��rcx����Ϊ��SysEntryָ����ʹ����rcx��������native api�У���rcx��ֵ��Ҳ���ǵ�һ����������ʱ�������r10��

    sub         rsp, 8h                     ; �ڶ�ջ������������Ŀռ�
    push        rbp                         ; save standard register����rbp�Ĵ���

	;��������ܹ��ڶ�ջ��������38H���ֽڴ�С�Ŀռ�,������������158H���ֽڵĿռ䣬�ܹ�190H�ֽڴ�С�Ŀռ䣬���ö�Ӧһ��_KTRAP_FRAME�ṹ��С
	;�����ڶ�ջ��������ڴ����ö�Ӧ_KTRAP_FRAME�ṹ��+158H��+190H�ĳ�Ա
    sub         rsp, 158h                   ; allocate fixed frame
  
	lea         rbp, [rsp+80h]              ; ����rbpΪ_KTRAP_FRAME�ṹ��TrapFrame��Ա
    mov         [rbp+0C0h], rbx             ; �������ʧ�Ĵ���
    mov         [rbp+0C8h], rdi             ;
    mov         [rbp+0D0h], rsi             ;
    mov         byte ptr [rbp-55h], 2h      ; set service active
    mov         rbx, gs:[188h]              ; ���浱ǰ�̶߳������ʼ��ַ
    prefetchw   byte ptr [rbx+90h]          ; prefetch with write intent
    stmxcsr     dword ptr [rbp-54h]         ; save current MXCSR
    ldmxcsr     dword ptr gs:[180h]         ; set default MXCSR
    cmp         byte ptr [rbx+3], 0         ; �����ǰ�̴߳��ڵ���״̬
    mov         word ptr [rbp+80h], 0       ; assume debug not enabled
    jz          KiSS05                      ; �����ڵ���״̬��ת��KiSS05������_KTRAP_FRAME�б���洢ϵͳ������õ�ͷ4������
    mov         [rbp-50h], rax              ; ���ڵ���״̬���ͱ��������ֵ
    mov         [rbp-48h], rcx              ;
    mov         [rbp-40h], rdx              ;
    mov         [rbp-38h], r8               ;
    mov         [rbp-30h], r9               ;

    int         3                           ; 
    align       10h

    KiSS05:
    ;sti                                    ; enable interrupts
    mov         [rbx+88h], rcx              ;����һ���������浽�̶߳����FirstArgument��Ա
    mov         [rbx+80h], eax				;

KiSystemCall64_Emulate ENDP

;�˺���5�䶼��Ϊ���ҵ�������SSDT���е�ʵ������
KiSystemServiceStart_Emulate PROC
    mov         [rbx+90h], rsp				;TrapFrame
    mov         edi, eax					;����ţ�����ŵĵ�13λ��bit 12������������SSDT����shadow SSDT�������һλ��λ��������rdiΪ0x20�������Ϊ0
    shr         edi, 7						
    and         edi, 20h
    and         eax, 0FFFh
KiSystemServiceStart_Emulate ENDP

KiSystemServiceRepeat_Emulate PROC
    ; RAX = [IN ] syscall index
    ; RAX = [OUT] number of parameters
    ; R10 = [OUT] function address
    ; R11 = [I/O] trashed

    lea         r11, offset HookTable		;ȡSSDT��ĵ�ַ
    mov         r10, qword ptr [r11 + rax * 8h]

    lea         r11, offset ArgTble			;ȡ������ĵ�ַ,����˱�HOOK���������ĸ���
    movzx       rax, byte ptr [r11 + rax]   ; RAX = paramter count

    jmp         [KiServiceCopyEndPtr]
KiSystemServiceRepeat_Emulate ENDP

END