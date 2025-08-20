#include <citro2d.h>

#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define PIPE_PAIRS 4
#define PIPE_DISTANCE 125
#define PIPE_GAP 80
#define PIPE_HEIGHT_RANGE 100
#define PIPE_VELOCITY 2
#define OFFSCREEN_OFFSET 50

#define BIRD_GRAVITY 0.25
#define BIRD_JUMP_SIZE 5
#define BIRD_X_POS 50

#define SCREEN_WIDTH  400
#define SCREEN_HEIGHT 240

enum GameState {
	START, 
	PLAYING, 
	DEAD
};

// Simple sprite struct
typedef struct{
	C2D_Sprite spr;
	float dx, dy; 
} BirdSprite;

typedef struct {
	C2D_Sprite topPipe;
	C2D_Sprite bottomPipe;
	float dx, dy;
} PipePair;

static C2D_SpriteSheet spriteSheet;
static PipePair pipes[PIPE_PAIRS]; 
static size_t numSprites = PIPE_PAIRS;

static BirdSprite bird; 

//---------------------------------------------------------------------------------
static void initSprites() {
//---------------------------------------------------------------------------------

	C2D_SpriteFromSheet(&bird.spr, spriteSheet, 1);
	C2D_SpriteSetCenter(&bird.spr, 0.5f, 0.5f);
	C2D_SpriteSetPos(&bird.spr, BIRD_X_POS, 120);
	bird.dy = -BIRD_JUMP_SIZE;


	for (size_t i = 0; i < PIPE_PAIRS; i++){
		PipePair* pipepair = &pipes[i];

		C2D_SpriteFromSheet(&pipepair->topPipe, spriteSheet, 0);
		C2D_SpriteFromSheet(&pipepair->bottomPipe, spriteSheet, 0);

		C2D_SpriteSetCenter(&pipepair->topPipe, 0.5f, 0.5f);
		C2D_SpriteSetCenter(&pipepair->bottomPipe, 0.5f, 0.5f);

		int rd_pipe_height = rand() % (PIPE_HEIGHT_RANGE + 1) + 0;
		rd_pipe_height = rd_pipe_height - (PIPE_HEIGHT_RANGE / 2);

		C2D_SpriteSetPos(&pipepair->topPipe, SCREEN_WIDTH + (PIPE_DISTANCE*i) + OFFSCREEN_OFFSET, -PIPE_GAP/2 - rd_pipe_height);
		C2D_SpriteSetPos(&pipepair->bottomPipe, SCREEN_WIDTH + (PIPE_DISTANCE*i) + OFFSCREEN_OFFSET, SCREEN_HEIGHT + PIPE_GAP/2 - rd_pipe_height);
		C2D_SpriteSetRotation(&pipepair->topPipe, C3D_Angle(0.5));
		
		pipepair->dx = -PIPE_VELOCITY;
	}
}

//---------------------------------------------------------------------------------
static void resetSprites() {
//--------------------------------------------------------------------------------
	C2D_SpriteSetPos(&bird.spr, BIRD_X_POS, 120);
	bird.dy = -BIRD_JUMP_SIZE;

	for (size_t i = 0; i < PIPE_PAIRS; i++){
		PipePair* pipepair = &pipes[i];

		int rd_pipe_height = rand() % (PIPE_HEIGHT_RANGE + 1) + 0;
		rd_pipe_height = rd_pipe_height - (PIPE_HEIGHT_RANGE / 2);

		C2D_SpriteSetPos(&pipepair->topPipe, SCREEN_WIDTH + (PIPE_DISTANCE*i) + OFFSCREEN_OFFSET, -PIPE_GAP/2 - rd_pipe_height);
		C2D_SpriteSetPos(&pipepair->bottomPipe, SCREEN_WIDTH + (PIPE_DISTANCE*i) + OFFSCREEN_OFFSET, SCREEN_HEIGHT + PIPE_GAP/2 - rd_pipe_height);
		
		pipepair->dx = -PIPE_VELOCITY;
	}
}

//---------------------------------------------------------------------------------
static void movePipes() {
//---------------------------------------------------------------------------------
	
for (size_t i = 0; i < numSprites; i++){
		PipePair* pipepair = &pipes[i];

		C2D_Sprite* top = &pipepair->topPipe;
		C2D_Sprite* bot = &pipepair->bottomPipe;

		C2D_SpriteMove(top, pipepair->dx, pipepair->dy);
		C2D_SpriteMove(bot, pipepair->dx, pipepair->dy);

		if ((top->params.pos.x < -OFFSCREEN_OFFSET && pipepair->dx < 0.0f) ||
		(bot->params.pos.x < -OFFSCREEN_OFFSET && pipepair->dx < 0.0f)){

			int rd_pipe_height = rand() % (PIPE_HEIGHT_RANGE + 1) + 0;
		 	rd_pipe_height = rd_pipe_height - (PIPE_HEIGHT_RANGE / 2);

			C2D_SpriteSetPos(&pipepair->topPipe, SCREEN_WIDTH + OFFSCREEN_OFFSET, -PIPE_GAP/2 + rd_pipe_height);
			C2D_SpriteSetPos(&pipepair->bottomPipe, SCREEN_WIDTH + OFFSCREEN_OFFSET, SCREEN_HEIGHT + PIPE_GAP/2 + rd_pipe_height);
		}
	}
}

//---------------------------------------------------------------------------------
static void moveBird() {
//---------------------------------------------------------------------------------
	
float displacement = (bird.dy) + (0.5 * BIRD_GRAVITY * BIRD_GRAVITY);
	bird.dy = bird.dy + BIRD_GRAVITY;
	C2D_SpriteMove(&bird.spr, bird.dx, displacement);
}

static void moveDeadBird() {

	if (bird.spr.params.pos.y + bird.spr.params.pos.h < SCREEN_HEIGHT){
		float displacement = (bird.dy) + (0.5 * BIRD_GRAVITY * BIRD_GRAVITY);
		bird.dy = bird.dy + BIRD_GRAVITY;
		C2D_SpriteMove(&bird.spr, bird.dx, displacement);
	}
}

//---------------------------------------------------------------------------------
static bool hasCollided() {
//---------------------------------------------------------------------------------

	if(bird.spr.params.pos.y + bird.spr.params.pos.h > SCREEN_HEIGHT ||
	bird.spr.params.pos.y < 0){
		return true;
	}

	float x = bird.spr.params.pos.x;
	float y = bird.spr.params.pos.y;
	float w = bird.spr.params.pos.w;
	float h = bird.spr.params.pos.h;

	float bird_l = x - (w/2);
	float bird_r = x + (w/2);
	float bird_u = y - (h/2);
	float bird_b = y + (h/2);

	for (size_t i = 0; i < numSprites; i++){
		PipePair* pipepair = &pipes[i];

		C2D_Sprite* top = &pipepair->topPipe;
		C2D_Sprite* bot = &pipepair->bottomPipe;

		float top_x = top->params.pos.x;
		float top_y = top->params.pos.y;
		float top_w = top->params.pos.w;
		float top_h = top->params.pos.h;

		float bot_x = bot->params.pos.x;
		float bot_y = bot->params.pos.y;
		float bot_w = bot->params.pos.w;
		float bot_h = bot->params.pos.h;

		float pipe_l = top_x - (top_w/2);
		float pipe_r = top_x + (top_w/2);
		float pipe_u = top_y - (top_h/2);
		float pipe_b = top_y + (top_h/2);

		bool top_col = false;
		bool bot_col = false;
		bool right_col = false; 
		bool left_col =false;
		
		if(bird_u < pipe_b && bird_u > pipe_u){
			top_col = true;
		}
		if(bird_b < pipe_b && bird_b > pipe_u){
			bot_col = true;
		}
		if(bird_r > pipe_l && bird_r < pipe_r){
			right_col = true;
		}
		if(bird_l > pipe_l && bird_l < pipe_r){
			left_col = true;
		}

		if((top_col || bot_col) && (right_col || left_col)){
			return true;
		}

		pipe_l = bot_x - (bot_w/2);
		pipe_r = bot_x + (bot_w/2);
		pipe_u = bot_y - (bot_h/2);
		pipe_b = bot_y + (bot_h/2);

		top_col = false;
		bot_col = false;
		right_col = false; 
		left_col =false;
		
		if(bird_u < pipe_b && bird_u > pipe_u){
			top_col = true;
		}
		if(bird_b < pipe_b && bird_b > pipe_u){
			bot_col = true;
		}
		if(bird_r > pipe_l && bird_r < pipe_r){
			right_col = true;
		}
		if(bird_l > pipe_l && bird_l < pipe_r){
			left_col = true;
		}

		if((top_col || bot_col) && (right_col || left_col)){
			return true;
		}
	}
	return false;
}

//---------------------------------------------------------------------------------
static void updateScore(int* score, int* pipeIndex) {
//---------------------------------------------------------------------------------
	PipePair* pipepair = &pipes[*pipeIndex];
	C2D_Sprite* top = &pipepair->topPipe;
	if(top->params.pos.x < BIRD_X_POS){
		*score = *score + 1;
		*pipeIndex = (*pipeIndex + 1) % 4;
	}
}


//---------------------------------------------------------------------------------
int main(int argc, char* argv[]) {
//---------------------------------------------------------------------------------

	enum GameState currentState = START;
	int score = 0, pipeIndex = 0;
	
	// Init libs
	romfsInit();
	gfxInitDefault();
	C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
	C2D_Init(C2D_DEFAULT_MAX_OBJECTS);
	C2D_Prepare();
	consoleInit(GFX_BOTTOM, NULL);

	// Create screens
	C3D_RenderTarget* top = C2D_CreateScreenTarget(GFX_TOP, GFX_LEFT);

	// Load graphics
	spriteSheet = C2D_SpriteSheetLoad("romfs:/gfx/sprites.t3x");
	if (!spriteSheet) svcBreak(USERBREAK_PANIC);

	initSprites();

	// Main game loop
	while (aptMainLoop()){
		
		hidScanInput();
		u32 kDown = hidKeysDown();

		if (kDown & KEY_START){
			break;
		}

		switch(currentState){
			case START:
				if (kDown & KEY_A)
					currentState = PLAYING;
				break;

			case PLAYING:
				if (kDown & KEY_A)
					bird.dy = -BIRD_JUMP_SIZE;
				if (kDown & KEY_B)
					currentState = DEAD;
				movePipes();
				moveBird();
				if(hasCollided()){
					currentState = DEAD;
					break;
				}
				updateScore(&score, &pipeIndex);
				break;

			case DEAD:
				moveDeadBird();
				if (kDown & KEY_A){
					resetSprites();
					currentState = START;
					score = 0;
					pipeIndex = 0;
				}
				break;

			default:
				break;
		}

		printf("\x1b[2;1HPress A to start and flap");

		printf("\x1b[4;1HScore:    %2d\x1b[K", score);

		// Render the scene
		C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
		C2D_TargetClear(top, C2D_Color32f(0.0f, 0.0f, 0.0f, 1.0f));
		C2D_SceneBegin(top);

		for (size_t i = 0; i < numSprites; i ++){
			C2D_DrawSprite(&pipes[i].topPipe);
			C2D_DrawSprite(&pipes[i].bottomPipe);
		}
		C2D_DrawSprite(&bird.spr);

		C3D_FrameEnd(0);
	}

	// Delete graphics
	C2D_SpriteSheetFree(spriteSheet);

	// Deinit libs
	C2D_Fini();
	C3D_Fini();
	gfxExit();
	romfsExit();
	return 0;
}
