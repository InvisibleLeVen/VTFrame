#include "idt.h"

#include "..\Include\Native.h"

VOID PrintIdt() 
{
	
	for (int i = 0; i < KeNumberProcessors; i++)
	{
		KeSetSystemAffinityThread((KAFFINITY)(1 << i));

		//��ȡIDTR�Ĵ���
		IDT_INFO idtr = { 0 };
		__sidt(&idtr);
		DbgPrint("IDT Base:%llx,Limit:%x\n", idtr.Base, idtr.Limit);

		//����ÿһ���һ��KIDTENTRY64�ṹ�ĵ�ַ,�����ж�Ӧ���жϺ�����ַ
		PKIDTENTRY64 pEntry = (PKIDTENTRY64)idtr.Base;
		TRAPADDR trap = { 0 };
		trap.field.low = pEntry[1].OffsetLow;
		trap.field.mid = pEntry[1].OffsetMiddle;
		trap.field.hig = pEntry[1].OffsetHigh;
		DbgPrint("low:%x,mid:%x,hig:%x\n", pEntry[1].OffsetLow, pEntry[1].OffsetMiddle, pEntry[1].OffsetHigh);
		DbgPrint("Trap 1 address:%llx\n", trap.All);

		//�滻0x0fΪ1���жϴ�������Ȼ����VT�����н�һ���ж�ת����0x0f�ж�
		KIRQL irql = WPOFFx64();
		pEntry[0x0f].Alignment = pEntry[1].Alignment;
		WPONx64(irql);

		KeRevertToUserAffinityThread();
	}
}

ULONG64 GetTrap03Address() 
{
	PVOID pKiExceptionDispatch = 0,pKiDispatchException = 0,pDbgkForwardException = 0;
	//��ȡIDTR�Ĵ���
	IDT_INFO idtr = { 0 };
	__sidt(&idtr);
	DbgPrint("IDT Base:%llx,Limit:%x\n", idtr.Base, idtr.Limit);

	//����ÿһ���һ��KIDTENTRY64�ṹ�ĵ�ַ,�����ж�Ӧ���жϺ�����ַ
	PKIDTENTRY64 pEntry = (PKIDTENTRY64)idtr.Base;
	TRAPADDR trap = { 0 };
	trap.field.low = pEntry[3].OffsetLow;
	trap.field.mid = pEntry[3].OffsetMiddle;
	trap.field.hig = pEntry[3].OffsetHigh;
	DbgPrint("Trap 3 address:%llx\n", trap.All);

	//Ӳ����d5,ldasm��ȡtrap03ĳЩָ�������
	pKiExceptionDispatch = (PVOID)GetSubFunInFunction2((PVOID)(trap.All+0xd5), 0);
	pKiDispatchException = (PVOID)GetSubFunInFunction2(pKiExceptionDispatch, 1);
	pDbgkForwardException = (PVOID)GetSubFunInFunction2(pKiDispatchException, 9);
	return (ULONG64)pDbgkForwardException;
}