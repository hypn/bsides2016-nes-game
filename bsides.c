/*
 *
 *	By @HypnZA for BSides Cape Town 2016
 *	Using LOTS of code by Doug Fraker
 *
 *  See https://nesdoug.com/ for Doug's NES programming tutorials
 *  or http://www.hypn.za.net/blog/2016/12/05/bsides-2016-nes-game/ for more information about this game
 *
 * Game Genie Codes:
 *   AAYKAT - show "hacked" screen (challenge success)
 *   AELKIT - show megaman animation
 *   AEGKIT - show "hack the planet" screen
 *
 */

#include "DEFINE.c"

#include "BG/1.h" // title screen, called n1
#include "BG/2.h" // game screen, called n2
#include "BG/3.h" // hacked computer #1, called n3
#include "BG/4.h" // hacked computer #2, called n4
#include "BG/5.h" // hacked computer #3, called n5
#include "BG/6.h" // successfully hacked, called n6
#include "BG/7.h" // credits screen
#include "BG/8.h" // konami code screen
#include "BG/9.h" // konami code screen
#include "BG/mm1.h" // konami code screen
#include "BG/mm2.h" // konami code screen
#include "BG/mm3.h" // konami code screen
#include "BG/mm4.h" // konami code screen
#include "BG/htp.h" // konami code screen
// NOTE: the "n" numbers are 1 higher than the "screenNum" value (array offset)

const unsigned char * const screens[] = {
	n1,  // title screen
	n2,  // main game
	n3,  // computer #1
	n4,  // computer #2
	n5,  // computer #3
	n6,  // hacked screen
	n7,  // credits screen
	n8,  // hacker's manifesto #1
	n9,  // hacker's manifesto #2
	mm1, // megaman animation #1
	mm2, // megaman animation #2
	mm3, // megaman animation #3
	mm4, // megaman animation #4
	mm3, // megaman animation #3 - easier than looping backwards :P
	mm2, // megaman animation #2 - easier than looping backwards :P
	mm1, // megaman animation #1 - easier than looping backwards :P
	htp  // hack the planet screen
};

/////////////////////////////////////////
// MAGIC FUNCTIONS FROM DOUG:
/////////////////////////////////////////

void All_Off (void) {
	PPU_CTRL = 0;
	PPU_MASK = 0;
}

void All_On (void) {
	PPU_CTRL = 0x90; // screen is on, NMI on
	PPU_MASK = 0x1e;
}

void Reset_Scroll (void) {
	PPU_ADDRESS = 0;
	PPU_ADDRESS = 0;
	SCROLL = 0;
	SCROLL = 0;
}

void Load_Palette (void) {
	PPU_ADDRESS = 0x3f;
	PPU_ADDRESS = 0x00;
	for (index = 0; index < sizeof(PALETTE); ++index) {
		PPU_DATA = PALETTE[index];
	}
}

/////////////////////////////////////////
// GAME LOGIC:
/////////////////////////////////////////

void main (void) {
	All_Off(); 	// turn off screen
	Load_Palette();
	Reset_Scroll();

	// draw first screen (title) at start
	++redrawScreen;
	screenNum = 0;

	// set a starting position for the sprite (player)
	X1 = 0x84;
	Y1 = 0xD0;

	All_On(); 	// turn on screen

	gameGenieCodes(); // check for game genie codes

	while(1) {
		while (NMI_flag == 0); // wait till NMI

		rotatePalette();
		Reset_Scroll();

		// only redraw the screen when flagged is set
		if (redrawScreen != 0) {
			redrawScreen = 0;
			drawScreen();
		}

		Get_Input();

		konamiCode(); // check for the konami code

		// different logic depending on the screen the user is seeing
		if (screenNum == 0) {
			titleScreen();

		} else if (screenNum == 1) {
			gameScreen();

		} else if ((screenNum == 2) || (screenNum == 3) || (screenNum == 4)) {
			computerScreen();

		} else if (screenNum == 5) {
			hackedScreen();

		} else if (screenNum == 6) {
			creditsScreen();

		} else if ((screenNum == 7) || (screenNum == 8)) {
			if (redrawScreen == 0) {
				hackersManifestoScreen();
			}

		// start of the megaman animation
		} else if ((screenNum == 9) || (screenNum == 10) || (screenNum == 11) || (screenNum == 12) || (screenNum == 13) || (screenNum == 14)) {
			if (paletteRotateCounter > 5) {
				paletteRotateCounter = 0;
				++redrawScreen;
				++screenNum;
			}

		// end of megaman animation - loop back to the start
		} else if (screenNum == 15) {
			megaManStartScreen();
		}

		NMI_flag = 0;
	}
}

/////////////////////////////////////////
// LOWER LEVEL CODE
/////////////////////////////////////////

void drawScreen(void) {
	All_Off();
	PPU_ADDRESS = 0x20;
	PPU_ADDRESS = 0x00;

	UnRLE(screens[screenNum]); // uncompresses our data, send to PPU

	Wait_Vblank(); // don't turn on screen until in v-blank
	All_On();
	Reset_Scroll();
}

void rotatePalette(void) {
	// this function became a bit of a mess, and this logic should probably be split in to a function for each screen
	++paletteRotateCounter;

	PPU_ADDRESS = 0x3f;
	PPU_ADDRESS = 0x0b;
	PPU_DATA = 0x26;

	// title screen, rotate through orange/red values
	if (screenNum == 0) {
		Load_Palette();

		PPU_ADDRESS = 0x3f;
		PPU_ADDRESS = 0x0b;

		if (paletteRotateCounter < 10) {
			PPU_DATA = 0x26;

		} else if (paletteRotateCounter < 20) {
			PPU_DATA = 0x27;

		} else {
			PPU_DATA = 0x16;
			paletteRotateCounter = 0;
		}

	// main room, rotate through computer screen colours
	} else if (screenNum == 1) {
		PPU_ADDRESS = 0x3f;
		PPU_ADDRESS = 0x0f;

		if (paletteRotateCounter > 25) {
			PPU_DATA = 0x0f;
		} else {
			PPU_DATA = 0x16;
		}

		if (paletteRotateCounter > 60) {
			paletteRotateCounter = 0;
		}

	// first two computer screens - push red in to the palette
	} else if ((screenNum == 2) || (screenNum == 3)) {
		// Set red
		PPU_ADDRESS = 0x3f;
		PPU_ADDRESS = 0x0f;
		PPU_DATA = 0x06;

	// last computer screen and hacked screen (game genie palette rotation)
	} else if ((screenNum == 4) || (screenNum == 5)) {
		// Set red
		PPU_ADDRESS = 0x3f;
		PPU_ADDRESS = 0x0f;
		PPU_DATA = 0x06;

		PPU_ADDRESS = 0x3f;
		PPU_ADDRESS = 0x03;

		if (paletteRotateCounter < 8) {
			PPU_DATA = 0x30;
		} else if (paletteRotateCounter < 16) {
			PPU_DATA = 0x3d;
		} else if (paletteRotateCounter < 24) {
			PPU_DATA = 0x24;
		} else if (paletteRotateCounter < 32) {
			PPU_DATA = 0x26;
		} else if (paletteRotateCounter < 40) {
			PPU_DATA = 0x31;
		} else if (paletteRotateCounter < 48) {
			PPU_DATA = 0x2a;
		} else if (paletteRotateCounter < 56) {
			PPU_DATA = 0x2c;
		} else {
			PPU_DATA = 0x2c;
			paletteRotateCounter = 0;
		}

	// credits screen - default palette
	} else if (screenNum == 6) {
		Load_Palette();

		PPU_ADDRESS = 0x3f;
		PPU_ADDRESS = 0x0e;
		PPU_DATA = 0x00;

	// hacker's manifesto screens
	} else if ((screenNum == 7) || (screenNum == 8)) {
		Load_Palette();

		// hide skull jaws
		PPU_ADDRESS = 0x3f;
		PPU_ADDRESS = 0x0a;
		PPU_DATA = 0x0f;
		PPU_ADDRESS = 0x3f;
		PPU_ADDRESS = 0x09;
		PPU_DATA = 0x00f;

		// blink skulls
		if (paletteRotateCounter > 45) {
			PPU_ADDRESS = 0x3f;
			PPU_ADDRESS = 0x0b;
			PPU_DATA = 0x10;

		} else if (paletteRotateCounter > 30) {
			PPU_ADDRESS = 0x3f;
			PPU_ADDRESS = 0x0b;
			PPU_DATA = 0x20;

		} else if (paletteRotateCounter > 15) {
			PPU_ADDRESS = 0x3f;
			PPU_ADDRESS = 0x0b;
			PPU_DATA = 0x10;

		} else  {
			PPU_ADDRESS = 0x3f;
			PPU_ADDRESS = 0x0b;
			PPU_DATA = 0x00;
		}

		if (paletteRotateCounter > 60) {
			paletteRotateCounter = 0;
		}

	} else if (screenNum == 16) {
		Load_Palette();

		// animate skull jaws
		if (paletteRotateCounter > 25) {
			PPU_ADDRESS = 0x3f;
			PPU_ADDRESS = 0x05;
			PPU_DATA = 0x30;

			PPU_ADDRESS = 0x3f;
			PPU_ADDRESS = 0x06;
			PPU_DATA = 0x0f;

		} else {
			PPU_ADDRESS = 0x3f;
			PPU_ADDRESS = 0x05;
			PPU_DATA = 0x0f;

			PPU_ADDRESS = 0x3f;
			PPU_ADDRESS = 0x06;
			PPU_DATA = 0x30;
		}

		if (paletteRotateCounter > 60) {
			paletteRotateCounter = 0;
		}
	}
}

void drawSprites(void) {
	state4 = state << 2; // shift left 2 = multiply 4
	index4 = 0;
	for (index = 0; index < 4; ++index ){
		SPRITES[index4] = MetaSprite_Y[index] + Y1; // relative y + master y
		++index4;
		SPRITES[index4] = MetaSprite_Tile[index + state4]; // tile numbers
		++index4;
		SPRITES[index4] = MetaSprite_Attrib[index]; // attributes, all zero here
		++index4;
		SPRITES[index4] = MetaSprite_X[index] + X1; // relative x + master x
		++index4;
	}
}

void moveSprite(void) {
	unsigned int old_X1 = X1;
	unsigned int old_Y1 = Y1;

	if ((joypad1 & RIGHT) != 0) {
		state = Going_Right;
		if (X1 < 224) {
			++X1;
		}
	}
	if ((joypad1 & LEFT) != 0){
		state = Going_Left;

		if (X1 > 16) {
			--X1;
		}
	}
	if ((joypad1 & DOWN) != 0){
		if (Y1 < 206) {
			state = Going_Down;
			++Y1;
		}
	}
	if ((joypad1 & UP) != 0){
		state = Going_Up;
		if (Y1 > 22) {
			--Y1;
		}
	}

	//////////////////////////////////////////////////////////
	// Collision detection (rollback movement on collision)
	/////////////////////////////////////////////////////////

	// top island
	if ((X1 > 129) && (X1 < 175) && (Y1 < 56)) {
		X1 = old_X1;
		Y1 = old_Y1;
	}

	// left room
	if ((X1 > 33) && (X1 < 127) && (Y1 > 30) && (Y1 < 150)) {
		if ((Y1 < 96) && (Y1 > 86) && (X1 < 96)) {
			// doorway, allow movement

		} else if ((X1 > 63) && (X1 < 96) && (Y1 > 70) && (Y1 < 111)) {
			// room and computer terminal, allow movement

		} else {
			X1 = old_X1;
			Y1 = old_Y1;
		}
	}

	// right room
	if ((X1 > 112) && (X1 < 223) && (Y1 > 63) && (Y1 < 187)) {
		if ((Y1 > 119) && (Y1 < 128) && (X1 > 143)) {
			// doorway, allow movement

		} else if ((X1 > 143) && (X1 < 193) && (Y1 > 102) && (Y1 < 159)) {
			// room and computer terminal, allow movement

		} else {
			X1 = old_X1;
			Y1 = old_Y1;
		}
	}

	// middle-left island
	if ((X1 > 17) && (X1 < 48) && (Y1 < 168) && (Y1 > 110)) {
		X1 = old_X1;
		Y1 = old_Y1;
	}

	// bottom-left island
	if ((X1 > 32) && (X1 < 80) && (Y1 > 175)) {
		X1 = old_X1;
		Y1 = old_Y1;
	}

	// bottom-right island
	if ((X1 > 144) && (X1 < 192) && (Y1 > 190)) {
		X1 = old_X1;
		Y1 = old_Y1;
	}
}

/////////////////////////////////////////
// SCREENS:
/////////////////////////////////////////

void titleScreen(void) {
	if ( ((joypad1 & START) != 0) && ((joypad1old & START) == 0) ) {
		// show game
		screenNum = 1;
		++redrawScreen;
	}

	if (konamiCodeStep < 8) {
		// if user presses the A or B button
		if (
			(((joypad1 & A_BUTTON) != 0) && ((joypad1old & A_BUTTON) == 0)) ||
			(((joypad1 & B_BUTTON) != 0) && ((joypad1old & B_BUTTON) == 0))
		) {
			// show credits screen
			previousScreenNum = 0;
			screenNum = 6;
			++redrawScreen;
		}
	}
}

void gameScreen(void) {
	moveSprite();
	useComputer();
	drawSprites();
}

void computerScreen(void) {
	dismissComputer();
	Blank_sprite();
}

void nextComputerScreen(void) {
	numComputersUsed = numComputersUsed + 1;
	if (numComputersUsed > 3) {
		numComputersUsed = 3;
	}
}

void hackedScreen(void) {
	// if user presses the A or B button
	if (
		(((joypad1 & A_BUTTON) != 0) && ((joypad1old & A_BUTTON) == 0)) ||
		(((joypad1 & B_BUTTON) != 0) && ((joypad1old & B_BUTTON) == 0))
	) {
		// show credits screen
		previousScreenNum = 5;
		++redrawScreen;
		screenNum = 6;
	}
}

void creditsScreen(void) {
	// if user presses the A or B button
	if (
		(((joypad1 & A_BUTTON) != 0) && ((joypad1old & A_BUTTON) == 0)) ||
		(((joypad1 & B_BUTTON) != 0) && ((joypad1old & B_BUTTON) == 0))
	) {
		// show previous screen (title/hacked screen)
		++redrawScreen;
		screenNum = previousScreenNum;
	}
}

void hackersManifestoScreen(void) {
	// if user presses the A or B button
	if (
		(((joypad1 & A_BUTTON) != 0) && ((joypad1old & A_BUTTON) == 0)) ||
		(((joypad1 & B_BUTTON) != 0) && ((joypad1old & B_BUTTON) == 0))
	) {
		// show page 2 of the manifesto
		if (screenNum == 7) {
			++redrawScreen;
			screenNum = 8;

		} else if (screenNum == 8) {
			++redrawScreen;
			screenNum = 0;
		}
	}
}

void megaManStartScreen(void) {
	Load_Palette();

	// setup white, black, darkblue and teal in our 1st palette:

	PPU_ADDRESS = 0x3f;
	PPU_ADDRESS = 0x00;
	PPU_DATA = 0x36;

	PPU_ADDRESS = 0x3f;
	PPU_ADDRESS = 0x01;
	PPU_DATA = 0x7f;

	PPU_ADDRESS = 0x3f;
	PPU_ADDRESS = 0x02;
	PPU_DATA = 0x11;

	PPU_ADDRESS = 0x3f;
	PPU_ADDRESS = 0x03;
	PPU_DATA = 0x2c;

	++redrawScreen;
	screenNum = 9;
}


/////////////////////////////////////////
// USER ACTIONS:
/////////////////////////////////////////

void gameGenieCodes(void) {
	// GAME GENIE CODE LOGIC:
	// all of these "if"s will (normally) fail, but are used to trigger (and find memory offsets for) game genie codes
	// to create a Game Genie code, do the following:
	// 1. compile the game as is, and save the .nes file under a name (eg: "before.nes")
	// 2. change the relevant "if" to check for a "2" instead of a "1" (a "0" results in the output (logic?) changing quite a lot)
	// 3. compile the game again, and save the .nes under a different name (eg: "after.nes")
	// 4. use a hex-diff program (eg: Fairdell HexCmp2) to compare the 2 files
	// 5. find the hex offset of the (hopefully only) difference in the two files
	// 6. subtract 0x10 from the address (for the file header which is not loaded in to the game's memory)
	// 7. add 0x4000 - because of ROM size and where the Game Genie expects the ROM to start (from Doug!)
	// 8. use a Game Genie code generator, eg: http://www.d.umn.edu/~bold0070/projects/game_genie_codes/javascript_game_genie_encoders-decoders.html
	// 9. enter the addres from step 7
	// 10. enter the value of "0" (so now the "if" will read "if (gameGenieCode == 0) {" - making it true, and execute)
	// 11. generate and test the Game Genie code! :D

	// // this "if" is just used to nudge some code offsets along, to produce better game genie codes for the other "if"s
	// if (gameGenieCode == 1) {
	// 	gameGenieCode = 2;
	// }

	unsigned int temp = 0;
	if (temp == 1) { screenNum = 1; } 		 // show the "intro" screen
	if (temp == 1) { screenNum = 2; } 		 // show the game screen
	if (temp == 1) { screenNum = 3; } 		 // show the "computer #1" screen
	if (temp == 1) { screenNum = 4; } 		 // show the "computer #2" screen
	if (temp == 1) { screenNum = 5; } 		 // show the "hacked" (success) screen
	if (temp == 1) { screenNum = 6; } 		 // show the "credits" screen
	if (temp == 1) { screenNum = 7; } 		 // show the "hacker's manifesto #1" screen
	if (temp == 1) { screenNum = 8; } 		 // show the "hacker's manifesto #2" screen
	if (temp == 1) { megaManStartScreen(); } // show the mega man animation (screen 9)
	if (temp == 1) { screenNum = 16; } 		 // show the "hack the planet" screen

	if (previousScreenNum != screenNum) {
		previousScreenNum = screenNum;
		++redrawScreen;
	}
}

void konamiCode(void) {
	// ↑ ↑ ↓ ↓ ← → ← → B A
	// on any keypress, either progress the konami code progress or reset it
	if (
		(((joypad1 & UP) != 0) && ((joypad1old & UP) == 0)) ||
		(((joypad1 & DOWN) != 0) && ((joypad1old & DOWN) == 0)) ||
		(((joypad1 & LEFT) != 0) && ((joypad1old & LEFT) == 0)) ||
		(((joypad1 & RIGHT) != 0) && ((joypad1old & RIGHT) == 0)) ||
		(((joypad1 & B_BUTTON) != 0) && ((joypad1old & B_BUTTON) == 0)) ||
		(((joypad1 & A_BUTTON) != 0) && ((joypad1old & A_BUTTON) == 0))
	) {

		// up up
		if ((konamiCodeStep < 2) && ((joypad1 & UP) != 0) && ((joypad1old & UP) == 0)) {
			++konamiCodeStep;

		// down down
		} else if ((konamiCodeStep < 4) && ((joypad1 & DOWN) != 0) && ((joypad1old & DOWN) == 0)) {
			++konamiCodeStep;

		// left (right) left
		} else if (((konamiCodeStep == 4) || (konamiCodeStep == 6)) && ((joypad1 & LEFT) != 0) && ((joypad1old & LEFT) == 0)) {
			++konamiCodeStep;

		// (left) right (left) right
		} else if (((konamiCodeStep == 5) || (konamiCodeStep == 7)) && ((joypad1 & RIGHT) != 0) && ((joypad1old & RIGHT) == 0)) {
			++konamiCodeStep;

		// B
		} else if ((konamiCodeStep == 8) && ((joypad1 & B_BUTTON) != 0) && ((joypad1old & B_BUTTON) == 0)) {
			++konamiCodeStep;
		// A
		} else if ((konamiCodeStep == 9) && ((joypad1 & A_BUTTON) != 0) && ((joypad1old & A_BUTTON) == 0)) {
			++konamiCodeStep;

		// mistake
		} else {
			konamiCodeStep = 0;
		}
	}

	if (konamiCodeStep == 10) {
		konamiCodeStep = 0;
		++redrawScreen;
		screenNum = 7;

		Blank_sprite();
	}
}

void useComputer(void) {
	// if user presses the A or B button
	if (
		(((joypad1 & A_BUTTON) != 0) && ((joypad1old & A_BUTTON) == 0)) ||
		(((joypad1 & B_BUTTON) != 0) && ((joypad1old & B_BUTTON) == 0))
	) {
		// try and use a computer

		// check bottom-most computer
		if ( (Y1 > 144) && (Y1 < 158) && (X1 > 89) && (X1 < 103) ) {
			++redrawScreen;
			if (computer1ScreenNum != 0) {
				screenNum = computer1ScreenNum;
			} else {
				nextComputerScreen();
				screenNum = numComputersUsed + 1;
				computer1ScreenNum = screenNum;
			}

		// check right-most computer
		} else if ((Y1 > 97) && (Y1 < 110) && (X1 > 169) && (X1 < 184)) {
			++redrawScreen;
			if (computer2ScreenNum != 0) {
				screenNum = computer2ScreenNum;
			} else {
				nextComputerScreen();
				screenNum = numComputersUsed + 1;
				computer2ScreenNum = screenNum;
			}

		// check left-most computer
		} else if ((Y1 > 64) && (Y1 < 79) && (X1 > 74) && (X1 < 86)) {
			++redrawScreen;
			if (computer3ScreenNum != 0) {
				screenNum = computer3ScreenNum;
			} else {
				nextComputerScreen();
				screenNum = numComputersUsed + 1;
				computer3ScreenNum = screenNum;
			}

		// check top computers
		} else if ((Y1 > 16) && (Y1 < 31)) {

			// top left computer
			if ((X1 > 56) && (X1 < 71)) {
				++redrawScreen;
				if (computer4ScreenNum != 0) {
					screenNum = computer4ScreenNum;
				} else {
					nextComputerScreen();
					screenNum = numComputersUsed + 1;
					computer4ScreenNum = screenNum;
				}

			// top right computer
			} else if ((X1 > 201) && (X1 < 216)) {
				++redrawScreen;
				if (computer5ScreenNum != 0) {
					screenNum = computer5ScreenNum;
				} else {
					nextComputerScreen();
					screenNum = numComputersUsed + 1;
					computer5ScreenNum = screenNum;
				}
			}
		}
	}
}

void dismissComputer(void) {
	// if user presses the A or B button
	if (
		(((joypad1 & A_BUTTON) != 0) && ((joypad1old & A_BUTTON) == 0)) ||
		(((joypad1 & B_BUTTON) != 0) && ((joypad1old & B_BUTTON) == 0))
	) {
		++redrawScreen;
		screenNum = 1;
	}
}
