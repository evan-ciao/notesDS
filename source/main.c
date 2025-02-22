#include <stdio.h>
#include <string.h>

#include <gl2d.h>
#include <nds.h>

#include "superui.h"

#include "city.h"
#include "defaultFont.h"

int main(int argc, char **argv)
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
    fbClearBuffer(&uiSubFrameBuffer, ARGB16(0, 0, 0, 0));

    fbDrawString(&canvasSubFrameBuffer, &defaultFontBitmap, 30, 50, "finally some fucking text!");
    fbDrawString(&canvasMainFrameBuffer, &defaultFontBitmap, 30, 50, "on both fucking screens.");

    fbDrawString(&uiSubFrameBuffer, &defaultFontBitmap, 10, FRAMEBUFFER_HEIGHT - 18, "0");
    fbDrawString(&uiSubFrameBuffer, &defaultFontBitmap, 30, FRAMEBUFFER_HEIGHT - 18, "pencil");

    lcdMainOnBottom();

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

            if(lastTouch)
            {
                fbDrawLine(&canvasMainFrameBuffer, lastTouchPos.px, lastTouchPos.py, touchPos.px, touchPos.py);
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

        if (keys & KEY_Y)
        {
            fbCopyBuffer(&canvasMainFrameBuffer, &canvasSubFrameBuffer);
            fbDrawOnTop(&uiSubFrameBuffer);
        }

        // render scene
        // ---------------
        fbDraw(&canvasMainFrameBuffer);
        fbDraw(&canvasSubFrameBuffer);
        fbDrawOnTop(&uiSubFrameBuffer);

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