#include "InlineHook.h"

#include "../Util/LDasm.h"

VOID InitJumpThunk(IN OUT PJUMP_THUNK pThunk, IN ULONG64 To)
{
	PULARGE_INTEGER liTo = (PULARGE_INTEGER)&To;

	pThunk->PushOp = 0x68;
	pThunk->AddressLow = liTo->LowPart;
	pThunk->MovOp = 0x042444C7;
	pThunk->AddressHigh = liTo->HighPart;
	pThunk->RetOp = 0xC3;
}

ULONG64 SetLineHook(PVOID Ori, PVOID Fun)
{
	ULONG len = 0;
	JUMP_THUNK thunk = { 0 }, jmpRet = { 0 };
	ldasm_data data = { 0 };
	KIRQL irql = 0;

	do 
	{
		len += ldasm(Ori, &data, TRUE);
	} while (len < sizeof(JUMP_THUNK));
	
	//����ԭʼ����ǰN���ֽں���תָ�����������ڴ棬����ԭʼ�����ĵ�ַ
	PVOID pOriFun = ExAllocatePool(NonPagedPool, sizeof(JUMP_THUNK)+len);
	RtlCopyMemory(pOriFun,Ori,len);
	InitJumpThunk(&jmpRet,(ULONG64)Ori+len);
	RtlCopyMemory((PVOID)((ULONG64)pOriFun + len), &jmpRet, sizeof(JUMP_THUNK));


	
	//����ԭʼ����ǰN���ֽڣ�Ϊ��ת�����ǵĹ��˺���
	InitJumpThunk(&thunk, (ULONG64)Fun);
	irql = WPOFFx64();

	memset(Ori,0x90,len);
	RtlCopyMemory(Ori,&thunk,sizeof(JUMP_THUNK));

	WPONx64(irql);

	return (ULONG64)pOriFun;
}

BOOLEAN RemoveLineHook(PVOID Ori, PVOID Fun)
{
	return TRUE;
}


