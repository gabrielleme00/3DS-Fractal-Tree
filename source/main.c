// Simple citro2d untextured shape example
#include <citro2d.h>
#include <math.h>

#define VERSION "1.0.0"

#define deg2rad(deg) (deg * M_PI / 180.0)

#define SCREEN_WIDTH 400
#define SCREEN_HEIGHT 240

#define MAX_BRANCH_LEVELS 12
#define MIN_BRANCH_LEVELS 0

#define MAX_BRANCH_LENGTH 100
#define MIN_BRANCH_LENGTH 5
#define BRANCH_LENGTH_STEP 1

#define MAX_BRANCH_LEAN 90
#define MIN_BRANCH_LEAN 0
#define BRANCH_LEAN_STEP 1

#define MAX_BRANCH_GROWTH 0.99
#define MIN_BRANCH_GROWTH 0.01
#define BRANCH_GROWTH_STEP 0.01

#define COLOR_MAX 5
#define COLOR_MIN 0

#define BRANCH_THICKNESS 1
#define BRANCH_DEPTH 1

int levels;
int length;
int lean;
float growth;
short color;

u32 getColor(short idx, bool fg)
{
	switch (idx)
	{
	case 0:
		return fg ? C2D_Color32(0xAA, 0x96, 0x8A, 0xFF) : C2D_Color32(0x60, 0x4D, 0x53, 0xFF);
		break;
	case 1:
		return fg ? C2D_Color32(0x88, 0x36, 0x77, 0xFF) : C2D_Color32(0xFF, 0x95, 0x8C, 0xFF);
		break;
	case 2:
		return fg ? C2D_Color32(0xDC, 0x96, 0x5A, 0xFF) : C2D_Color32(0x24, 0x23, 0x25, 0xFF);
		break;
	case 3:
		return fg ? C2D_Color32(0x33, 0xFF, 0x00, 0xFF) : C2D_Color32(0x00, 0x00, 0x00, 0xFF);
		break;
	case 4:
		return fg ? C2D_Color32(0xFF, 0xFF, 0xFF, 0xFF) : C2D_Color32(0x00, 0x00, 0x00, 0xFF);
		break;
	case 5:
		return fg ? C2D_Color32(0x00, 0x00, 0x00, 0xFF) : C2D_Color32(0xFF, 0xFF, 0xFF, 0xFF);
		break;

	default:
		return fg ? C2D_Color32(0xFF, 0xFF, 0xFF, 0xFF) : C2D_Color32(0x00, 0x00, 0x00, 0xFF);
		break;
	}
}

void writeCommand(int row, char *cmd, char *desc, bool plusMinus)
{
	if (plusMinus)
	{
		printf("\x1b[%d;2H\x1b[33m%s", row, cmd);
		printf("\x1b[%d;16H\x1b[31m-\x1b[32m+ \x1b[37m%s", row, desc);
	}
	else
	{
		printf("\x1b[%d;2H\x1b[33m%s", row, cmd);
		printf("\x1b[%d;16H\x1b[37m%s", row, desc);
	}
}

void drawBranch(int lvl, float x0, float y0, float dir, float len, u32 clr)
{
	// Branch existance condition
	if (lvl < 1)
		return;
	lvl--;

	// Calculate target x and y
	float angle = deg2rad(dir);
	float x1 = x0 + cos(angle) * len;
	float y1 = y0 - sin(angle) * len;

	// Draw current branch
	C2D_DrawLine(x0, y0, clr, x1, y1, clr, BRANCH_THICKNESS, BRANCH_DEPTH);

	// Calculate lean and leght for the next branches
	float nextLen = len * growth;

	// Recursively draw next branches
	drawBranch(lvl, x1, y1, dir + lean, nextLen, clr);
	drawBranch(lvl, x1, y1, dir - lean, nextLen, clr);
}

int main(int argc, char *argv[])
{
	// Init libs
	gfxInitDefault();
	C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
	C2D_Init(C2D_DEFAULT_MAX_OBJECTS);
	C2D_Prepare();
	consoleInit(GFX_BOTTOM, NULL);

	// Create screens
	C3D_RenderTarget *top = C2D_CreateScreenTarget(GFX_TOP, GFX_LEFT);

	// Main parameters
	levels = 10;
	length = 50;
	lean = 12;
	growth = 0.8;
	color = 0;

	// Main loop
	while (aptMainLoop())
	{
		// Input scan
		hidScanInput();
		u32 kDown = hidKeysDown();
		u32 kHeld = hidKeysHeld();

		// Exit application
		if (kDown & KEY_START)
			break;

		// Control branch levels
		if ((kDown & KEY_A) && (levels < MAX_BRANCH_LEVELS))
			levels++;
		if ((kDown & KEY_B) && (levels > MIN_BRANCH_LEVELS))
			levels--;

		// Control branch length
		if ((kHeld & KEY_UP) && (length < MAX_BRANCH_LENGTH))
			length += BRANCH_LENGTH_STEP;
		if ((kHeld & KEY_DOWN) && (length > MIN_BRANCH_LENGTH))
			length -= BRANCH_LENGTH_STEP;

		// Control branch lean factor
		if ((kHeld & KEY_RIGHT) && (lean < MAX_BRANCH_LEAN))
			lean += BRANCH_LEAN_STEP;
		if ((kHeld & KEY_LEFT) && (lean > MIN_BRANCH_LEAN))
			lean -= BRANCH_LEAN_STEP;

		// Control branch growth factor
		if ((kHeld & KEY_X) && (growth < MAX_BRANCH_GROWTH))
			growth += BRANCH_GROWTH_STEP;
		if ((kHeld & KEY_Y) && (growth > MIN_BRANCH_GROWTH))
			growth -= BRANCH_GROWTH_STEP;

		// Control colors
		if ((kDown & KEY_R) && (color < COLOR_MAX))
			color++;
		if ((kDown & KEY_L) && (color > COLOR_MIN))
			color--;

		// Menu
		printf("\x1b[2;2H\x1b[36mFractalTree v%s", VERSION);
		writeCommand(6, "LEFT, RIGHT", "Lean     ", true);
		printf("%d  ", lean);
		writeCommand(8, "   DOWN, UP", "Length   ", true);
		printf("%d  ", length);
		writeCommand(10, "       B, A", "Levels   ", true);
		printf("%d  ", levels);
		writeCommand(12, "       Y, X", "Growth   ", true);
		printf("%.0f%% ", growth * 100);
		writeCommand(14, "       L, R", "Colors   ", true);
		printf("%d ", color);
		printf("\x1b[29;2H\x1b[36mMade by ZetaMek");

		// Colors
		u32 clrFg = getColor(color, true);
		u32 clrBg = getColor(color, false);

		// Render the scene
		C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
		C2D_TargetClear(top, clrBg);
		C2D_SceneBegin(top);

		drawBranch(levels, SCREEN_WIDTH / 2, SCREEN_HEIGHT, 90, length, clrFg);

		C3D_FrameEnd(0);
	}

	// Deinit libs
	C2D_Fini();
	C3D_Fini();
	gfxExit();
	return 0;
}
