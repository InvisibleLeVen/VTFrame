#pragma once
#include "VMX.h"


//�ڶ���ҳ�������
#define NUM_PAGES	24		// 24GB�ڴ� ע�����������ϵͳ�����ڴ�����
//VPID��
#define VM_VPID             1
#define EPT_TABLE_ORDER     9

//�����ڴ��С,�˴��ڴ�Ӧ�ô��������ڴ�Ĵ�С,����Ῠ��
#define EPT_MEMORY_SIZE 10



//EPTҳ��ͷָ��Ľṹ
typedef union _EPT_TABLE_POINTER
{
	ULONG64 All;
	struct
	{
		ULONG64 MemoryType : 3;         // EPT Paging structure memory type (0 for UC)
		ULONG64 PageWalkLength : 3;     // Page-walk length
		ULONG64 reserved1 : 6;
		ULONG64 PhysAddr : 40;          // Physical address of the EPT PML4 table
		ULONG64 reserved2 : 12;
	} Fields;
} EPT_TABLE_POINTER, *PEPT_TABLE_POINTER;


//����EptViolation EXIT�¼�ʱ����Ҫ����Ϣ�ṹ
typedef union _EPT_VIOLATION_DATA
{
	ULONG64 All;
	struct
	{
		ULONG64 Read : 1;           // Read access
		ULONG64 Write : 1;          // Write access
		ULONG64 Execute : 1;        // Execute access
		ULONG64 PTERead : 1;        // PTE entry has read access
		ULONG64 PTEWrite : 1;       // PTE entry has write access
		ULONG64 PTEExecute : 1;     // PTE entry has execute access
		ULONG64 Reserved1 : 1;      // 
		ULONG64 GuestLinear : 1;    // �Ƿ�����Ϊ���Ե�ַ��ת����ɵ�VM-EXIT	bit7
		ULONG64 FailType : 1;       // 
		ULONG64 Reserved2 : 3;      // 
		ULONG64 NMIBlock : 1;       // NMI unblocking due to IRET
		ULONG64 Reserved3 : 51;     // 
	} Fields;
} EPT_VIOLATION_DATA, *PEPT_VIOLATION_DATA;

//PTE�ṹ����������ҳ��HOOK
typedef union _EPT_PTE_ENTRY
{
	ULONG64 All;
	struct
	{
		ULONG64 Read : 1;           // Region is present (read access)
		ULONG64 Write : 1;          // Region is writable
		ULONG64 Execute : 1;        // Region is executable
		ULONG64 MemoryType : 3;     // EPT Memory type
		ULONG64 IgnorePat : 1;      // Flag for whether to ignore PAT
		ULONG64 reserved1 : 5;      // Reserved
		ULONG64 PhysAddr : 40;      // Physical address
		ULONG64 reserved2 : 12;     // Reserved
	} Fields;
} EPT_PTE_ENTRY, *PEPT_PTE_ENTRY;

//ǰ����ҳ��ͨ�ýṹ
typedef union _EPT_MMPTE
{
	ULONG64 All;
	struct
	{
		ULONG64 Present : 1;    // If the region is present (read access)
		ULONG64 Write : 1;      // If the region is writable
		ULONG64 Execute : 1;    // If the region is executable
		ULONG64 reserved1 : 9;  // Reserved
		ULONG64 PhysAddr : 40;  // Physical address
		ULONG64 reserved2 : 12; // Reserved
	} Fields;
} EPT_PML4_ENTRY, EPT_MMPTE, *PEPT_PML4_ENTRY, *PEPT_MMPTE;



extern LIST_ENTRY g_PageList;

VOID EptEnable(IN ULONG64 *PML4);
PEPT_PTE_ENTRY GetPteEntry(ULONG64 pfn);
inline ULONG64 EptpTableOffset(IN ULONG64 pfn, IN CHAR level);
VOID VmExitEptViolation(IN PGUEST_STATE GuestState);
VOID VmExitEptMisconfig(IN PGUEST_STATE GuestState);
VOID PteModify(ULONG64 data, ULONG64 code);
VOID UnPteModify(ULONG64 data);
VOID VmExitMTF(IN PGUEST_STATE GuestState);
extern inline VOID ToggleMTF(IN BOOLEAN State);


