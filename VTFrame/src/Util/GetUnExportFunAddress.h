#pragma once
#include <ntddk.h>

//�ṹ��
typedef struct _SYSTEM_SERVICE_TABLE {
	PVOID ServiceTableBase;
	PVOID ServiceCounterTableBase;
	SIZE_T NumberOfServices;
	PVOID ParamTableBase;
} SYSTEM_SERVICE_TABLE, *PSYSTEM_SERVICE_TABLE;


//��ú����б����ú����ĵ�ַ
ULONG64 GetSubFunInFunction(
	PVOID pFun, //��������ַ
	ULONG index //�����ĵڼ����Ӻ�����ַ,��0��ʼ
);

//���SSDT���к����ĵ�ַ
ULONGLONG GetSSDTFunAddrress(ULONG id);

//���Zw������ַ
SIZE_T GetZwFunAddress(ULONG id);