#include "raylib.h"

//----------------------------------------------------------------------------------
// Global Definitions
//----------------------------------------------------------------------------------
#define PLAYER_BASE_SIZE    25.0f
#define PLAYER_SPEED        6.0f
#define PLAYER_MAX_SHOOTS   20

#define NUMBER_OF_WALLS     4
#define WALLS_WIDTH         15
#define NUMBER_OF_LEVELS    5

#define METEORS_SPEED       2
#define BALL_SIZE           10
#define MULTISHOT_DURATION  300

//----------------------------------------------------------------------------------
// Types and Structures Definitions
//----------------------------------------------------------------------------------
typedef struct Player {
    Vector2 position;
    float speed;
    float acceleration;
    float rotation;
    Vector3 collider;
    Color color;
} Player;

typedef struct Shoot {
    Vector2 position;
    Vector2 speed;
    float radius;
    float rotation;
    int lifeSpawn;
    bool active;
    Color color;
} Shoot;

typedef struct Meteor {
    Vector2 position;
    Vector2 speed;
    float radius;
    float rotation;
    bool active;
    Color color;
} Meteor;

typedef struct Wall {
    Rectangle rect;
} Wall;

//----------------------------------------------------------------------------------
// Functions Definitions
//----------------------------------------------------------------------------------
void InitializeTextures(void);
void InitializeLevel(void);
void InitializePlayer(void);
void InitializeWalls(void);
void InitializeShoots(void);
void InitializeMeteors(void);

void PlayerLogic(void);
void ShootLogic(void);
void MeteorsLogic(void);
void CollisionLogic(void);
void EndGameplay(int);

//------------------------------------------------------------------------------------
// Global Variables Declaration
//------------------------------------------------------------------------------------
bool pause = false;
bool blockTopScreen = false;
bool blockBotScreen = false;
bool blockLeftScreen = false;
bool blockRightScreen = false;

float shipHeight = (PLAYER_BASE_SIZE/2)/tanf(20*DEG2RAD);

int currentLevel = 0;
int gameScore = 0;
float gameTimeScore = 0;

int frameCount = 0;
bool boostAppear = false;

int playerLives = 0;
bool playerBlink = false;
float playerBlinkTimer = 0;

int MAX_BIG_METEORS = 0;
int MAX_MEDIUM_METEORS = 0;
int MAX_SMALL_METEORS = 0;

Player player = { 0 };
Wall walls[NUMBER_OF_WALLS] = { 0 };
Shoot shoot[PLAYER_MAX_SHOOTS] = { 0 };

Meteor bigMeteor[12] = { 0 };
Meteor mediumMeteor[24] = { 0 };
Meteor smallMeteor[48] = { 0 };

Image background_image;
Texture2D background_texture;
    
Image ship_image;
Texture2D ship_texture;
Rectangle ship_sourceRec;
Rectangle ship_destRec;
Vector2 ship_origin;

Image ball_image;
Texture2D ball_texture;
Rectangle ball_sourceRec;
Rectangle ball_destRec;
Vector2 ball_origin;

Image asteroid_image;
Texture2D asteroid_texture;
Rectangle asteroid_sourceRec;
Rectangle asteroid_destRec;
Vector2 asteroid_origin;

Image multishot_image;
Texture2D multishot_texture;
int multishot_counter;
Vector2 multishot_position;
bool boostMultiActive;

int midMeteorsCount = 0;
int smallMeteorsCount = 0;
int destroyedMeteorsCount = 0;
int wallsLength = 0;

static int finishScreen;

void InitGameplayScreen(void){
    finishScreen = 0;
    currentLevel = 1;
    playerLives = 3;
    gameScore = 0;
    gameTimeScore = 0;
    destroyedMeteorsCount = 0;
    multishot_counter = GetRandomValue(120, 800);
    multishot_position = (Vector2){GetRandomValue(WALLS_WIDTH*4, screenWidth-WALLS_WIDTH*4), GetRandomValue(WALLS_WIDTH*4, screenHeight-WALLS_WIDTH*4)};
    boostMultiActive = false;
    
    pause = false;
    playerBlink = false;
    playerBlinkTimer = 0;
    
    InitializeTextures();
    InitializeLevel();
}

void UpdateGameplayScreen(void){
    if(IsKeyPressed('P')) pause = !pause;

    if(!pause) {
        frameCount++;
        if(frameCount == multishot_counter) boostAppear = true;
        if(boostMultiActive){
            if(frameCount == MULTISHOT_DURATION){
                boostMultiActive = false;
            }
        }
        
        PlayerLogic();
        ShootLogic();
        MeteorsLogic();
        CollisionLogic();
    }
    else{
        if(IsKeyPressed(KEY_ENTER)) {
            PlaySound(fxSelectConfirm);
            finishScreen = 2;
        }
    }

    if (destroyedMeteorsCount == MAX_BIG_METEORS + MAX_MEDIUM_METEORS + MAX_SMALL_METEORS) {
        currentLevel++;
        if(currentLevel > NUMBER_OF_LEVELS) EndGameplay(1);
        else {
            PlaySound(fxLevelUp);
            destroyedMeteorsCount = 0;
            InitializeLevel();
        }
    }
}

void DrawGameplayScreen(void){
    DrawTextureEx(background_texture, (Vector2){0, 0}, 0, 1.6f, WHITE);
    
    DrawTextEx(font, TextFormat("SCORE: %i", gameScore), (Vector2){30, 25}, fontSize, fontSpacing, WHITE);
    DrawTextEx(font, TextFormat("TIME SCORE: %02.02fs", gameTimeScore), (Vector2){30, 65}, fontSize, fontSpacing, WHITE);
    DrawTextEx(font, TextFormat("LEVEL: %i", currentLevel), (Vector2){screenWidth/2 - (MeasureTextEx(font, "LEVEL 1", fontSize, fontSpacing).x/2), 25}, fontSize, fontSpacing, WHITE);
    DrawTextEx(font, TextFormat("LIVES: %i", playerLives), (Vector2){screenWidth - 170, 25}, fontSize, fontSpacing, WHITE);

    ship_destRec = (Rectangle){ player.position.x, player.position.y, PLAYER_BASE_SIZE*2, PLAYER_BASE_SIZE*2 };
    if(playerBlink){
        playerBlinkTimer += GetFrameTime();
        if((int)(gameTimeScore*10) % 2 == 0) DrawTexturePro(ship_texture, ship_sourceRec, ship_destRec, ship_origin, player.rotation, WHITE);
        if(playerBlinkTimer > 2){
            playerBlinkTimer = 0;
            playerBlink = false;
        }
    }
    else DrawTexturePro(ship_texture, ship_sourceRec, ship_destRec, ship_origin, player.rotation, WHITE);
    
    if(boostAppear){
        DrawTextureEx(multishot_texture, multishot_position, 0, 0.3f, WHITE);
    }
    
    //Draw walls
    for (int i = 0; i < wallsLength; i++) DrawRectangleRec(walls[i].rect, DARKGRAY);

    // Draw meteors
    for (int i = 0; i < MAX_BIG_METEORS; i++) {
        if (bigMeteor[i].active) {
            asteroid_destRec = (Rectangle){ bigMeteor[i].position.x, bigMeteor[i].position.y, bigMeteor[i].radius*2, bigMeteor[i].radius*2 };
            asteroid_origin = (Vector2){ bigMeteor[i].radius, bigMeteor[i].radius };
            DrawTexturePro(asteroid_texture, asteroid_sourceRec, asteroid_destRec, asteroid_origin, bigMeteor[i].rotation, WHITE);
        }
    }

    for (int i = 0; i < MAX_MEDIUM_METEORS; i++) {
        if (mediumMeteor[i].active) {
            asteroid_destRec = (Rectangle){ mediumMeteor[i].position.x, mediumMeteor[i].position.y, mediumMeteor[i].radius*2, mediumMeteor[i].radius*2 };
            asteroid_origin = (Vector2){ mediumMeteor[i].radius, mediumMeteor[i].radius };
            DrawTexturePro(asteroid_texture, asteroid_sourceRec, asteroid_destRec, asteroid_origin, mediumMeteor[i].rotation, WHITE);
        }
    }

    for (int i = 0; i < MAX_SMALL_METEORS; i++) {
        if (smallMeteor[i].active) {
            asteroid_destRec = (Rectangle){ smallMeteor[i].position.x, smallMeteor[i].position.y, smallMeteor[i].radius*2, smallMeteor[i].radius*2 };
            asteroid_origin = (Vector2){ smallMeteor[i].radius, smallMeteor[i].radius };
            DrawTexturePro(asteroid_texture, asteroid_sourceRec, asteroid_destRec, asteroid_origin, smallMeteor[i].rotation, WHITE);
        }
    }

    // Draw shoot
    for (int i = 0; i < PLAYER_MAX_SHOOTS; i++) {
        if (shoot[i].active) {
            ball_destRec = (Rectangle){ shoot[i].position.x, shoot[i].position.y, BALL_SIZE, BALL_SIZE };
            DrawTexturePro(ball_texture, ball_sourceRec, ball_destRec, ball_origin, 0, WHITE);
        }
    }

    if (pause) {
        DrawTextEx(font, "GAME PAUSED", (Vector2){screenWidth/2 - (MeasureTextEx(font, "GAME PAUSED", fontSize, fontSpacing).x/2), screenHeight/2}, fontSize, fontSpacing, LIGHTGRAY);
        DrawTextEx(font, "PRESS [ENTER] TO GO TO MENU", (Vector2){screenWidth/2 - (MeasureTextEx(font, "PRESS [ENTER] TO GO TO MENU", fontSize/2, fontSpacing).x/2), screenHeight/2 + fontSize}, fontSize/2, fontSpacing, GRAY);
    }
}

void UnloadGameplayScreen(void){
    
}

int FinishGameplayScreen(void){
    return finishScreen;
}

void InitializeTextures(){
    background_image = LoadImage("resources/space_background.png");
    background_texture = LoadTextureFromImage(background_image);
    UnloadImage(background_image);
    
    ship_image = LoadImage("resources/spaceship.png");
    ship_texture = LoadTextureFromImage(ship_image);
    ship_sourceRec = (Rectangle){ 0.0f, 0.0f, (float)ship_texture.width, (float)ship_texture.height };
    ship_destRec = (Rectangle){ screenWidth/2, screenHeight/2, PLAYER_BASE_SIZE*2, PLAYER_BASE_SIZE*2 };
    ship_origin = (Vector2){ PLAYER_BASE_SIZE, PLAYER_BASE_SIZE*1.5f };
    UnloadImage(ship_image);
    
    ball_image = LoadImage("resources/light_ball.png");
    ball_texture = LoadTextureFromImage(ball_image);
    ball_sourceRec = (Rectangle){ 0.0f, 0.0f, (float)ball_texture.width, (float)ball_texture.height };
    ball_destRec = (Rectangle){ screenWidth/2, screenHeight/2, BALL_SIZE, BALL_SIZE };
    ball_origin = (Vector2){ BALL_SIZE/2, BALL_SIZE/2 };
    UnloadImage(ball_image);
    
    asteroid_image = LoadImage("resources/asteroid.png");
    asteroid_texture = LoadTextureFromImage(asteroid_image);
    asteroid_sourceRec = (Rectangle){ 0.0f, 0.0f, (float)asteroid_texture.width, (float)asteroid_texture.height };
    asteroid_destRec = (Rectangle){ screenWidth/2, screenHeight/2, 0, 0 };
    asteroid_origin = (Vector2){ 0, 0 };
    UnloadImage(asteroid_image);
    
    multishot_image = LoadImage("resources/multishot_icon.png");
    multishot_texture = LoadTextureFromImage(multishot_image);
    UnloadImage(multishot_image);
    
}

void InitializeLevel(){
    switch(currentLevel){
        case 1: {
            MAX_BIG_METEORS = 4;
            MAX_MEDIUM_METEORS = 8;
            MAX_SMALL_METEORS = 16;
        } break;
        case 2: {
            MAX_BIG_METEORS = 6;
            MAX_MEDIUM_METEORS = 12;
            MAX_SMALL_METEORS = 24;
        } break;
        case 3: {
            MAX_BIG_METEORS = 8;
            MAX_MEDIUM_METEORS = 16;
            MAX_SMALL_METEORS = 32;
        } break;
        case 4: {
            MAX_BIG_METEORS = 10;
            MAX_MEDIUM_METEORS = 20;
            MAX_SMALL_METEORS = 40;
        } break;
        case 5: {
            MAX_BIG_METEORS = 12;
            MAX_MEDIUM_METEORS = 24;
            MAX_SMALL_METEORS = 48;
        } break;
        default: break;
    }
    
    InitializePlayer();
    InitializeWalls();
    InitializeShoots();
    InitializeMeteors();
}

void InitializePlayer(){
    player.position = (Vector2){screenWidth/2, screenHeight/2 - shipHeight/2};
    player.speed = 300.0f;
    player.acceleration = 0;
    player.rotation = 0;
    player.collider = (Vector3){player.position.x + sin(player.rotation*DEG2RAD)*(shipHeight/2.5f), player.position.y - cos(player.rotation*DEG2RAD)*(shipHeight/2.5f), 12};
    player.color = LIGHTGRAY;
}

void InitializeWalls(){
    walls[0].rect = (Rectangle) { 0, 0, screenWidth, WALLS_WIDTH };
    walls[1].rect = (Rectangle) { screenWidth - WALLS_WIDTH, 0, WALLS_WIDTH, screenHeight };
    walls[2].rect = (Rectangle) { 0, screenHeight - WALLS_WIDTH, screenWidth, WALLS_WIDTH };
    walls[3].rect = (Rectangle) { 0, 0, WALLS_WIDTH, screenHeight };
    
    wallsLength = sizeof(walls)/sizeof(walls[0]);
}

void InitializeShoots(){
    for (int i = 0; i < PLAYER_MAX_SHOOTS; i++)
    {
        shoot[i].position = (Vector2){0, 0};
        shoot[i].speed = (Vector2){0, 0};
        shoot[i].radius = BALL_SIZE;
        shoot[i].active = false;
        shoot[i].lifeSpawn = 0;
        shoot[i].color = WHITE;
    }
}

void InitializeMeteors(){
    bool correctRange = false;
    for (int i = 0; i < MAX_BIG_METEORS; i++)
    {
        correctRange = false;
        int velx = GetRandomValue(-METEORS_SPEED, METEORS_SPEED);
        int vely = GetRandomValue(-METEORS_SPEED, METEORS_SPEED);

        while (!correctRange)
        {
            if (velx == 0 && vely == 0)
            {
                velx = GetRandomValue(-METEORS_SPEED, METEORS_SPEED);
                vely = GetRandomValue(-METEORS_SPEED, METEORS_SPEED);
            }
            else correctRange = true;
        }

        bigMeteor[i].speed = (Vector2){velx, vely};
        bigMeteor[i].radius = 30;
        bigMeteor[i].rotation = GetRandomValue(0, 360);
        bigMeteor[i].active = true;
        bigMeteor[i].color = BLUE;
        
        Vector2 topEntry = {screenWidth/2, bigMeteor[i].radius + WALLS_WIDTH};
        Vector2 rightEntry = {screenWidth - bigMeteor[i].radius - WALLS_WIDTH, screenHeight/2};
        Vector2 botEntry = {screenWidth/2, screenHeight - bigMeteor[i].radius - WALLS_WIDTH};
        Vector2 leftEntry = {bigMeteor[i].radius + WALLS_WIDTH, screenHeight/2};
        if(i == 0) bigMeteor[i].position = topEntry;
        else if(i == 1) bigMeteor[i].position = rightEntry;
        else if(i == 2) bigMeteor[i].position = botEntry;
        else if(i == 3) bigMeteor[i].position = leftEntry;
        else if(i == 4) bigMeteor[i].position = topEntry;
        else if(i == 5) bigMeteor[i].position = rightEntry;
        else if(i == 6) bigMeteor[i].position = botEntry;
        else if(i == 7) bigMeteor[i].position = leftEntry;
        else if(i == 8) bigMeteor[i].position = topEntry;
        else if(i == 9) bigMeteor[i].position = rightEntry;
        else if(i == 10) bigMeteor[i].position = botEntry;
        else if(i == 11) bigMeteor[i].position = leftEntry;
        else bigMeteor[i].position = topEntry;
    }

    for (int i = 0; i < MAX_MEDIUM_METEORS; i++)
    {
        mediumMeteor[i].position = (Vector2){-100, -100};
        mediumMeteor[i].speed = (Vector2){0,0};
        mediumMeteor[i].radius = 20;
        mediumMeteor[i].rotation = GetRandomValue(0, 360);
        mediumMeteor[i].active = false;
        mediumMeteor[i].color = BLUE;
    }

    for (int i = 0; i < MAX_SMALL_METEORS; i++)
    {
        smallMeteor[i].position = (Vector2){-100, -100};
        smallMeteor[i].speed = (Vector2){0,0};
        smallMeteor[i].radius = 10;
        smallMeteor[i].rotation = GetRandomValue(0, 360);
        smallMeteor[i].active = false;
        smallMeteor[i].color = BLUE;
    }

    midMeteorsCount = 0;
    smallMeteorsCount = 0;
}

void PlayerLogic(){
    float delta = GetFrameTime();
    gameTimeScore += delta;
    
    if (IsKeyDown(KEY_A) && !blockLeftScreen) player.position.x -= player.speed*delta;
    if (IsKeyDown(KEY_D) && !blockRightScreen) player.position.x += player.speed*delta;
    if (IsKeyDown(KEY_W) && !blockTopScreen) player.position.y -= player.speed*delta;
    if (IsKeyDown(KEY_S) && !blockBotScreen) player.position.y += player.speed*delta;
    
    //Player pointing logic
    int posX = GetMouseX();
    int posY = GetMouseY();
    int deltaX = posX - player.position.x;
    int deltaY = posY - player.position.y;
    double angle = atan2(deltaY, deltaX);
    angle = angle * (180.0 / PI);
    player.rotation = angle + 90.0;

    // Collision logic: player vs walls
    int wallStopPlayer = 35;
    if (player.position.x > screenWidth - wallStopPlayer) blockRightScreen = true;
    else blockRightScreen = false;
    if (player.position.x < wallStopPlayer) blockLeftScreen = true;
    else blockLeftScreen = false;
    if (player.position.y > screenHeight - wallStopPlayer) blockBotScreen = true;
    else blockBotScreen = false;
    if (player.position.y < wallStopPlayer) blockTopScreen = true;
    else blockTopScreen = false;
}

void ShootLogic(){
    if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        for(int i = 0; i < PLAYER_MAX_SHOOTS; i++) {
            if(boostMultiActive){
                if(!shoot[i].active && !shoot[i+1].active && !shoot[i+2].active) {
                    PlaySound(fxShot);
                    PlaySound(fxShot);
                    PlaySound(fxShot);
                    shoot[i].position = (Vector2){ player.position.x + sin(player.rotation*DEG2RAD)*(shipHeight), player.position.y - cos(player.rotation*DEG2RAD)*(shipHeight) };
                    shoot[i].active = true;
                    shoot[i].speed.x = 1.5*sin((player.rotation-45)*DEG2RAD)*PLAYER_SPEED;
                    shoot[i].speed.y = 1.5*cos((player.rotation-45)*DEG2RAD)*PLAYER_SPEED;
                    shoot[i].rotation = player.rotation;
                    
                    shoot[i+1].position = (Vector2){ player.position.x + sin(player.rotation*DEG2RAD)*(shipHeight), player.position.y - cos(player.rotation*DEG2RAD)*(shipHeight) };
                    shoot[i+1].active = true;
                    shoot[i+1].speed.x = 1.5*sin(player.rotation*DEG2RAD)*PLAYER_SPEED;
                    shoot[i+1].speed.y = 1.5*cos(player.rotation*DEG2RAD)*PLAYER_SPEED;
                    shoot[i+1].rotation = player.rotation;
                    
                    shoot[i+2].position = (Vector2){ player.position.x + sin(player.rotation*DEG2RAD)*(shipHeight), player.position.y - cos(player.rotation*DEG2RAD)*(shipHeight) };
                    shoot[i+2].active = true;
                    shoot[i+2].speed.x = 1.5*sin((player.rotation+45)*DEG2RAD)*PLAYER_SPEED;
                    shoot[i+2].speed.y = 1.5*cos((player.rotation+45)*DEG2RAD)*PLAYER_SPEED;
                    shoot[i+2].rotation = player.rotation;
                    break;
                }
            }
            else {
                if(!shoot[i].active) {
                    PlaySound(fxShot);
                    shoot[i].position = (Vector2){ player.position.x + sin(player.rotation*DEG2RAD)*(shipHeight), player.position.y - cos(player.rotation*DEG2RAD)*(shipHeight) };
                    shoot[i].active = true;
                    shoot[i].speed.x = 1.5*sin(player.rotation*DEG2RAD)*PLAYER_SPEED;
                    shoot[i].speed.y = 1.5*cos(player.rotation*DEG2RAD)*PLAYER_SPEED;
                    shoot[i].rotation = player.rotation;
                    break;
                }
            }
        }
    }

    for(int i = 0; i < PLAYER_MAX_SHOOTS; i++) {
        if (shoot[i].active) shoot[i].lifeSpawn++;
    }

    for(int i = 0; i < PLAYER_MAX_SHOOTS; i++) {
        if (shoot[i].active) {
            // Movement
            shoot[i].position.x += shoot[i].speed.x;
            shoot[i].position.y -= shoot[i].speed.y;

            // Collision logic: shoot vs walls
            if  (shoot[i].position.x > screenWidth + shoot[i].radius) {
                shoot[i].active = false;
                shoot[i].lifeSpawn = 0;
            }
            else if (shoot[i].position.x < 0 - shoot[i].radius) {
                shoot[i].active = false;
                shoot[i].lifeSpawn = 0;
            }
            if (shoot[i].position.y > screenHeight + shoot[i].radius) {
                shoot[i].active = false;
                shoot[i].lifeSpawn = 0;
            }
            else if (shoot[i].position.y < 0 - shoot[i].radius) {
                shoot[i].active = false;
                shoot[i].lifeSpawn = 0;
            }

            // Life of shoot
            if (shoot[i].lifeSpawn >= 30) {
                shoot[i].position = (Vector2){0, 0};
                shoot[i].speed = (Vector2){0, 0};
                shoot[i].lifeSpawn = 0;
                shoot[i].active = false;
            }
        }
    }
}

void MeteorsLogic(){
    for (int i = 0; i < MAX_BIG_METEORS; i++) {
        if (bigMeteor[i].active) {
            // Movement
            bigMeteor[i].position.x += bigMeteor[i].speed.x;
            bigMeteor[i].position.y += bigMeteor[i].speed.y;
            bigMeteor[i].rotation += (bigMeteor[i].speed.x - bigMeteor[i].speed.y);

            // Collision logic: meteor vs wall
            if  (bigMeteor[i].position.x > screenWidth - bigMeteor[i].radius - WALLS_WIDTH) {
                bigMeteor[i].speed.x = -bigMeteor[i].speed.x;
            }
            else if (bigMeteor[i].position.x < 0 + bigMeteor[i].radius + WALLS_WIDTH) {
                bigMeteor[i].speed.x = -bigMeteor[i].speed.x;
            }
            if (bigMeteor[i].position.y > screenHeight - bigMeteor[i].radius - WALLS_WIDTH) {
                bigMeteor[i].speed.y = -bigMeteor[i].speed.y;
            }
            else if (bigMeteor[i].position.y < 0 + bigMeteor[i].radius + WALLS_WIDTH) {
                bigMeteor[i].speed.y = -bigMeteor[i].speed.y;
            }
        }
    }

    // Meteors logic: medium meteors
    for (int i = 0; i < MAX_MEDIUM_METEORS; i++) {
        if (mediumMeteor[i].active) {
            // Movement
            mediumMeteor[i].position.x += mediumMeteor[i].speed.x;
            mediumMeteor[i].position.y += mediumMeteor[i].speed.y;
            mediumMeteor[i].rotation += (mediumMeteor[i].speed.x - mediumMeteor[i].speed.y);

            // Collision logic: meteor vs wall
            if  (mediumMeteor[i].position.x > screenWidth - mediumMeteor[i].radius - WALLS_WIDTH) {
                mediumMeteor[i].speed.x = -mediumMeteor[i].speed.x;
            }
            else if (mediumMeteor[i].position.x < 0 + mediumMeteor[i].radius + WALLS_WIDTH) {
                mediumMeteor[i].speed.x = -mediumMeteor[i].speed.x;
            }
            if (mediumMeteor[i].position.y > screenHeight - mediumMeteor[i].radius - WALLS_WIDTH) {
                mediumMeteor[i].speed.y = -mediumMeteor[i].speed.y;
            }
            else if (mediumMeteor[i].position.y < 0 + mediumMeteor[i].radius + WALLS_WIDTH) {
                mediumMeteor[i].speed.y = -mediumMeteor[i].speed.y;
            }
        }
    }

    // Meteors logic: small meteors
    for (int i = 0; i < MAX_SMALL_METEORS; i++) {
        if (smallMeteor[i].active) {
            // Movement
            smallMeteor[i].position.x += smallMeteor[i].speed.x;
            smallMeteor[i].position.y += smallMeteor[i].speed.y;
            smallMeteor[i].rotation += (smallMeteor[i].speed.x - smallMeteor[i].speed.y);

            // Collision logic: meteor vs wall
            if  (smallMeteor[i].position.x > screenWidth - smallMeteor[i].radius - WALLS_WIDTH) {
                smallMeteor[i].speed.x = -smallMeteor[i].speed.x;
            }
            else if (smallMeteor[i].position.x < 0 + smallMeteor[i].radius + WALLS_WIDTH) {
                smallMeteor[i].speed.x = -smallMeteor[i].speed.x;
            }
            if (smallMeteor[i].position.y > screenHeight - smallMeteor[i].radius - WALLS_WIDTH) {
                smallMeteor[i].speed.y = -smallMeteor[i].speed.y;
            }
            else if (smallMeteor[i].position.y < 0 + smallMeteor[i].radius + WALLS_WIDTH) {
                smallMeteor[i].speed.y = -smallMeteor[i].speed.y;
            }
        }
    }
}

void CollisionLogic() {
    // Collision logic: player vs meteors
    player.collider = (Vector3){player.position.x + sin(player.rotation*DEG2RAD)*(shipHeight/2.5f), player.position.y - cos(player.rotation*DEG2RAD)*(shipHeight/2.5f), 12};

    if (CheckCollisionCircles((Vector2){player.collider.x, player.collider.y}, player.collider.z, (Vector2){multishot_position.x+30, multishot_position.y+30}, 20)) {
        if(boostAppear){
            PlaySound(fxPowerUp);
            boostAppear = false;
            frameCount = 0;
            multishot_counter = GetRandomValue(120, 800);
            multishot_position = (Vector2){GetRandomValue(WALLS_WIDTH*4, screenWidth-WALLS_WIDTH*4), GetRandomValue(WALLS_WIDTH*4, screenHeight-WALLS_WIDTH*4)};
            boostMultiActive = true;
        }
    }
        
    for (int a = 0; a < MAX_BIG_METEORS; a++) {
        if (CheckCollisionCircles((Vector2){player.collider.x, player.collider.y}, player.collider.z, bigMeteor[a].position, bigMeteor[a].radius) && bigMeteor[a].active) {
            if(!playerBlink){
                PlaySound(fxShipCrush);
                playerLives--;
                if(playerLives < 0) EndGameplay(0);
                else playerBlink = true;
            }
        }
    }

    for (int a = 0; a < MAX_MEDIUM_METEORS; a++) {
        if (CheckCollisionCircles((Vector2){player.collider.x, player.collider.y}, player.collider.z, mediumMeteor[a].position, mediumMeteor[a].radius) && mediumMeteor[a].active) {
            if(!playerBlink){
                PlaySound(fxShipCrush);
                playerLives--;
                if(playerLives < 0) EndGameplay(0);
                else playerBlink = true;
            }
        }
    }

    for (int a = 0; a < MAX_SMALL_METEORS; a++) {
        if (CheckCollisionCircles((Vector2){player.collider.x, player.collider.y}, player.collider.z, smallMeteor[a].position, smallMeteor[a].radius) && smallMeteor[a].active) {
            if(!playerBlink){
                PlaySound(fxShipCrush);
                playerLives--;
                if(playerLives < 0) EndGameplay(0);
                else playerBlink = true;
            }
        }
    }
    
    // Collision logic: player-shoots vs meteors
    for (int i = 0; i < PLAYER_MAX_SHOOTS; i++) {
        if ((shoot[i].active)) {
            for (int a = 0; a < MAX_BIG_METEORS; a++) {
                if (bigMeteor[a].active && CheckCollisionCircles(shoot[i].position, shoot[i].radius, bigMeteor[a].position, bigMeteor[a].radius)) {
                    PlaySound(fxStoneCrush);
                    shoot[i].active = false;
                    shoot[i].lifeSpawn = 0;
                    bigMeteor[a].active = false;
                    destroyedMeteorsCount++;
                    gameScore++;

                    for (int j = 0; j < 2; j ++) {
                        if (midMeteorsCount%2 == 0) {
                            mediumMeteor[midMeteorsCount].position = (Vector2){bigMeteor[a].position.x, bigMeteor[a].position.y};
                            mediumMeteor[midMeteorsCount].speed = (Vector2){cos(shoot[i].rotation*DEG2RAD)*METEORS_SPEED*-1, sin(shoot[i].rotation*DEG2RAD)*METEORS_SPEED*-1};
                        }
                        else {
                            mediumMeteor[midMeteorsCount].position = (Vector2){bigMeteor[a].position.x, bigMeteor[a].position.y};
                            mediumMeteor[midMeteorsCount].speed = (Vector2){cos(shoot[i].rotation*DEG2RAD)*METEORS_SPEED, sin(shoot[i].rotation*DEG2RAD)*METEORS_SPEED};
                        }

                        mediumMeteor[midMeteorsCount].active = true;
                        midMeteorsCount ++;
                    }
                    //bigMeteor[a].position = (Vector2){-100, -100};
                    bigMeteor[a].color = RED;
                    a = MAX_BIG_METEORS;
                }
            }

            for (int b = 0; b < MAX_MEDIUM_METEORS; b++) {
                if (mediumMeteor[b].active && CheckCollisionCircles(shoot[i].position, shoot[i].radius, mediumMeteor[b].position, mediumMeteor[b].radius)) {
                    PlaySound(fxStoneCrush);
                    shoot[i].active = false;
                    shoot[i].lifeSpawn = 0;
                    mediumMeteor[b].active = false;
                    destroyedMeteorsCount++;
                    gameScore++;

                    for (int j = 0; j < 2; j ++) {
                         if (smallMeteorsCount%2 == 0) {
                            smallMeteor[smallMeteorsCount].position = (Vector2){mediumMeteor[b].position.x, mediumMeteor[b].position.y};
                            smallMeteor[smallMeteorsCount].speed = (Vector2){cos(shoot[i].rotation*DEG2RAD)*METEORS_SPEED*-1, sin(shoot[i].rotation*DEG2RAD)*METEORS_SPEED*-1};
                        }
                        else {
                            smallMeteor[smallMeteorsCount].position = (Vector2){mediumMeteor[b].position.x, mediumMeteor[b].position.y};
                            smallMeteor[smallMeteorsCount].speed = (Vector2){cos(shoot[i].rotation*DEG2RAD)*METEORS_SPEED, sin(shoot[i].rotation*DEG2RAD)*METEORS_SPEED};
                        }

                        smallMeteor[smallMeteorsCount].active = true;
                        smallMeteorsCount ++;
                    }
                    //mediumMeteor[b].position = (Vector2){-100, -100};
                    mediumMeteor[b].color = GREEN;
                    b = MAX_MEDIUM_METEORS;
                }
            }

            for (int c = 0; c < MAX_SMALL_METEORS; c++) {
                if (smallMeteor[c].active && CheckCollisionCircles(shoot[i].position, shoot[i].radius, smallMeteor[c].position, smallMeteor[c].radius)) {
                    PlaySound(fxStoneCrush);
                    shoot[i].active = false;
                    shoot[i].lifeSpawn = 0;
                    smallMeteor[c].active = false;
                    destroyedMeteorsCount++;
                    gameScore++;
                    smallMeteor[c].color = YELLOW;
                   // smallMeteor[c].position = (Vector2){-100, -100};
                    c = MAX_SMALL_METEORS;
                }
            }
        }
    }
}

void EndGameplay(int winner){
    SaveStorageValue(STORAGE_POSITION_WINNER, winner);
    SaveStorageValue(STORAGE_POSITION_SCORE, gameScore);
    SaveStorageValue(STORAGE_POSITION_TIMESCORE, gameTimeScore*100);
    int highScore = LoadStorageValue(STORAGE_POSITION_HIGHSCORE);
    if(!highScore || highScore > 10000) highScore = 0;
    float highTimeScore = LoadStorageValue(STORAGE_POSITION_HIGHTIMESCORE);
    if(!highTimeScore || highTimeScore > 10000) highTimeScore = 0.0f;
    if(gameScore > highScore) {
        SaveStorageValue(STORAGE_POSITION_HIGHSCORE, gameScore);
        SaveStorageValue(STORAGE_POSITION_HIGHTIMESCORE, gameTimeScore*100);
    }
    else if(gameScore == highScore && gameTimeScore < highTimeScore/100){
        SaveStorageValue(STORAGE_POSITION_HIGHSCORE, gameScore);
        SaveStorageValue(STORAGE_POSITION_HIGHTIMESCORE, gameTimeScore*100);
    }
    finishScreen = 1;
}