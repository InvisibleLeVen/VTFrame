#include <ntifs.h>


//��ַ
//����
#define RoleBase 0x04DB7F48
//���
#define MouseBase 0x04DE254C
//����
#define ScoreBase 0x04CBFACC 

//ƫ��
//����
#define SwordOffset 0x00003110
//����
#define ScoreOffset 0xC0C
//��Ʒ��
#define GoodsOffset 0x00006324
//��Ʒ��1
#define GoodOne 0x0000000C
//��Ʒ��2
#define GoodTwo 0x00000010
//����ƫ��
#define PackOffset 0x00000058
//������ʼ
#define FrozenBegOffset 0x00000B6C
//��������
#define FrozenEndOffset 0x00000B70
//�޵�
#define InvincibleOffset 0x00000A08  
//����
#define btpianyi 0x0000091C

//B68 B6C

//ֵ
#define ScoreValue  21546666//9999999

//shellcode
//˳ͼ
UCHAR shellcode[50];
//60 8B 0D A0 71 3C 04 8B 89 28 A0 20 00 8B 89 8C 00 00 00 6A FF 6A FF 6A 00 6A 00 6A 00 6A 00 6A 00 6A 00 B8 00 BA 1D 01 61 C3
UCHAR shellcode1[50];

VOID Function1();
VOID Function2();
VOID Function3();
VOID Function4();
VOID Function5();
VOID Function6();
VOID Function7();
VOID AllocateMem();
VOID StartThread();
VOID SetThreadAlertable();

typedef NTSTATUS (NTAPI *ZwCreateThreadEx_x)(OUT PHANDLE ThreadHandle,
	IN ACCESS_MASK DesiredAccess,
	IN POBJECT_ATTRIBUTES ObjectAttributes OPTIONAL,
	IN HANDLE ProcessHandle,
	IN ULONG64 StartRoutine,
	IN PVOID StartContext,
	IN ULONG CreateThreadFlags,
	IN SIZE_T ZeroBits OPTIONAL,
	IN SIZE_T StackSize OPTIONAL,
	IN SIZE_T MaximumStackSize OPTIONAL,
	IN ULONG64 AttributeList);

ZwCreateThreadEx_x ZwCreateThreadEx;


//����ָ�����
ULONG pShunTu;