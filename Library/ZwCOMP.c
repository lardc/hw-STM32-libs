// Includes
//
#include "ZwCOMP.h"


//========================Настройка компараторов 1-7==========================
void COMP_Config(struct Comp_Struct Parametrs, COMP_TypeDef * COMPx)
{
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
	COMPx->CSR = 0;
	//COMPx->CSR |= Parametrs.LOCK;			//Блокировка записи в регистр CSR
	//COMPx->CSR |= (Parametrs.BLANKING)<<18;	//Сброс компараторов 1,2,3
	//COMPx->CSR |= Parametrs.HYST;			//4 уровня гистерезиса
	//COMPx->CSR |= Parametrs.POL;			//Инверсия выхода
	COMPx->CSR |= (Parametrs.OUTSEL)<<10;   //Выбор приемника сигнала с компаратора
	//COMPx->CSR |= Parametrs.WINMODE;		//Оконный режим компараторов 1 и 2, 3 и 4, 5 и 6
	//COMPx->CSR |= (Parametrs.INPSEL)<<7;	//Подключение неинвертирующего входа
	COMPx->CSR |= (Parametrs.INMSEL)<<4;	//Подключение инвертирующего входа
	//COMPx->CSR |= Parametrs.MODE;			//Режим работы
	//COMPx->CSR |= Parametrs.INP_DAC;		//Поключение неинвертирующего вывода компаратора 1 к ЦАП
	COMPx->CSR |= Parametrs.COMP_EN;		//Включение компаратора
}
//============================================================================


//=========================Чтение выхода компаратора==========================
bool COMP_Out_Read(COMP_TypeDef * COMPx)
{
	if(COMPx->CSR & (1<<30)) return 1;
	else return 0;
}
//============================================================================
