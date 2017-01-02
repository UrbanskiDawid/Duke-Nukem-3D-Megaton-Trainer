#pragma once

//Linker/Input/AdditionalDirectories CUESDK.x64_2013.lib
//Linker/Input/General/AdditionalLibraryDirectories
#include "..\CUESDK\include\CorsairLedIdEnum.h"
#include "..\CUESDK\include\CUESDK.h"

namespace CorsairKeyboard {

	const char* toString(CorsairError error);

	void setColor(CorsairLedColor &ledColor, int val, int red = 25, int yelow = 50);

	CorsairError init();
	void close();

	const CorsairLedId numPadL[] = {
		CLK_Keypad7,
		CLK_Keypad8,
		CLK_Keypad4,
		CLK_Keypad5,
		CLK_Keypad1,
		CLK_Keypad2,
		CLK_Keypad0,
		CLK_NumLock,
		CLK_KeypadSlash
	};

	const CorsairLedId numPadR[] = {
		CLK_Keypad9,
		CLK_Keypad6,
		CLK_Keypad3,
		CLK_KeypadPlus,
		CLK_KeypadAsterisk,
		CLK_KeypadMinus,
		CLK_KeypadPeriodAndDelete,
		CLK_KeypadEnter
	};
}