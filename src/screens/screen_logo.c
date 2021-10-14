#include "raylib.h"

static int finishScreen = 0;
static int framesCounter = 0;
static float alpha = 0.0f;
static bool fade_in = false;

Image logo_image;
Texture2D logo_texture;
Vector2 logo_position = {0, 0};



void InitLogoScreen(void){
    finishScreen = 0;
    framesCounter = 0;
    alpha = 1.0f;
    fade_in = true;
    
    logo_image = LoadImage("resources/uoc_logo.png");
    logo_texture = LoadTextureFromImage(logo_image);
    UnloadImage(logo_image);
    
    logo_position.x = (float)(screenWidth/2 - logo_texture.width/2);
    logo_position.y = (float)(screenHeight/2 - logo_texture.height/2);
}

void UpdateLogoScreen(void){
    framesCounter++;
    
    if (fade_in) {
        alpha -= 0.02f;
        if(alpha < 0.0f) {
            fade_in = false;
            framesCounter = 0;
            alpha = 0.0f;
        }
    }
    else if(!fade_in && framesCounter > 100) {
        alpha += 0.02f;

        if (alpha > 1.0f)
        {
            alpha = 1.0f;
            finishScreen = 1;
        }
    }
}

void DrawLogoScreen(void){
    DrawTextureV(logo_texture, logo_position, WHITE);
    DrawRectangle(0, 0, screenWidth, screenHeight, Fade(BLACK, alpha));
}

void UnloadLogoScreen(void){
    UnloadTexture(logo_texture);
}

int FinishLogoScreen(void){
    return finishScreen;
}