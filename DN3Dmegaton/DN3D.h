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

	note: damage calculation
	struct weaponhit
	https://github.com/fabiensanglard/chocolate_duke3D/blob/ef372086621d1a55be6dead76ae70896074ac568/Game/src/duke3d.h#L568

	https://github.com/fabiensanglard/chocolate_duke3D/blob/ef372086621d1a55be6dead76ae70896074ac568/Game/src/global.c#L76
	struct weaponhit hittype[MAXSPRITES];

	note: sprite array
	https://github.com/fabiensanglard/chocolate_duke3D/blob/ef372086621d1a55be6dead76ae70896074ac568/Engine/src/build.h#L141
	EXTERN spritetype sprite[MAXSPRITES];
	*/

	/*note: inventory items
	    case FIRSTAID:
        case STEROIDS:
        case HEATSENSOR:
        case BOOTS:
        case JETPACK:
        case HOLODUKE:
        case AIRTANK:
    */
	const UINT ITEMID_steroids = 512;
	const UINT ITEMID_jetpack = 1024;
	const UINT ITEMID_holoduke = 768;
	const UINT ITEMID_nightVision = 1280;
	const UINT ITEMID_medKit = 256;
	const UINT ITEMID_scuba = 1536;
	
	const char* WEAPON_NAMES[] = { "mighty leg", "pistol","shotgun","ripper","RPG","pipebomb","shrinker","devastator","laserTripbomb","freezeThrower" };

	//-------------
	enum ePICID{
		SPRITE_PICID_SHOT_PIG = 1625,
		SPRITE_PICID_SHOT_OCTOPUS = 1360,
		SPRITE_PICID_SHOT_HUBEMONSTER = 4095,

		SPRITE_PICID_ENEMIE_PIG = 1680,
		SPRITE_PICID_ENEMIE_SHOTFLY = 1960,
		SPRITE_PICID_ENEMIE_MASHINGUN = 2120,
		SPRITE_PICID_ENEMIE_OCTOPUS = 1820,
		SPRITE_PICID_ENEMIE_PIG2= 2000,
		SPRITE_PICID_ENEMIE_TURRET = 2360,
		SPRITE_PICID_ENEMIE_SHARK = 1550,
		SPRITE_PICID_ENEMIE_SHARKELECTRIC = 1880,
		SPRITE_PICID_ENEMIE_SLIME = 2370,
		SPRITE_PICID_ENEMIE_HUGEMONSTER = 2630,
		SPRITE_PICID_ENEMIE_FASTMONSTER = 4610,
		SPRITE_PICID_ENEMIE_INVALIDMONSTER = 1930,
		SPRITE_PICID_TANK = 1975
	};

	const UINT SPRITE_PICID_LADY_BUSH = 1294;
	//note: BUILD engine sprite
	//https://github.com/fabiensanglard/chocolate_duke3D/blob/ef372086621d1a55be6dead76ae70896074ac568/Engine/src/build.h#L124
	/*
	* cstat:
	*   bit 0: 1 = Blocking sprite (use with clipmove, getzrange)       "B"
	*   bit 1: 1 = transluscence, 0 = normal                            "T"
	*   bit 2: 1 = x-flipped, 0 = normal                                "F"
	*   bit 3: 1 = y-flipped, 0 = normal                                "F"
	*   bits 5-4: 00 = FACE sprite (default)                            "R"
	*             01 = WALL sprite (like masked walls)
	*             10 = FLOOR sprite (parallel to ceilings&floors)
	*   bit 6: 1 = 1-sided sprite, 0 = normal                           "1"
	*   bit 7: 1 = Real centered centering, 0 = foot center             "C"
	*   bit 8: 1 = Blocking sprite (use with hitscan / cliptype 1)      "H"
	*   bit 9: 1 = Transluscence reversing, 0 = normal                  "T"
	*   bits 10-14: reserved
	*   bit 15: 1 = Invisible sprite, 0 = not invisible
	*/
	struct sSprite {
		 int32_t posX;    //4B
		 int32_t posY;    //4B
		 int32_t posZ;    //4B
		uint16_t cstat;   //2B
						  //source: https://github.com/fabiensanglard/chocolate_duke3D/blob/ef372086621d1a55be6dead76ae70896074ac568/Game/src/names.h#L27
		uint16_t picnum;  //2B 
						  //630 ladydown,869 geisha nude,1296 lady up,1313striperA,1317striperB,1321lady, 1323ladyup2, 1325ladydance, 1334,ladysit, 1395ladyDance2,1680 pig, 2000 shotgun pig
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

	const UINT POSA_offset         = 0x15CBDEC;//duke3d.exe + 15CBDEC size 2bytes (rotation)
	const UINT POS_ZY              = 0x15CBDB4;//duke3d.exe + 15CBDB4 size 2bytes
	const UINT STATS_TIME          = 0x15CBDC4;//duke3d.exe + 15CBDC4 size 4bytes (starts at 0 and goes up) [value 26= 1sec]
	const UINT STATS_KILLS_CURR    = 0x15CBF08;//duke3d.exe + 15CBF08 size 4 byte
	const UINT STATS_KILLS_MAX     = 0x15CBF04;//duke3d.exe + 15CBF08 - 4 size 4 byte
	const UINT STATS_SECRETS_CURR  = 0x15CBF00;//duke3d.exe + 15CBF08 - 8
	const UINT STATS_SECRETS_MAX   = 0x15CBEFC;//duke3d.exe + 15CBF08 - C	
	const UINT POS_ONGROUND        = 0x15CBEB2;//duke3d.exe + 15CBEB2 size 1byte
	const UINT PLAYERPOS_offset    = 0x15CBD74;//duke3d.exe + 15CBD74 size 2bytes
	struct sPlayerPos { //note:   https://github.com/fabiensanglard/chocolate_duke3D/blob/ef372086621d1a55be6dead76ae70896074ac568/Game/src/duke3d.h#L450
		int32_t Xpos;//0x15CBD74  part of struct player_struct ps[MAXPLAYERS];
		int32_t Ypos;//0x15CBD78  index of 'you': myconnectindex
		int32_t Zpos;//0x15CBD7C
	};
	
	const UINT CAMERAYrot_Offset   = 0x15CBD80;//duke3d.exe + 15CBD80 size 2byte
	const UINT CAMERAXrot_Offset   = 0x15CBDEC;//duke3d.exe + 15CBDEC size 2byte
	const UINT POS_A               = CAMERAXrot_Offset;//duke3d.exe + 15CBDEC rotation size 2byte - rotation 0-2046
	const UINT playerXv            = 0x15CBDAE;//duke3d.exe + 15CBDAE
	const UINT playerYv            = 0x15CBDB2;//duke3d.exe + 15CBDAE
	const UINT playerZv            = 0x15CBDB6;//duke3d.exe + 15CBDAE
	const UINT HEALTH_Offset       = 0x15CBDF6;//duke3d.exe + 0x15CBDF6 size 1byte
	const UINT WEAPON_ammoOffset   = 0x15CBDFA;//duke3d.exe + 0x15CBDFC size 29 bytes = [10x 2byte]
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
	const UINT CARDS_offset        = 0x15CBE3C;//duke3d.exe + 0x15CBE3C size 1 byte [8 bits: 0,0,0,0,0,Y,R,B B=blue,R=red,Y=yellow]
	const UINT ARMOR_Offset        = 0x15CBE96;//duke3d.exe + 0x15CBE96 size 1byte
	const UINT CURRENT_ITEM        = 0x15CBED2;//duke3d.exe + 0x15CBED2 size 2 bytes
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
