#include <ntddk.h>
#include "VMX/VMX.h"
#include "Test/Test.h"
#include "Monitor/Monitor.h"
#include "Include/DriverDef.h"
#include "IDT/idt.h"

VOID Unload(PDRIVER_OBJECT DriverObject);


//���������Դ����ص�
VOID BypassCheckSign(PDRIVER_OBJECT pDriverObj)
{
	//STRUCT FOR WIN64
	typedef struct _LDR_DATA                         			// 24 elements, 0xE0 bytes (sizeof)
	{
		struct _LIST_ENTRY InLoadOrderLinks;                     // 2 elements, 0x10 bytes (sizeof)
		struct _LIST_ENTRY InMemoryOrderLinks;                   // 2 elements, 0x10 bytes (sizeof)
		struct _LIST_ENTRY InInitializationOrderLinks;           // 2 elements, 0x10 bytes (sizeof)
		VOID*        DllBase;
		VOID*        EntryPoint;
		ULONG32      SizeOfImage;
		UINT8        _PADDING0_[0x4];
		struct _UNICODE_STRING FullDllName;                      // 3 elements, 0x10 bytes (sizeof)
		struct _UNICODE_STRING BaseDllName;                      // 3 elements, 0x10 bytes (sizeof)
		ULONG32      Flags;
	}LDR_DATA, *PLDR_DATA;
	PLDR_DATA ldr;
	ldr = (PLDR_DATA)(pDriverObj->DriverSection);
	ldr->Flags |= 0x20;
}

NTSTATUS DriverEntry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath)
{
	
	NTSTATUS status;
	// ��ѯӲ���Ƿ�֧��VT
	if (!IsVTSupport())
		return STATUS_UNSUCCESSFUL;

	// ����ȫ�ֱ������ڴ�
	if (!AllocGlobalMemory())
		return STATUS_UNSUCCESSFUL;

	//PrintIdt();
	//BypassCheckSign(DriverObject);

	//TestCallBack();
	// ����VT��Ҫ����
	//if (!StartVT())
	//	return STATUS_UNSUCCESSFUL;

	//// �Ƿ���VT�ɹ�
	//for (int i = 0; i <= (g_data->vcpus - 1); i++)
	//{
	//	if (g_data->cpu_data[i].VmxState == VMX_STATE_ON)
	//	{
	//		DbgPrint("VTFrame:CPU:%d����VT�ɹ�\n", i);
	//	}
	//}

	////Inline Hook��ֹSSDT HOOKǰ�棬���ǵ�SSDT HOOK�ᵼ��ĳЩSSDT������ַ��ȡ������ʹInline HOOKʧ��
	//TestPageHook();
	////TestInlineHook();
	//TestSSDTHook();
	
	
	//�����������ػص�,�ȵ�TP�����ٿ���VT
	status = addDriverMonitor();
	if (NT_SUCCESS(status))
		DbgPrint("�ȴ�TP��������....\n");
	else
		DbgPrint("�����������ػص�ʧ��\n");

	////���DNF���̴���
	//status = addProcessMonitor();
	//if (NT_SUCCESS(status))
	//	DbgPrint("��ʼ���DNF���̴���\n");
	//else
	//	DbgPrint("DNF���̴�����ؿ���ʧ��\n");

	status = CreateDeviceAndSymbol(DriverObject);
	if (!NT_SUCCESS(status))
		return status;

	DriverObject->MajorFunction[IRP_MJ_CREATE] = CREATE_DISPATCH;
	DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = DEVICE_CONTROL_DISPATCH;
	DriverObject->DriverUnload = Unload;

	
	return status;
}

VOID Unload(PDRIVER_OBJECT DriverObject)
{
	//ж�����̣�����DPC�����е���VMCALLִ��__vmx_off��һЩ�Ĵ����Ĵ��������ͷ�������ڴ�
	// �˴�Ҳʹ����KeSetSystemAffinityThread��������ͬʱж��,��������ж��
	//removeMonitor();
	UnloadTest();
	//removeDriverMonitor();
	//removeProcessMonitor();

	for (int i = 0; i < KeNumberProcessors; i++)
	{
		KeSetSystemAffinityThread((KAFFINITY)(1 << i));

		SetupVT(NULL);

		KeRevertToUserAffinityThread();
	}

	FreeGlobalData(g_data);
	DeleteDeviceAndSymbol();
	DbgPrint("VTFrame:ж��VT�ɹ�\n");
	DbgPrint("VTFrame:Driver Unload\n");
}