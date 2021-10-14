#include "raylib.h"

static int finishScreen;

static bool showedAnimation = false;
static int textsPositionX = 0;

static int selectedTitleOption = 0;
static int maxTitleOptions = 0;

Image title_image;
Texture2D title_texture;

void InitTitleScreen(void){
    finishScreen = 0;
    SaveStorageValue(STORAGE_POSITION_SCORE, 0);
    SaveStorageValue(STORAGE_POSITION_TIMESCORE, 0.0f);
    SaveStorageValue(STORAGE_POSITION_WINNER, 0);
    
    showedAnimation = false;
    textsPositionX = -300;
    
    selectedTitleOption = 1;
    maxTitleOptions = 3;
    
    title_image = LoadImage("resources/title_background.png");
    title_texture = LoadTextureFromImage(title_image);
    UnloadImage(title_image);
}

void UpdateTitleScreen(void){
    if(showedAnimation){
        if (IsKeyPressed(KEY_ENTER)){
            finishScreen = selectedTitleOption;
            PlaySound(fxSelectConfirm);
        }
        if(IsKeyPressed(KEY_DOWN)){
            selectedTitleOption++;
            if(selectedTitleOption > maxTitleOptions) selectedTitleOption = 1;
            PlaySound(fxSelectChange);
        }
        else if(IsKeyPressed(KEY_UP)){
            selectedTitleOption--;
            if(selectedTitleOption == 0) selectedTitleOption = maxTitleOptions;
            PlaySound(fxSelectChange);
        }
    }
    else {
        textsPositionX += 10;
        if(textsPositionX >= screenWidth/2){
            textsPositionX = screenWidth/2;
            showedAnimation = true;
        }
    }
}

void DrawTitleScreen(void){
    DrawTextureEx(title_texture, (Vector2){0, 0}, 0, 0.4f, WHITE);
    switch (selectedTitleOption){
        case 1: {
            DrawTextEx(font, "PLAY GAME", (Vector2){textsPositionX - (MeasureTextEx(font, "PLAY GAME", fontSize, fontSpacing).x/2), screenHeight/2 - fontSize}, fontSize, fontSpacing, GREEN);
            DrawTextEx(font, "OPTIONS", (Vector2){textsPositionX - (MeasureTextEx(font, "OPTIONS", fontSize, fontSpacing).x/2), screenHeight/2}, fontSize, fontSpacing, WHITE);
            DrawTextEx(font, "EXIT GAME", (Vector2){textsPositionX - (MeasureTextEx(font, "EXIT GAME", fontSize, fontSpacing).x/2), screenHeight/2 + fontSize}, fontSize, fontSpacing, WHITE);
        } break;
        case 2: {
            DrawTextEx(font, "PLAY GAME", (Vector2){textsPositionX - (MeasureTextEx(font, "PLAY GAME", fontSize, fontSpacing).x/2), screenHeight/2 - fontSize}, fontSize, fontSpacing, WHITE);
            DrawTextEx(font, "OPTIONS", (Vector2){textsPositionX - (MeasureTextEx(font, "OPTIONS", fontSize, fontSpacing).x/2), screenHeight/2}, fontSize, fontSpacing, GREEN);
            DrawTextEx(font, "EXIT GAME", (Vector2){textsPositionX - (MeasureTextEx(font, "EXIT GAME", fontSize, fontSpacing).x/2), screenHeight/2 + fontSize}, fontSize, fontSpacing, WHITE);
        } break;
        case 3: {
            DrawTextEx(font, "PLAY GAME", (Vector2){textsPositionX - (MeasureTextEx(font, "PLAY GAME", fontSize, fontSpacing).x/2), screenHeight/2 - fontSize}, fontSize, fontSpacing, WHITE);
            DrawTextEx(font, "OPTIONS", (Vector2){textsPositionX - (MeasureTextEx(font, "OPTIONS", fontSize, fontSpacing).x/2), screenHeight/2}, fontSize, fontSpacing, WHITE);
            DrawTextEx(font, "EXIT GAME", (Vector2){textsPositionX - (MeasureTextEx(font, "EXIT GAME", fontSize, fontSpacing).x/2), screenHeight/2 + fontSize}, fontSize, fontSpacing, GREEN);
        } break;
        default: break;
    }
}

void UnloadTitleScreen(void){
    
}

int FinishTitleScreen(void){
    return finishScreen;
}