#include <nds.h>
#ifndef _superui_h
#define _superui_h

#pragma region Frame Buffer

#define FRAMEBUFFER_SIZE 256 * 192
#define FRAMEBUFFER_WIDTH 256
#define FRAMEBUFFER_HEIGHT 192

struct frameBuffer
{
    u16* vramBuffer;    // pointer to VRAM memory
    u16* buffer;        // pointer to the frame buffer
};

void fbInit(struct frameBuffer* fb, u16* vramBuffer);
void fbDeconstruct(struct frameBuffer* fb);

#pragma region Drawing functions
void fbPutPixel(struct frameBuffer* fb, int x, int y, u16 color);
void fbDrawLine(struct frameBuffer* fb, int x0, int y0, int x1, int y1);
void fbClearBuffer(struct frameBuffer* fb, u16 color);
void fbDrawChar(struct frameBuffer* fb, const void* font, int x, int y, char c);
void fbDrawString(struct frameBuffer* fb, const void* font, int x, int y, char* str);
#pragma endregion

#pragma region Buffer copy
void fbCopyBuffer(struct frameBuffer* source, struct frameBuffer* destination);
#pragma endregion

#pragma region VRAM buffer draw
void fbDraw(struct frameBuffer* fb);
void fbDrawOnTop(struct frameBuffer* fb);
#pragma endregion

#pragma endregion

#endif