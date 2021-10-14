typedef enum GameScreen { LOGO = 0, TITLE, OPTIONS, GAMEPLAY, ENDING } GameScreen;

typedef enum {
    STORAGE_POSITION_SCORE = 0,
    STORAGE_POSITION_TIMESCORE = 1,
    STORAGE_POSITION_HIGHSCORE = 2,
    STORAGE_POSITION_HIGHTIMESCORE = 3,
    STORAGE_POSITION_WINNER = 4,
    STORAGE_POSITION_MUSIC_VOL = 5,
    STORAGE_POSITION_SOUNDS_VOL = 6
} StorageData;

extern GameScreen currentScreen;
extern Font font;
extern Music music;
extern Sound fxSelectChange;
extern Sound fxSelectConfirm;
extern Sound fxShot;
extern Sound fxStoneCrush;
extern Sound fxShipCrush;
extern Sound fxPowerUp;
extern Sound fxLevelUp;

int screenWidth = 1280;
int screenHeight = 720;

int fontSize = 40;
int fontSpacing = 5;

//------------------------------------------------------------------------------------
// Module Functions Declarations
//------------------------------------------------------------------------------------
void InitLogoScreen(void);
void UpdateLogoScreen(void);
void DrawLogoScreen(void);
void UnloadLogoScreen(void);
int FinishLogoScreen(void);

void InitTitleScreen(void);
void UpdateTitleScreen(void);
void DrawTitleScreen(void);
void UnloadTitleScreen(void);
int FinishTitleScreen(void);

void InitOptionsScreen(void);
void UpdateOptionsScreen(void);
void DrawOptionsScreen(void);
void UnloadOptionsScreen(void);
int FinishOptionsScreen(void);

void InitGameplayScreen(void);
void UpdateGameplayScreen(void);
void DrawGameplayScreen(void);
void UnloadGameplayScreen(void);
int FinishGameplayScreen(void);

void InitEndingScreen(void);
void UpdateEndingScreen(void);
void DrawEndingScreen(void);
void UnloadEndingScreen(void);
int FinishEndingScreen(void);
