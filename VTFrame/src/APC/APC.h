#pragma once
#include <ntddk.h>
#include "../Util/GetUnExportFunAddress.h"

typedef VOID(*PFUNCTION)();

//APC��д�����ڴ棬�ɹ�DXF�ڴ��д
#define MALLOC_NPP(_s)        ExAllocatePool(NonPagedPool, _s)
#define FREE(_p)        ExFreePool(_p)

typedef enum _KAPC_ENVIRONMENT {
	OriginalApcEnvironment,
	AttachedApcEnvironment,
	CurrentApcEnvironment,
	InsertKernelApcEnvironment
} KAPC_ENVIRONMENT;

typedef struct _RWPM_INFO
{
	PFUNCTION fun;
	KEVENT Event;
}RWPM_INFO, *PRWPM_INFO;



BOOLEAN ExecFun(PFUNCTION pfun);
VOID GetThreadFunAdd();
VOID ThreadEipInsert();
BOOLEAN ExecFun1(PVOID pfun);

NTKERNELAPI UCHAR* PsGetProcessImageFileName(PEPROCESS process);
NTKERNELAPI NTSTATUS PsLookupProcessByProcessId(HANDLE Id, PEPROCESS *Process);

NTKERNELAPI PEPROCESS IoThreadToProcess(
	_In_ PETHREAD Thread
);
NTKERNELAPI NTSTATUS KeInitializeApc(__int64 a1, __int64 a2, int a3, __int64 a4, __int64 a5, __int64 a6, char a7, __int64 a8);
NTKERNELAPI char __fastcall KeInsertQueueApc(__int64 a1, __int64 a2, __int64 a3);
VOID __vmx_vmcall(ULONG index, ULONG64 arg1, ULONG64 arg2, ULONG64 arg3);

typedef NTSTATUS (*PsSuspendThreadx)(IN PETHREAD Thread, OUT PULONG PreviousSuspendCount OPTIONAL);
PsSuspendThreadx PsSuspendThread;

typedef NTSTATUS (*PsResumeThreadx)
(PETHREAD Thread, //�߳� ETHREAD
	OUT PULONG PreviousCount); //�ָ��Ĵ�����ÿ�ָ�һ�δ�ֵ�� 1��Ϊ 0 ʱ�̲߳�����
PsResumeThreadx PsResumeThread;

NTKERNELAPI NTSTATUS
PsGetContextThread(
	__in PETHREAD Thread,
	__inout PCONTEXT ThreadContext,
	__in KPROCESSOR_MODE Mode
);
