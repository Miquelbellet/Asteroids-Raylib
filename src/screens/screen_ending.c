#include "raylib.h"

static int finishScreen;

static int selectedEndOption = 0;
static int maxEndOptions = 0;

int score = 0;
float timeScore = 0;
int highScore = 0;
float highTimeScore = 0;
int winner = 0;

Image ending_image;
Texture2D ending_texture;

void InitEndingScreen(void){
    finishScreen = 0;
    
    selectedEndOption = 1;
    maxEndOptions = 3;
    
    score = LoadStorageValue(STORAGE_POSITION_SCORE);
    if(!score || score > 10000) score = 0;
    timeScore = LoadStorageValue(STORAGE_POSITION_TIMESCORE);
    if(!timeScore || timeScore > 10000) timeScore = 0.0f;
    highScore = LoadStorageValue(STORAGE_POSITION_HIGHSCORE);
    if(!highScore || highScore > 10000) highScore = 0;
    highTimeScore = LoadStorageValue(STORAGE_POSITION_HIGHTIMESCORE);
    if(!highTimeScore || highTimeScore > 10000) highTimeScore = 0.0f;
    winner = LoadStorageValue(STORAGE_POSITION_WINNER);
    if(!winner || winner > 10000) winner = 0;
    
    ending_image = LoadImage("resources/title_background.png");
    ending_texture = LoadTextureFromImage(ending_image);
    UnloadImage(ending_image);
}

void UpdateEndingScreen(void){
    if (IsKeyPressed(KEY_ENTER)){
        finishScreen = selectedEndOption;
        PlaySound(fxSelectConfirm);
    }
    if(IsKeyPressed(KEY_DOWN)){
        selectedEndOption++;
        if(selectedEndOption > maxEndOptions) selectedEndOption = 1;
        PlaySound(fxSelectChange);
    }
    else if(IsKeyPressed(KEY_UP)){
        selectedEndOption--;
        if(selectedEndOption == 0) selectedEndOption = maxEndOptions;
        PlaySound(fxSelectChange);
    }
}

void DrawEndingScreen(void){
    DrawTextureEx(ending_texture, (Vector2){0, 0}, 0, 0.4f, WHITE);
    if(winner) {
        DrawTextEx(font, "YOU WON!", (Vector2){screenWidth/2 - (MeasureTextEx(font, "YOU WON!", fontSize*2, fontSpacing).x/2), 130}, fontSize*2, fontSpacing, LIME);
    }
    else {
        DrawTextEx(font, "YOU LOOSE!", (Vector2){screenWidth/2 - (MeasureTextEx(font, "YOU LOOSE!", fontSize*2, fontSpacing).x/2), 130}, fontSize*2, fontSpacing, RED);
    }
    DrawTextEx(font, TextFormat("SCORE: %i", score), (Vector2){15, 15}, fontSize, fontSpacing, WHITE);
    DrawTextEx(font, TextFormat("TIME SCORE: %02.02fs", timeScore/100), (Vector2){15, 55}, fontSize, fontSpacing, WHITE);
    
    DrawTextEx(font, TextFormat("HIGH SCORE: %i", highScore), (Vector2){screenWidth - (MeasureTextEx(font, TextFormat("HIGH SCORE: %i", highScore), fontSize, fontSpacing).x) - 15, 15}, fontSize, fontSpacing, SKYBLUE);
    DrawTextEx(font, TextFormat("HIGH TIME SCORE: %02.02fs", highTimeScore/100), (Vector2){screenWidth - (MeasureTextEx(font, TextFormat("HIGH TIME SCORE: %02.02fs", highTimeScore/100), fontSize, fontSpacing).x) - 15, 55}, fontSize, fontSpacing, SKYBLUE);
    switch (selectedEndOption){
        case 1: {
            DrawTextEx(font, "RESTART GAME", (Vector2){textsPositionX - (MeasureTextEx(font, "RESTART GAME", fontSize, fontSpacing).x/2), screenHeight/2 - fontSize}, fontSize, fontSpacing, GREEN);
            DrawTextEx(font, "GO TO MENU", (Vector2){textsPositionX - (MeasureTextEx(font, "GO TO MENU", fontSize, fontSpacing).x/2), screenHeight/2}, fontSize, fontSpacing, WHITE);
            DrawTextEx(font, "EXIT GAME", (Vector2){textsPositionX - (MeasureTextEx(font, "EXIT GAME", fontSize, fontSpacing).x/2), screenHeight/2 + fontSize}, fontSize, fontSpacing, WHITE);
        } break;
        case 2: {
            DrawTextEx(font, "RESTART GAME", (Vector2){textsPositionX - (MeasureTextEx(font, "RESTART GAME", fontSize, fontSpacing).x/2), screenHeight/2 - fontSize}, fontSize, fontSpacing, WHITE);
            DrawTextEx(font, "GO TO MENU", (Vector2){textsPositionX - (MeasureTextEx(font, "GO TO MENU", fontSize, fontSpacing).x/2), screenHeight/2}, fontSize, fontSpacing, GREEN);
            DrawTextEx(font, "EXIT GAME", (Vector2){textsPositionX - (MeasureTextEx(font, "EXIT GAME", fontSize, fontSpacing).x/2), screenHeight/2 + fontSize}, fontSize, fontSpacing, WHITE);
        } break;
        case 3: {
            DrawTextEx(font, "RESTART GAME", (Vector2){textsPositionX - (MeasureTextEx(font, "RESTART GAME", fontSize, fontSpacing).x/2), screenHeight/2 - fontSize}, fontSize, fontSpacing, WHITE);
            DrawTextEx(font, "GO TO MENU", (Vector2){textsPositionX - (MeasureTextEx(font, "GO TO MENU", fontSize, fontSpacing).x/2), screenHeight/2}, fontSize, fontSpacing, WHITE);
            DrawTextEx(font, "EXIT GAME", (Vector2){textsPositionX - (MeasureTextEx(font, "EXIT GAME", fontSize, fontSpacing).x/2), screenHeight/2 + fontSize}, fontSize, fontSpacing, GREEN);
        } break;
        default: break;
    }
}

void UnloadEndingScreen(void){
    
}

int FinishEndingScreen(void){
    return finishScreen;
}