#include "stdafx.h"
#include "memoryHelpers.h"

//sources
// http://www.cheatengine.org/forum/viewtopic.php?t=526508&sid=973568657e876839ea70dea3e545df9b
// http://www.cheatengine.org/forum/viewtopic.php?t=415186&sid=913dbca3654d4d2cf44a
namespace memory {

	HANDLE hProcess = 0;
	SIZE_T stBytes = 0;
	
	void writeBit0(const UINT_PTR &addr, bool onOff) {
		byte ret;
		read(addr, ret);
		if (onOff)	ret = ret | 0b00000001;
		else        ret = ret & 0b11111110;
		read(addr, ret);
	}
}