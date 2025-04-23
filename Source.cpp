#include <iostream>
#include "IndProj.h"

using namespace std;


void Log(HDC hdc, string text, int x, int y)
{
    TCHAR* newText = new TCHAR[text.length()];
    for (int c = 0; c < text.length(); c++)
    {
        newText[c] = text[c];
    }
    TextOut(hdc, x, y, newText, _tcsclen(newText));
}
COLORREF ColorMult(int r, int g, int b, float multiplier)
{
    return RGB((float)r * multiplier, (float)g * multiplier, (float)b * multiplier);
}



std::map<string, HBRUSH> Brushes;

void InitializeBrush()
{
    Brushes =
    {
        { "white", CreateSolidBrush(RGB(255,255,255)) },
        { "grey", CreateSolidBrush(RGB(40,40,40)) },
        { "burning", CreateSolidBrush(RGB(200, 100, 70)) },
        { "grass", CreateSolidBrush(RGB(70, 180, 100)) },
        { "tree", CreateSolidBrush(RGB(50, 220, 80)) },
        { "burnedOut", CreateSolidBrush(RGB(80, 30, 20)) }
    };
}

void Module::Initialize()
{
    setlocale(LC_CTYPE, "rus");
    srand(time(0));
    
    width = 100;
    height = 60;
    fireSourceX = fireSourceY = 0;
    WindowWidth = 1280; WindowHeight = 720;

    windDir = Vector(windDir.X * windSpeed, windDir.Y * windSpeed);

    field = new Cell * [width];
    for (int x = 0; x < width; x++)
    {
        field[x] = new Cell[height]();
    }
    FillWithRandom();

    field[fireSourceX][fireSourceY].burning = true;
    InitializeBrush();
}

void Module::CopyField(bool toNewField)
{
    for (int x = 0; x < width; x++)
        for (int y = 0; y < height; y++)
        {
            if (toNewField) updatedField[x][y] = field[x][y];
            else field[x][y] = updatedField[x][y];
        }
}

void Module::GetWindowSize(int width, int height)
{
    WindowWidth = width;
    WindowHeight = height;
}

void Module::Update()
{
    updatedField = new Cell * [width];
    for (int c = 0; c < width; c++) updatedField[c] = new Cell[height]();

    CopyField(true);


    for (int x = 0; x < width; x++)
    {
        for (int y = 0; y < height; y++)
        {
            if (field[x][y].burning)
            {
                BurnNearCells(x, y);
                if (field[x][y].fuel > 0) updatedField[x][y].fuel--;
                else updatedField[x][y].burning = false;
            }
        }
    }

    CopyField(false);
}

int Min(int a, int b) { return (a < b ? a : b); }
int Max(int a, int b) { return (a > b ? a : b); }

void Clear(HDC hdc, int width, int height)
{
    SelectObject(hdc, Brushes["white"]);
    Rectangle(hdc, 0, 0, width, height);
}

bool InBounds(int x, int y, int width, int heigth)
{
    return (x >= 0 && x < width && y >= 0 && y <= heigth);
}

bool Module::RandomProbability(float percent)
{
    return (rand() % 100 + 1) / 100.0 < percent;
}

void Module::BurnNearCells(int fromX, int fromY)
{
    if (windDir == Vector(0, 0))
    {
        for (int c = 0; c < 8; c++)
        {
            int checkPosX = fromX;
            int checkPosY = fromY;


            switch (c)
            {
            case 0: checkPosX -= 1; checkPosY += 0; break;
            case 1: checkPosX -= 1; checkPosY -= 1; break;
            case 2: checkPosX += 0; checkPosY -= 1; break;
            case 3: checkPosX += 1; checkPosY -= 1; break;
            case 4: checkPosX += 1; checkPosY += 0; break;
            case 5: checkPosX += 1; checkPosY += 1; break;
            case 6: checkPosX += 0; checkPosY += 1; break;
            case 7: checkPosX -= 1; checkPosY += 1; break;
            }

            if (InBounds(checkPosX, checkPosY, width, height) &&
                !field[checkPosX][checkPosY].burning &&
                field[checkPosX][checkPosY].fuel > 0)
            {
                if (RandomProbability(burnProbability))
                    updatedField[checkPosX][checkPosY].burning = true;
            }
        }
    }
    else
    {
        Vector burnCords[3];
        if (windDir.X == 1 && windDir.Y == 0)
        {
            burnCords[0] = Vector(1, -1);
            burnCords[1] = Vector(1, 0);
            burnCords[2] = Vector(1, 1);
        }
        else if (windDir.X == 1 && windDir.Y == 1)
        {
            burnCords[0] = Vector(1, 0);
            burnCords[1] = Vector(1, 1);
            burnCords[2] = Vector(0, 1);
        }
        else if (windDir.X == 0 && windDir.Y == 1)
        {
            burnCords[0] = Vector(1, 1);
            burnCords[1] = Vector(0, 1);
            burnCords[2] = Vector(-1, 1);
        }
        else if (windDir.X == -1 && windDir.Y == 1)
        {
            burnCords[0] = Vector(0, 1);
            burnCords[1] = Vector(-1, 1);
            burnCords[2] = Vector(-1, 0);
        }
        else if (windDir.X == -1 && windDir.Y == 0)
        {
            burnCords[0] = Vector(-1, 1);
            burnCords[1] = Vector(-1, 0);
            burnCords[2] = Vector(-1, -1);
        }
        else if (windDir.X == -1 && windDir.Y == -1)
        {
            burnCords[0] = Vector(-1, 0);
            burnCords[1] = Vector(-1, -1);
            burnCords[2] = Vector(0, -1);
        }
        else if (windDir.X == 0 && windDir.Y == -1)
        {
            burnCords[0] = Vector(-1, -1);
            burnCords[1] = Vector(0, -1);
            burnCords[2] = Vector(1, -1);
        }
        else if (windDir.X == 1 && windDir.Y == -1)
        {
            burnCords[0] = Vector(0, -1);
            burnCords[1] = Vector(1, -1);
            burnCords[2] = Vector(0, 1);
        }
        for (int c = 0; c < 3; c++)
        {
            int cellX = (int)burnCords[c].X + fromX;
            int cellY = (int)burnCords[c].Y + fromY;
            if (InBounds(cellX, cellY, width, height) && !field[cellX][cellY].burning && field[cellX][cellY].fuel > 0)
            {
                if (RandomProbability(burnProbability))
                    updatedField[cellX][cellY].burning = true;
            }
        }
    }    
}

void Module::Render(HDC hdc)
{
    Clear(hdc, WindowWidth, WindowHeight);
    SelectObject(hdc, Brushes["grey"]);

    int basicOffset = 5;

    float fullSize = ((float)Min(WindowHeight, WindowWidth) - 2 * basicOffset)
            / (float)(WindowWidth < WindowHeight ? width : height);

    const float offset = fullSize * 0.05;
    const float size = fullSize - offset;

    for (int x = 0; x < width; x++)
    {
        for (int y = 0; y < height; y++)
        {
            if (field[x][y].burning)
            {
                SelectObject(hdc, Brushes["burning"]);
            }
            else if(field[x][y].fuel > 0 || field[x][y].type == "dirt")
            {
                SelectObject(hdc, Brushes[field[x][y].type]);
            }
            else SelectObject(hdc, Brushes["burnedOut"]);
            
            Vector cellPos = Vector(basicOffset + x * (size + offset), basicOffset + y * (size + offset));
            Rectangle(hdc, (int)cellPos.X, (int)cellPos.Y, (int)cellPos.X + size, (int)cellPos.Y + size);
        }
    }

    if(!renderWind) return;
    for (int x = 0; x < width; x++)
    {
        for (int y = 0; y < height; y++)
        {
            Vector cellPos = Vector(basicOffset + x * (size + offset), basicOffset + y * (size + offset));
            MoveToEx(hdc, ((int)cellPos.X + (size + offset) / 2), ((int)cellPos.Y + (size + offset) / 2), 0);

            LineTo(hdc, ((int)cellPos.X + (size + offset) / 2) + (int)windDir.X * (size + offset),
                ((int)cellPos.Y + (size + offset) / 2) + (int)windDir.Y * (size + offset));
        }
    }
}

void Module::FillWithRandom()
{
    for (int x = 0; x < width; x++)
    {
        for (int y = 0; y < height; y++)
        {
            field[x][y].fuel = 0;
            field[x][y].burning = false;

            int randType = rand()%3;

            switch (randType)
            {
            default:
                field[x][y].type = "tree";
                field[x][y].fuel = 3;
                break;
            case 0:
                field[x][y].type = "grass";
                field[x][y].fuel = 0;
                break;
            case 1:
                field[x][y].type = "grass";
                field[x][y].fuel = 1;
                break;
            }
        }
    }
}

void Module::Exit()
{
    for (int x = 0; x < width; x++)
        delete[] field[x];
    delete[] field;
}
