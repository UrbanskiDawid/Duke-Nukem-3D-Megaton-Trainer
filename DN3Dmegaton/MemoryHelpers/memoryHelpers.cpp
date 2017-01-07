#include "stdafx.h"
#include <windows.h>
#include <stdlib.h>
#include <bitset>
#include <iostream>
#include <string>
#include <Psapi.h>

//sources
// http://www.cheatengine.org/forum/viewtopic.php?t=526508&sid=973568657e876839ea70dea3e545df9b
// http://www.cheatengine.org/forum/viewtopic.php?t=415186&sid=913dbca3654d4d2cf44a
namespace memory {

	HANDLE hProcess = 0;
	SIZE_T stBytes = 0;

	void read1Byte(const UINT_PTR &addr, byte &ret) {	ReadProcessMemory(hProcess, (LPVOID)addr, &ret, 1, &stBytes); 	}

	void read2Byte(const UINT_PTR &addr, uint16_t &ret) {	ReadProcessMemory(hProcess, (LPVOID)addr, &ret, 2, &stBytes);	}
	void read2Byte(const UINT_PTR &addr,  int16_t &ret)	{	ReadProcessMemory(hProcess, (LPVOID)addr, &ret, 2, &stBytes);	}

	void read(const UINT_PTR &addr, int32_t &ret) { ReadProcessMemory(hProcess, (LPVOID)addr, &ret, sizeof(int32_t), &stBytes); }

	void writeByte(const UINT_PTR &addr, byte ret) {	WriteProcessMemory(hProcess, (LPVOID)addr, &ret, 1, &stBytes);	}

	void writeBit0(const UINT_PTR &addr, bool onOff) {
		byte ret;
		read1Byte(addr, ret);
		if (onOff)	ret = ret | 0b00000001;
		else        ret = ret & 0b11111110;
		writeByte(addr, ret);
	}
}