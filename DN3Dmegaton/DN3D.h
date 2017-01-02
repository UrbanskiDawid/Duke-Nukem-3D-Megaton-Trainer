#pragma once
#include <windows.h>
#include <stdlib.h>

namespace DN3D {

	/*
	note: https://github.com/fabiensanglard/chocolate_duke3D/blob/ef372086621d1a55be6dead76ae70896074ac568/Game/src/duke3d.h

	note: moveactors (headspritestat[1])
	https://github.com/fabiensanglard/chocolate_duke3D/blob/ef372086621d1a55be6dead76ae70896074ac568/Game/src/actors.c#L3139

	note: moveexplosions (headspritestat[5])
	https://github.com/fabiensanglard/chocolate_duke3D/blob/ef372086621d1a55be6dead76ae70896074ac568/Game/src/actors.c#L4395

	note: moveeffectors (headspritestat[3]) note boss is effector:P
	https://github.com/fabiensanglard/chocolate_duke3D/blob/ef372086621d1a55be6dead76ae70896074ac568/Game/src/actors.c#L4928

	//items amounts:
	source: https://github.com/fabiensanglard/chocolate_duke3D/blob/ef372086621d1a55be6dead76ae70896074ac568/Game/src/game.c#L6264

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

	const char* WEAPON_NAMES[] = { "mighty leg", "pistol","shotgun","ripper","RPG","pipebomb","shrinker","devastator","laserTripbomb","freezeThrower" };

	//note: BUILD engine sprite
	//https://github.com/fabiensanglard/chocolate_duke3D/blob/ef372086621d1a55be6dead76ae70896074ac568/Engine/src/build.h#L124
	struct sSprite {
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
		uint8_t  wtf[20]; //20B
		int16_t  health;  //2B  //-32768 - 0 -32767 (if <0 can't kill)
						  //sizeof 44B
	};

	const UINT POSA_offset = 0x15CBDEC;//duke3d.exe + 15CBDEC size 2bytes (rotation)
	const UINT POS_ZY = 0x15CBDB4;//duke3d.exe + 15CBDB4 size 2bytes
	const UINT STATS_TIME = 0x15CBDC4;//duke3d.exe + 15CBDC4 size 4bytes (starts at 0 and goes up) [value 26= 1sec]
	const UINT STATS_KILLS_CURR = 0x15CBF08;//duke3d.exe + 15CBF08 size 4 byte
	const UINT STATS_KILLS_MAX = 0x15CBF04;//duke3d.exe + 15CBF08 - 4 size 4 byte
	const UINT STATS_SECRETS_CURR = 0x15CBF00;//duke3d.exe + 15CBF08 - 8
	const UINT STATS_SECRETS_MAX = 0x15CBEFC;//duke3d.exe + 15CBF08 - C	
	const UINT POS_ONGROUND = 0x15CBEB2;//duke3d.exe + 15CBEB2 size 1byte
	const UINT PLAYERPOS_offset = 0x15CBD74;//duke3d.exe + 15CBD74 size 2bytes
	struct sPlayerPos {
		int Xpos;//0x15CBD74
		int wtf1;//0x15CBD76  65535(default) or 0
		int Ypos;//0x15CBD78
		int wtf2;//0x15CBD7A  65535 or 0 (default)
		int Zpos;//0x15CBD7C
		int wtf3;//0x15CBD7E  65535(default) or 0
	};
	const UINT CAMERAYrot_Offset = 0x15CBD80;//duke3d.exe + 15CBD80 size 2byte
	const UINT CAMERAXrot_Offset = 0x15CBDEC;//duke3d.exe + 15CBDEC size 2byte
	const UINT playerXv = 0x15CBDAE;//duke3d.exe + 15CBDAE
	const UINT playerYv = 0x15CBDB2;//duke3d.exe + 15CBDAE
	const UINT playerZv = 0x15CBDB6;//duke3d.exe + 15CBDAE
	const UINT HEALTH_Offset = 0x15CBDF6;//duke3d.exe + 0x15CBDF6 size 1byte
	const UINT WEAPON_ammoOffset = 0x15CBDFA;//duke3d.exe + 0x15CBDFC size 29 bytes = [10x 2byte]
	struct sWeaponsAmmo {
		short leg;          //0x15CBDFA
		short pistol;		//0x15CBDFC
		short shotgun;      //0x15CBDFE
		short ripper;       //0x15CBE00
		short RPG;          //0x15CBE02
		short pipebomb;     //0x15CBE04
		short shrinker;     //0x15CBE06
		short devastator;   //0x15CBE08
		short laserTripbomb;//0x15CBE0A
		short freezeThrower;//0x15CBE0B
	};
	const UINT CURRENTWEAPONID_Offset = 0x15CBE18;//duke3d.exe + 0x15CBE18 size 1byte
												  //source: https://github.com/fabiensanglard/chocolate_duke3D/blob/ef372086621d1a55be6dead76ae70896074ac568/Game/src/game.c#L6553
												  //ps[myconnectindex].got_access =              7;
	const UINT CARDS_offset = 0x15CBE3C;//duke3d.exe + 0x15CBE3C size 1 byte [8 bits: 0,0,0,0,0,Y,R,B B=blue,R=red,Y=yellow]
	const UINT ARMOR_Offset = 0x15CBE96;//duke3d.exe + 0x15CBE96 size 1byte
	const UINT WEAPON_enableOffset = 0x15CBEDD;//duke3d.exe + 0x15CBEDE size 18 bytes = [9x short(2bytes)]
	struct sWeaponsEnabled {
		byte leg;           //0x15CBEDE
		byte pistol;        //0x15CBEDF
		byte shotgun;       //0x15CBEE0
		byte ripper;        //0x15CBEE1
		byte RPG;           //0x15CBEE2
		byte pipebomb;      //0x15CBEE3
		byte shrinker;      //0x15CBEE4
		byte devastator;    //0x15CBEE5
		byte laserTripbomb; //0x15CBEE6
		byte freezeThrower; //0x15CBEE7
	};
	const UINT CURRENTWEAPONAMMO_Offset = 0x15F1BCA;//duke3d.exe + 0x15F1BCA size 1byte

}
