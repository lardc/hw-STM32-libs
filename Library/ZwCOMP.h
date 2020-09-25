#ifndef __ZW_COMP_H
#define __ZW_COMP_H

// Includes
//
#include "stdinc.h"

#define COMP_CSR_READonly		0x80000000
#define COMP_LOW_HYST			0x00010000
#define COMP_MED_HYST			0x00020000
#define COMP_HIGH_HYST			0x00030000
#define COMP_POL_INV			0x00008000
#define COMP_WINMODE			0x00000200
#define COMP_MODE_LowPower		0x00000004
#define COMP_MODE_MedSpeed		0x00000008
#define COMP_MODE_HighSpeed		0x0000000C
#define COMP_INpDAC				0x00000002
#define COMP_Enable				0x00000001

struct Comp_Struct {
  uint32_t LOCK;
  uint32_t BLANKING;
  uint32_t HYST;
  uint32_t POL;
  uint32_t OUTSEL;
  uint32_t WINMODE;
  uint32_t INPSEL;
  uint32_t INMSEL;
  uint32_t MODE;
  uint32_t INP_DAC;
  uint32_t COMP_EN;
  uint32_t COMP_OUT;
};

void COMP_Config(struct Comp_Struct Parametrs, COMP_TypeDef * COMPx);
bool COMP_Out_Read(COMP_TypeDef * COMPx);

#endif // __ZW_COMP_H
