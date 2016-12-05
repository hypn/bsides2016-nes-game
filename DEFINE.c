// let's define some stuff

#define PPU_CTRL		*((unsigned char*)0x2000)
#define PPU_MASK		*((unsigned char*)0x2001)
#define PPU_STATUS		*((unsigned char*)0x2002)
#define OAM_ADDRESS		*((unsigned char*)0x2003)
#define SCROLL			*((unsigned char*)0x2005)
#define PPU_ADDRESS		*((unsigned char*)0x2006)
#define PPU_DATA		*((unsigned char*)0x2007)
#define OAM_DMA			*((unsigned char*)0x4014)


#define RIGHT		0x01
#define LEFT		0x02
#define DOWN		0x04
#define UP			0x08
#define START		0x10
#define SELECT		0x20
#define B_BUTTON	0x40
#define A_BUTTON	0x80


// Globals
// our startup code initialized all values to zero
#pragma bss-name(push, "ZEROPAGE")
unsigned char NMI_flag;
unsigned char Frame_Count;
unsigned char index;
unsigned char index4;
unsigned char X1;
unsigned char Y1;
unsigned char state;
unsigned char state4;
unsigned char joypad1;
unsigned char joypad1old;
unsigned char joypad1test;
unsigned char joypad2;
unsigned char joypad2old;
unsigned char joypad2test;
unsigned char redrawScreen;
unsigned char screenNum = 0;
unsigned char computer1ScreenNum = 0;
unsigned char computer2ScreenNum = 0;
unsigned char computer3ScreenNum = 0;
unsigned char computer4ScreenNum = 0;
unsigned char computer5ScreenNum = 0;
unsigned char previousScreenNum = 0;
unsigned char numComputersUsed = 0;
unsigned char konamiCodeStep = 0;
unsigned char paletteRotateCounter = 0;

#pragma bss-name(push, "OAM")
unsigned char SPRITES[256];
// OAM equals ram addresses 200-2ff

// Title Screen, and starting sprite colors
const unsigned char PALETTE[] = {
	0x00, 0x16, 0x2d, 0x11, // 0 = title screen + room floor
	0x00, 0x02, 0x00, 0x30, // 1 = walls
	0x0f, 0x19, 0x09, 0x00, // 3 = floors
	0x00, 0x00, 0x10, 0x00, // 4 = computers

	0x0f, 0x0f, 0x02, 0x22, // player sprite
	0x00, 0x00, 0x00, 0x00, // not used
	0x00, 0x00, 0x00, 0x00, // not used
	0x00, 0x00, 0x00, 0x00  // not used
};

const unsigned char MetaSprite_Y[] = {0, 0, 8, 8}; // relative y coordinates

const unsigned char MetaSprite_Tile[] = { // tile numbers
	2, 3, 0x12, 0x13, // right
	0, 1, 0x10, 0x11, // down
	6, 7, 0x16, 0x17, // left
	4, 5, 0x14, 0x15}; // up

enum {Going_Right, Going_Down, Going_Left, Going_Up};

const unsigned char MetaSprite_Attrib[] = {0, 0, 0, 0}; // attributes = flipping, palette

const unsigned char MetaSprite_X[] = {0, 8, 0, 8}; // relative x coordinates
// we are using 4 sprites, each one has a relative position from the top left sprite

// Prototypes
void All_Off(void);
void All_On(void);
void Reset_Scroll(void);
void Load_Palette(void);

void drawScreen(void);
void rotatePalette(void);
void drawSprites(void);
void moveSprite(void);

void titleScreen(void);
void gameScreen(void);
void computerScreen(void);
void nextComputerScreen(void);
void hackedScreen(void);
void creditsScreen(void);
void hackersManifestoScreen(void);
void megaManStartScreen(void);

void gameGenieCodes(void);
void konamiCode(void);
void useComputer(void);
void dismissComputer(void);

void Blank_sprite(void);
void Wait_Vblank(void);
void __fastcall__ UnRLE(const unsigned char *data);
void Get_Input(void);
