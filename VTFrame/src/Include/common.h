#pragma once
#include <ntddk.h>

#include "Native.h"

/************************************************************************/
/* ���ļ���Ҫ���һЩȫ�����ݽṹ�ͳ�������                                                                     */
/************************************************************************/

#define DPRINT(format, ...)         DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, format, __VA_ARGS__)
//#define DPRINT(format, ...)        
#define VF_POOL_TAG                '0mVZ'


/************************************************************************/
/* VMCALL������                                                         */
/************************************************************************/
#define VTFrame_UNLOAD            0x1//ж��VT
#define VTFrame_HOOK_LSTAR        0x2//HOOK SysCall
#define VTFrame_UNHOOK_LSTAR      0x3//UNHOOK SysCall
#define VTFrame_HOOK_PAGE         0x4//ҳ�쳣HOOK
#define VTFrame_UNHOOK_PAGE       0x5//ҳ�쳣UNHOOK
#define VTFrame_Test			  0x6//����
#define VTFrame_Test2			  0x7//����



//BUG
#define BUG_CHECK_UNSPECIFIED       0
#define BUG_CHECK_INVALID_VM        1
#define BUG_CHECK_TRIPLE_FAULT      2
#define BUG_CHECK_EPT_MISCONFIG     3
#define BUG_CHECK_EPT_VIOLATION     4
#define BUG_CHECK_EPT_NO_PAGES      5

//��ǰCPU��ID
#define CPU_IDX                     (KeGetCurrentProcessorNumberEx( NULL ))
//�����ַҳ֡
#define PFN(addr)                   (ULONG64)((addr) >> PAGE_SHIFT)


/************************************************************************/
/* CPU����VT��״̬                                                      */
/************************************************************************/
typedef enum _VCPU_VMX_STATE
{
	VMX_STATE_OFF = 0,   //δ���⻯
	VMX_STATE_TRANSITION = 1,   //���⻯�У���δ�ָ�������
	VMX_STATE_ON = 2    //���⻯�ɹ�
} VCPU_VMX_STATE;



/************************************************************************/
/* VMCS��VMXON����ṹ��                                                */
/************************************************************************/
typedef struct _VMX_VMCS
{
	ULONG RevisionId;//�汾��ʶ
	ULONG AbortIndicator;
	UCHAR Data[PAGE_SIZE - 2 * sizeof(ULONG)];	//4KB��С
} VMX_VMCS, *PVMX_VMCS;



typedef struct _VMX_FEATURES
{
	ULONG64 SecondaryControls : 1;  // Secondary controls are enabled
	ULONG64 TrueMSRs : 1;           // True VMX MSR values are supported
	ULONG64 EPT : 1;                // EPT supported by CPU
	ULONG64 VPID : 1;               // VPID supported by CPU
	ULONG64 ExecOnlyEPT : 1;        // EPT translation with execute-only access is supported
	ULONG64 InvSingleAddress : 1;   // IVVPID for single address
	ULONG64 VMFUNC : 1;             // VMFUNC is supported
} VMX_FEATURES, *PVMX_FEATURES;




/************************************************************************/
/* �ؼ���һ�����ݽṹ��������ÿ���߼�CPU��VMCS����д��Ҫ����            */
/************************************************************************/
typedef struct _VCPU
{
	KPROCESSOR_STATE HostState;             // �ڽ������⻯֮ǰ������״̬�򣬰���һЩͨ�üĴ���������Ĵ���
	volatile VCPU_VMX_STATE VmxState;       // CPU��VMX����״̬,��Ϊ������1.offδ����  2.transition������ 3.on�����ɹ�
	ULONG64 SystemDirectoryTableBase;       // ���Ҫ��Ҫ����ν,ͬ����Ϊ�˼���...���ø�
	PVMX_VMCS VMXON;                        // VMXON region
	PVMX_VMCS VMCS;                         // VMCS region
	PVOID VMMStack;                         // VMM��ջ�ڴ���
	ULONG64 *ept_PML4T;						// EPTҳ��
	ULONG64 OriginalLSTAR;                  // ԭ����ϵͳ�������
	ULONG64 TpHookSTAR;						//TPHook��ϵͳ�������
} VCPU, *PVCPU;

/************************************************************************/
/*                                                                      */
/************************************************************************/
typedef struct _GLOBAL_DATA
{
	LONG vcpus;                             //����CPU�ĸ���
	PUCHAR MSRBitmap;
	VCPU cpu_data[ANYSIZE_ARRAY];           //ÿ��CPU��VT�ṹ,�����Ǹ����飬�м���CPU���м���VCPU�ṹ
} GLOBAL_DATA, *PGLOBAL_DATA;

extern PGLOBAL_DATA g_Data;

