#pragma once

#include <windows.h>

SIZE_T stBytes = 0;

namespace memory {
	extern HANDLE hProcess;//handle to game process

	void read(const UINT_PTR &addr, int32_t &ret);
	void read1Byte(const UINT_PTR &addr, byte &ret);
	void read2Byte(const UINT_PTR &addr, int &ret);
	void read2Byte(const UINT_PTR &addr, uint16_t &ret);
	void writeByte(const UINT_PTR &addr, byte ret);
	void writeBit0(const UINT_PTR &addr, bool onOff);
}
