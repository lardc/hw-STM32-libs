// Header
#include "MemLabel.h"

// Forward functions
static Boolean MemLabel_ReadRaw(Int8U deviceIndex, Int8U *Buffer);
static Int8U MemLabel_GetLabelCount(const Int8U *Buffer);
static Boolean MemLabel_FindLabelOffset(const Int8U *Buffer, Int8U Type, Int8U *Offset);
static void MemLabel_EncodeLabel(Int8U *Buffer, MemLabelEntry Label);

// Functions
// Чтение меток из DS2431 в массив Labels с остановкой на первой пустой записи
Int8U MemLabel_Read(Int8U deviceIndex, MemLabelEntry *Labels, Int8U MaxLabels)
{
	Int8U Buffer[DS2431_EEPROM_SIZE];
	Int16U ReadBytes;
	Int8U Filled = 0;

	if(MaxLabels == 0)
		return 0;

	if(Labels == NULL)
		return 0;

	// Читаем только часть EEPROM, достаточную для MaxLabels записей.
	ReadBytes = (Int16U)MaxLabels * MEM_LABEL_LABEL_SIZE;
	if(ReadBytes > DS2431_EEPROM_SIZE)
		ReadBytes = DS2431_EEPROM_SIZE;

	if(!DS2431_ReadArray(deviceIndex, Buffer, ReadBytes))
		return 0;

	// Разбор останавливаем на первой пустой записи (Type == 0xFF).
	for(Int16U Offset = 0; Offset + MEM_LABEL_LABEL_SIZE <= ReadBytes; Offset += MEM_LABEL_LABEL_SIZE)
	{
		if(Buffer[Offset] == ML_None)
			break;

		if(Filled >= MaxLabels)
			break;

		Labels[Filled].Type = Buffer[Offset];
		Labels[Filled].Value = (Int16U)Buffer[Offset + 1] | ((Int16U)Buffer[Offset + 2] << 8);
		Filled++;
	}

	// Возвращаем количество реально заполненных записей в Labels[].
	return Filled;
}
//-------------------

// Добавление одной метки в конец списка
Boolean MemLabel_AddOne(Int8U deviceIndex, MemLabelEntry Label)
{
	return MemLabel_AddArray(deviceIndex, &Label, 1);
}
//-------------------

// Добавление массива меток в конец списка с проверкой дублей типов
Boolean MemLabel_AddArray(Int8U deviceIndex, const MemLabelEntry *Labels, Int8U LabelCount)
{
	Int8U Buffer[DS2431_EEPROM_SIZE];
	Int8U StoredLabelCount;
	Int8U Offset;

	if(LabelCount == 0)
		return true;

	if(Labels == NULL)
		return false;

	if(!MemLabel_ReadRaw(deviceIndex, Buffer))
		return false;

	StoredLabelCount = MemLabel_GetLabelCount(Buffer);

	if((Int16U)StoredLabelCount + LabelCount > MEM_LABEL_MAX_LABELS)
		return false;

	for(Int8U Index = 0; Index < LabelCount; Index++)
	{
		if(Labels[Index].Type == ML_None)
			return false;

		if(MemLabel_FindLabelOffset(Buffer, Labels[Index].Type, &Offset))
			return false;

		Offset = StoredLabelCount * MEM_LABEL_LABEL_SIZE;
		MemLabel_EncodeLabel(&Buffer[Offset], Labels[Index]);
		StoredLabelCount++;
	}

	return DS2431_WriteArray(deviceIndex, Buffer, StoredLabelCount * MEM_LABEL_LABEL_SIZE);
}
//-------------------

// Обновление существующей метки по её типу
Boolean MemLabel_Update(Int8U deviceIndex, MemLabelEntry Label)
{
	Int8U Buffer[DS2431_EEPROM_SIZE];
	Int8U Offset;

	if(Label.Type == ML_None)
		return false;

	if(!MemLabel_ReadRaw(deviceIndex, Buffer))
		return false;

	if(!MemLabel_FindLabelOffset(Buffer, Label.Type, &Offset))
		return false;

	MemLabel_EncodeLabel(&Buffer[Offset], Label);

	return DS2431_WriteArray(deviceIndex, Buffer, Offset + MEM_LABEL_LABEL_SIZE);
}
//-------------------

// Полное стирание памяти метки DS2431
Boolean MemLabel_EraseAll(Int8U deviceIndex)
{
	return DS2431_EraseAll(deviceIndex, true);
}
//-------------------

// Чтение всей EEPROM DS2431 во временный буфер
static Boolean MemLabel_ReadRaw(Int8U deviceIndex, Int8U *Buffer)
{
	if(Buffer == NULL)
		return false;

	return DS2431_ReadArray(deviceIndex, Buffer, DS2431_EEPROM_SIZE);
}
//-------------------

// Подсчёт количества записанных меток до первой пустой записи
static Int8U MemLabel_GetLabelCount(const Int8U *Buffer)
{
	Int8U LabelCount = 0;

	for(Int8U Offset = 0; Offset <= (DS2431_EEPROM_SIZE - MEM_LABEL_LABEL_SIZE); Offset += MEM_LABEL_LABEL_SIZE)
	{
		if(Buffer[Offset] == ML_None)
			break;

		LabelCount++;
	}

	return LabelCount;
}
//-------------------

// Поиск смещения метки по типу в буфере
static Boolean MemLabel_FindLabelOffset(const Int8U *Buffer, Int8U Type, Int8U *Offset)
{
	for(Int8U LabelOffset = 0; LabelOffset <= (DS2431_EEPROM_SIZE - MEM_LABEL_LABEL_SIZE); LabelOffset += MEM_LABEL_LABEL_SIZE)
	{
		if(Buffer[LabelOffset] == ML_None)
			return false;

		if(Buffer[LabelOffset] == Type)
		{
			if(Offset != NULL)
				*Offset = LabelOffset;

			return true;
		}
	}

	return false;
}
//-------------------

// Кодирование структуры метки в три байта EEPROM
static void MemLabel_EncodeLabel(Int8U *Buffer, MemLabelEntry Label)
{
	Buffer[0] = Label.Type;
	Buffer[1] = (Int8U)(Label.Value & 0xFF);
	Buffer[2] = (Int8U)((Label.Value >> 8) & 0xFF);
}
//-------------------
