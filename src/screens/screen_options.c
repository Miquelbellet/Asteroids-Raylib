#include "raylib.h"

static int finishScreen;
int optionSelected = 0;
int maxOptions = 0;

int musicVolume = 0;
int soundsVolume = 0;

Image options_image;
Texture2D options_texture;

void InitOptionsScreen(void){
    finishScreen = 0;
    optionSelected = 1;
    maxOptions = 2;
    
    musicVolume = LoadStorageValue(STORAGE_POSITION_MUSIC_VOL);
    if(!musicVolume || musicVolume > 10) musicVolume = 6;
    soundsVolume = LoadStorageValue(STORAGE_POSITION_SOUNDS_VOL);
    if(!soundsVolume || soundsVolume > 10) soundsVolume = 6;
    
    options_image = LoadImage("resources/title_background.png");
    options_texture = LoadTextureFromImage(options_image);
    UnloadImage(options_image);
}

void UpdateOptionsScreen(void){
    if (IsKeyPressed(KEY_ENTER)){
        SaveStorageValue(STORAGE_POSITION_MUSIC_VOL, musicVolume);
        PlaySound(fxSelectConfirm);
        finishScreen = 1;
    }
    else if(IsKeyPressed(KEY_DOWN)){
        optionSelected++;
        if(optionSelected > maxOptions) optionSelected = 1;
        PlaySound(fxSelectChange);
    }
    else if(IsKeyPressed(KEY_UP)){
        optionSelected--;
        if(optionSelected == 0) optionSelected = maxOptions;
        PlaySound(fxSelectChange);
    }
    else if(IsKeyPressed(KEY_RIGHT)){
        PlaySound(fxSelectChange);
        switch(optionSelected) {
            case 1: {
                musicVolume++;
                if(musicVolume > 10) musicVolume = 10;
                SetMusicVolume(music, (float)musicVolume/10);
            } break;
            case 2: {
                soundsVolume++;
                if(soundsVolume > 10) soundsVolume = 10;
                SetSoundVolume(fxSelectChange, (float)soundsVolume/10);
                SetSoundVolume(fxSelectConfirm, (float)soundsVolume/10);
                SetSoundVolume(fxShot, (float)soundsVolume/10);
                SetSoundVolume(fxStoneCrush, (float)soundsVolume/10);
                SetSoundVolume(fxShipCrush, (float)soundsVolume/10);
                SetSoundVolume(fxPowerUp, (float)soundsVolume/10);
                SetSoundVolume(fxLevelUp, (float)soundsVolume/10);
            } break;
            default: break;
        }
    }
    else if(IsKeyPressed(KEY_LEFT)){
        PlaySound(fxSelectChange);
        switch(optionSelected) {
            case 1: {
                musicVolume--;
                if(musicVolume < 0) musicVolume = 0;
                SetMusicVolume(music, (float)musicVolume/10);
            } break;
            case 2: {
                soundsVolume--;
                if(soundsVolume < 0) soundsVolume = 0;
                SetSoundVolume(fxSelectChange, (float)soundsVolume/10);
                SetSoundVolume(fxSelectConfirm, (float)soundsVolume/10);
                SetSoundVolume(fxShot, (float)soundsVolume/10);
                SetSoundVolume(fxStoneCrush, (float)soundsVolume/10);
                SetSoundVolume(fxShipCrush, (float)soundsVolume/10);
                SetSoundVolume(fxPowerUp, (float)soundsVolume/10);
                SetSoundVolume(fxLevelUp, (float)soundsVolume/10);
            } break;
            default: break;
        }
    }
}

void DrawOptionsScreen(void){
    DrawTextureEx(title_texture, (Vector2){0, 0}, 0, 0.4f, WHITE);
    switch(optionSelected){
        case 1: {
            DrawTextEx(font, TextFormat("MUSIC VOLUME < %i >", musicVolume), (Vector2){textsPositionX - (MeasureTextEx(font, TextFormat("MUSIC VOLUME < %i >", musicVolume), fontSize, fontSpacing).x/2), screenHeight/2 - fontSize}, fontSize, fontSpacing, GREEN);
            DrawTextEx(font, TextFormat("SOUNDS VOLUME < %i >", soundsVolume), (Vector2){textsPositionX - (MeasureTextEx(font, TextFormat("SOUNDS VOLUME < %i >", soundsVolume), fontSize, fontSpacing).x/2), screenHeight/2}, fontSize, fontSpacing, WHITE);
        } break;
        case 2: {
            DrawTextEx(font, TextFormat("MUSIC VOLUME < %i >", musicVolume), (Vector2){textsPositionX - (MeasureTextEx(font, TextFormat("MUSIC VOLUME < %i >", musicVolume), fontSize, fontSpacing).x/2), screenHeight/2 - fontSize}, fontSize, fontSpacing, WHITE);
            DrawTextEx(font, TextFormat("SOUNDS VOLUME < %i >", soundsVolume), (Vector2){textsPositionX - (MeasureTextEx(font, TextFormat("SOUNDS VOLUME < %i >", soundsVolume), fontSize, fontSpacing).x/2), screenHeight/2}, fontSize, fontSpacing, GREEN);
        } break;
        default: break;
    }
    DrawTextEx(font, "PRESS [ENTER] TO SAVE AND GO TO MENU", (Vector2){screenWidth/2 - (MeasureTextEx(font, "PRESS [ENTER] TO SAVE AND GO TO MENU", fontSize/2, fontSpacing).x/2), screenHeight/2 + fontSize + 15}, fontSize/2, fontSpacing, GRAY);
}

void UnloadOptionsScreen(void){
    
}

int FinishOptionsScreen(void){
    return finishScreen;
}