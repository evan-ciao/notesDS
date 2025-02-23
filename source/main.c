#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include <gl2d.h>
#include <nds.h>

#include "superui.h"
#include "defaultFont.h"
#include "logo.h"
#include "ball.h"

#define GREY ARGB16(1, 13, 13, 13)

struct menuPointGfx
{
    int16 x;
    int16 y;
    int16 vx;
    int16 vy;
};

void moveMenuPoint(struct menuPointGfx* p)
{
    p->x += p->vx;
    p->y += p->vy;
    if(p->x > SCREEN_WIDTH || p->x < 0)
        p->vx *= -1;
    if(p->y > SCREEN_HEIGHT || p->y < 0)
        p->vy *= -1;
}

int main(int argc, char **argv)
{    
    touchPosition touchPos;

    // modes
    videoSetMode(MODE_5_2D);
    videoSetModeSub(MODE_5_2D);

    // set vram banks for the two framebuffers
    vramSetBankA(VRAM_A_MAIN_BG);
    vramSetBankC(VRAM_C_SUB_BG);

    // set vram bank for sub sprite memory
    vramSetBankI(VRAM_I_SUB_SPRITE);

    int canvasBg = bgInit(2, BgType_Bmp16, BgSize_B16_256x256, 0, 0);
    u16* canvasVram = bgGetGfxPtr(canvasBg);
    
    int canvasSubBg = bgInitSub(2, BgType_Bmp16, BgSize_B16_256x256, 0, 0);
    u16* canvasSubVram = bgGetGfxPtr(canvasSubBg);

    // trying out the new superui library
    struct frameBuffer uiMainFrameBuffer;
    struct frameBuffer uiSubFrameBuffer;
    
    fbInit(&uiMainFrameBuffer, canvasVram);
    fbInit(&uiSubFrameBuffer, canvasSubVram);

    fbClearBuffer(&uiMainFrameBuffer, ARGB16(1, 31, 31, 31));
    fbClearBuffer(&uiSubFrameBuffer, ARGB16(1, 31, 31, 31));

    struct menuPointGfx menuPoints[4];
    
    srand(time(NULL));
    for (size_t i = 0; i < sizeof(menuPoints) / sizeof(struct menuPointGfx); i++)
        menuPoints[i] = (struct menuPointGfx){ rand() % SCREEN_WIDTH, rand() % SCREEN_HEIGHT, rand() % 9 - 4, rand() % 9 - 4 };

    /* TO DO: Refactor all of this crap */
    // OAM
    oamInit(&oamSub, SpriteMapping_1D_32, false);
    u16 *gfxSub = oamAllocateGfx(&oamSub, SpriteSize_32x32, SpriteColorFormat_256Color);

    dmaCopy(logoTiles, gfxSub, logoTilesLen);
    dmaCopy(logoPal, SPRITE_PALETTE_SUB, logoPalLen);

    // logo oam entry
    oamSet(&oamSub, 0,
        SCREEN_WIDTH / 2 - 64, SCREEN_HEIGHT / 2 - 40, // X, Y
        0, // Priority
        0, // Palette index
        SpriteSize_64x32, SpriteColorFormat_256Color, // Size, format
        gfxSub,  // Graphics offset
        0, // Affine matrix index
        true, // Double size
        false, // Hide
        false, false, // H flip, V flip
        false); // Mosaic
    
    oamEnable(&oamSub);
    DC_FlushAll();

    lcdMainOnBottom();

    s16 angle = 0;

    while (1)
    {
        // handle user input
        // -----------------
        scanKeys();

        uint16_t keys = keysHeld();

        if (keys & KEY_START)
            break;

        if (keys & KEY_TOUCH)
        {
            touchRead(&touchPos);
        }

        // render scene
        // ---------------
        fbDraw(&uiMainFrameBuffer);
        fbDraw(&uiSubFrameBuffer);

        fbClearBuffer(&uiSubFrameBuffer, ARGB16(1, 31, 31, 31));
        
        size_t menuPointsSize = sizeof(menuPoints) / sizeof(struct menuPointGfx); 
        for (size_t i = 0; i < menuPointsSize; i++)
        {
            moveMenuPoint(&menuPoints[i]);

            for (size_t j = 0; i < menuPointsSize; j++)
            {
                if (j == i)
                    break;
            
                fbDrawLineColor(&uiSubFrameBuffer, menuPoints[i].x, menuPoints[i].y, menuPoints[j].x, menuPoints[j].y, GREY);   
            }

            fbDrawLineColor(&uiSubFrameBuffer, menuPoints[i].x, menuPoints[i].y, 0, 0, GREY);
            fbDrawLineColor(&uiSubFrameBuffer, menuPoints[i].x, menuPoints[i].y, SCREEN_WIDTH, 0, GREY);
            fbDrawLineColor(&uiSubFrameBuffer, menuPoints[i].x, menuPoints[i].y, SCREEN_WIDTH, SCREEN_HEIGHT, GREY);
            fbDrawLineColor(&uiSubFrameBuffer, menuPoints[i].x, menuPoints[i].y, 0, SCREEN_HEIGHT, GREY);
            
            fbDrawLineColor(&uiSubFrameBuffer, menuPoints[i].x, menuPoints[i].y, SCREEN_WIDTH, SCREEN_HEIGHT / 2, GREY);
            fbDrawLineColor(&uiSubFrameBuffer, menuPoints[i].x, menuPoints[i].y, 0, SCREEN_HEIGHT / 2, GREY);
            fbDrawLineColor(&uiSubFrameBuffer, menuPoints[i].x, menuPoints[i].y, SCREEN_WIDTH / 2, 0, GREY);
            fbDrawLineColor(&uiSubFrameBuffer, menuPoints[i].x, menuPoints[i].y, SCREEN_WIDTH / 2, SCREEN_HEIGHT, GREY);
        }

        fbDrawString(&uiSubFrameBuffer, &defaultFontBitmap, SCREEN_WIDTH / 2 - 26, SCREEN_HEIGHT - 20, "by evan");

        // update OAM
        oamRotateScale(&oamSub, 0, 0, 150 + sinLerp(angle) / 250, 150 + cosLerp(angle) / 300);
        oamUpdate(&oamSub);

        angle += degreesToAngle(4);

        // synchronize game loop to the screen refresh
        swiWaitForVBlank();
    }

    oamFreeGfx(&oamSub, gfxSub);

    return 0;
}