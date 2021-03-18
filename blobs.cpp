#include "pico_display.hpp"
#include <stdlib.h>
#include <time.h>
#include <math.h>

using namespace pimoroni;

double changeRate = 2.0;

const uint8_t PIXEL_W = 4;
const uint8_t PIXEL_H = 9;

const size_t PIXELS_W = PicoDisplay::WIDTH / PIXEL_W;
const size_t PIXELS_H = PicoDisplay::HEIGHT/ PIXEL_H;

const size_t BLOBS = 3;

struct Blob
{
    uint8_t x, y, speed;
    int8_t dirX, dirY;
    bool inflating;
    double intensity;
};

uint16_t buffer[PicoDisplay::WIDTH * PicoDisplay::HEIGHT];
PicoDisplay pico_display(buffer);

uint8_t pixels[PIXELS_W * PIXELS_H];
Blob blobs[BLOBS];

void initBlobs()
{
    const uint8_t MAX_SPEED = 1;

    for (size_t i = 0; i < BLOBS; ++i)
    {
        blobs[i].x = rand() % PIXELS_W;
        blobs[i].y = rand() % PIXELS_H;

        blobs[i].speed = rand() % MAX_SPEED + 1;

        blobs[i].dirX = rand() % 3 - 1;
        blobs[i].dirY = rand() % 3 - 1;

        if (blobs[i].dirX == 0)
        {
            blobs[i].dirX = 1;
        }
        
        if (blobs[i].dirY == 0)
        {
            blobs[i].dirY = 1;
        }

        if (blobs[i].dirX == 1)
        {
            blobs[i].inflating = true;
        }
        
        blobs[i].intensity = double(rand() % 254) + 2.0;
    }
}

void updateBlobs()
{
    for (size_t i = 0; i < BLOBS; ++i)
    {
        blobs[i].x += blobs[i].dirX * blobs[i].speed;
        blobs[i].y += blobs[i].dirY * blobs[i].speed;
        
        if (blobs[i].x < 1)
        {
            blobs[i].x = 0;
            blobs[i].dirX = 1;
        }
        
        if (blobs[i].x > PIXELS_W - 1)
        {
            blobs[i].x = PIXELS_W;
            blobs[i].dirX = -1;
        }

        if (blobs[i].y < 1)
        {
            blobs[i].y = 0;
            blobs[i].dirY = 1;
        }
        
        if (blobs[i].y > PIXELS_H - 1)
        {
            blobs[i].y = PIXELS_H;
            blobs[i].dirY = -1;
        }

        if (blobs[i].inflating)
        {
            blobs[i].intensity += changeRate;

            if (blobs[i].intensity > 255.0)
            {
                blobs[i].intensity = 255.0;
                blobs[i].inflating = false;
            }
        }
        else
        {
            blobs[i].intensity -= changeRate;

            if (blobs[i].intensity < 1.0)
            {
                blobs[i].intensity = 1.0;
                blobs[i].inflating = true;
            }
        }
    }
}

void clearScreen(uint8_t r, uint8_t g, uint8_t b)
{
    pico_display.set_pen(r, g, b);
    pico_display.clear();
}

void clearScreen()
{
    clearScreen(0, 0, 0);
}

void setBlobColor(uint8_t value)
{
    if (value < 20)                  { pico_display.set_pen(  2,   0,   3); return; }
    if (value >= 20 && value < 45)   { pico_display.set_pen(  6,   5,  84); return; }
    if (value >= 45 && value < 70)   { pico_display.set_pen(  9,   9, 121); return; }
    if (value >= 70 && value < 95)   { pico_display.set_pen(  7,  54, 150); return; }
    if (value >= 95 && value < 120)  { pico_display.set_pen(  5,  89, 173); return; }
    if (value >= 120 && value < 145) { pico_display.set_pen(  4, 119, 193); return; }
    if (value >= 145 && value < 170) { pico_display.set_pen(  3, 152, 215); return; }
    if (value >= 170 && value < 195) { pico_display.set_pen(  2, 176, 231); return; }
    if (value >= 195 && value < 220) { pico_display.set_pen(  0, 212, 255); return; }
    if (value >= 220 && value < 240) { pico_display.set_pen(143, 236, 255); return; }
    if (value >= 240)                { pico_display.set_pen(255, 255, 255); return; }
}

void updatePixels()
{
    const double PIXELS_HALF_W = PIXELS_W / 2.0;
    const double PIXELS_HALF_H = PIXELS_H / 2.0;
    const uint8_t ambientIntensity = 64;

    for (uint16_t y = 0; y < PIXELS_H; ++y)
    {
        for (uint16_t x = 0; x < PIXELS_W; ++x)
        {
            uint8_t intensity = ambientIntensity;

            for (size_t i = 0; i < BLOBS; ++i)
            {
                double dx = (x - blobs[i].x) / PIXELS_HALF_W;
                double dy = (y - blobs[i].y) / PIXELS_HALF_H;

                double distance = 1.0f - sqrt(pow(dx, 2) + pow(dy, 2));

                uint8_t value = (uint8_t)(blobs[i].intensity * distance);

                if (intensity + value > 255)
                {
                    intensity = 255;
                    break;
                }
                
                intensity += value;
            }
            
            pixels[x + y * PIXELS_W] = intensity;
        }
    }
}

void drawPixels()
{
    for (size_t y = 0; y < PIXELS_H; ++y)
    {
        for (size_t x = 0; x < PIXELS_W; ++x)
        {
            size_t index = x + y * PIXELS_W;
            setBlobColor(pixels[index]);
            Rect block(x * PIXEL_W, y * PIXEL_H, PIXEL_W, PIXEL_H);
            pico_display.rectangle(block);
        }
    }
}

void getInput()
{
    if (pico_display.is_pressed(PicoDisplay::A) && changeRate < 255.0)
    {
        changeRate += 1.0;
    }
    
    if (pico_display.is_pressed(PicoDisplay::B) && changeRate > 0.0)
    {
        changeRate -= 1.0;
    }
}

int main()
{
    srand(time(NULL));
    pico_display.init();
    pico_display.set_backlight(100);
    clearScreen();
    initBlobs();

    while (true)
    {
        getInput();
        updateBlobs();
        updatePixels();
        drawPixels();
        pico_display.update();
    }

    return 0;
}
