//
// Created by rubienr on 08/11/17.
//

//#include <Wire.h>
#include "types/Ressources.h"
//#include <SSD1306.h>
//#include <FastLED.h>
#include "wifi/Wifi.h"
#include "esp/esp.h"

Ressources r;


// COOLING: How much does the air cool as it rises?
// Less cooling = taller flames.  More cooling = shorter flames.
// Default 55, suggested range 20-100
#define COOLING  55

// SPARKING: What chance (out of 255) is there that a new spark will be lit?
// Higher chance = more roaring fire.  Lower chance = more flickery fire.
// Default 120, suggested range 50-200.
#define SPARKING 120
bool gReverseDirection = false;

CRGBPalette16 gPal = HeatColors_p;

void Fire2012WithPalette()
{
// Array of temperature readings at each simulation cell
    static byte heat[r.ledStrip.numLeds];

    // Step 1.  Cool down every cell a little
    for (int i = 0; i < r.ledStrip.numLeds; i++)
    {
        heat[i] = qsub8(heat[i], random8(0, ((COOLING * 10) / r.ledStrip.numLeds) + 2));
    }

    // Step 2.  Heat from each cell drifts 'up' and diffuses a little
    for (int k = r.ledStrip.numLeds - 1; k >= 2; k--)
    {
        heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2]) / 3;
    }

    // Step 3.  Randomly ignite new 'sparks' of heat near the bottom
    if (random8() < SPARKING)
    {
        int y = random8(7);
        heat[y] = qadd8(heat[y], random8(160, 255));
    }

    // Step 4.  Map from heat cells to LED colors
    for (int j = 0; j < r.ledStrip.numLeds; j++)
    {
        // Scale the heat value from 0-255 down to 0-240
        // for best results with color palettes.
        byte colorindex = scale8(heat[j], 240);
        CRGB color      = ColorFromPalette(gPal, colorindex);
        int  pixelnumber;
        if (gReverseDirection)
        {
            pixelnumber = (r.ledStrip.numLeds - 1) - j;
        }
        else
        {
            pixelnumber = j;
        }
        r.ledStrip.leds[pixelnumber] = color;
    }
}

void setup()
{
    r.init();
    r.builtin.ledOn();

    /*initEsp(true);
    r.display->println("Office Gadget Started");
    printEspInfo();
    r.display->println("WiFi: enabling...");
    initWifi();
     */
}

void loop()
{
    Fire2012WithPalette(); // run simulation frame, using palette colors
    r.ledStrip.fastLED.show(); // display this frame
    //r.ledStrip.fastLED.delay(1000 / 60);
    r.sensors.measureTemperature();
    r.keyboard.update();

    r.displays.display0.clear();
    r.displays.display0.print("The office gadget!\n");
    r.displays.display0.print("temperature ");
    r.displays.display0.print(r.sensors.getTemperature());
    r.displays.display0.print("°C\n");
    r.displays.display0.print("lux: ");
    r.displays.display0.print(r.sensors.currentLux());
    r.displays.display0.print("\n\n\n");
    r.displays.display0.drawLogBuffer(0, 0);
    r.displays.display0.display();

    r.displays.display1.clear();
    r.displays.display0.print("Keyboard Events\n");
    r.displays.display1.printf("pressed  %d\n", r.keyboard.buttonPressed);
    r.displays.display1.printf("released %d\n", r.keyboard.buttonReleased);
    r.displays.display1.printf("repeated %d\n", r.keyboard.buttonRepeated);
    r.displays.display1.printf("delay    %d count %d\n", r.keyboard.buttonElapsedTime, r.keyboard.buttonEventCount);
    r.displays.display1.drawLogBuffer(0, 0);
    r.displays.display1.display();
}