#include "Device/Include/stm32f10x.h"   // Device header
#include <stdlib.h>
#include <string.h>
#include "Composer.h"
#include "KeyDisp.h"

#define u32 unsigned int
#define u8 unsigned char

#define IsNum(x) ((x)>='0' && (x)<='9')
#define ToNum(x) ((x)-'0')

const char* Script = NULL;
u32 Counter = 0;
const char* Jumps[10];
u8 Last = 0;
signed char Rise = 0;
signed char CRaise = 0;
u8 Delayed=1;
signed char BaseTone = 0;

void LoadScript(const char* NewScript)
{
	Script = NewScript;
	Counter = 0;
	memset(Jumps, NULL, sizeof(Jumps));
	Rise = 0;
	CRaise = 0;
	Delayed=1;
}

u8 GetNote(u8 Num, signed char CRaise, signed char Rise)
{
	if (Num)
	{
		short nn;
		if (Num < 4)
		{
			nn = Num * 2 +CRaise* 12+Rise - 2;
		}
		else
		{
			nn= Num * 2 +CRaise* 12+Rise - 3;
		}
		if (nn < 0)
		{
			nn = 0;
		}
		return (u8)nn;
	}
	else
	{
		return 0xFF;
	}
}

u32 ScriptTick(void)
{
	u8 CurNum = 0;
	u8 NoExit = 0;
	if (Script == NULL)
	{
		return 0;
	}
	if (Counter--)
	{
		return 1;
	}
	if(!Delayed)
	{
		SetTuneEnable(Last, 0);
		SetKeyDisp(Last, 0);
		Delayed=1;
		Counter=0;
		return 1;
	}
	Delayed=0;
	
	if (*Script == 0)
	{
		Counter = 0;
		return 2;
	}
	do
	{
		NoExit = 0;
		if (IsNum(*Script))
		{
			CurNum = ToNum(*Script);
			Counter = 32;
			Rise = 0;
			CRaise = BaseTone;
		}
		else if (*Script == '\n')
		{
			if (CurNum == 0)
			{
				Counter = 1;
				Script++;
			}
			break;
		}
		else
		{
			switch (*Script)
			{
			case 'X':
			case 'x':
				Counter = 2;
				break;
			case '=':
				Counter /= 2;
				break;
			case '.':
				Counter += Counter / 2;
				break;
			case '~':
				Counter *= 2;
				break;
			case '+':
				CRaise++;
				break;
			case '-':
				CRaise--;
				break;
			case 'b':
			case 'B':
				Rise--;
				break;
			case '#':
				Rise++;
				break;
			case 'l':
			case 'L':
				Script++;
				if (IsNum(*Script))
				{
					Jumps[ToNum(*Script)] = Script + 1;
					NoExit = 1;
					break;
				}
				else
				{
					return 0;
				}
			case 'g':
			case 'G':
				Script++;
				if (IsNum(*Script))
				{
					if (Jumps[ToNum(*Script)])
					{
						const char* Dest = Jumps[ToNum(*Script)];
						Jumps[ToNum(*Script)] = 0;
						Script = Dest;
						continue;
					}
					break;
				}
				else
				{
					return 0;
				}
			case ' ':
			case '\r':
				break;
			default:
				return 0;
			}
		}
		Script++;
	} while (*Script && (!IsNum(*Script) || NoExit));
	if (CurNum==0 && *Script==0)
	{
		return 2;
	}
	Last = GetNote(CurNum, CRaise,Rise);
	SetKeyDisp(Last, 1);
	SetTuneEnable(Last, 1);
	return 1;
}

void SetBaseTone(signed char NewBase)
{
	BaseTone=NewBase;
	CRaise=NewBase;
}
