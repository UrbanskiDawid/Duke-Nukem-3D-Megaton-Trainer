// DN3Dmegaton.cpp
/*
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "stdafx.h"
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <bitset>
#include <iostream>
#include <string>
#include <Psapi.h>
#include <iomanip> // std::setw
#include <thread>
#include <atomic>  // std::atomic
#include "consts.h"
#include "MemoryHelpers\memoryHelpers.h"
#include "CorsairKeyboard\corsairKeyboard.h"
#include "DN3D.h"
using namespace std;

HWND hWindow;
UINT_PTR hBase;//base addres of game module

std::atomic<bool> canRun(true);//if false program ends
bool CorsainKeyBoardConnected = false;//is connected to Corsair CUE SDK

//current adresses + values
//====================================================
UINT_PTR healthAddr;
byte health;

UINT_PTR armorAddr;
byte armor;

UINT_PTR currentWeapon_id_Addr;
uint8_t currentWeaponId;

UINT_PTR currentWeapon_ammo_Addr;
uint8_t currentWeaponAmmo;

UINT_PTR cardsAddr;
byte cards;

UINT_PTR weaponAmmoAddr;
DN3D::sWeaponsAmmo weaponsAmmo;

UINT_PTR weaponEnableAddr;
DN3D::sWeaponsEnabled weaponsEnabled;

UINT_PTR playerPosAddr;
DN3D::sPlayerPos playerPos;

UINT_PTR timerAddr;
int32_t timer;

UINT_PTR cameraYrotAddr;
uint16_t cameraYrot;

UINT_PTR cameraXrotAddr;
uint16_t cameraXrot;

UINT_PTR currentItemAddr;
uint16_t currentItem;

UINT_PTR duke3d_psAddr;
DN3D::sSprite sprite[DN3D::MAXSPRITES];

UINT_PTR duke3d_wallAddr;
DN3D::sWallType walls[DN3D::MAXWALLS];
//====================================================


/*
* read curent values from memory
*/
void memoryReaderTask()
{
	while (canRun) {

		//timer
		//---------------------------------------
		memory::read(timerAddr, timer);


		//pos
		//---------------------------------------
		memory::read(playerPosAddr, playerPos);
		memory::read(cameraXrotAddr, cameraXrot);
		memory::read(cameraYrotAddr, cameraYrot);

		//HEALTH
		//---------------------------------------
		memory::read(healthAddr, health);

		//ARMOR
		//---------------------------------------
		memory::read(armorAddr, armor);

		//currentWeapon
		//---------------------------------------
		memory::read(currentWeapon_id_Addr, currentWeaponId);
		memory::read(currentWeapon_ammo_Addr, currentWeaponAmmo);

		//currentItem
		//---------------------------------------
		memory::read(currentItemAddr, currentItem);

		//CARDS
		//---------------------------------------
		memory::read(cardsAddr, cards);

		//WEAPONS
		//---------------------------------------
		memory::read(weaponAmmoAddr, weaponsAmmo);
		memory::read(weaponEnableAddr, weaponsEnabled);

		//list of Enemies/sprites
		//----------------------------------------------------------------
		memory::read(duke3d_psAddr, sprite);

		//list of walls
		//-----------------------------------------------------------------
		memory::read(duke3d_wallAddr, walls);//TODO: don't refresh each time!

		Sleep(50);

	}//while(true)
}

bool init() {

	//find window/process
	//====================================================
	DWORD processId = -1;
	std::cout << "looking for '" << WND_TITLE << "' window... ";
	hWindow = FindWindow(0, WND_TITLE);
	GetWindowThreadProcessId(hWindow, &processId);
	if (processId == -1)
		return false;
	std::cout << OK << " (PID: 0x" << std::hex << processId << ")" << endl;

	//open proces
	//====================================================
	std::cout << "opening process [PID: 0x" << std::hex << processId << "] ... ";
	memory::hProcess = OpenProcess(PROCESS_ALL_ACCESS, false, processId);
	if (memory::hProcess == 0)
		return false;
	std::cout << OK << " (adress 0x" << std::hex << (UINT_PTR)memory::hProcess << ")" << endl;

	//base adress [GetModuleHandleA("duke3d.exe"); ??] [probably 0x00E90000?]
	//====================================================
	std::cout << "getting module base address: ... ";
	HMODULE hModule;
	DWORD cbNeeded;
	if (!EnumProcessModulesEx(memory::hProcess, &hModule, sizeof(hModule), &cbNeeded, LIST_MODULES_32BIT))
		return false;
	hBase = (UINT_PTR)hModule;
	std::cout << OK << " (adress 0x" << std::hex << hBase << ")" << endl; //Print the pointer

	//Corsair CUE (optional)
	//====================================================
	std::cout << "connecting to Corsair CUE ... ";
	const auto error = CorsairKeyboard::init();
	if (error) {
		std::cout << "Handshake failed " << CorsairKeyboard::toString(error) << std::endl;
	}
	CorsainKeyBoardConnected = true;
	std::cout << OK << endl;

	return true;	//done
}

/*
* update corsair keyboard color leds
*/
void updateKeyboardTask()
{
	if (!CorsainKeyBoardConnected) return;

	while (canRun) {
		Sleep(100);

		static bool BLINK = false;
		BLINK = !BLINK;

		using namespace CorsairKeyboard;

		static CorsairLedColor ledColor = CorsairLedColor{ CLK_Space, 0, 0, 0 };

		setColor(ledColor, health);
		for (const auto ledId : CorsairKeyboard::numPadL) {
			ledColor.ledId = ledId;
			CorsairSetLedsColors(1, &ledColor);
		}


		setColor(ledColor, armor);
		for (const auto ledId : CorsairKeyboard::numPadR) {
			ledColor.ledId = ledId;
			CorsairSetLedsColors(1, &ledColor);
		}

		setKeyColor(CLK_1, (weaponsEnabled.leg ? COLOR_WHITE : COLOR_BLACK));
		setKeyColor(CLK_2, (weaponsEnabled.pistol ? COLOR_WHITE : COLOR_BLACK));
		setKeyColor(CLK_3, (weaponsEnabled.shotgun ? COLOR_WHITE : COLOR_BLACK));
		setKeyColor(CLK_4, (weaponsEnabled.ripper ? COLOR_WHITE : COLOR_BLACK));
		setKeyColor(CLK_5, (weaponsEnabled.RPG ? COLOR_WHITE : COLOR_BLACK));
		setKeyColor(CLK_6, (weaponsEnabled.pipebomb ? COLOR_WHITE : COLOR_BLACK));
		setKeyColor(CLK_7, (weaponsEnabled.shrinker ? COLOR_WHITE : COLOR_BLACK));
		setKeyColor(CLK_8, (weaponsEnabled.devastator ? COLOR_WHITE : COLOR_BLACK));
		setKeyColor(CLK_9, (weaponsEnabled.laserTripbomb ? COLOR_WHITE : COLOR_BLACK));
		setKeyColor(CLK_0, (weaponsEnabled.freezeThrower ? COLOR_WHITE : COLOR_BLACK));

		CorsairLedId currentItemkey = CLI_Invalid;
		switch (currentItem) {
		case DN3D::ITEMID_steroids: currentItemkey = CLK_S; break;
		case DN3D::ITEMID_jetpack:  currentItemkey = CLK_J; break;
		case DN3D::ITEMID_medKit:   currentItemkey = CLK_M; break;
		case DN3D::ITEMID_holoduke: currentItemkey = CLK_H; break;
		case DN3D::ITEMID_nightVision:currentItemkey = CLK_N; break;
		case DN3D::ITEMID_scuba:    break;
		}

		setKeyColor(CLK_S, (currentItem == DN3D::ITEMID_steroids  && BLINK ? COLOR_WHITE : COLOR_BLACK));
		setKeyColor(CLK_J, (currentItem == DN3D::ITEMID_jetpack   && BLINK ? COLOR_WHITE : COLOR_BLACK));
		setKeyColor(CLK_M, (currentItem == DN3D::ITEMID_medKit  && BLINK ? COLOR_WHITE : COLOR_BLACK));
		setKeyColor(CLK_H, (currentItem == DN3D::ITEMID_holoduke&& BLINK ? COLOR_WHITE : COLOR_BLACK));
		setKeyColor(CLK_N, (currentItem == DN3D::ITEMID_nightVision&& BLINK ? COLOR_WHITE : COLOR_BLACK));
	}

	CorsairKeyboard::close();
}

/*
* draw radar [windows GDI]
*/
namespace RADAR {

	HWND window = 0;

	const float SCALE = 0.05f;

	const COLORREF COLOR_BLACK = RGB(0, 0, 0);
	const COLORREF COLOR_SPRITE = RGB(25, 0, 25);
	const COLORREF COLOR_ENEMIE = RGB(255, 0, 25);

	//sizes
	int centerX = 0;
	int centerY = 0;
	int rL, rT, rR, rB;
	//--

	HDC hdc = ::GetDC(0);//destkop
	HDC memDC = 0;
	HBITMAP hBitmap;//draw buffer
	struct POINT32 { int32_t x; int32_t y; };

	void resize()
	{
		RECT rect; GetClientRect(window, &rect);

		rL = rect.left;
		rT = rect.top;
		rR = rect.right;
		rB = rect.bottom;

		centerX = rL + (rR - rL) / 2;
		centerY = rT + (rB - rT) / 2;

		hBitmap = CreateCompatibleBitmap(hdc,
			rect.right - rect.left, //width
			rect.bottom - rect.top  //height
		);

		SelectObject(memDC, hBitmap);
	}

	LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		switch (msg)
		{
		case WM_WINDOWPOSCHANGED:
			resize();
			break;

		case WM_DESTROY:		
			PostQuitMessage(0);
			return 0;
		}

		return DefWindowProc(hwnd, msg, wParam, lParam);
	}

	
	
	void runWindowTask()
	{
		const WCHAR* myclass = L"myclass";

		const WNDCLASSEX wndclass = {
			sizeof(WNDCLASSEX),
			CS_DBLCLKS,
			WndProc,
			0, 0,
			GetModuleHandle(0),
			LoadIcon(0,IDI_APPLICATION),
			LoadCursor(0,IDC_ARROW),
			HBRUSH(COLOR_WINDOW + 1),
			0,
			myclass, LoadIcon(0,IDI_APPLICATION)
		};

		if (RegisterClassEx(&wndclass))
		{
			window = CreateWindowEx(//NOTE: must be it the same thread as GetMessage/DispatchMessage
				0, myclass,
				WND_TITLE,
				WS_OVERLAPPEDWINDOW,
				CW_USEDEFAULT, CW_USEDEFAULT,
				CW_USEDEFAULT, CW_USEDEFAULT, 0, 0,
				GetModuleHandle(0), 0);

			if (window)
			{
				ShowWindow(window, SW_SHOWDEFAULT);
				MSG msg;
				while (GetMessage(&msg, 0, 0, 0)) DispatchMessage(&msg);

				canRun = false;
			}
		}
	}

	void mark(HDC wdc, int x, int y, const COLORREF & color) {
		SetPixel(wdc, x + 1, y, color);
		SetPixel(wdc, x, y + 1, color);
		SetPixel(wdc, x, y - 1, color);
		SetPixel(wdc, x, y, color);
		SetPixel(wdc, x - 1, y, color);
	}

	bool isEnemy(const DN3D::sSprite &sprite) {
		switch (sprite.picnum) {
		case DN3D::ePICID::SPRITE_PICID_ENEMIE_PIG:
		case DN3D::ePICID::SPRITE_PICID_ENEMIE_SHOTFLY:
		case DN3D::ePICID::SPRITE_PICID_ENEMIE_MASHINGUN:
		case DN3D::ePICID::SPRITE_PICID_ENEMIE_OCTOPUS:
		case DN3D::ePICID::SPRITE_PICID_ENEMIE_PIG2:
		case DN3D::ePICID::SPRITE_PICID_ENEMIE_TURRET:
		case DN3D::ePICID::SPRITE_PICID_ENEMIE_SHARK:
		case DN3D::ePICID::SPRITE_PICID_ENEMIE_SHARKELECTRIC:
		case DN3D::ePICID::SPRITE_PICID_ENEMIE_SLIME:
		case DN3D::ePICID::SPRITE_PICID_ENEMIE_HUGEMONSTER:
		case DN3D::ePICID::SPRITE_PICID_ENEMIE_FASTMONSTER:
		case DN3D::ePICID::SPRITE_PICID_ENEMIE_INVALIDMONSTER:
		case DN3D::ePICID::SPRITE_PICID_ENEMIE_TANK:
			return true;
		}

		return false;
	}

	POINT32 game2radar(const int32_t &X, const int32_t &Y, const float &c, const float &s) {
		POINT32 p;
		p.x = (X - playerPos.Xpos)* SCALE;
		p.y = (Y - playerPos.Ypos)* SCALE;

		double xnew = p.x * c - p.y * s;
		double ynew = p.x * s + p.y * c;
		p.x = xnew;
		p.y = ynew;

		p.x += centerX;
		p.y += centerY;

		return p;
	}

	void drawRadarTask() {

		//createwindow
		std::thread t(RADAR::runWindowTask);

		//wait for window to be created (note: CreateWindow must be in subthread)
		{
			int j = 10;
			while (window == 0) {
				Sleep(500);
				if (--j == 0) return;
			}
		}//----

		//draw context
		hdc = ::GetDC(window);
		SetTextColor(hdc, COLOR_BLACK);
		//--

		//draw memory/bitmap context
		memDC = CreateCompatibleDC(hdc);
		resize();
		
		//--

		const HBRUSH brushRED = CreateSolidBrush(RGB(250, 0, 0));
		const HBRUSH brushWHITE = CreateSolidBrush(GetBkColor(memDC));

		//canRun variables
		BITMAP bitmap;
		CHAR text[44];
		POINT32 p1, p2;
		POINT arr[2];
		float rotDegree, rotRad,s,c;
		//--
	
		while (canRun) {

			Sleep(10);

			rotDegree = cameraXrot * 360.0f / 2046.0f;

			//fix to window cordinates
			rotDegree += 90;
			rotDegree = -rotDegree;
			//--

			rotRad = rotDegree*3.1415 / 180.0f;
			s = sin(rotRad);
			c = cos(rotRad);

			Rectangle(memDC, rL, rT, rR, rB);
			mark(memDC, centerX, centerY, COLOR_BLACK);

			for (int j = 0;
				j < DN3D::MAXSPRITES;
				j++) {

				if ((sprite[j].cstat & 0b1000000000000000) == 0) continue; //visible

				POINT32 p = game2radar(sprite[j].posX, sprite[j].posY, c, s);

				if (isEnemy(sprite[j])) {
					mark(memDC, p.x, p.y, COLOR_ENEMIE);

					//draw triangle UP, triangle DOWN or rect based on height diff
					const int SIZE = 5;
					const int delta = 10000;
					SelectObject(memDC, brushRED);
					//monster-above -> triangleUP
					if (sprite[j].posZ - delta > playerPos.Zpos)		{
						POINT points[] = {
						{ p.x - SIZE, p.y },
						{ p.x,        p.y + SIZE },
						{ p.x + SIZE, p.y }
						};
						Polygon(memDC, points, 3);
						
					//triangleDown
					}else if (sprite[j].posZ+ delta < playerPos.Zpos)	{
						POINT points[] = {
							{ p.x - SIZE, p.y },
							{ p.x,        p.y - SIZE },
							{ p.x + SIZE, p.y }
						};
						Polygon(memDC, points, 3);
					//same height-rect
					}else {						
						POINT points[] = {
							{ p.x - SIZE, p.y },
							{ p.x,        p.y + SIZE },
							{ p.x + SIZE, p.y },
						    { p.x,        p.y - SIZE },
						};
						Polygon(memDC, points, 4);
					}
					SelectObject(memDC, brushWHITE);

				} else {
					mark(memDC, p.x, p.y, COLOR_SPRITE);

					_itoa_s(sprite[j].picnum, text, 10);
					TextOutA(memDC, p.x, p.y, text, strlen(text));
				}
			}//for sprites

			for (int j = 0;
				 j<DN3D::MAXWALLS;
				 j++) {

				const DN3D::sWallType &wall1 = walls[j];

				if (wall1.point2 == 0 || wall1.point2 == j) continue;

				const DN3D::sWallType &wall2 = walls[wall1.point2];

				p1 = game2radar(wall1.x, wall1.y, c, s);
				p2 = game2radar(wall2.x, wall2.y, c, s);

				arr[0] = { p1.x,p1.y };
				arr[1] = { p2.x,p2.y };
				Polyline(memDC, arr, 2);

			}//for walls

			GetObject(hBitmap, sizeof(bitmap), &bitmap);
			BitBlt(hdc, 0, 0, bitmap.bmWidth, bitmap.bmHeight, memDC, 0, 0, SRCCOPY);

		}//while(canRun)

		t.join();

	}//drawRadarTask

	
}//RADAR

void trainerTask()
{
	while (canRun) {
		Sleep(10);

		//1.keep min health
		if (health < 50) { memory::write(healthAddr, health + 5); }

		//2.keep min armor
		if (armor < 50) { memory::write(armorAddr, armor + 5); }

		//3.keep current weapon ammo
		short *sAmmo = reinterpret_cast<short*>(&weaponsAmmo);
		if (sAmmo[currentWeaponId] < 20) {
			sAmmo[currentWeaponId] = 20;
			memory::write(weaponAmmoAddr, weaponsAmmo);
		}
	}
}

void consoleTask()
{
	//Main thread
	DWORD exitCode;
	SIZE_T stBytes = 0;
	while (canRun) {

		//check if process is running
		if ( (!GetExitCodeProcess(memory::hProcess, &exitCode))
			 ||
			exitCode != STILL_ACTIVE)
		{
			canRun = false;
			break;
		}

		Sleep(50);

		//reset console cursor
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), { 0, 8 });

		//timeAddr TODO: test
		//---------------------------------------
		long timeSec = timer / 26;
		std::cout << "time: " << std::dec << timeSec << "sec" << endl;

		//POS
		//---------------------------------------
		std::cout << std::dec
			<< "  postion: "
			<< "    X" << std::setw(10) << playerPos.Xpos
			<< "    Y" << std::setw(10) << playerPos.Ypos
			<< "    Z" << std::setw(10) << playerPos.Zpos
			<< endl;

		std::cout << std::dec << " camera "
			<< " X:" << (long)cameraXrot << "(" << cameraXrot * 360 / 2046 << "*)"
			<< " Y:" << (long)cameraYrot
			<< endl;

		//HEALTH
		//---------------------------------------
		std::cout << std::setw(10) << "HEALTH: " << std::dec << (int)health << endl;

		//ARMOR
		//---------------------------------------
		std::cout << std::setw(10) << "ARMOR: " << std::dec << (int)armor << endl;

		//currentWeapon
		//---------------------------------------
		std::cout << "Curent weapon:"
			<< std::setw(15) << DN3D::WEAPON_NAMES[currentWeaponId] << "(" << (int)currentWeaponId << ")"
			<< "  ammo:" << std::setw(15) << currentWeaponAmmo
			<< endl;

		//currentItem
		//---------------------------------------
		static string itemName;
		switch (currentItem) {
		case 0: itemName = "nothing"; break;
		case DN3D::ITEMID_steroids: itemName = "steroids"; break;
		case DN3D::ITEMID_jetpack:  itemName = "jetpack"; break;
		case DN3D::ITEMID_medKit:  itemName = "medpack"; break;
		case DN3D::ITEMID_holoduke: itemName = "holoduke"; break;
		case DN3D::ITEMID_nightVision:itemName = "nightVision"; break;
		case DN3D::ITEMID_scuba:itemName = "scuba"; break;
		default:   itemName = "?";  break;
		}
		std::cout << "Curent item:  " << itemName << "(" << std::dec << currentItem << ")" << endl;

		//CARDS
		//---------------------------------------
		bool cardBlue = ((cards & 0b00000001) == 1);
		bool cardRed = ((cards & 0b00000010) == 1);
		bool cardYellow = ((cards & 0b00000100) == 1);
		std::cout << "Cards"
			<< " blue:" << (cardBlue ? YES : NO)
			<< " red:" << (cardRed ? YES : NO)
			<< " yellow:" << (cardYellow ? YES : NO)
			<< "  @ 0x" << std::hex << cardsAddr << "value= 0b" << bitset<8>(cards)
			<< endl;

		//WEAPONS
		//---------------------------------------
		short *sAmmo = reinterpret_cast<short*>(&weaponsAmmo);
		byte *sEnabled = reinterpret_cast<byte*>(&weaponsEnabled);
		for (int i = 0; i <= 8; i++) {
			bool enabled = (sEnabled[i] == 1);

			std::cout
				//name
				<< std::setw(15) << DN3D::WEAPON_NAMES[i] << " "
				//ammo
				<< std::setw(5) << std::dec << (int)sAmmo[i] << " @ 0x" << std::hex << (weaponAmmoAddr + 2 * i)
				//enabled
				<< (enabled ? YES : NO) << " @ 0x" << std::hex << (weaponEnableAddr + sizeof(byte)*i)
				<< endl;
		}

		//list of sprites
		//----------------------------------------------------------------
		/*
		const int w = 10;
		static INT start = 0;
		static INT_PTR idx = hBase + 0x19D9860;
		std::cout << "list of enemies:" << std::dec<<sizeof(DN3D::sSprite) << "B" << endl;
		//0x26
		//Enemies
		DN3D::sSprite enemie;
		for(int i=80+ start;i!=-1+ start;i--){
		UINT_PTR enemie1Addr = hBase + 0x19D9860 + sizeof(enemie)*i;
		if (enemie1Addr < hBase) {
		std::cout << "---" << endl;
		continue;
		}

		ReadProcessMemory(memory::hProcess, (LPVOID)enemie1Addr, &enemie, sizeof(enemie), &stBytes);
		if (idx == enemie1Addr) {
		std::cout << ">";
		if (GetKeyState(VK_NUMPAD5) & 0x8000) {
		enemie.picnum = 0;
		WriteProcessMemory(memory::hProcess, (LPVOID)enemie1Addr, &enemie, sizeof(enemie), &stBytes);
		}
		}else
		std::cout << " ";
		if (GetKeyState(VK_NUMPAD6) & 0x8000) {
		enemie.picnum = 0;
		WriteProcessMemory(memory::hProcess, (LPVOID)enemie1Addr, &enemie, sizeof(enemie), &stBytes);
		}

		std::cout
		<< "0x"<<std::hex<< (enemie1Addr- hBase)<< "0x"<<(enemie1Addr)
		<< std::dec
		<< " pos: ["
		<<        std::setw(w) << (UINT)enemie.posZ
		<< "," << std::setw(w) << (UINT)enemie.posX
		<< "," << std::setw(w) << (UINT)enemie.posY
		<< "]"
		<< " rot:" << std::setw(w) << (UINT)enemie.cstat
		<< " wtf1:" << std::setw(w) << (UINT)enemie.picnum
		<< " type" << std::setw(w) << (UINT)enemie.picnum
		<< " wtf2:" << std::setw(w) << (UINT)enemie.wtf2
		<< " color: " << std::setw(w) << (UINT)enemie.color
		<< " wtf3:" << std::setw(w) << (UINT)enemie.wtf3
		<< " wtf4:" << std::setw(w) << (UINT)enemie.wtf4
		<< " width:" << std::setw(w) << (UINT)enemie.width
		<< " height:" << std::setw(w) << (UINT)enemie.height
		<< " width"
		;
		//for (int16_t b : enemie.wtf)cout <<" "<<std::setw(w)<< (UINT)b;
		std::cout <<" H: " << std::setw(w) << (UINT) enemie.health<< endl;
		}

		if (GetKeyState(VK_NUMPAD7) & 0x8000) { idx += sizeof(enemie); }
		if (GetKeyState(VK_NUMPAD1) & 0x8000) { idx -= sizeof(enemie); }

		if (GetKeyState(VK_NUMPAD8) & 0x8000) { start += 50; idx += sizeof(enemie)*50; }
		if (GetKeyState(VK_NUMPAD2) & 0x8000) { start -= 50; idx -= sizeof(enemie)*50; }
		*/
	}

	std::cout << "END";
}




int main()
{
	//Hello
	SetConsoleTitleA(PROGRAM_NAME);
	std::cout << PROGRAM_NAME << endl << endl;
	Sleep(2000);

	//Init
	if (!init()) {
		std::cout << FAIL << endl;
		Sleep(10000);
		return 1;
	}

	//current adresses + values
	//====================================================
	healthAddr = hBase + DN3D::HEALTH_Offset;
	armorAddr = hBase + DN3D::ARMOR_Offset;
	currentWeapon_id_Addr = hBase + DN3D::CURRENTWEAPONID_Offset;
	currentWeapon_ammo_Addr = hBase + DN3D::CURRENTWEAPONAMMO_Offset;
	cardsAddr = hBase + DN3D::CARDS_offset;
	weaponAmmoAddr = hBase + DN3D::WEAPON_ammoOffset;
	weaponEnableAddr = hBase + DN3D::WEAPON_enableOffset;
	playerPosAddr = hBase + DN3D::PLAYERPOS_offset;
	timerAddr = hBase + DN3D::STATS_TIME;
	cameraYrotAddr = hBase + DN3D::CAMERAYrot_Offset;
	cameraXrotAddr = hBase + DN3D::CAMERAXrot_Offset;
	currentItemAddr = hBase + DN3D::CURRENT_ITEM;
	duke3d_wallAddr = hBase + DN3D::wall_Offset;
	duke3d_psAddr = hBase + DN3D::ps_Offset;
	//====================================================

	//threads
	std::thread t1(memoryReaderTask);
	std::thread t2(RADAR::drawRadarTask);
	std::thread t3(updateKeyboardTask);
	std::thread t4(trainerTask);
	std::thread t5(consoleTask);

	try{
	t1.join();
	t2.join();
	t3.join();
	t4.join();
	t5.join();
	}
	catch (exception e) {

	}

	CloseHandle(memory::hProcess);

	return 0;
}
