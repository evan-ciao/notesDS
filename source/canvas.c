#include <stdio.h>
#include <string.h>

#include <gl2d.h>
#include <nds.h>

#include "superui.h"
#include "defaultFont.h"

    #pragma region Globals
u16 penColor = ARGB16(1, 0, 0, 0);
    #pragma endregion

void drawUI(struct frameBuffer* uifb)
{
    fbClearBuffer(uifb, ARGB16(0, 0, 0, 0));
    
    fbDrawString(uifb, &defaultFontBitmap, 10, FRAMEBUFFER_HEIGHT - 18, "page 1/2");
    char uiPenColor[5];
    snprintf(uiPenColor, sizeof(uiPenColor), "%04x", penColor & 0x7FFF);
    fbDrawStringColor(uifb, &defaultFontBitmap, FRAMEBUFFER_WIDTH - 10 - 24, FRAMEBUFFER_HEIGHT - 18, uiPenColor, penColor);
}

int canvasSetup(int argc, char **argv)
{
    // some vars
    touchPosition touchPos;
    touchPosition lastTouchPos;
    bool lastTouch = false;

    // modes
    videoSetMode(MODE_5_2D);
    videoSetModeSub(MODE_5_2D);

    // set vram banks for the two framebuffers
    vramSetBankA(VRAM_A_MAIN_BG);
    vramSetBankC(VRAM_C_SUB_BG);

    int canvasBg = bgInit(2, BgType_Bmp16, BgSize_B16_256x256, 0, 0);
    u16* canvasVram = bgGetGfxPtr(canvasBg);
    
    int canvasSubBg = bgInitSub(2, BgType_Bmp16, BgSize_B16_256x256, 0, 0);
    u16* canvasSubVram = bgGetGfxPtr(canvasSubBg);

    // trying out the new superui library
    struct frameBuffer canvasMainFrameBuffer;
    struct frameBuffer canvasSubFrameBuffer;
    struct frameBuffer uiSubFrameBuffer;
    
    fbInit(&canvasMainFrameBuffer, canvasVram);
    fbInit(&canvasSubFrameBuffer, canvasSubVram);
    fbInit(&uiSubFrameBuffer, canvasSubVram);

    fbClearBuffer(&canvasMainFrameBuffer, ARGB16(1, 31, 31, 31));
    fbClearBuffer(&canvasSubFrameBuffer, ARGB16(1, 31, 31, 31));

    fbDrawStringColor(&canvasSubFrameBuffer, &defaultFontBitmap, FRAMEBUFFER_WIDTH / 2 - 22, FRAMEBUFFER_HEIGHT / 2 - 18, "empty", ARGB16(1, 13, 13, 13));

    lcdMainOnBottom();

    while (1)
    {
        // handle user input
        // -----------------
        scanKeys();

        uint16_t keys = keysHeld();

        if (keys & KEY_START)
            break;

        /* PEN LOGIC */
        if (keys & KEY_TOUCH)
        {
            touchRead(&touchPos);

            if(lastTouch)
            {
                fbDrawLineColor(&canvasMainFrameBuffer, lastTouchPos.px, lastTouchPos.py, touchPos.px, touchPos.py, penColor);
                lastTouchPos = touchPos;
            }
            else
            {
                lastTouchPos = touchPos;
                lastTouch = true;
            }
        }
        else
        {
            lastTouch = false;
        }

        /* PEN COLOR */
        if (keys & KEY_RIGHT)
        {
            penColor += 1;

            if(penColor > 65534)
                penColor = 8;
        }

        // render scene
        // ---------------
        fbDraw(&canvasMainFrameBuffer);
        fbDraw(&canvasSubFrameBuffer);
        fbDrawOnTop(&uiSubFrameBuffer);

        drawUI(&uiSubFrameBuffer);

        if (keys & KEY_Y)
        {
            fbCopyBuffer(&canvasMainFrameBuffer, &canvasSubFrameBuffer);
            fbDrawOnTop(&uiSubFrameBuffer);
        }

        if (keys & KEY_X)
        {
            fbClearBuffer(&canvasSubFrameBuffer, ARGB16(1, 31, 31, 31));
            fbDrawOnTop(&uiSubFrameBuffer);
        }

        // synchronize game loop to the screen refresh
        swiWaitForVBlank();
    }

    return 0;
}