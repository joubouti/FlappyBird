#include "raylib.h"
#include "easings.h"
#include <stdio.h>
#include <string.h>
#define SW 480
#define SH 800

int ms = 3; // Mouvement Speed
int Score = 0;
int HScore = 0;
char scoreBuff[10] = "0";
char HscoreBuff[10] = "0";

const int MEDAL_BRONZE   = 10;
const int MEDAL_SILVER   = 50;
const int MEDAL_GOLD     = 100;
const int MEDAL_PLATINUM = 200;


Font font;
Texture2D textures;

typedef struct  Pipe
{
    int x;
    int y;

}               Pipe;

bool valueInRange(int value, int min, int max)
{
    return (value >= min) && (value <= max);
}

bool rectOverlap(Rectangle A, Rectangle B)
{
    bool xOverlap = valueInRange(A.x, B.x, B.x + B.width) ||
                    valueInRange(B.x, A.x, A.x + A.width);

    bool yOverlap = valueInRange(A.y, B.y, B.y + B.height) ||
                    valueInRange(B.y, A.y, A.y + A.height);

    return xOverlap && yOverlap;
}

void    DrawTextCenter(const char *text, int x, int y, int fontSize, Color color)
{
    Vector2 textSize = MeasureTextEx(font, text, fontSize, 1);
    x = x - textSize.x / 2;
    DrawTextEx(font, text, (Vector2){x, y}, fontSize, 1, color);
}

void    DrawMousePosition()
{
    char Pos[20];
    int x = GetMouseX();
    int y = GetMouseY();
    sprintf(Pos, "(%d, %d)", x, y);
    DrawText(Pos, x + 10, y, 20, RED);
}

Rectangle numTexture[10] = {{288, 100, 7, 10},
                            {290, 118, 7, 10},
                            {289, 134, 7, 10},
                            {289, 150, 7, 10},
                            {287, 173, 7, 10},
                            {287, 185, 7, 10},
                            {165, 245, 7, 10},
                            {175, 245, 7, 10},
                            {185, 245, 7, 10},
                            {195, 245, 7, 10},
                            };

void    DrawTextN(const char *text, Vector2 pos, float fontSize)
{
    int len = strlen(text);
    int i;
    int x = 0;
    Rectangle tx;
    for (i = 0; i < len; i++)
    {
        tx = numTexture[text[i] - 48];
        DrawTexturePro(textures, tx, (Rectangle){pos.x + x, pos.y, tx.width * fontSize, tx.height * fontSize},
                        (Vector2){0, 0}, 0, WHITE);
        x += tx.width * fontSize;
    }
}

void    addScore()
{
    Score++;
    sprintf(scoreBuff, "%d", Score);
    if (Score > HScore)
    {
        HScore = Score;
        sprintf(HscoreBuff, "%d", HScore);
    }
}

bool    isMouseInPosition(Rectangle rect)
{
    Vector2 pos = GetMousePosition();
    if (rect.x <= pos.x && pos.x <= rect.x + rect.width &&
        rect.y <= pos.y && pos.y <= rect.y + rect.height)
        return (true);
    return (false);
}

int main()
{
    InitWindow(SW, SH, "Flappy Bird");
    Image icon = LoadImage("flappy.ico");
    SetWindowIcon(icon);

    font = LoadFont("flappy.ttf");
    textures = LoadTexture("flappybird.png");
    Rectangle floor = {146, 0, 154, 56};
    int fX = 0; // Floor X (For floor animation)
    Rectangle Pipe = {302, 0, 26, 135};
    Vector3 Pipe1 = {SW, GetRandomValue(-250, 0), 1};
    Vector3 Pipe2 = {3 * SW / 2, GetRandomValue(-250, 0), 1};

    Vector2 Bird = {230, 380};
    int pipeSpace = 150; //Vertical space between pipes 
    //int pX = 0; // Pipe X (For floor animation)

    /*int bt = 0; // current time (in any unit measure, but same unit as duration)
    float bb = 0.0f; // starting value to interpolate
    float bc = 60.0f; // the total change in value of b that needs to occur
    int bd = 60; // total time it should take to complete (duration)*/
    int by = 0;  // bird animation Y
    //int y = 1;
    float tick = GetTime();
    //float ttick = GetTime();
    //bool banim = false;

    int birdAnim = 1;
    float animTick = GetTime();

    float rotation = 0;
    Rectangle BTexture;
    //float fAnim = GetTime(); // animation Time

    int State = 0; // 0 : Waiting to start | 1 : Playing | 2 : Dead

    

    SetTargetFPS(60);
    while(!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(WHITE);
        // Background
        DrawTexturePro(textures, (Rectangle){0, 0, 144, 256}, (Rectangle){0, 0, SW, SH}, (Vector2){0, 0}, 0, WHITE);
        // Pipes
        DrawTexturePro(textures, Pipe, (Rectangle){Pipe1.x, Pipe1.y, 80, 400}, (Vector2){0, 0}, 0, WHITE);
        DrawTexturePro(textures, Pipe, (Rectangle){Pipe1.x, Pipe1.y + pipeSpace, 80, 400}, (Vector2){80, SH}, 180, WHITE);
        
        DrawTexturePro(textures, Pipe, (Rectangle){Pipe2.x, Pipe2.y, 80, 400}, (Vector2){0, 0}, 0, WHITE);
        DrawTexturePro(textures, Pipe, (Rectangle){Pipe2.x, Pipe2.y + pipeSpace, 80, 400}, (Vector2){80, SH}, 180, WHITE);
        //DrawTexturePro(textures, Pipe, (Rectangle){SW + fX - 80, Pipe1.y, 80, 400}, (Vector2){0, 0}, 0, WHITE);
        //DrawTexturePro(textures, Pipe, (Rectangle){SW + fX - 80, pipeSpace + Pipe1.y, 80, 400}, (Vector2){80, SH}, 180, WHITE);
        // Floor
        DrawTexturePro(textures, floor, (Rectangle){fX, SH - floor.height * 3, SW, floor.height * 3}, (Vector2){0, 0}, 0, WHITE);
        DrawTexturePro(textures, floor, (Rectangle){SW + fX, SH - floor.height * 3, SW, floor.height * 3}, (Vector2){0, 0}, 0, WHITE);

        // Floor & Pipes Animation
        if (State == 1)
        {
            Pipe1.x -= ms;
            Pipe2.x -= ms;
            if (Pipe1.x < -80)
            {
                Pipe1.x = SW;
                Pipe1.y = GetRandomValue(-250, 0);
                Pipe1.z = 1;
            }
            else if (Pipe1.x < SW / 2 && Pipe1.z)
            {
                addScore();
                Pipe1.z = 0;
            }
            if (Pipe2.x < -80)
            {
                Pipe2.x = SW;
                Pipe2.y = GetRandomValue(-250, 0);
                Pipe2.z = 1;
            }
            else if (Pipe2.x < SW / 2 && Pipe2.z)
            {
                addScore();
                Pipe2.z = 0;
            }
            //fAnim = GetTime();
        }
        if (State != 2)
        {
            fX -= ms;
            if (fX < -SW)
                fX += SW;
        }
        // Bird
        if (birdAnim == 0)
            BTexture = (Rectangle){264, 64, 17, 12};
        else if (birdAnim == 1)
            BTexture = (Rectangle){264, 90, 17, 12};
        else if (birdAnim == 2)
            BTexture = (Rectangle){223, 124, 17, 12};
        
        if (rectOverlap((Rectangle){Bird.x, Bird.y, 48, 33}, (Rectangle){Pipe1.x + 5, Pipe1.y, 80 - 5, 400}) || 
            rectOverlap((Rectangle){Bird.x, Bird.y, 48, 33}, (Rectangle){Pipe1.x + 5, Pipe1.y + 400 + pipeSpace, 80 - 5, 400}) ||
            rectOverlap((Rectangle){Bird.x, Bird.y, 48, 33}, (Rectangle){Pipe2.x + 5, Pipe2.y, 80 - 5, 400}) ||
            rectOverlap((Rectangle){Bird.x, Bird.y, 48, 33}, (Rectangle){Pipe2.x + 5, Pipe2.y + 400 + pipeSpace, 80 - 5, 400}))
            State = 2;

        DrawTexturePro(textures, BTexture, (Rectangle){Bird.x, Bird.y, 48, 33}, (Vector2){0, 0}, rotation, WHITE);
        if (State != 2 && (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) || IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_UP)))
        {
            //banim = true;
            State = 1;
            by = 9;
            //y = 1;
        }
        if (State == 1)
        {
            //Score++;
            //DrawTextCenter(scoreBuff, SW / 2, 40, 40, WHITE);
            DrawTextN(scoreBuff, (Vector2){SW / 2, 40}, 3);
        }
        if (State != 0)
        {
            if (GetTime() - animTick > 0.1f)
            {
                birdAnim++;
                if (birdAnim > 2)
                    birdAnim = 0;
                animTick = GetTime();
            }
            if (GetTime() - tick > 0.03f)
            {
                if (by > -10)
                    by -= 1;
                tick = GetTime();
            }
            Bird.y -= by;

            if (Bird.y < 0)
                Bird.y = 0;
            else if (Bird.y > SH - floor.height * 3 - 33)
            {
                Bird.y = SH - floor.height * 3 - 33;
                State = 2;
            }

            if (by > 0)
                rotation = -20;
            else
            {
                rotation = (-5 * by) - 20;
                birdAnim = 1;
            }

        }
        // Game Over
        if (State == 2)
        {
            DrawTexturePro(textures, (Rectangle){146, 199, 93, 20}, (Rectangle){SW / 2, SH / 2 - 150, 93 * 3, 20 * 3}, (Vector2){93 * 3 / 2, 20 * 3}, 0, WHITE);
            DrawTexturePro(textures, (Rectangle){146, 58, 113, 58}, (Rectangle){SW / 2, SH / 2, 113 * 3, 58 * 3}, (Vector2){113 * 3 / 2, 58 * 3 / 2}, 0, WHITE);
            //DrawTextEx(font, scoreBuff, (Vector2){320, 360}, 30, 1, BLACK);
            //DrawTextEx(font, HscoreBuff, (Vector2){320, 422}, 30, 1, BLACK);

            DrawTextN(scoreBuff, (Vector2){320, 365}, 2);
            DrawTextN(HscoreBuff, (Vector2){320, 427}, 2);

            if (Score >= MEDAL_PLATINUM)
                DrawTexturePro(textures, (Rectangle){220, 144, 22, 22}, (Rectangle){111, 376, 22 * 3, 22 * 3}, (Vector2){0, 0}, 0, (Color){255, 255, 255, 255});
            else if (Score >= MEDAL_GOLD)
                DrawTexturePro(textures, (Rectangle){242, 229, 22, 22}, (Rectangle){111, 376, 22 * 3, 22 * 3}, (Vector2){0, 0}, 0, (Color){255, 255, 255, 255});
            else if (Score >= MEDAL_SILVER)
                DrawTexturePro(textures, (Rectangle){266, 229, 22, 22}, (Rectangle){111, 376, 22 * 3, 22 * 3}, (Vector2){0, 0}, 0, (Color){255, 255, 255, 255});
            else if (Score >= MEDAL_BRONZE)
                DrawTexturePro(textures, (Rectangle){302, 137, 22, 22}, (Rectangle){111, 376, 22 * 3, 22 * 3}, (Vector2){0, 0}, 0, (Color){255, 255, 255, 255});
            //DrawTexturePro(textures, (Rectangle){246, 134, 40, 14}, (Rectangle){SW / 2, SH / 2 + 150, 40 * 3, 14 * 3}, (Vector2){40 * 3 / 2, 14 * 3}, 0, WHITE);
            if (isMouseInPosition((Rectangle){SW / 2 - (40 * 3 / 2), SH / 2 + 150 - (14 * 3), 40 * 3, 14 * 3}))
            {
                DrawTexturePro(textures, (Rectangle){246, 134, 40, 14}, (Rectangle){SW / 2, SH / 2 + 150, 40 * 3, 14 * 3}, (Vector2){40 * 3 / 2, 14 * 3}, 0, (Color){255, 255, 255, 150});
                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
                {
                    Score = 0;
                    sprintf(scoreBuff, "0");
                    floor = (Rectangle){146, 0, 154, 56};
                    fX = 0; // Floor X (For floor animation)
                    Pipe1 = (Vector3){SW, GetRandomValue(-250, 0), 1};
                    Pipe2 = (Vector3){3 * SW / 2, GetRandomValue(-250, 0), 1};

                    Bird = (Vector2){230, 380};
                    by = 0;  // bird animation Y
                    tick = GetTime();
                    birdAnim = 1;
                    animTick = GetTime();

                    rotation = 0;

                    State = 0; // 0 : Waiting to start | 1 : Playing | 2 : Dead

                }
            }
            else
                DrawTexturePro(textures, (Rectangle){246, 134, 40, 14}, (Rectangle){SW / 2, SH / 2 + 150, 40 * 3, 14 * 3}, (Vector2){40 * 3 / 2, 14 * 3}, 0, WHITE);            
        }
        //////
        //DrawMousePosition();
        //DrawFPS(10, 10);
        EndDrawing();
    }
    CloseWindow();
    return (0);
}