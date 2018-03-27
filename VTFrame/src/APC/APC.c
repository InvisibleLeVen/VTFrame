#include "APC.h"

NTKERNELAPI NTSTATUS PsLookupThreadByThreadId(HANDLE Id, PETHREAD *Process);
PETHREAD LookupThread(HANDLE Tid)
{
	PETHREAD ethread;
	if (NT_SUCCESS(PsLookupThreadByThreadId(Tid, &ethread)))
		return ethread;
	else
		return NULL;
}

PEPROCESS LookupProcess(HANDLE pid)
{
	PEPROCESS Process;
	if (NT_SUCCESS(PsLookupProcessByProcessId(pid, &Process)))
		return Process;
	else
		return NULL;

}

PEPROCESS GetProcessByName(UCHAR* ProcessName) 
{
	ULONG i = 0;
	UCHAR szName[16] = { 0 };
	//��4��2^18��ʼö�ٽ���,����Ϊ4
	for (i = 4; i <= 262144; i += 4)
	{
		PEPROCESS process = LookupProcess((HANDLE)i);
		if (process != NULL)
		{
			if (strcmp(ProcessName, PsGetProcessImageFileName(process)) == NULL)
			{
				return process;
			}
		}
	}
	return NULL;
	
}

//APC������
VOID APCFuntion(PKAPC pApc, ULONG64 *NormalRoutine, PVOID *NormalContext, PVOID *SystemArgument1, PVOID *SystemArgument2)
{
	PRWPM_INFO pInfo = (PRWPM_INFO)(pApc->NormalContext);
	__try
	{
		DbgPrint("APC����������\n");
		ULONG temp = *(ULONG*)0x00400000;
		((PFUNCTION)pInfo->fun)();
	}
	__except (1)
	{
		;
	}
	KeSetEvent(&(pInfo->Event), IO_NO_INCREMENT, FALSE);
	ExFreePool(pApc);
}

//����APC
NTSTATUS InsertKernelApc(PETHREAD Thread, PRWPM_INFO pInfo)
{
	NTSTATUS st = STATUS_UNSUCCESSFUL;
	PKAPC pApc = 0;
	if (MmIsAddressValid(Thread))
	{
		pApc = MALLOC_NPP(sizeof(KAPC));
		if (pApc)
		{
			LARGE_INTEGER interval = { 0 };

			//APC��ʼ��,�ں�ģʽ
			KeInitializeApc(pApc,
				Thread,	//������߳�
				OriginalApcEnvironment,
				APCFuntion,  //APC����
				0, 0, KernelMode, 0);

			pApc->NormalContext = pInfo;
			KeInitializeEvent(&(pInfo->Event), NotificationEvent, TRUE);
			KeClearEvent(&(pInfo->Event));
			if (KeInsertQueueApc(pApc, 0, 0, 0))
			{
				interval.QuadPart = -10000;//DELAY_ONE_MILLISECOND;
				interval.QuadPart *= 1000;
				st = KeWaitForSingleObject(&(pInfo->Event), Executive, KernelMode, 0, &interval);
			}
			else
			{
				ExFreePool(pApc);
			}
		}
	}
	return st;
}

BOOLEAN ExecFun(PFUNCTION pfun)
{
	ULONG i;
	BOOLEAN b = 0;
	PEPROCESS Process = GetProcessByName("dnf.exe");
	if (Process == NULL)
	{
		DbgPrint("δ�ҵ�DNF����\n");
		return FALSE;
	}
	for (i = 4; i < 1048576; i = i + 4)
	{
		PETHREAD ethrd = LookupThread((HANDLE)i);
		if (ethrd != NULL)
		{
			PEPROCESS eproc = IoThreadToProcess(ethrd);
			ObDereferenceObject(ethrd);
			if (eproc == Process)
			{
				PRWPM_INFO pInfo = MALLOC_NPP(sizeof(RWPM_INFO));
				pInfo->fun = pfun;
				if (NT_SUCCESS(InsertKernelApc(ethrd, pInfo)))
				{
					FREE(pInfo);
					b = 1; break;
				}
			}
		}
	}
	return b;
}
