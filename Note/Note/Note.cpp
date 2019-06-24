// Note.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <stdlib.h>
#include <string.h>
#include <Windows.h>
#include <iostream>
	HMIDIOUT h;
	char s1[] = { R"(63+=.5+==
3+=6=1+
6==1+==6==1+==3+
3+==2+==3+==1+==6
3+.=5+==6=+6=+
6=+3=+5+
3+==5+==3+==5+==6+=6+=
6+=3+=5+
3+x5+=6=3+x5+=6=
3+=2#+==3+==1+
2+2+
2+=1+==2+==3+=5+=
6+3++
3+3++
3+=3++=3+=3++=
3+=2+==#3+==1+
2+2+
2+=1+==2+==3+=5+=
6+~)" };

#define u32 unsigned int
#define u8 unsigned char

#define IsNum(x) ((x)>='0' && (x)<='9')
#define ToNum(x) ((x)-'0')

char* Script = NULL;
u32 Counter = 0;
char* Jumps[10];
u8 Last = 0;
char Rise = 0;
char CRaise = 0;

DWORD f(u8 n)
{
	switch (n)
	{
	case 0xff:
		return 0;
	case 0:
		return 490;
	case 1:
		return 523;
	case 2:
		return 554;
	case 3:
		return 578;
	case 4:
		return 622;
	case 5:
		return 659;
	case 6:
		return 698;
	case 7:
		return 740;
	case 8:
		return 784;
	case 9:
		return 831;
	case 11:
		return 880;
	case 12:
		return 932;
	case 13:
		return 988;
	case 14:
		return 1000;
	default:
		return 0;
	}
}

void LoadScript(char* NewScript)
{
	Script = NewScript;
	Counter = 0;
	memset(Jumps, NULL, sizeof(Jumps));
	Rise = 0;
	CRaise = 0;
}

u8 GetNote(u8 Num, char CRaise,char Rise)
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
		//if (nn > 14)
		{
			//nn = 14;
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
	if (Last != 0xff)midiOutShortMsg(h, 0x90 + ((61 + Last) * 0x100) + (0 * 0x10000) + 0);
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
			Counter = 16;
			Rise = 0;
			CRaise = 0;
		}
		else if (*Script == '\n')
		{
			if (CurNum == 0)
			{
				Counter = 1;
				Script++;
				std::cout << "\n";
			}
			break;
		}
		else
		{
			switch (*Script)
			{
			case 'X':
			case 'x':
				Counter = 1;
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
						char* Dest = Jumps[ToNum(*Script)];
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
				break;
			case ' ':
			case '\r':
				break;
			default:
				return 0;
			}
		}
		Script++;
	} while (*Script && !IsNum(*Script) || NoExit);
	if (CurNum==0 && *Script==0)
	{
		return 2;
	}
	Last = GetNote(CurNum, CRaise,Rise);
	std::cout << (int)Last << " ";
	//std::cout << (int)CurNum << " ";
	if(Last!=0xff)midiOutShortMsg(h, 0x90 + ((61+Last)*0x100) + (100* 0x10000) + 0);
	return 1;
}
int main()
{
	midiOutOpen(&h, 0, 0, 0, 0);
	midiOutShortMsg(h,40*0x100 + 0xC0);
	while (1)
	{
	LoadScript(s1);
	while (ScriptTick()==1)
	{
		Sleep(30);
	}
	std::cout << "finish";
	char c;
	//std::cin >> c;

	}
	return 0;
}
