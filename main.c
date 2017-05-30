/*****************************************
 * !!!!!!!!!!---SPACE RACER---!!!!!!!!!!!!
 * Created by Devon Cryts (dcryts91@gmail.com)
 * Georgia Institute of Technology
 * CS 2261, Spring 2014
 * 
 * Instructions:
 * Up:          Moves ship upward
 * Down:        Moves ship downward
 * Left:        Moves the player left (backwards)
 * Right:       Moves the player right &&
 *              increases players speed
 * Left Bumper: If held, speed will not change
 * Right Bumper:
 * A:           Shoots lasers (pressed or held)
 * B:           Toggles invincibility (cheat)
 * Start:       Pauses game
 * Select:      In game- nothing
 *              In pause- returns to title screen
 * 
 * Goal:
 * Your goal is to travel through space and
 * collect a rare and volatile crystal substance.
 * You must get there as quick as you can in
 * order to recover as much of it as possible.
 * But in this part of space, there are tons of
 * asteroids! Be careful not to be hit by them
 * (your shields are invisible and have a slightly
 * larger hit box than what your ship is perceived
 * to have, so keep that in mind) because once your 
 * shields are down, your ship is exposed, and it will 
 * deteriorate from the amount of pressure in this 
 * area of space. Luckily, you can destroy the 
 * asteroids with your lasers, and as added bonus, 
 * these asteroids usually contain some trace 
 * amounts of the rare crystal substance that 
 * you're after, so destroying asteroids is
 * beneficial. Don't worry though, your ship 
 * automatically collects the small amount of 
 * crystals left from the asteroids. But the larger
 * piece(s), you'll have to find and collect yourself!
 * 
 * Tips and Hints:
 * -You gain more points the faster you're going
 *      and if space is dragging you along itself
 *      you won't be gaining any points except for
 *      from asteroids you destroy.
 * -The faster your speed is, the better you can
 *      maneuver around!
 * -Hold down Left Bumper to maintain whatever speed
 *      you're currently going. Use this mainly to 
 *      maintain one of the fastest speeds while
 *      still being able to move around the map.
 * -Holding down A auto-fires your lasers, but you
 *      can press it quickly to fire them even faster!
 * -Some asteroids break into smaller asteroids,
 *      so watch out!
 * -Going in a straight line and constantly firing
 *      your lasers may seem like the easiest route,
 *      but you'd be missing out on so many more
 *      points/ crystals gained from destroying 
 *      more asteroids!
 * -While your ship is red (after being hit), it 
 *      will not take any more damage. But note 
 *      this is a short window of time!
 * -The warning signs show where an asteroid is
 *      ahead. The faster you go, the more tricky
 *      they are to read. Be careful!
 * 
    ******************************************/

#include <stdio.h>
#include <stdlib.h>
#include "myLib.h"
#include "text.h"
#include "space1.h"
#include "gameSprites.h"
#include "backSpace.h"
#include "frontSpace.h"
#include "laser.h"
#include "midHorizons.h"
#include "startHorizons.h"

unsigned int buttons;
unsigned int oldButtons;

int hOff=0;
int vOff=0;

int hOffFront = 0;
int vOffFront = 0;

OBJ_ATTR shadowOAM[128];

#define ROWMASK 0xFF
#define COLMASK 0x1FF

typedef struct  
{
    int health;
    int power;
	int row;
	int col;
        int rdel;
        int cdel;
        int cdelTimer;
	int width;
        int height;
        int aniCounter;
        int aniState;
        int prevAniState;
        int currFrame;
        
        int inv;
        int invTimer;
        
        int start;
        int on;
} MOVOBJ;

#define ASTRHEALTH 2
int invincible = 0;

#define NUMADS 40
#define NUMWA 7
#define NUMBU 12
#define NUMSA 20
MOVOBJ ship;
MOVOBJ asteroid[NUMADS];
MOVOBJ smAst[NUMSA];
MOVOBJ crystal;
MOVOBJ warn[NUMWA];
MOVOBJ bullets[NUMBU];
int nextBu = 0;
int nextAst = 0;
int nextSmAst = 0;

int holdCnt;

int deployed = 0;

// iteration variable
int i;
int j;

// time stuff
int time = 0;
int vbCount = 0;

// states used for the spaceship animation
enum { SHIPFRONT, SHIPBACK, SHIPRIGHT, SHIPLEFT, SHIPIDLE};
int random[ ] = {0, 1, 2, 3, 4, 5, 6};
int randomSeed = 0;



int distance;
#define MAXDISTANCE 18000
int topScore = 0;
int score;
int bPower = 1;

typedef enum {SPLASHSCREEN, INSTRSCREEN, GAMESCREEN, PAUSESCREEN, WINSCREEN, LOSESCREEN} screen_t;
screen_t SCREEN = SPLASHSCREEN;
//screen_t LAST; // used to check whether to initialize or not in gameSetup()

int setupBool;
char buffer[41];

void startSetup();
void start();
void instructions();
void gameSetup();
void game();
void pause();
void win();
void lose();
void hideSprites();
int collides(int r1,int c1,int width1,int height1,int r2,int c2,int width2,int height2);
void drawSprites();
void sOAM(u16 A0, u16 A1, u16 A2, int spriteNum);
void checkCollisions();
void doTime();
void astRot();
void deploy(int start, int stop, int spot, int rdel, int dep, int pow);

// SOUND
typedef struct{
    unsigned char* data;
    int length;
    int frequency;
    int isPlaying;
    int loops;
    int duration;
}SOUND;

SOUND soundA;
SOUND soundB;
int vbCountA;
int vbCountB;

void setupSounds();
void playSoundA( const unsigned char* sound, int length, int frequency, int isLooping);
void playSoundB( const unsigned char* sound, int length, int frequency, int isLooping);

void setupInterrupts();
void interruptHandler();

void muteSound();
void unmuteSound();
void stopSounds();



int main()
{
    REG_DISPCTL = MODE4 | BG2_ENABLE;
    setupBool = 0; 
    
    // SOUNDS
    setupInterrupts();
    setupSounds();
    
    playSoundA(startHorizons,STARTHORIZONSLEN,STARTHORIZONSFREQ,1);
    
    while(1)
    {
        oldButtons = buttons;
        buttons = BUTTONS;
        
        switch(SCREEN)
        {
            case SPLASHSCREEN:
                if (!setupBool) {
                    startSetup();
                }
                start();
                break;
            case INSTRSCREEN:
                instructions();
                break;
            case GAMESCREEN:
                if (!setupBool) {
                    gameSetup();
                }
                game();
                break;
            case PAUSESCREEN:
                pause();
                break;
            case WINSCREEN:
                if (!setupBool) {
                    startSetup();
                }
                win();
                break;
            case LOSESCREEN:
                if (!setupBool) {
                    startSetup();
                }
                lose();
                break;
        }
    }
    return 0;
}

void startSetup() {
    setupBool = 1;
    // Change mode
    REG_DISPCTL = MODE4 | BG2_ENABLE;

    // Setup palette
    PALETTE[WHITEINDEX] = WHITE; 
    PALETTE[BLACKINDEX] = BLACK; 

    flipPage();
}

void gameSetup() {
    setupBool = 1;

    
    srand(randomSeed);
    
    // This was in game loop;;
    REG_DISPCTL = MODE0 | BG0_ENABLE | BG1_ENABLE | SPRITE_ENABLE;
    REG_BG0CNT = CBB(0) | SBB(31) | BG_SIZE0 | COLOR256;
    REG_BG1CNT = CBB(1) | SBB(30) | BG_SIZE0 | COLOR256 | 1;
    loadPalette(frontSpacePal);
    DMANow(3, (unsigned int *)backSpaceTiles, &CHARBLOCKBASE[0], backSpaceTilesLen/2);
    DMANow(3, (unsigned int *)backSpaceMap, &SCREENBLOCKBASE[31],backSpaceMapLen/2);
    
    DMANow(3, (unsigned int *)frontSpaceTiles, &CHARBLOCKBASE[1], frontSpaceTilesLen/2);
    DMANow(3, (unsigned int *)frontSpaceMap, &SCREENBLOCKBASE[30],frontSpaceMapLen/2);
    
    
    // SPRITE
    DMANow(3, (unsigned int *)gameSpritesPal, SPRITE_PALETTE, 256);
    DMANow(3, (unsigned int *)gameSpritesTiles, &CHARBLOCKBASE[4], gameSpritesTilesLen);

    hideSprites();
    
    initialize();
    playSoundA(midHorizons,MIDHORIZONSLEN,MIDHORIZONSFREQ,1);
}

void start() {
    randomSeed++;
    fillScreen4(BLACKINDEX);
    loadPalette(space1Pal);
    drawBackgroundImage4(space1Bitmap);
    sprintf(buffer,"Space Racer");
    drawString4(40,5,buffer, WHITEINDEX);
    sprintf(buffer,"START:  Game");
    drawString4(150,113,buffer, WHITEINDEX);
    sprintf(buffer,"SELECT: Instructions");
    drawString4(140,113,buffer, WHITEINDEX);
    flipPage();
    fillScreen4(BLACKINDEX);
    
    while(1) {
        oldButtons = buttons;
	buttons = BUTTONS;
        if(BUTTON_PRESSED(BUTTON_START))
        {
            setupBool = 0;
            SCREEN = GAMESCREEN;
            return;
        }
        if(BUTTON_PRESSED(BUTTON_SELECT))
        {
            SCREEN = INSTRSCREEN;
            return;
        }
    }
}

void instructions() {
    randomSeed++;
    fillScreen4(BLACKINDEX);
    loadPalette(space1Pal);
    drawBackgroundImage4(space1Bitmap);
    // INSTUCTIONS
    sprintf(buffer,"Instructions:");
    drawString4(40,5,buffer, WHITEINDEX);
    sprintf(buffer,"Find the crystal!");
    drawString4(50,5,buffer, WHITEINDEX);
    sprintf(buffer,"Dodge or shoot asteroids!");
    drawString4(60,5,buffer, WHITEINDEX);
    sprintf(buffer,"A: Shoot");
    drawString4(70,5,buffer, WHITEINDEX);
    sprintf(buffer,"Right: Speed up");
    drawString4(80,5,buffer, WHITEINDEX);
    sprintf(buffer,"Left Bumper: Maintain speed");
    drawString4(90,5,buffer, WHITEINDEX);
    sprintf(buffer,"B: Invincibility.");
    drawString4(100,5,buffer, WHITEINDEX);
    
    sprintf(buffer,"START:  Game");
    drawString4(150,113,buffer, WHITEINDEX);
    sprintf(buffer,"SELECT: Title Screen");
    drawString4(140,113,buffer, WHITEINDEX);
    flipPage();
    fillScreen4(BLACKINDEX);
    
    while(1) {
        oldButtons = buttons;
	buttons = BUTTONS;
        if(BUTTON_PRESSED(BUTTON_START))
        {
            setupBool = 0;
            SCREEN = GAMESCREEN;
            return;
        }
        if(BUTTON_PRESSED(BUTTON_SELECT))
        {
            SCREEN = SPLASHSCREEN;
            return;
        }
    }
}

void game() {
    waitForVblank();
    
    while(1) {
        oldButtons = buttons;
        buttons = BUTTONS;
        
        if(BUTTON_HELD(BUTTON_UP)) {
            if (ship.row > 5) {
                ship.row-=ship.rdel;
            }
        }
        if(BUTTON_HELD(BUTTON_DOWN)) {
            if (ship.row < 130) {
                ship.row+=ship.rdel;
            }
        }
        
        // SPEED CONTROL
        if(BUTTON_HELD(BUTTON_RIGHT)) {
            if (ship.col < 180) {
                ship.col++;
            }
            ship.cdelTimer++;
            if (ship.cdelTimer%25==0) {
                if (ship.cdel < 6 && (!BUTTON_HELD(BUTTON_L))) {
                    ship.cdel++;
                }
                ship.cdelTimer = 0;
            }
        }
        else {
            if(BUTTON_HELD(BUTTON_LEFT)) {
                if (ship.col > 4) {
                    ship.col-=2;
                }
            }
            else if (ship.col > 4) {
                ship.col--;
            }
            
            
            ship.cdelTimer++;
            if (ship.cdelTimer%35==0) {
                if (ship.cdel > 0 && (!BUTTON_HELD(BUTTON_L))) {
                    ship.cdel--;
                }
                ship.cdelTimer = 0;
            }
        }
                
        if(BUTTON_PRESSED(BUTTON_START)) {
            SCREEN = PAUSESCREEN;
            muteSound();
            return;
        }
        if(BUTTON_PRESSED(BUTTON_A)) {
            holdCnt = 14;
        }
        if(BUTTON_HELD(BUTTON_A)) {
            holdCnt++;
            // mod affects fire rate (possible upgrade)
            if (holdCnt%15==0) {
                playSoundB(laser,LASERLEN,LASERFREQ,0);
                bullets[nextBu].on = 1;

                nextBu++;
                if (nextBu > NUMBU-1) {
                    nextBu = 0;
                }
                holdCnt = 0;
            }
        }
        if(BUTTON_PRESSED(BUTTON_B)) {
            invincible = !invincible;
        }
        if(BUTTON_PRESSED(BUTTON_R)) {

        }

        // DRAW SPRITES
        drawSprites();

        //Copy the shadowOAM into the OAM
        DMANow(3,shadowOAM,OAM,(128*sizeof(OBJ_ATTR)/2));
        
        
                
                
         // END CONDITIONS
         if (collides(ship.row,ship.col,ship.width,ship.height,crystal.row,crystal.col,crystal.width,crystal.height)) {
                setupBool = 0;
                SCREEN = WINSCREEN;
                return;
         }
         if (ship.health<1) {
             setupBool = 0;
             SCREEN = LOSESCREEN;
             return;
         }
         
         
         
         
         waitForVblank();
         checkCollisions();
         doTime();
         update();
         
         distance+=(2+ship.cdel);
         
         if (distance < MAXDISTANCE) {
             score += ship.cdel;
         }
         //SET BACKGROUND POSITIONS
         hOffFront+=(2+ship.cdel);
         hOff++;
         REG_BG0HOFS = hOffFront;
         REG_BG0VOFS = vOffFront;
         REG_BG1HOFS = hOff;
         REG_BG1VOFS = vOff;
    }
}

void pause() {
    // PAUSE SPRITE
    sOAM(((160/2)-(32/2) | ATTR0_WIDE), (((240/2)-(64/2)) | ATTR1_SIZE64), SPRITEOFFSET16(11,4), 0);
    DMANow(3,shadowOAM,OAM,(128*sizeof(OBJ_ATTR)/2));
    
    
    while(1){
        oldButtons = buttons;
	buttons = BUTTONS;
        muteSound();

        if(BUTTON_PRESSED(BUTTON_START)){
            //setupBool = 0;
            shadowOAM[0].attr0 = ATTR0_HIDE;
            SCREEN = GAMESCREEN;
            unmuteSound();
            return;
        }
        if(BUTTON_PRESSED(BUTTON_SELECT)){
            setupBool = 0;
            SCREEN = SPLASHSCREEN;
            playSoundA(startHorizons,STARTHORIZONSLEN,STARTHORIZONSFREQ,1);
            return;
        }
    }
}

void win() {
    if (score > topScore) {
        topScore = score;
    }
    
    stopSounds();
    flipPage();
    fillScreen4(BLACKINDEX);
    loadPalette(space1Pal);
    drawBackgroundImage4(space1Bitmap);
    sprintf(buffer,"You win!");
    drawString4(40,5,buffer, WHITEINDEX);
    sprintf(buffer,"Score: %d", score);
    drawString4(60,5,buffer, WHITEINDEX);
    sprintf(buffer,"Best score: %d", topScore);
    drawString4(70,5,buffer, WHITEINDEX);
    sprintf(buffer,"START:  Play again");
    drawString4(150,113,buffer, WHITEINDEX);
    sprintf(buffer,"SELECT: Title Screen");
    drawString4(140,113,buffer, WHITEINDEX);
    flipPage();
    
    while(1) {
        oldButtons = buttons;
	buttons = BUTTONS;
        if(BUTTON_PRESSED(BUTTON_START))
        {
            setupBool = 0;
            SCREEN = GAMESCREEN;
            playSoundA(midHorizons,MIDHORIZONSLEN,MIDHORIZONSFREQ,1);
            return;
        }
        if(BUTTON_PRESSED(BUTTON_SELECT))
        {
            SCREEN = SPLASHSCREEN;
            playSoundA(startHorizons,STARTHORIZONSLEN,STARTHORIZONSFREQ,1);
            return;
        }
    }
}

void lose() {
    stopSounds();
    flipPage();
    fillScreen4(BLACKINDEX);
    loadPalette(space1Pal);
    drawBackgroundImage4(space1Bitmap);
    sprintf(buffer,"Game Over");
    drawString4(40,5,buffer, WHITEINDEX);
    sprintf(buffer,"START:  Retry");
    drawString4(150,113,buffer, WHITEINDEX);
    sprintf(buffer,"SELECT: Title Screen");
    drawString4(140,113,buffer, WHITEINDEX);
    flipPage();    
    
    while(1) {
        oldButtons = buttons;
	buttons = BUTTONS;
        if(BUTTON_PRESSED(BUTTON_START))
        {
            setupBool = 0;
            SCREEN = GAMESCREEN;
            playSoundA(midHorizons,MIDHORIZONSLEN,MIDHORIZONSFREQ,1);
            return;
        }
        if(BUTTON_PRESSED(BUTTON_SELECT))
        {
            SCREEN = SPLASHSCREEN;
            playSoundA(startHorizons,STARTHORIZONSLEN,STARTHORIZONSFREQ,1);
            return;
        }
    }
}

void hideSprites()
{
    //COMPLETE THIS FUNCTION
    //Loop through all the shadowOAM sprites and hide them.
    for (i=0;i<128;i++) {
        shadowOAM[i].attr0 = ATTR0_HIDE;
    }
}

void initialize()
{
    holdCnt = 0;
    vbCount = 0;
    time = 0;
    invincible = 0;
    deployed = 0;
    distance = 0;
    score = 0;

    // SHIP
    ship.health = 10;
    ship.width = 16;
    ship.height = 16;
    ship.rdel = 1;
    ship.cdel = 0;
    ship.cdelTimer = 0;
    ship.row = 160/2-ship.width/2;
    ship.col = 240/2-ship.height/2;
    ship.inv = 0;
    ship.invTimer = 0;
    
    // BULLETS
    for (i=0;i<NUMBU;i++) {
        bullets[i].row = 200;
        bullets[i].col = ship.col+16;
        bullets[i].width = 8;
        bullets[i].height = 8;
        bullets[i].on = 0;
        bullets[i].start = 0;
    }
    
    // ASTEROIDS
    for (i=0;i<NUMADS;i++) {
        asteroid[i].health = ASTRHEALTH;
        asteroid[i].power = 0;
        asteroid[i].row = 200;
        asteroid[i].col = 250;
        asteroid[i].rdel = 0;
        asteroid[i].cdel = 0;
        asteroid[i].width = 16;
        asteroid[i].height = 16;
        asteroid[i].aniCounter = 0;
        asteroid[i].currFrame = 0;
        asteroid[i].aniState = 4;
        asteroid[i].on = 0;
    }
    
    // SMALL ASTEROIDS
    for (i=0;i<NUMSA;i++) {
        smAst[i].health = ASTRHEALTH;
        smAst[i].row = 200;
        smAst[i].col = 250;
        smAst[i].rdel = 0;
        smAst[i].cdel = 0;
        smAst[i].width = 8;
        smAst[i].height = 8;
        smAst[i].aniCounter = 0;
        smAst[i].currFrame = 0;
        smAst[i].aniState = 4;
        smAst[i].on = 0;
    }
    
    // WARNINGS
    for (i=0;i<NUMWA;i++) {
        warn[i].row = 5+(i*20);
        warn[i].col = 208;
        warn[i].width = 32;
        warn[i].height = 16;
        warn[i].aniCounter = 0;
        warn[i].currFrame = 0;
        warn[i].aniState = 1;
        warn[i].start = 0;
        warn[i].on = 0;
    }
    
    // CRYSTAL
    crystal.row = 200;
    crystal.col = 250;
    crystal.width = 16;
    crystal.height = 16;
    crystal.on = 0;
    
    ship.aniCounter = 0;
    ship.currFrame = 0;
    ship.aniState = SHIPRIGHT; 
    
    buttons = BUTTONS;
    
    hOff = 0;
    vOff = 0;
    hOffFront = 0;
    vOffFront = 0;
}

void drawSprites() {
    // sprite 0 is reserved for pause sprite
    int sNum = 1;
    
    //SHIP
    sOAM((ROWMASK & ship.row),
            ((COLMASK & ship.col) | ATTR1_SIZE16),
            SPRITEOFFSET16(2*ship.currFrame,2*(ship.aniState+invincible)), sNum++);
    
    
    // ASTEROIDS
    for (i=0;i<NUMADS;i++) {
        if (asteroid[i].on) {
            sOAM((ROWMASK & asteroid[i].row), 
                    ((COLMASK & asteroid[i].col) | ATTR1_SIZE16), 
                    SPRITEOFFSET16(2*asteroid[i].currFrame,2*asteroid[i].aniState), sNum++);
        }
        else {
            shadowOAM[sNum++].attr0 = ATTR0_HIDE;
        }
    }
    
    // SMALL ASTEROIDS
    for (i=0;i<NUMSA;i++) {
        if (smAst[i].on) {
            sOAM((ROWMASK & smAst[i].row), 
                    ((COLMASK & smAst[i].col)), 
                    SPRITEOFFSET16(smAst[i].currFrame,12), sNum++);
        }
        else {
            shadowOAM[sNum++].attr0 = ATTR0_HIDE;
        }
    }
    
    //CRYSTAL
    if (crystal.on) {
        sOAM((ROWMASK & crystal.row), 
                ((COLMASK & crystal.col) | ATTR1_SIZE16), 
                SPRITEOFFSET16(0,2*7), sNum++);
    }
    else {
        shadowOAM[sNum++].attr0 = ATTR0_HIDE;
    }
    
    // WARNINGS
    for (i=0;i<NUMWA;i++) {
        if (warn[i].on) {
                sOAM(((ROWMASK & warn[i].row) | ATTR0_WIDE), 
                        ((COLMASK & warn[i].col) | ATTR1_SIZE32), 
                        SPRITEOFFSET16(2*(3+warn[i].currFrame),4*warn[i].aniState), sNum++);
        }
        else {
            shadowOAM[sNum++].attr0 = ATTR0_HIDE;
        }
    }
    
    // BULLETS
    for (i=0;i<NUMBU;i++) {
        if (bullets[i].on) {
                sOAM(((ROWMASK & bullets[i].row)), 
                        ((COLMASK & bullets[i].col) | ATTR1_SIZE8), 
                        SPRITEOFFSET16(1,13), sNum++);
        }
        else {
            shadowOAM[sNum++].attr0 = ATTR0_HIDE;
        }
    }
    
    // SCORE
    sOAM((150), (44), SPRITEOFFSET16(10,(score%1000000)/100000), sNum++);
    sOAM((150), (50), SPRITEOFFSET16(10,(score%100000)/10000), sNum++);
    sOAM((150), (56), SPRITEOFFSET16(10,(score%10000)/1000), sNum++);
    sOAM((150), (62), SPRITEOFFSET16(10,(score%1000)/100), sNum++);
    sOAM((150), (68), SPRITEOFFSET16(10,(score%100)/10), sNum++);
    sOAM((150), (74), SPRITEOFFSET16(10,score%10), sNum++);
    sOAM((150 | ATTR0_WIDE), (20 | ATTR1_SIZE16), SPRITEOFFSET16(11,0), sNum++);
    
    // SHIP HEALTH
    sOAM((150), (120), SPRITEOFFSET16(10,(ship.health%100)/10), sNum++);
    sOAM((150), (126), SPRITEOFFSET16(10,ship.health%10), sNum++);
    sOAM((150 | ATTR0_WIDE), (92 | ATTR1_SIZE16), SPRITEOFFSET16(12,0), sNum++);
    
    // TIME
    sOAM((150), (172), SPRITEOFFSET16(10,(time%10000)/1000), sNum++);
    sOAM((150), (178), SPRITEOFFSET16(10,(time%1000)/100), sNum++);
    sOAM((150), (184), SPRITEOFFSET16(10,(time%100)/10), sNum++);
    sOAM((150), (190), SPRITEOFFSET16(10,time%10), sNum++);
    sOAM((150 | ATTR0_WIDE), (150 | ATTR1_SIZE16), SPRITEOFFSET16(13,0), sNum++);
}

void update() {
    // UPDATE SHIP
    // SPEED
    ship.rdel = ship.cdel/2+1;
    if (ship.cdel>3) {
        ship.cdel = 3;
    }
    // ANIMATION
    ship.aniCounter++;
    if(ship.aniCounter%20==0) { 
        ship.currFrame++;
        if (ship.currFrame > 2) {
            ship.currFrame = 0;
        }
    }
    // invincibility timer (not cheat invincibility)
    if (ship.inv) {
        ship.aniState = 1;
        ship.invTimer++;
        if (ship.invTimer>20) {
            ship.inv = 0;
            ship.invTimer = 0;
            ship.aniState = 2;
        }
    }
    
    // UPDATE BULLETS
    for (i=0;i<NUMBU;i++) {
        
        if (bullets[i].on) {
            if (bullets[i].start==0) {
                bullets[i].row = ship.row+4;
            }
            bullets[i].col+=2;
            bullets[i].start++;
            if (bullets[i].start>90 || (bullets[i].col>235)) {
                bullets[i].on = 0;
            } 
        }
        else {
            bullets[i].row = 200;
            bullets[i].col = ship.col+16;
            bullets[i].start = 0;
        }
    }
    
    // UPDATE ASTROIDS   
    for (i=0;i<NUMADS;i++) {
        if (asteroid[i].on) {
            // existence
            if (asteroid[i].health<1 || asteroid[i].col+20<0 || asteroid[i].row+20<0 || asteroid[i].row>164) {
                if (asteroid[i].health<1) {
                    if (asteroid[i].power>0) {
                        for (j=0;j<3;j++) {
                            smAst[nextSmAst].on = 1;
                            smAst[nextSmAst].row = asteroid[i].row + (j*6);
                            smAst[nextSmAst].col = asteroid[i].col + 8;
                            smAst[nextSmAst].rdel = (j-1);
                            smAst[nextSmAst++].cdel = -1;
                            if (nextSmAst>NUMSA) {
                                nextSmAst = 0;
                            }
                        }
                    }
                    
                    score+=100;
                }
                asteroid[i].on = 0;
                asteroid[i].health = ASTRHEALTH;
                asteroid[i].row = 200;
                asteroid[i].col = 250;
                asteroid[i].rdel = 0;
            }
            
            // position
            //if (asteroid[i].start<1) {
                asteroid[i].row += asteroid[i].rdel;
                asteroid[i].col += asteroid[i].cdel - ship.cdel - 1;
            //}
            // animate
            asteroid[i].aniCounter++;
            if(asteroid[i].aniCounter%20==0) {
                // changes frame /3
                asteroid[i].currFrame++;
                if (asteroid[i].currFrame > 2) {
                    asteroid[i].currFrame = 0;
                }
            }
        }
        else {
            if (asteroid[i].start>0) {
                asteroid[i].aniCounter++;
                if(asteroid[i].aniCounter%10==0) {
                    asteroid[i].start--;
                    if (asteroid[i].start==1) {
                        asteroid[i].on = 1;
                    }
                }
            }
            else {
                asteroid[i].health = ASTRHEALTH;
                asteroid[i].row = 200;
                asteroid[i].col = 250;
                asteroid[i].rdel = 0;
            }
        }
    }
    
    
    
    // UPDATE SMALL ASTROIDS   
    for (i=0;i<NUMSA;i++) {
        if (smAst[i].on) {
            // existence
            if (smAst[i].health<1 || smAst[i].col+10<0) {
                if (smAst[i].health<1) {
                    score+=10;
                }
                smAst[i].on = 0;
                smAst[i].health = 1;
                smAst[i].row = 200;
                smAst[i].col = 250;
                smAst[i].rdel = 0;
                smAst[i].cdel = 0;
            }
            
            // position
            //if (asteroid[i].start<1) {
                smAst[i].row += smAst[i].rdel;
                smAst[i].col += smAst[i].cdel - ship.cdel - 1;
            //}
            // animate
            smAst[i].aniCounter++;
            if(smAst[i].aniCounter%10==0) {
                // changes frame /3
                smAst[i].currFrame++;
                if (smAst[i].currFrame > 2) {
                    smAst[i].currFrame = 0;
                }
            }
        }
        else {
            smAst[i].health = 1;
            smAst[i].row = 200;
            smAst[i].col = 250;
            smAst[i].rdel = 0;
            smAst[i].cdel = 0;
        }
    }
    
    
    // UPDATE WARNINGS  
    for (i=0;i<NUMWA;i++) {
        
        if (warn[i].on) {
            warn[i].aniCounter++;
            if(warn[i].aniCounter%10==0) {
                warn[i].start++;
                // changes frame /2
                warn[i].currFrame++;
                if (warn[i].currFrame > 1) {
                    warn[i].currFrame = 0;
                }
            }
            // change aniStates
            if (warn[i].start>10) {
                warn[i].on = 0;
                warn[i].currFrame = 0;
                warn[i].aniState = 1;
                warn[i].start = 0;
            }
            else if (warn[i].start>1) {
                warn[i].aniState = 0;
            }  
        }
    }
    
    
    // UPDATE OTHER
    if (crystal.on) {
        crystal.col += crystal.cdel;
        if (crystal.col<180) {
            crystal.cdel = 0;
        }
    }
}

int collides(int r1,int c1,int width1,int height1,int r2,int c2,int width2,int height2)
{
    if ((r1+height1>r2) & (r1<r2+height2) & (c1+width1>c2) & (c1<c2+width2))
        return 1;
    return 0;
}

void sOAM(u16 A0, u16 A1, u16 A2, int spriteNum) {
    shadowOAM[spriteNum].attr0 = A0;
    shadowOAM[spriteNum].attr1 = A1;
    shadowOAM[spriteNum].attr2 = A2;
}

void checkCollisions() {
    // CHECK BULLETS VS ASTROIDS
    for (i=0;i<NUMBU;i++) {
        for (j=0;j<NUMADS;j++) {
            if (collides(bullets[i].row,bullets[i].col,bullets[i].width,bullets[i].height,asteroid[j].row,asteroid[j].col,asteroid[j].width,asteroid[j].height)) {
                asteroid[j].health -= bPower;
                bullets[i].on = 0;
            }
        }
    }
    
    // CHECK BULLETS VS SMALL ASTROIDS
    for (i=0;i<NUMBU;i++) {
        for (j=0;j<NUMSA;j++) {
            if (collides(bullets[i].row,bullets[i].col,bullets[i].width,bullets[i].height,smAst[j].row,smAst[j].col,smAst[j].width,smAst[j].height)) {
                smAst[j].health -= bPower;
                bullets[i].on = 0;
            }
        }
    }
    
    // CHECK SHIP VS ASTROIDS
    if (!invincible) {//invincible == 0) {
        for (i=0;i<NUMADS;i++) {
            if (!ship.inv) {
                if (collides(ship.row,ship.col,ship.width,ship.height,asteroid[i].row,asteroid[i].col,asteroid[i].width,asteroid[i].height)) {
                    ship.health--;
                    ship.inv = 1;
                    asteroid[i].on = 0;
                    // flash state?
                }
            }
        }
    }
    
    // CHECK SHIP VS SMALL ASTROIDS
    if (!invincible) {//invincible == 0) {
        for (i=0;i<NUMSA;i++) {
            if (!ship.inv) {
                if (collides(ship.row,ship.col,ship.width,ship.height,smAst[i].row,smAst[i].col,smAst[i].width,smAst[i].height)) {
                    ship.health--;
                    ship.inv = 1;
                    smAst[i].on = 0;
                }
            }
        }
    }
    
    // CHECK WARNINGS VS ASTROIDS
    for (i=0;i<NUMWA;i++) {
        for (j=0;j<NUMADS;j++) {
            if (collides(warn[i].row,warn[i].col+32,warn[i].width,warn[i].height,asteroid[j].row,asteroid[j].col,asteroid[j].width,asteroid[j].height)) {
                warn[i].on = 1;
            }
        }
    }
}

void astRot() {
    nextAst++;
    if (nextAst>NUMADS) {
        nextAst = 0;
    }
}

void deploy(int start, int stop, int spot, int rdel, int dep, int pow) {
    if (distance>=start && distance<(start+ship.cdel+1)) {
        if (dep) {
            asteroid[nextAst].start = 10;
            asteroid[nextAst].row = 10+(spot*20);
            asteroid[nextAst].col = 240;
            asteroid[nextAst].rdel = rdel;
            asteroid[nextAst].power = pow;
            astRot();
            
            deployed = !deployed;
        }
    }
}

void doTime() {
    if (distance>=70 && distance<85) {
        if (!deployed) {
            for (i=0;i<7;i++) {
                asteroid[nextAst].start = 10;
                asteroid[nextAst].row = 10+(i*20);
                asteroid[nextAst].col = 240;
                astRot();
            }
            deployed = !deployed;
        }
    }
    if (distance>=500 && distance<515) {
        if (deployed) {
            for (i=0;i<7;i++) {
                asteroid[nextAst].start = 10;
                asteroid[nextAst].row = 10+(i*20);
                asteroid[nextAst].col = 240;
                astRot();
            }
            deployed = !deployed;
        }
    }
    if (distance>=920 && distance<935) {
        if (!deployed) {
            for (i=0;i<7;i++) {
                asteroid[nextAst].start = 10;
                asteroid[nextAst].row = 10+(i*20);
                asteroid[nextAst].col = 240;
                astRot();
            }
            deployed = !deployed;
        }
    }
    if (distance>=1500 && distance<1515) {
        if (deployed) {
            for (i=0;i<7;i++) {
                asteroid[nextAst].start = 10;
                asteroid[nextAst].row = 10+(i*20);
                asteroid[nextAst].col = 240;
                astRot();
            }
            deployed = !deployed;
        }
    }
    
    
    // WAVES 2 diagonal
    deploy(2000, 2005, 0, 0, !deployed, 0);
    deploy(2100, 2105, 1, 0, deployed, 0);
    deploy(2200, 2205, 2, 0, !deployed, 0);
    deploy(2300, 2305, 3, 0, deployed, 0);
    deploy(2400, 2405, 4, 0, !deployed, 0);
    deploy(2500, 2505, 5, 0, deployed, 0);
    deploy(2600, 2605, 6, 0, !deployed, 0);
    deploy(2700, 2705, 5, 0, deployed, 0);
    deploy(2800, 2805, 4, 0, !deployed, 0);
    deploy(2900, 2905, 3, 0, deployed, 0);
    deploy(3000, 3005, 2, 0, !deployed, 0);
    deploy(3100, 3105, 1, 0, deployed, 0);
    deploy(3200, 3205, 0, 0, !deployed, 0);
    
    
    
    // WAVES 3
    deploy(3700, 3705, 1, 1, deployed, 0);
    deploy(3800, 3805, 5, 0, !deployed, 0);
    deploy(3900, 3905, 4, 0, deployed, 0);
    deploy(4000, 4005, 4, -1, !deployed, 0);
    deploy(4100, 4105, 6, -1, deployed, 0);
    deploy(4200, 4205, 3, 0, !deployed, 0);
    deploy(4300, 4305, 6, -1, deployed, 0);
    deploy(4400, 4405, 0, 1, !deployed, 0);
    deploy(4500, 4505, 4, 1, deployed, 0);
    deploy(4600, 4605, 4, -1, !deployed, 0);
    deploy(4700, 4705, 3, 0, deployed, 0);
    deploy(4800, 4805, 0, 1, !deployed, 0);
    deploy(4900, 4905, 5, 0, deployed, 0);
    
    
    // wAVES 4
    deploy(5000, 5005, 0, 0, !deployed, 0);
    deploy(5000, 5005, 6, 0, deployed, 0);
    deploy(5200, 5205, 1, 0, !deployed, 0);
    deploy(5200, 5205, 5, 0, deployed, 0);
    deploy(5400, 5405, 2, 0, !deployed, 0);
    deploy(5400, 5405, 4, 0, deployed, 0);
    deploy(5600, 5605, 3, 0, !deployed, 0);
    deploy(5800, 5805, 4, 0, deployed, 0);
    deploy(5800, 5805, 2, 0, !deployed, 0);
    deploy(6000, 6005, 5, 0, deployed, 0);
    deploy(6000, 6005, 1, 0, !deployed, 0);
    deploy(6200, 6205, 6, 0, deployed, 0);
    deploy(6200, 6205, 0, 0, !deployed, 0);
    deploy(6400, 6405, 5, 0, deployed, 0);
    deploy(6400, 6405, 1, 0, !deployed, 0);
    deploy(6600, 6605, 4, 0, deployed, 0);
    deploy(6600, 6605, 2, 0, !deployed, 0);
    deploy(6800, 6805, 3, 0, deployed, 0);
    deploy(7000, 7005, 2, 0, !deployed, 0);
    deploy(7000, 7005, 4, 0, deployed, 0);
    deploy(7200, 7205, 1, 0, !deployed, 0);
    deploy(7200, 7205, 5, 0, deployed, 0);
    deploy(7400, 7405, 0, 0, !deployed, 0);
    deploy(7400, 7405, 6, 0, deployed, 0);
    
    // WAVES 5
    deploy(7800, 7805, 0, 0, !deployed, 1);
    deploy(8100, 8105, 2, 0, deployed, 1);
    deploy(8400, 8405, 1, 0, !deployed, 1);
    deploy(8700, 8705, 3, 0, deployed, 1);
    deploy(9000, 9005, 2, 0, !deployed, 1);
    deploy(9300, 9305, 4, 0, deployed, 1);
    deploy(9700, 9705, 3, 0, !deployed, 1);
    deploy(10000, 10005, 5, 0, deployed, 1);
    deploy(10300, 10305, 4, 0, !deployed, 1);
    deploy(10600, 10605, 6, 0, deployed, 1);
    deploy(10900, 10905, 5, 0, !deployed, 1);
    
    
    // WAVES 6
    deploy(11000, 0, random[rand()%7], 0, deployed, 1);
    deploy(11200, 0, random[rand()%7], 0, !deployed, 1);
    deploy(11400, 0, random[rand()%7], 1, deployed, 1);
    deploy(11600, 0, random[rand()%7], 0, !deployed, 1);
    deploy(11800, 0, random[rand()%7], 0, deployed, 1);
    deploy(12000, 0, random[rand()%7], 0, !deployed, 1);
    deploy(12200, 0, random[rand()%7], -1, deployed, 1);
    deploy(12400, 0, random[rand()%7], 0, !deployed, 1);
    deploy(12600, 0, random[rand()%7], 0, deployed, 1);
    deploy(12800, 0, random[rand()%7], 0, !deployed, 1);
    deploy(13000, 0, random[rand()%7], -1, deployed, 1);
    deploy(13200, 0, random[rand()%7], 0, !deployed, 1);
    deploy(13400, 0, random[rand()%7], 0, deployed, 1);
    deploy(13600, 0, random[rand()%7], 0, !deployed, 1);
    deploy(13700, 0, random[rand()%7], 0, deployed, 1);
    deploy(13800, 0, random[rand()%7], -1, !deployed, 1);
    deploy(13900, 0, random[rand()%7], 0, deployed, 1);
    deploy(14000, 0, random[rand()%7], 0, !deployed, 1);
    deploy(14100, 0, random[rand()%7], 0, deployed, 1);
    deploy(14200, 0, random[rand()%7], 0, !deployed, 1);
    deploy(14300, 0, random[rand()%7], 1, deployed, 1);
    deploy(14400, 0, random[rand()%7], 0, !deployed, 1);
    deploy(14500, 0, random[rand()%7], 0, deployed, 1);
    deploy(14600, 0, random[rand()%7], 0, !deployed, 1);
    deploy(14700, 0, random[rand()%7], 0, deployed, 1);
    deploy(14800, 0, random[rand()%7], -1, !deployed, 1);
    deploy(14900, 0, random[rand()%7], 0, deployed, 1);
    deploy(15000, 0, random[rand()%7], 0, !deployed, 1);
    deploy(15100, 0, random[rand()%7], 0, deployed, 1);
    deploy(15200, 0, random[rand()%7], 0, !deployed, 1);
    deploy(15300, 0, random[rand()%7], 1, deployed, 1);
    deploy(15400, 0, random[rand()%7], 0, !deployed, 1);
    deploy(15500, 0, random[rand()%7], 0, deployed, 1);
    deploy(15600, 0, random[rand()%7], 0, !deployed, 1);
    deploy(15700, 0, random[rand()%7], 0, deployed, 1);
    deploy(15800, 0, random[rand()%7], 1, !deployed, 1);
    deploy(15900, 0, random[rand()%7], 0, deployed, 1);
    deploy(16000, 0, random[rand()%7], 0, !deployed, 1);
    deploy(16100, 0, random[rand()%7], 0, deployed, 1);
    deploy(16200, 0, random[rand()%7], 0, !deployed, 1);
    deploy(16300, 0, random[rand()%7], -1, deployed, 1);
    deploy(16400, 0, random[rand()%7], 0, !deployed, 1);
    deploy(16500, 0, random[rand()%7], 0, deployed, 1);
    deploy(16600, 0, random[rand()%7], 0, !deployed, 1);
    deploy(16700, 0, random[rand()%7], 0, deployed, 1);
    deploy(16800, 0, random[rand()%7], 0, !deployed, 1);
    deploy(16900, 0, random[rand()%7], 1, deployed, 1);
    deploy(17000, 0, random[rand()%7], 0, !deployed, 1);
    
    
    if (distance>MAXDISTANCE && distance<MAXDISTANCE+15) {
        crystal.on = 1;
        crystal.row = 10+(3*20);
        crystal.col = 240;
        crystal.cdel = -1;
    }
    
}



void setupSounds() {
        REG_SOUNDCNT_X = SND_ENABLED;

	REG_SOUNDCNT_H = SND_OUTPUT_RATIO_100 | 
                        DSA_OUTPUT_RATIO_100 | 
                        DSA_OUTPUT_TO_BOTH | 
                        DSA_TIMER0 | 
                        DSA_FIFO_RESET |
                        DSB_OUTPUT_RATIO_100 | 
                        DSB_OUTPUT_TO_BOTH | 
                        DSB_TIMER1 | 
                        DSB_FIFO_RESET;

	REG_SOUNDCNT_L = 0;
}

void playSoundA( const unsigned char* sound, int length, int frequency, int isLooping) {
        dma[1].cnt = 0;
        vbCountA = 0;
	
        int interval = 16777216/frequency;
	
        DMANow(1, sound, REG_FIFO_A, DMA_DESTINATION_FIXED | DMA_AT_REFRESH | DMA_REPEAT | DMA_32);
	
        REG_TM0CNT = 0;
	
        REG_TM0D = -interval;
        REG_TM0CNT = TIMER_ON;
	
        soundA.data = (u8 *)sound;
        soundA.duration = ((60*length)/frequency) - ((length/frequency)*3)-1;
        soundA.frequency = frequency;
        soundA.isPlaying = 1;
        soundA.length = length;
        soundA.loops = isLooping;
}


void playSoundB( const unsigned char* sound, int length, int frequency, int isLooping) {
        dma[2].cnt = 0;
        vbCountB = 0;

        int interval = 16777216/frequency;

        DMANow(2, sound, REG_FIFO_B, DMA_DESTINATION_FIXED | DMA_AT_REFRESH | DMA_REPEAT | DMA_32);

        REG_TM1CNT = 0;
	
        REG_TM1D = -interval;
        REG_TM1CNT = TIMER_ON;
	
	soundB.data = (u8 *)sound;
        soundB.duration = ((60*length)/frequency) - ((length/frequency)*3)-1;
        soundB.frequency = frequency;
        soundB.isPlaying = 1;
        soundB.length = length;
        soundB.loops = isLooping;
}

void setupInterrupts()
{
	REG_IME = 0;
	REG_INTERRUPT = (unsigned int)interruptHandler;
	REG_IE |= INT_VBLANK;
	REG_DISPSTAT |= INT_VBLANK_ENABLE;
	REG_IME = 1;
}

void interruptHandler()
{
    if ((SCREEN == GAMESCREEN) && (distance < MAXDISTANCE)) {
        vbCount++;
        time = vbCount/60;
    }
    
	REG_IME = 0;
	if(REG_IF & INT_VBLANK)
	{
            if (soundA.isPlaying) {
                vbCountA++;
                if (vbCountA>=soundA.duration) {
                    if (soundA.loops) {
                        playSoundA(soundA.data,soundA.length,soundA.frequency,1);
                        
                    }
                    else {
                        soundA.isPlaying = 0;
                        dma[1].cnt = 0;
                        REG_TM0CNT = 0;
                    }
                }  
            }
            if (soundB.isPlaying) {
                vbCountB++;
                if (vbCountB>=soundB.duration) {
                    if (soundB.loops) {
                        playSoundB(soundB.data,soundB.length,soundB.frequency,1);
                    }
                    else {
                        soundB.isPlaying = 0;
                        dma[2].cnt = 0;
                        REG_TM1CNT = 0;
                    }
                }  
            }
            REG_IF = INT_VBLANK;     
	}
	REG_IME = 1;
}

void muteSound() {
    REG_TM0CNT &= ~TIMER_ON;
    REG_TM1CNT &= ~TIMER_ON;
}

void unmuteSound() {
    REG_TM0CNT |= TIMER_ON;
    REG_TM1CNT |= TIMER_ON;
}

void stopSounds() {
    dma[0].cnt = 0;
    dma[1].cnt = 0;
}