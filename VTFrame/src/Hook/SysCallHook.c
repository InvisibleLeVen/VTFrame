#include "SysCallHook.h"

#include "../Include/Native.h"
#include "../Include/CPU.H"
#include "../VMX/VMX.h"



ULONG64 GetKiSystemServiceCopyEndaddress64()
{
	//����ϵͳ�������
	PUCHAR StarAddress = (PUCHAR)__readmsr(0xc0000082);
	PUCHAR EndAddress = StarAddress + 0x1000;

	PUCHAR i = NULL;
	UCHAR b1, b2, b3;
	ULONG temp;
	ULONG64 addr;

	// F7 05 ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? 0F 85 ? ? ? ? ? ? ? ? 41 FF D2
	for (i = StarAddress; i < EndAddress; i++)
	{
		if (MmIsAddressValid(i) && MmIsAddressValid(i + 1) /*&& MmIsAddressValid(i + 18)*/)
		{
			b1 = *i;
			b2 = *(i + 1);

			if (b1 == 0xF7 && b2 == 0x05)
			{
				addr = (ULONG64)i;
				return addr;
			}
		}

	}
	return 0;
}

VOID InitSysCallHook()
{
	// �������Ԫ��
	RtlZeroMemory(HookEnabled,0x1000);
	RtlZeroMemory(ArgTble, 0x1000);
	RtlZeroMemory(HookTable, 0x1000*8);

	// ��ȡԭϵͳ���õ�ַ
	KiSystemCall64Ptr = 0;
	KiSystemCall64Ptr = __readmsr(MSR_LSTAR);

	// ��ȡKiSystemServiceCopyEnd��ַ,�����ַ���������Զ����ϵͳ�������õ�.��ϵͳ���÷���
	KiServiceCopyEndPtr = 0;
	KiServiceCopyEndPtr = GetKiSystemServiceCopyEndaddress64();

	for (int i = 0; i < KeNumberProcessors; i++)
	{
		KeSetSystemAffinityThread((KAFFINITY)(1 << i));//00000000 00000001 00000010 00000100 

		__vmx_vmcall(HYPERCALL_HOOK_LSTAR,KiSystemCall64Ptr,0,0);

		KeRevertToUserAffinityThread();
	}

	for (int i = 0; i <= (g_data->vcpus-1); i++)
	{
		if (g_data->cpu_data[i].OriginalLSTAR != 0)
		{
			DbgPrint("VTFrame:CPU:%d����ϵͳ����HOOK�ɹ�\n", i);
		}
	}
}

//VMCALLָ������VMM�У���ϵͳ�����������Ϊԭ����
VOID UnHookSysCall()
{
	for (int i = 0; i < KeNumberProcessors; i++)
	{
		KeSetSystemAffinityThread((KAFFINITY)(1 << i));//00000000 00000001 00000010 00000100 

		__vmx_vmcall(HYPERCALL_UNHOOK_LSTAR, 0, 0, 0);

		KeRevertToUserAffinityThread();
	}
}

//�˺����������HOOK,�˺�������ǰ���뿪��ϵͳ����HOOK,����û��Ч��
//index��ʾ����HOOK�ĺ�����SSDT���е�����
//pNewFunc��ʾ���HOOK������ַ
//ParameterNum��ʾ����HOOK�����Ĳ�������
VOID AddSSDTHook(ULONG index,PVOID pNewFunc, CHAR ParameterNum)
{
	InterlockedExchange64((PLONG64)&HookTable[index], (LONG64)pNewFunc);
	InterlockedExchange8(&ArgTble[index], ParameterNum);
	InterlockedExchange8(&HookEnabled[index], TRUE);
}