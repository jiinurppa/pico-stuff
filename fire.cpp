#include "pico_display.hpp"
#include <stdlib.h>
#include <time.h>
#include <math.h>

using namespace pimoroni;

uint16_t buffer[PicoDisplay::WIDTH * PicoDisplay::HEIGHT];
PicoDisplay pico_display(buffer);

uint8_t decayRate = 12;

const uint8_t PIXEL_W = 4;
const uint8_t PIXEL_H = 1;

const size_t PIXELS_W = PicoDisplay::WIDTH / PIXEL_W;
const size_t PIXELS_H = PicoDisplay::HEIGHT / PIXEL_H;

uint8_t pixels[PIXELS_W * PIXELS_H];

void clearScreen(uint8_t r, uint8_t g, uint8_t b)
{
    pico_display.set_pen(r, g, b);
    pico_display.clear();
}

void clearScreen()
{
    clearScreen(0, 0, 0);
}

void setFireColor(uint8_t value)
{
    if (value < 16)                  { pico_display.set_pen(  0,   0,   0); return; }
    if (value >= 16 && value < 48)   { pico_display.set_pen(181,  23,   0); return; }
    if (value >= 48 && value < 96)   { pico_display.set_pen(237,  35,  13); return; }
    if (value >= 96 && value < 128)  { pico_display.set_pen(255, 148,   0); return; }
    if (value >= 128 && value < 160) { pico_display.set_pen(247, 186,   0); return; }
    if (value >= 160 && value < 192) { pico_display.set_pen(249, 226,  49); return; }
    if (value >= 192 && value < 224) { pico_display.set_pen(255, 252, 103); return; }
    if (value >= 224)                { pico_display.set_pen(255, 255, 255); return; }
}

void updatePixels()
{
    for (uint8_t y = 0; y < PIXELS_H; ++y)
    {
        for (uint8_t x = 0; x < PIXELS_W; ++x)
        {
            uint8_t decay = 255;

            if (y != 0)
            {
                uint8_t rate = rand() % decayRate;
                uint8_t lowerPixel = pixels[x + (y - 1) * PIXELS_W];
                
                if (lowerPixel - rate < 0)
                {
                    rate = 0;
                }
                else
                {
                    rate = lowerPixel - rate;
                }

                decay = rate;
            }

            pixels[x + y * PIXELS_W] = decay;
        }
    }
}

void drawPixels()
{
    for (uint8_t y = 0; y < PIXELS_H; ++y)
    {
        for (uint8_t x = 0; x < PIXELS_W; ++x)
        {
            size_t index = x + y * PIXELS_W;
            setFireColor(pixels[index]);
            Rect block(x * PIXEL_W, y * PIXEL_H, PIXEL_W, PIXEL_H);
            pico_display.rectangle(block);
        }
    }
}

void setBacklight()
{
    static uint8_t counter = 0;

    if (counter == 2)
    {
        uint8_t light = rand() % 64 + 100;
        pico_display.set_backlight(light);
        counter = 0;
    }
    
    ++counter;
}

void getInput()
{
    if (pico_display.is_pressed(PicoDisplay::A) && decayRate < 255)
    {
        ++decayRate;
    }
    
    if (pico_display.is_pressed(PicoDisplay::B) && decayRate > 0)
    {
        --decayRate;
    }
}

int main()
{
    srand(time(NULL));
    pico_display.init();
    pico_display.set_backlight(100);
    clearScreen();

    while (true)
    {
        getInput();
        updatePixels();
        drawPixels();
        setBacklight();
        pico_display.update();
    }

    return 0;
}
