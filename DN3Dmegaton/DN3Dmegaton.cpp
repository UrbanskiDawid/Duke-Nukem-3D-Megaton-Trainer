// DN3Dmegaton.cpp
/*
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "stdafx.h"
#include <windows.h>
#include <stdlib.h>
#include <bitset>
#include <iostream>
#include <string>
#include <Psapi.h>
#include <iomanip> // std::setw
#include <string>

#include "consts.h"
#include "MemoryHelpers\memoryHelpers.h"
#include "CorsairKeyboard\corsairKeyboard.h"
#include "DN3D.h"

using namespace std;

HANDLE hProcess;//handle to game process
UINT_PTR hBase;//base addres of game module

//=========================================================================================================================================================================
void printWeapons(
	const UINT_PTR &ammoAddr, DN3D::sWeaponsAmmo& ammo,
	const UINT_PTR &enabledAddr, DN3D::sWeaponsEnabled & enabled)
{
	short *sAmmo   = reinterpret_cast<short*>(&ammo);
	byte *sEnabled = reinterpret_cast<byte*>(&enabled);

	for (int i = 0; i <=8; i++) {
		//bitset<8> flag(sEnabled[i]);
		bool enabled = (sEnabled[i] == 1);

		std::cout
			//name
			<< std::setw(15) << DN3D::WEAPON_NAMES[i] << " "
			//ammo
			<< std::setw(5) << std::dec << (int)sAmmo[i] << " @ 0x" << std::hex << (ammoAddr + 2*i)
			//enabled
			<< (enabled ? YES : NO) << " @ 0x" << std::hex << (enabledAddr + sizeof(byte)*i)
			//<< "0b"<<flag

			<< endl;
	}
}

bool CorsainKeyBoardConnected = false;

bool init() {

	//find window/process
	//====================================================
	DWORD processId = -1;
	std::cout << "looking for '" << WND_TITLE << "' window... ";
	HWND hWindow = FindWindowA(0, WND_TITLE);
	GetWindowThreadProcessId(hWindow, &processId);
	if (processId == -1)
	{
		return false;
	} else {
		std::cout << OK << endl;
	}

	//open proces
	//====================================================
	std::cout << "opening process PID:" << processId << " ... ";
	hProcess = OpenProcess(PROCESS_ALL_ACCESS, false, processId);
	if (hProcess == 0) {
		return false;
	}
	std::cout << OK << endl;

	//base adress [GetModuleHandleA("duke3d.exe"); ??]
	//====================================================
	std::cout << "getting base address:" << processId << " ... ";
	HMODULE hModule;
	DWORD cbNeeded;
	if (!EnumProcessModulesEx(hProcess, &hModule, sizeof(hModule), &cbNeeded, LIST_MODULES_32BIT | LIST_MODULES_64BIT))
	{
		return false;
	}
	hBase = (UINT_PTR)hModule;
	std::cout << OK << " [The base is 0x" << hBase << "]" << endl; //Print the pointer
	
	//Corsair CUE (optional)
	//====================================================
	std::cout << "connecting to Corsair CUE ... ";
	const auto error = CorsairKeyboard::init();
	if (error) {
		std::cout << "Handshake failed " << CorsairKeyboard::toString(error) << std::endl;
	}else {
		CorsainKeyBoardConnected = true;
		std::cout << OK << endl;
	}

	return true;	//done
}

/*
 * update corsair keyboard color leds
 */
void updateKeyboard(
	const int health,
	const int armor,
	const uint16_t currentItem,
	const DN3D::sWeaponsEnabled & weaponsEnabled,
	const DN3D::sWeaponsAmmo & weaponsAmmo)
{
	if (!CorsainKeyBoardConnected) return;

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
	setKeyColor(CLK_3, (weaponsEnabled.shotgun? COLOR_WHITE : COLOR_BLACK));
	setKeyColor(CLK_4, (weaponsEnabled.ripper ? COLOR_WHITE : COLOR_BLACK));
	setKeyColor(CLK_5, (weaponsEnabled.RPG ? COLOR_WHITE : COLOR_BLACK));
	setKeyColor(CLK_6, (weaponsEnabled.pipebomb ? COLOR_WHITE : COLOR_BLACK));
	setKeyColor(CLK_7, (weaponsEnabled.shrinker ? COLOR_WHITE : COLOR_BLACK));
	setKeyColor(CLK_8, (weaponsEnabled.devastator ? COLOR_WHITE : COLOR_BLACK));
	setKeyColor(CLK_9, (weaponsEnabled.laserTripbomb ? COLOR_WHITE : COLOR_BLACK));
	setKeyColor(CLK_0, (weaponsEnabled.freezeThrower ? COLOR_WHITE : COLOR_BLACK));

	CorsairLedId currentItemkey = CLI_Invalid;
	switch(currentItem) {
		case DN3D::ITEMID_steroids: currentItemkey  = CLK_S; break;
		case DN3D::ITEMID_jetpack:  currentItemkey  = CLK_J; break;
		case DN3D::ITEMID_medKit:   currentItemkey  = CLK_M; break;
		case DN3D::ITEMID_holoduke: currentItemkey  = CLK_H; break;
		case DN3D::ITEMID_nightVision:currentItemkey= CLK_N; break;
	}
	
	setKeyColor(CLK_S, (currentItem==DN3D::ITEMID_steroids  && BLINK ? COLOR_WHITE : COLOR_BLACK));
	setKeyColor(CLK_J, (currentItem==DN3D::ITEMID_jetpack   && BLINK ? COLOR_WHITE : COLOR_BLACK));
	setKeyColor(CLK_M, (currentItem == DN3D::ITEMID_medKit  && BLINK ? COLOR_WHITE : COLOR_BLACK));
	setKeyColor(CLK_H, (currentItem == DN3D::ITEMID_holoduke&& BLINK ? COLOR_WHITE : COLOR_BLACK));
	setKeyColor(CLK_N, (currentItem == DN3D::ITEMID_nightVision&& BLINK ? COLOR_WHITE : COLOR_BLACK));
}

int main()
{
	SetConsoleTitleA(PROGRAM_NAME);
	std::cout << PROGRAM_NAME<<endl << endl;
	Sleep(2000);
	if (!init()) {
		std::cout << FAIL << endl;
		int a; cin >> a;
		return 1;
	}

	//current adresses
	//====================================================
	const UINT_PTR healthAddr = hBase + DN3D::HEALTH_Offset;
	const UINT_PTR armorAddr = hBase + DN3D::ARMOR_Offset;
	const UINT_PTR currentWeapon_id_Addr = hBase + DN3D::CURRENTWEAPONID_Offset;
	const UINT_PTR currentWeapon_ammo_Addr = hBase + DN3D::CURRENTWEAPONAMMO_Offset;
	const UINT_PTR cardsAddr = hBase + DN3D::CARDS_offset;
	const UINT_PTR weaponAmmoAddr = hBase + DN3D::WEAPON_ammoOffset;
	const UINT_PTR weaponEnableAddr =  hBase + DN3D::WEAPON_enableOffset;
	const UINT_PTR playerPosAddr = hBase + DN3D::PLAYERPOS_offset;
	const UINT_PTR timeAddr = hBase + DN3D::STATS_TIME;
	const UINT_PTR cameraYrotAddr = hBase + DN3D::CAMERAYrot_Offset;
	const UINT_PTR cameraXrotAddr = hBase + DN3D::CAMERAXrot_Offset;
	const UINT_PTR currentItemAddr = hBase + DN3D::CURRENT_ITEM;
	//====================================================

	DN3D::sWeaponsAmmo weaponsAmmo;
	DN3D::sWeaponsEnabled weaponsEnabled;
	DN3D::sPlayerPos playerPos;
	uint16_t currentItem;
	byte currentWeaponId;
	byte currentWeaponAmmo;
	byte cards;
	byte health;
	byte armor;
	byte cameraYrot, cameraXrot;

	DWORD exitCode;
	SIZE_T stBytes = 0;
	while (true) {

		//check if process is running
		if (!GetExitCodeProcess(hProcess, &exitCode)) break;
		if (exitCode != STILL_ACTIVE) break;

		//reset console cursor
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), { 0, 8 });

	    //timeAddr
		int32_t time;
		ReadProcessMemory(hProcess, (LPVOID)timeAddr, &time, sizeof(time), &stBytes);
		long timeSec = time / 26;
		std::cout << "time: "<<std::dec << timeSec << "sec" << endl;

		//pos
		//posyAddr
		//memory::read2Byte(posyAddr, posy);
		ReadProcessMemory(hProcess, (LPVOID)playerPosAddr, &playerPos, sizeof(playerPos), &stBytes);
		std::cout << std::dec
			<< "  postion: "
			<< "    X" << std::setw(10) << playerPos.Xpos
			<< "    Y" << std::setw(10) << playerPos.Ypos
			<< "    Z" << std::setw(10) << playerPos.Zpos
			<< endl;

		memory::read1Byte(hProcess, cameraXrotAddr, cameraXrot);
		memory::read1Byte(hProcess, cameraYrotAddr, cameraYrot);
		std::cout << std::dec << " camera "
			<< " X:" << (int)cameraXrot
			<< " Y:" << (int)cameraYrot
			<< endl;

		//HEALTH
		//---------------------------------------
		memory::read1Byte(hProcess, healthAddr, health);
		
		std::cout << std::setw(10) << "HEALTH: " <<std::dec<< (int)health<<endl;
		if (health < 50) { memory::writeByte(hProcess, healthAddr, health+5); }

		//ARMOR
		//---------------------------------------
		memory::read1Byte(hProcess, armorAddr, armor);
		if (armor < 50) { memory::writeByte(hProcess, armorAddr, armor + 5); }
		std::cout << std::setw(10) << "ARMOR: " << std::dec << (int)armor<<endl;

		
		//currentWeapon
        //---------------------------------------
		memory::read1Byte(hProcess, currentWeapon_id_Addr, currentWeaponId);
		memory::read1Byte(hProcess, currentWeapon_ammo_Addr, currentWeaponAmmo);
		std::cout << "Curent weapon:"
			<< std::setw(15) << DN3D::WEAPON_NAMES[(int)currentWeaponId] << "(" << (int)currentWeaponId << ")" 
			<< "  ammo:" << std::setw(15) << (int)currentWeaponAmmo
			<< endl;
		
		//currentItem
		//---------------------------------------
		memory::read2Byte(hProcess, currentItemAddr, currentItem);		
		string itemName;
		switch(currentItem) {
		    case 0: itemName = "nothing"; break;
		    case DN3D::ITEMID_steroids: itemName = "steroids"; break;
			case DN3D::ITEMID_jetpack:  itemName = "jetpack"; break;
			case DN3D::ITEMID_medKit:  itemName = "medpack"; break;
			case DN3D::ITEMID_holoduke: itemName = "holoduke"; break;
			case DN3D::ITEMID_nightVision:itemName = "nightVision"; break;
			default:   itemName = "?";  break;
		}
		std::cout << "Curent item:  "<<itemName<< "("<<std::dec<<currentItem <<")"<< endl;

		//CARDS
		//---------------------------------------
		memory::read1Byte(hProcess, cardsAddr, cards);
		bool cardBlue  = ((cards & 0b00000001) == 1);
		bool cardRed   = ((cards & 0b00000010) == 1);
		bool cardYellow= ((cards & 0b00000100) == 1);
		std::cout << "Cards"
			<< " blue:"   << (cardBlue ? YES : NO )
			<< " red:"    << (cardRed ? YES : NO)
			<< " yellow:" << (cardYellow ? YES : NO)
			<< "  @ 0x" << std::hex << cardsAddr << "value= 0b"<<bitset<8>(cards)
			<< endl;

		//WEAPONS
		//---------------------------------------
		ReadProcessMemory(hProcess, (LPVOID)weaponAmmoAddr,  &weaponsAmmo,    sizeof(weaponsAmmo),    &stBytes);
		ReadProcessMemory(hProcess, (LPVOID)weaponEnableAddr,&weaponsEnabled, sizeof(weaponsEnabled), &stBytes);

		printWeapons(
			weaponAmmoAddr,    weaponsAmmo,  //ammo
			weaponEnableAddr,  weaponsEnabled//enabled
		);

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

			ReadProcessMemory(hProcess, (LPVOID)enemie1Addr, &enemie, sizeof(enemie), &stBytes);
			if (idx == enemie1Addr) {
				std::cout << ">";
				if (GetKeyState(VK_NUMPAD5) & 0x8000) {
					enemie.type = 0;
					WriteProcessMemory(hProcess, (LPVOID)enemie1Addr, &enemie, sizeof(enemie), &stBytes);
				}
			}
			if (GetKeyState(VK_NUMPAD6) & 0x8000) {
				enemie.type = 0;
				WriteProcessMemory(hProcess, (LPVOID)enemie1Addr, &enemie, sizeof(enemie), &stBytes);
			}

			std::cout
				<< " 0x"<<std::hex<< (enemie1Addr- hBase)<< "0x"<<(enemie1Addr)
				<< std::dec
				<< " pos: ["
				<<        std::setw(w) << (UINT)enemie.posZ
				<< "," << std::setw(w) << (UINT)enemie.posX
				<< "," << std::setw(w) << (UINT)enemie.posY
		        << "]"
				<< " rot:" << std::setw(w) << (UINT)enemie.rotateZ
				<< " wtf1:" << std::setw(w) << (UINT)enemie.wtf1
				<< " type" << std::setw(w) << (UINT)enemie.type
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

		/*
		{
		UINT type = enemie.type;
		if (GetKeyState(VK_NUMPAD8) & 0x8000) {			enemie.type = ++type;		}
		if (GetKeyState(VK_NUMPAD2) & 0x8000) {			enemie.type = --type;		}
		WriteProcessMemory(hProcess, (LPVOID)enemie1Addr, &enemie, sizeof(enemie), &stBytes);
		}
		*/

		if (GetKeyState(VK_NUMPAD7) & 0x8000) { idx += sizeof(enemie); }
		if (GetKeyState(VK_NUMPAD1) & 0x8000) { idx -= sizeof(enemie); }

		if (GetKeyState(VK_NUMPAD8) & 0x8000) { start += 50; idx += sizeof(enemie)*50; }
		if (GetKeyState(VK_NUMPAD2) & 0x8000) { start -= 50; idx -= sizeof(enemie)*50; }

		updateKeyboard(
			health,
			armor,
			currentItem,
			weaponsEnabled,weaponsAmmo);

		Sleep(250);
	}

	std::cout << "END";
	CloseHandle(hProcess);
	CorsairKeyboard::close();
	
    return 0;
}

