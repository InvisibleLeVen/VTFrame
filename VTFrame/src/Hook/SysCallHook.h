#pragma once
#include <ntddk.h>

#define HYPERCALL_HOOK_LSTAR        0x2
#define HYPERCALL_UNHOOK_LSTAR      0x3//UNHOOK SysCall

// SSDT�к����Ƿ�HOOK
CHAR HookEnabled[0x1000];
// SSDT��HOOK�����Ĳ�������
CHAR ArgTble[0x1000];
// SSDT��HOOK�������º�����ַ
PVOID HookTable[0x1000];

extern VOID __vmx_vmcall(ULONG index, ULONG64 arg1, ULONG64 arg2, ULONG64 arg3);

ULONG64 KiSystemCall64Ptr;    // ԭʼ��ϵͳ���õ�ַ
ULONG64 KiServiceCopyEndPtr;    // KiSystemServiceCopyEnd��ַ

VOID AddSSDTHook(ULONG index, PVOID pNewFunc, CHAR ParameterNum);
VOID InitSysCallHook();
VOID UnHookSysCall();