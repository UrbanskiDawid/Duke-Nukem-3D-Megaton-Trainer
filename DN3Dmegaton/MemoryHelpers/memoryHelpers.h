#pragma once
#include <windows.h>

namespace memory {
	extern SIZE_T stBytes;

	extern HANDLE hProcess;//handle to game process

	template <typename T>
	inline void read(const UINT_PTR &addr,        T &ret) {
		ReadProcessMemory(hProcess, (LPVOID)addr, &ret, sizeof(T), &stBytes);
	}

	template <typename T>
	inline void write(const UINT_PTR &addr, const T &ret) {
		WriteProcessMemory(hProcess, (LPVOID)addr, &ret, sizeof(T), &stBytes);
	}

	void writeBit0(const UINT_PTR &addr, bool onOff);
}
