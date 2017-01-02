#pragma once

#include "stdafx.h"
#include <windows.h>

SIZE_T stBytes = 0;

namespace memory {
	void read1Byte(const HANDLE & hProcess, const UINT_PTR &addr, byte &ret);
	void read2Byte(const HANDLE &hProcess, const UINT_PTR &addr, int &ret);
	void read2Byte(const HANDLE &hProcess, const UINT_PTR &addr, uint16_t &ret);
	void writeByte(const HANDLE &hProcess, const UINT_PTR &addr, byte ret);
	void writeBit0(const HANDLE &hProcess, const UINT_PTR &addr, bool onOff);
}
