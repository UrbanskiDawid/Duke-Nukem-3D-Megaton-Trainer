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

using namespace std;

HANDLE hProcess;//handle to game proces
UINT_PTR hBase;//base addres of game module

namespace DN3D {

	//note: https://github.com/fabiensanglard/chocolate_duke3D/blob/ef372086621d1a55be6dead76ae70896074ac568/Game/src/duke3d.h

	//note: moveactors (headspritestat[1])
    //https://github.com/fabiensanglard/chocolate_duke3D/blob/ef372086621d1a55be6dead76ae70896074ac568/Game/src/actors.c#L3139

	//note: moveexplosions (headspritestat[5])
	//https://github.com/fabiensanglard/chocolate_duke3D/blob/ef372086621d1a55be6dead76ae70896074ac568/Game/src/actors.c#L4395

	//note: moveeffectors (headspritestat[3]) note boss is effector:P
    //https://github.com/fabiensanglard/chocolate_duke3D/blob/ef372086621d1a55be6dead76ae70896074ac568/Game/src/actors.c#L4928

	//=================================================================
	const UINT_PTR Xpos_offset = 0x15CBD74;//duke3d.exe + 15CBD74 size 2bytes
	//const UINT_PTR X??_offset              = 0x15CBD76;// 2bytes
	const UINT_PTR Ypos_offset = 0x15CBD78;//duke3d.exe + 15CBD78 size 2bytes
	//const UINT_PTR Y??_offset              = 0x15CBD7A;// 2bytes
	const UINT_PTR Zpos_offset = 0x15CBD7C;//duke3d.exe + 15CBD7C size 2bytes
	//const UINT_PTR Z??_offset              = 0x15CBD7E;// 2bytes

	const UINT_PTR PLAYERPOS_offset = 0x15CBD74;
	struct sPlayerPos {
		int Xpos;
		int wtf1;//65535(default) or 0
		int Ypos;
		int wtf2;//65535 or 0 (default)
		int Zpos;
		int wtf3;//65535(default) or 0
	};
	//===========================================================================================

	struct sEnemie {
		uint32_t posZ;    //4B
		uint32_t posX;    //4B
		uint32_t posY;    //4B
		uint8_t  rotateZ; //1B
		uint8_t  wtf1;    //1B

//source: https://github.com/fabiensanglard/chocolate_duke3D/blob/ef372086621d1a55be6dead76ae70896074ac568/Game/src/names.h#L27
		uint16_t type;    //2B 
//630 ladydown,869 geisha nude,1296 lady up,1313striperA,1317striperB,1321lady, 1323ladyup2, 1325ladydance, 1334,ladysit, 1395ladyDance2,1680 pig
		uint8_t  wtf2;    //1B
		uint8_t  color;   //1B //version?
		uint8_t  wtf3;    //1B
		uint8_t  wtf4;    //1B
		uint8_t  width;   //1B
		uint8_t  height;  //1B
		uint8_t wtf[20];  //20B
		 int16_t health;  //2B  //-32768 - 0 -32767 (if <0 can't kill)
		 //sizeof 44B
	};

	const UINT POSA_offset              =   0x15CBDEC;//duke3d.exe + 15CBDEC size 2bytes (rotation)
	const UINT POS_ZY                   =   0x15CBDB4;//duke3d.exe + 15CBDB4 size 2bytes
	const UINT STATS_TIME               =   0x15CBDC4;//duke3d.exe + 15CBDC4 size 4bytes (starts at 0 and goes up) [value 26= 1sec]
	const UINT STATS_KILLS_CURR         =   0x15CBF08;//duke3d.exe + 15CBF08      size 4 byte
	const UINT STATS_KILLS_MAX          =   0x15CBF04;//duke3d.exe + 15CBF08 - 4 size 4 byte
	const UINT STATS_SECRETS_CURR       =   0x15CBF00;//duke3d.exe + 15CBF08 - 8
	const UINT STATS_SECRETS_MAX        =   0x15CBEFC;//duke3d.exe + 15CBF08 - C	
	const UINT POS_ONGROUND             =   0x15CBEB2;//duke3d.exe + 15CBEB2 size 1byte
	const UINT HEALTH_Offset            =   0x15CBDF6;//22855158 //duke3d.exe + 0x15CBDF6 size 1byte
/*												0x15CBDF7
												0x15CBDF8
												0x15CBDF9
												0x15CBDFA
												0x15CBDFB
												0x15CBDFC
												0x15CBDFD
												0x15CBDFE
												0x15CBDFF
												0x15CBDFF
												0x15CBE00
												0x15CBE01
												0x15CBE02
												0x15CBE03
												0x15CBE04
												0x15CBE05
												0x15CBE06
												0x15CBE07
												0x15CBE08
												0x15CBE09
												0x15CBE10
												0x15CBE11
												0x15CBE12
												0x15CBE13
												0x15CBE14
												0x15CBE15
												0x15CBE16
												0x15CBE17 */
	const UINT_PTR CURRENTWEAPONID_Offset   =   0x15CBE18;//22855192 //duke3d.exe + 0x15CBE18 size 1byte
	const UINT_PTR ARMOR_Offset             = 0x15CBE96;//22855318 //duke3d.exe + 0x15CBE96 size 1byte
	const UINT_PTR WEAPON_ammoOffset        = 0x15CBDFC;//22855164 //duke3d.exe + 0x15CBDFC size  9 bytes = [9x byte(1byte)]
	//whats here?0x15CBEF0 - 0x15CBDFC unknown 244 bytes

	//source: https://github.com/fabiensanglard/chocolate_duke3D/blob/ef372086621d1a55be6dead76ae70896074ac568/Game/src/game.c#L6553
	//ps[myconnectindex].got_access =              7;
	const UINT_PTR CARDS_offset             = 0x15CBE3C;//22855228 size 1 byte [8 bits: 0,0,0,0,0,Y,R,B B=blue,R=red,Y=yellow]

	const UINT_PTR WEAPON_enableOffset      = 0x15CBEDE;//22855390 //duke3d.exe + 0x15CBEDE size 18 bytes = [9x short(2bytes)]
	const UINT_PTR CURRENTWEAPONAMMO_Offset = 0x15F1BCA;//23010250 //duke3d.exe + 0x15F1BCA size 1byte

	const UINT_PTR CAMERAYrot_Offset        = 0x15CBD80;//	duke3d.exe + 15CBD80 size 2byte
	const UINT_PTR CAMERAXrot_Offset        = 0x15CBDEC;//	duke3d.exe + 15CBDEC size 2byte
	
	const UINT_PTR playerXv = 0x15CBDAE;// duke3d.exe + 15CBDAE
	const UINT_PTR playerYv = 0x15CBDB2;// duke3d.exe + 15CBDAE
	const UINT_PTR playerZv = 0x15CBDB6;// duke3d.exe + 15CBDAE
	//=================================================================

	const string WEAPON_NAMES[] = { "mighty leg", "pistol","shotgun","ripper","RPG","pipebomb","shrinker","devastator","laserTripbomb","freezeThrower" };

	struct sWeaponsAmmo{
		short pistol;		//+0x2
		short shotgun;      //+0x4
		short ripper;       //+0x6
		short RPG;          //+0x8
		short pipebomb;     //+0xA
		short shrinker;     //+0xC
		short devastator;   //+0xD
		short laserTripbomb;//+0xE
		short freezeThrower;//+0x10
	};

	struct sWeaponsEnabled {
		byte pistol;
		byte shotgun;
		byte ripper;
		byte RPG;
		byte pipebomb;
		byte shrinker;
		byte devastator;
		byte laserTripbomb;
		byte freezeThrower;
	};

//items
//source: https://github.com/fabiensanglard/chocolate_duke3D/blob/ef372086621d1a55be6dead76ae70896074ac568/Game/src/game.c#L6264
/*
	ps[myconnectindex].ammo_amount[GROW_WEAPON] = 50;

	ps[myconnectindex].steroids_amount = 400;
	ps[myconnectindex].heat_amount = 1200;
	ps[myconnectindex].boot_amount = 200;
	ps[myconnectindex].shield_amount = 100;
	ps[myconnectindex].scuba_amount = 6400;
	ps[myconnectindex].holoduke_amount = 2400;
	ps[myconnectindex].jetpack_amount = 1600;
	ps[myconnectindex].firstaid_amount = max_player_health;

	ps[myconnectindex].got_access = 7;
	*/
}

//=========================================================================================================================================================================

void printWeapons(
	const string *names, 
	const UINT_PTR &ammoAddr, DN3D::sWeaponsAmmo& ammo,
	const UINT_PTR &enabledAddr, DN3D::sWeaponsEnabled & enabled)
{
	short *sAmmo   = reinterpret_cast<short*>(&ammo);
	byte *sEnabled = reinterpret_cast<byte*>(&enabled);

	for (int i = 0; i <=8; i++) {
		bitset<8> flag(sEnabled[i]); bool enabled = ((int)flag[0] == 1);

		cout
			//name
			<< std::setw(15) << names[i+1/*skip leg*/] << " "
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
	cout << "looking for '" << WND_TITLE << "' window... ";
	HWND hWindow = FindWindowA(0, WND_TITLE);
	GetWindowThreadProcessId(hWindow, &processId);
	if (processId == -1)
	{
		return false;
	}
	else {
		cout << OK << endl;
	}

	//open proces
	//====================================================
	cout << "opening process PID:" << processId << " ... ";
	hProcess = OpenProcess(PROCESS_ALL_ACCESS, false, processId);
	if (hProcess == 0) {
		return false;
	}
	cout << OK << endl;

	//base adress [GetModuleHandleA("duke3d.exe"); ??]
	//====================================================
	cout << "getting base address:" << processId << " ... ";	
	HMODULE hModule;
	DWORD cbNeeded;
	if (!EnumProcessModulesEx(hProcess, &hModule, sizeof(hModule), &cbNeeded, LIST_MODULES_32BIT | LIST_MODULES_64BIT))
	{
		return false;
	}
	hBase = (UINT_PTR)hModule;
	cout << OK << " [The base is 0x" << hBase << "]" << endl; //Print the pointer
	
	//Corsair CUE (optional)
	//====================================================
	cout << "connecting to Corsair CUE ... ";
	const auto error = CorsairKeyboard::init();
	if (error) {
		cout << "Handshake failed " << CorsairKeyboard::toString(error) << std::endl;
	}else {
		CorsainKeyBoardConnected = true;
		cout << OK << endl;
	}

	return true;	//done
}

/*
 * update corsair keyboard color leds
 */
void updateKeyboard(
	int health,
	int armor,
	const DN3D::sWeaponsEnabled & weaponsEnabled,
	const DN3D::sWeaponsAmmo & weaponsAmmo)
{

	if (!CorsainKeyBoardConnected) return;

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

	
	setKeyColor(CLK_2, (weaponsEnabled.pistol ? COLOR_WHITE : COLOR_BLACK));
	setKeyColor(CLK_3, (weaponsEnabled.shotgun? COLOR_WHITE : COLOR_BLACK));
	setKeyColor(CLK_4, (weaponsEnabled.ripper ? COLOR_WHITE : COLOR_BLACK));
	setKeyColor(CLK_5, (weaponsEnabled.RPG ? COLOR_WHITE : COLOR_BLACK));
	setKeyColor(CLK_6, (weaponsEnabled.pipebomb ? COLOR_WHITE : COLOR_BLACK));
	setKeyColor(CLK_7, (weaponsEnabled.shrinker ? COLOR_WHITE : COLOR_BLACK));
	setKeyColor(CLK_8, (weaponsEnabled.devastator ? COLOR_WHITE : COLOR_BLACK));
	setKeyColor(CLK_9, (weaponsEnabled.laserTripbomb ? COLOR_WHITE : COLOR_BLACK));
	setKeyColor(CLK_0, (weaponsEnabled.freezeThrower ? COLOR_WHITE : COLOR_BLACK));
}

int main()
{
	SetConsoleTitleA(PROGRAM_NAME);
	cout << PROGRAM_NAME<<endl << endl;
	Sleep(2000);
	if (!init()) {
		cout << FAIL << endl;
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
	//====================================================

	DN3D::sWeaponsAmmo weaponsAmmo;
	DN3D::sWeaponsEnabled weaponsEnabled;
	DN3D::sPlayerPos playerPos;
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
		cout << "time: "<<std::dec << timeSec << "sec" << endl;

		//pos
		//posyAddr
		//memory::read2Byte(posyAddr, posy);
		ReadProcessMemory(hProcess, (LPVOID)playerPosAddr, &playerPos, sizeof(playerPos), &stBytes);
		cout << std::dec
			<< "  postion: "
			<< "    X" << std::setw(10) << playerPos.Xpos
			<< "    Y" << std::setw(10) << playerPos.Ypos
			<< "    Z" << std::setw(10) << playerPos.Zpos
			<< endl;

		memory::read1Byte(hProcess, cameraXrotAddr, cameraXrot);
		memory::read1Byte(hProcess, cameraYrotAddr, cameraYrot);
		cout << std::dec << " camera "
			<< " X:" << (int)cameraXrot
			<< " Y:" << (int)cameraYrot
			<< endl;

		//HEALTH
		//---------------------------------------
		memory::read1Byte(hProcess, healthAddr, health);
		
		cout << std::setw(10) << "HEALTH: " <<std::dec<< (int)health<<endl;
		if (health < 50) { memory::writeByte(hProcess, healthAddr, health+5); }

		//ARMOR
		//---------------------------------------
		memory::read1Byte(hProcess, armorAddr, armor);
		if (armor < 50) { memory::writeByte(hProcess, armorAddr, armor + 5); }
		cout << std::setw(10) << "ARMOR: " << std::dec << (int)armor<<endl;

		
		//currentWeapon
        //---------------------------------------
		memory::read1Byte(hProcess, currentWeapon_id_Addr, currentWeaponId);
		memory::read1Byte(hProcess, currentWeapon_ammo_Addr, currentWeaponAmmo);
		cout << "Curent weapon:"
			<< std::setw(15) << DN3D::WEAPON_NAMES[(int)currentWeaponId] << "(" << (int)currentWeaponId << ")" 
			<< "  ammo:" << std::setw(15) << (int)currentWeaponAmmo
			<< endl;
		
		//CARDS
		//---------------------------------------
		memory::read1Byte(hProcess, cardsAddr, cards);
		bool cardBlue  = ((cards & 0b00000001) == 1);
		bool cardRed   = ((cards & 0b00000010) == 1);
		bool cardYellow= ((cards & 0b00000100) == 1);
		cout << "Cards"
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
			DN3D::WEAPON_NAMES,
			weaponAmmoAddr,    weaponsAmmo,  //ammo
			weaponEnableAddr,  weaponsEnabled//enabled
		);

		const int w = 10;
		static INT start = 0;
		static INT_PTR idx = hBase + 0x19D9860;
		cout << "list of enemies:" << std::dec<<sizeof(DN3D::sEnemie) << "B" << endl;
		//0x26
		//Enemies
		DN3D::sEnemie enemie;
		for(int i=80+ start;i!=-1+ start;i--){
			UINT_PTR enemie1Addr = hBase + 0x19D9860 + sizeof(enemie)*i;
			if (enemie1Addr < hBase) {
				cout << "---" << endl;
				continue;
			}

			ReadProcessMemory(hProcess, (LPVOID)enemie1Addr, &enemie, sizeof(enemie), &stBytes);
			if (idx == enemie1Addr) {
				cout << ">";
				if (GetKeyState(VK_NUMPAD5) & 0x8000) {
					enemie.type = 0;
					WriteProcessMemory(hProcess, (LPVOID)enemie1Addr, &enemie, sizeof(enemie), &stBytes);
				}
			}
			if (GetKeyState(VK_NUMPAD6) & 0x8000) {
				enemie.type = 0;
				WriteProcessMemory(hProcess, (LPVOID)enemie1Addr, &enemie, sizeof(enemie), &stBytes);
			}

			cout
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
			cout <<" H: " << std::setw(w) << (UINT) enemie.health<< endl;
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

		updateKeyboard(health,armor,weaponsEnabled,weaponsAmmo);

		Sleep(250);
	}

	cout << "END";
	CloseHandle(hProcess);
	CorsairKeyboard::close();
	
    return 0;
}

