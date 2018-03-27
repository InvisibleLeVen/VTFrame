#pragma once
#include <ntddk.h>

//hook�õ���jmp�ṹ,ͨ����ջ������ת
#pragma pack(push, 1)
typedef struct _JUMP_THUNK
{
	UCHAR PushOp;           // 0x68
	ULONG AddressLow;       // 
	ULONG MovOp;            // 0x042444C7
	ULONG AddressHigh;      // 
	UCHAR RetOp;            // 0xC3
} JUMP_THUNK, *PJUMP_THUNK;
#pragma pack(pop)

//�ⲿ��������
KIRQL WPOFFx64();
void WPONx64(KIRQL irql);

VOID InitJumpThunk(IN OUT PJUMP_THUNK pThunk, IN ULONG64 To);

//�����µ�ԭʼ������ַ
ULONG64 SetLineHook(PVOID Ori,PVOID Fun);
BOOLEAN RemoveLineHook(PVOID Ori, PVOID Fun);
