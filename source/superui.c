#include <stdio.h>

#include <nds.h>
#include "superui.h"

void fbInit(struct frameBuffer* fb, u16* vramBuffer)
{
    fb->buffer = malloc(FRAMEBUFFER_SIZE * sizeof(u16));
    fb->vramBuffer = vramBuffer;
}

#pragma region Drawing functions
void fbPutPixel(struct frameBuffer* fb, int x, int y, u16 color)
{
    if (x < 0 || x > FRAMEBUFFER_WIDTH || y < 0 || y > FRAMEBUFFER_HEIGHT)
        return;
        
    fb->buffer[x + 256 * y] = color;
}

void fbDrawLine(struct frameBuffer* fb, int x0, int y0, int x1, int y1)
{
    fbDrawLineColor(fb, x0, y0, x1, y1, ARGB16(1, 0, 0, 0));
}

void fbDrawLineColor(struct frameBuffer* fb, int x0, int y0, int x1, int y1, u16 color)
{
    int dx = abs(x1 - x0);
    int sx = x0 < x1 ? 1 : -1;
    int dy = -abs(y1 - y0);
    int sy = y0 < y1 ? 1 : -1;
    int error = dx + dy;

    while (1)
    {
        fbPutPixel(fb, x0, y0, color); // default to black
        int e2 = error * 2;
        
        if (e2 >= dy)
        {
            if (x0 == x1)
                break;
            
            error = error + dy;
            x0 += sx;
        }

        if (e2 <= dx)
        {
            if (y0 == y1)
                break;
            
            error = error + dx;
            y0 += sy;
        }
    }
}

void fbDrawChar(struct frameBuffer* fb, const void* font, int x, int y, char c, u16 color)
{
    char* bmpChar = (char*)font;

    int ci = (int)c * 8;
    
    for (size_t yl = 0; yl < 8; yl++)
    {
        for (size_t xr = 0; xr < 8; xr++)
        {
            if ((bmpChar[ci + yl] >> xr) & 1)   // reads the bmp font buffer and gets the right character and line.
                                                // the bitwise AND checks if the least significant bit (now shifted) is HIGH.
            {
                fbPutPixel(fb, x + xr, y + yl, color);
            }
        }
    }
}

void fbDrawString(struct frameBuffer* fb, const void* font, int x, int y, char* str)
{
    fbDrawStringColor(fb, font, x, y, str, ARGB16(1, 0, 0, 0));
}

void fbDrawStringColor(struct frameBuffer* fb, const void* font, int x, int y, char* str, u16 color)
{
    for (int i = 0; str[i] != '\0'; i++)
    {
        fbDrawChar(fb, font, x + i * 7, y, str[i], color);
    }
}

void fbClearBuffer(struct frameBuffer* fb, u16 color)
{
    for (size_t i = 0; i < FRAMEBUFFER_SIZE; i++)
        fb->buffer[i] = color;
}
#pragma endregion

#pragma region Buffer copy
void fbCopyBuffer(struct frameBuffer* source, struct frameBuffer* destination)
{
    dmaCopy(source->buffer, destination->buffer, FRAMEBUFFER_SIZE * sizeof(u16));
}
#pragma endregion

#pragma region VRAM buffer draw
void fbDraw(struct frameBuffer* fb)
{
    dmaCopy(fb->buffer, fb->vramBuffer, FRAMEBUFFER_SIZE * sizeof(u16));
}

void fbDrawOnTop(struct frameBuffer* fb)
{
    for (size_t i = 0; i < FRAMEBUFFER_SIZE; i++)
    {
        if(fb->buffer[i] >> 15 & 1)
            fb->vramBuffer[i] = fb->buffer[i];
    }
}
#pragma endregion