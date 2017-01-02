#pragma once
#include "stdafx.h"
#include "corsairKeyboard.h"


namespace CorsairKeyboard {

	const char* toString(CorsairError error)
	{
		switch (error) {
		case CE_Success:
			return "CE_Success";
		case CE_ServerNotFound:
			return "CE_ServerNotFound";
		case CE_NoControl:
			return "CE_NoControl";
		case CE_ProtocolHandshakeMissing:
			return "CE_ProtocolHandshakeMissing";
		case CE_IncompatibleProtocol:
			return "CE_IncompatibleProtocol";
		case CE_InvalidArguments:
			return "CE_InvalidArguments";
		default:
			return "unknown error";
		}
	}

	void setColor(CorsairLedColor &ledColor, int val, int red, int yelow)
	{
		if (val <= red) {
			ledColor.r = -20 * val + 255;
			ledColor.g = 0;
			ledColor.b = 0;
			return;
		}
		if (val <= yelow) {
			ledColor.r = 255;
			ledColor.g = 255;
			ledColor.b = 0;
			return;
		}

		ledColor.r = 0;
		ledColor.g = 255;
		ledColor.b = 0;
	}

	void setKeyColor(CorsairLedId ledID, const COLOR &color)
	{
		static CorsairLedColor ledColor;
		ledColor = CorsairLedColor{ ledID, color[0], color[1], color[2] };
		CorsairSetLedsColors(1, &ledColor);
	}

	CorsairError init()
	{
		CorsairPerformProtocolHandshake();
		if (const auto error = CorsairGetLastError()) {
			return error;
		}
		CorsairRequestControl(CAM_ExclusiveLightingControl);
		return CorsairError::CE_Success;
	}

	void close() {
		CorsairReleaseControl(CAM_ExclusiveLightingControl);
	}
}