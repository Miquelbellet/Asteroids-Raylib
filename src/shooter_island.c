#include "raylib.h"
#include <stdio.h>
#include <math.h>

#if defined(PLATFORM_WEB)
    #include <emscripten/emscripten.h>
#endif

// Include Screens
#include "screens/screens.c"
#include "screens/screen_logo.c"
#include "screens/screen_title.c"
#include "screens/screen_options.c"
#include "screens/screen_gameplay.c"
#include "screens/screen_ending.c"

GameScreen currentScreen = 0;
Font font = { 0 };
Music music = { 0 };
Sound fxSelectChange = { 0 };
Sound fxSelectConfirm = { 0 };
Sound fxShot = { 0 };
Sound fxStoneCrush = { 0 };
Sound fxShipCrush = { 0 };
Sound fxPowerUp = { 0 };
Sound fxLevelUp = { 0 };

// Required variables to manage screen transitions (fade-in, fade-out)
static float transAlpha = 0.0f;
static bool onTransition = false;
static bool transFadeOut = false;
static int transFromScreen = -1;
static int transToScreen = -1;

//----------------------------------------------------------------------------------
// Local Functions Declaration
//----------------------------------------------------------------------------------
static void TransitionToScreen(int screen);
static void UpdateTransition(void);
static void DrawTransition(void);

static void UpdateDrawFrame(void);

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{
    InitWindow(screenWidth, screenHeight, "Shooter Island");
    
    InitAudioDevice();
    font = LoadFont("resources/GemunuLibre-Regular.ttf");
    
    music = LoadMusicStream("resources/Music_Sounds/game_song.mp3");
    int musicVol = LoadStorageValue(STORAGE_POSITION_MUSIC_VOL);
    if(!musicVol || musicVol > 10000) {
        musicVol = 6;
        SaveStorageValue(STORAGE_POSITION_MUSIC_VOL, musicVolume);
    }
    SetMusicVolume(music, (float)musicVol/10);
    PlayMusicStream(music);
    
    fxSelectChange = LoadSound("resources/Music_Sounds/menu_select.wav");
    fxSelectConfirm = LoadSound("resources/Music_Sounds/menu_confirm.wav");
    fxShot = LoadSound("resources/Music_Sounds/shot.wav");
    fxStoneCrush = LoadSound("resources/Music_Sounds/stone_crush.wav");
    fxShipCrush = LoadSound("resources/Music_Sounds/ship_crush.wav");
    fxPowerUp = LoadSound("resources/Music_Sounds/power_up.wav");
    fxLevelUp = LoadSound("resources/Music_Sounds/level_up.mp3");
    
    currentScreen = LOGO;
    InitLogoScreen();

#if defined(PLATFORM_WEB)
    emscripten_set_main_loop(UpdateDrawFrame, 60, 1);
#else
    SetTargetFPS(60);
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update and Draw
        //----------------------------------------------------------------------------------
        UpdateDrawFrame();
        //----------------------------------------------------------------------------------
    }
#endif
    // De-Initialization
    //--------------------------------------------------------------------------------------
    switch (currentScreen)
    {
        case LOGO: UnloadLogoScreen(); break;
        case TITLE: UnloadTitleScreen(); break;
        case OPTIONS: UnloadOptionsScreen(); break;
        case GAMEPLAY: UnloadGameplayScreen(); break;
        case ENDING: UnloadEndingScreen(); break;
        default: break;
    }
    
    UnloadFont(font);
    UnloadMusicStream(music);
    UnloadSound(fxSelectChange);
    UnloadSound(fxSelectConfirm);
    UnloadSound(fxShot);
    UnloadSound(fxStoneCrush);
    UnloadSound(fxShipCrush);
    UnloadSound(fxPowerUp);
    UnloadSound(fxLevelUp);
    
    CloseAudioDevice();
    
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}

//------------------------------------------------------------------------------------
// Module Functions Definitions (local)
//------------------------------------------------------------------------------------

// Define transition to next screen
static void TransitionToScreen(int screen)
{
    onTransition = true;
    transFadeOut = false;
    transFromScreen = currentScreen;
    transToScreen = screen;
    transAlpha = 0.0f;
}

// Update transition effect
static void UpdateTransition(void)
{
    if (!transFadeOut)
    {
        transAlpha += 0.05f;
        
        // NOTE: Due to float internal representation, condition jumps on 1.0f instead of 1.05f
        // For that reason we compare against 1.01f, to avoid last frame loading stop
        if (transAlpha > 1.01f)
        {
            transAlpha = 1.0f;
        
            // Unload current screen
            switch (transFromScreen)
            {
                case LOGO: UnloadLogoScreen(); break;
                case TITLE: UnloadTitleScreen(); break;
                case OPTIONS: UnloadOptionsScreen(); break;
                case GAMEPLAY: UnloadGameplayScreen(); break;
                case ENDING: UnloadEndingScreen(); break;
                default: break;
            }
            
            // Load next screen
            switch (transToScreen)
            {
                case LOGO: InitLogoScreen(); break;
                case TITLE: InitTitleScreen(); break;
                case OPTIONS: InitOptionsScreen(); break;
                case GAMEPLAY: InitGameplayScreen(); break;
                case ENDING: InitEndingScreen(); break;
                default: break;
            }
            
            currentScreen = transToScreen;
            
            // Activate fade out effect to next loaded screen
            transFadeOut = true;
        }
    }
    else  // Transition fade out logic
    {
        transAlpha -= 0.02f;
        
        if (transAlpha < -0.01f)
        {
            transAlpha = 0.0f;
            transFadeOut = false;
            onTransition = false;
            transFromScreen = -1;
            transToScreen = -1;
        }
    }
}

// Draw transition effect (full-screen rectangle)
static void DrawTransition(void)
{
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, transAlpha));
}

// Update and Draw (one frame)
void UpdateDrawFrame(void)
{
    UpdateMusicStream(music);
    
    if (!onTransition)
    {
        switch(currentScreen) 
        {
            case LOGO: 
            {
                UpdateLogoScreen();
                
                if (FinishLogoScreen()) TransitionToScreen(TITLE);

            } break;
            case TITLE: 
            {
                UpdateTitleScreen();
                    
                if (FinishTitleScreen() == 1) TransitionToScreen(GAMEPLAY);
                else if (FinishTitleScreen() == 2) TransitionToScreen(OPTIONS);
                else if (FinishTitleScreen() == 3) CloseWindow();

            } break;
            case OPTIONS:
            {
                UpdateOptionsScreen();
                    
                if (FinishOptionsScreen()) TransitionToScreen(TITLE);

            } break;
            case GAMEPLAY:
            {
                UpdateGameplayScreen();
                
                if (FinishGameplayScreen() == 1) TransitionToScreen(ENDING);
                else if (FinishGameplayScreen() == 2) TransitionToScreen(TITLE);

            } break;
            case ENDING:
            {
                UpdateEndingScreen();
                
                if (FinishEndingScreen() == 1) TransitionToScreen(GAMEPLAY);
                else if (FinishEndingScreen() == 2) TransitionToScreen(TITLE);
                else if (FinishEndingScreen() == 3) CloseWindow();

            } break;
            default: break;
        }
    }
    else UpdateTransition();    // Update transition (fade-in, fade-out)
    //----------------------------------------------------------------------------------
    
    // Draw
    //----------------------------------------------------------------------------------
    BeginDrawing();
        
        ClearBackground(RAYWHITE);
            
        switch(currentScreen) 
        {
            case LOGO: DrawLogoScreen(); break;
            case TITLE: DrawTitleScreen(); break;
            case OPTIONS: DrawOptionsScreen(); break;
            case GAMEPLAY: DrawGameplayScreen(); break;
            case ENDING: DrawEndingScreen(); break;
            default: break;
        }
         
        // Draw full screen rectangle in front of everything
        if (onTransition) DrawTransition();
        
        //DrawFPS(10, 10);
        
    EndDrawing();
    //----------------------------------------------------------------------------------
}