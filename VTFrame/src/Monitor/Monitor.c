#include "Monitor.h"

#include "../VMX/VMX.h"
#include "../Test/Test.h"
#include "../IDT/idt.h"

VOID MonitorDriverLoad(
	_In_opt_ PUNICODE_STRING FullImageName,
	_In_ HANDLE ProcessId,                // pid into which image is being mapped
	_In_ PIMAGE_INFO ImageInfo
) 
{
	//��������
	if (ProcessId == 0)
	{
		
		//���TP���ؾͿ������ǵ�VT
		if (wcsstr(FullImageName->Buffer,L"TesSafe.sys") != NULL)
		{
			DbgPrint("��⵽TP����\n");	
			// ����VT��Ҫ����
			if (!StartVT())
				return ;

			// �Ƿ���VT�ɹ�
			for (int i = 0;i<=(g_data->vcpus-1);i++)
			{
				if (g_data->cpu_data[i].VmxState == VMX_STATE_ON)
				{
					DbgPrint("VTFrame:CPU:%d����VT�ɹ�\n", i);
				}
			}
			//����SSDT HOOK
			/*PrintIdt();
			TestPageHook();
			TestSSDTHook();*/
			

		}
	}
	
}


NTSTATUS addDriverMonitor()
{
	return PsSetLoadImageNotifyRoutine(MonitorDriverLoad);
}

NTSTATUS removeDriverMonitor()
{
	return PsRemoveLoadImageNotifyRoutine(MonitorDriverLoad);
}
