// Includes
//
#include "ZwCOMP.h"


//========================��������� ������������ 1-7==========================
void COMP_Config(struct Comp_Struct Parametrs, COMP_TypeDef * COMPx)
{
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
	COMPx->CSR = 0;
	//COMPx->CSR |= Parametrs.LOCK;			//���������� ������ � ������� CSR
	//COMPx->CSR |= (Parametrs.BLANKING)<<18;	//����� ������������ 1,2,3
	//COMPx->CSR |= Parametrs.HYST;			//4 ������ �����������
	//COMPx->CSR |= Parametrs.POL;			//�������� ������
	COMPx->CSR |= (Parametrs.OUTSEL)<<10;   //����� ��������� ������� � �����������
	//COMPx->CSR |= Parametrs.WINMODE;		//������� ����� ������������ 1 � 2, 3 � 4, 5 � 6
	//COMPx->CSR |= (Parametrs.INPSEL)<<7;	//����������� ���������������� �����
	COMPx->CSR |= (Parametrs.INMSEL)<<4;	//����������� �������������� �����
	//COMPx->CSR |= Parametrs.MODE;			//����� ������
	//COMPx->CSR |= Parametrs.INP_DAC;		//���������� ���������������� ������ ����������� 1 � ���
	COMPx->CSR |= Parametrs.COMP_EN;		//��������� �����������
}
//============================================================================


//=========================������ ������ �����������==========================
bool COMP_Out_Read(COMP_TypeDef * COMPx)
{
	if(COMPx->CSR & (1<<30)) return 1;
	else return 0;
}
//============================================================================
