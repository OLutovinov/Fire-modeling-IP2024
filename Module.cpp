#include "IndProj.h"
#include <iostream>

using namespace std;


void Module::Initialize()
{
    setlocale(LC_CTYPE, "rus");
    srand(time(0));
    
    width = 24;
    height = 16;

    fireSourceX = fireSourceY = 0;

    WindowWidth = 1920; WindowHeight = 1080;

    windDir = Vector(1, 0);
    windDir.Normalize();

    field = new Cell * [width];
    updatedField = new Cell * [width];

    for (int x = 0; x < width; x++)
    {
        field[x] = new Cell[height]();
        updatedField[x] = new Cell[height]();
    }

    FillWithRandom();

    field[fireSourceX][fireSourceY].Update(1000000);
}

void Module::CopyField(bool toNewField)
{
    for (int x = 0; x < width; x++)
        for (int y = 0; y < height; y++)
        {
            if (toNewField) updatedField[x][y].CopyTempsFrom(field[x][y]);
            else field[x][y].CopyTempsFrom(updatedField[x][y]);
        }
}

void Module::GetWindowSize(int width, int height)
{
    WindowWidth = width;
    WindowHeight = height;
}

void Module::Update()
{
    CopyField(true); // ����������� �� �������� ���� � "updatedField"

    for (int x = 0; x < width; x++)
    {
        for (int y = 0; y < height; y++) // ������ �� ���� �������
        {
            if (field[x][y].burning)
            {
                BurnNearCells(x, y); // �������� ����� �������� �������
                updatedField[x][y].Update(field[x][y].burnSpeed * field[x][y].combustionHeat);
            }
        }
    }

    CopyField(false); // ����������� �� "updateField" � ������� ����
}

bool Module::InBounds(Vector cord)
{
    return (cord.X >= 0 && cord.X < width && cord.Y >= 0 && cord.Y < height);
}

Cell Module::GetCell(Vector cord)
{
    return field[(int)cord.X][(int)cord.Y];
}

void Module::BurnNearCells(int fromX, int fromY)
{
    Vector checkPos = Vector(fromX, fromY);
    for (int c = 0; c < 8; c++)
    {
        checkPos = Vector(fromX, fromY);

        switch (c)
        {
            case 0: checkPos.X -= 1; checkPos.Y += 0; break;
            case 1: checkPos.X -= 1; checkPos.Y -= 1; break;
            case 2: checkPos.X += 0; checkPos.Y -= 1; break;
            case 3: checkPos.X += 1; checkPos.Y -= 1; break;
            case 4: checkPos.X += 1; checkPos.Y += 0; break;
            case 5: checkPos.X += 1; checkPos.Y += 1; break;
            case 6: checkPos.X += 0; checkPos.Y += 1; break;
            case 7: checkPos.X -= 1; checkPos.Y += 1; break;
        }
        checkPos.X += windDir.X;
        checkPos.Y += windDir.Y;

        if (!InBounds(checkPos)) continue;

        Cell currentCell = GetCell(checkPos);
        // ������-����� �� ������ ������ � ����� �������
        if(!currentCell.burning && currentCell.fuelMass > 0)
        {
            // �������� ����� ������-������
            updatedField[(int)checkPos.X][(int)checkPos.Y].Update(
                currentCell.burnSpeed * currentCell.combustionHeat);
        }
    }
}

void Clear(HDC hdc, int width, int height)
{
    SelectObject(hdc, CreateSolidBrush(RGB(40, 40, 40)));
    Rectangle(hdc, 0, 0, width, height);
} 


void Module::Render(HDC hdc)
{
    Clear(hdc, WindowWidth, WindowHeight); // ������� ������

    int basicOffset = 10;

    // ������ ������� ������ �� ������ �� ������ �������� ����
    float fullSize = 
        ((float)WindowHeight / (float)height < (float)WindowWidth / (float)width) ?
        (WindowHeight - 2 * basicOffset) / (float)height
        : (WindowWidth - 2 * basicOffset) / (float)width;

    // ����� �������� ���� �������� � 1/20 ����� �� �� �������
    const float offset = fullSize * 0.05;

    // ������ ������ � ������� ���������
    const float size = fullSize - offset;

    for (int x = 0; x < width; x++)
    {
        for (int y = 0; y < height; y++) // ������ �� ���� �������
        {
            // ������� ����� ������
            HBRUSH temporalBrush = CreateSolidBrush(field[x][y].GetColor());
            SelectObject(hdc, temporalBrush);

            // ������ ��������� ������
            Vector cellPos = Vector(
                basicOffset + x * fullSize, basicOffset + y * fullSize);

            // ��������� ������
            Rectangle(hdc, (int)cellPos.X, (int)cellPos.Y,
                (int)cellPos.X + size, (int)cellPos.Y + size);
            
            DeleteObject(temporalBrush);
        }
    }

    if(!renderWind) return;
    for (int x = 0; x < width; x++)
    {
        for (int y = 0; y < height; y++)
        {
            Vector cellPos = Vector(basicOffset + x * fullSize, basicOffset + y * fullSize);
            MoveToEx(hdc, ((int)cellPos.X + fullSize / 2), ((int)cellPos.Y + fullSize / 2), 0);

            LineTo(hdc, ((int)cellPos.X + fullSize / 2) + (int)windDir.X * fullSize,
                ((int)cellPos.Y + fullSize / 2) + (int)windDir.Y * fullSize);
        }
    }
}

void Module::FillWithRandom()
{
    const int N = 13;
    Vector noFuelCells[N] = { Vector(6,7), Vector(7,7), Vector(8,7), Vector(9,7),
    Vector(10,7), Vector(11, 7), Vector(12, 7), Vector(13, 7),
    Vector(13, 6), Vector(13, 5), Vector(13, 4), Vector(13, 3), Vector(13, 2) };

    const int M = 20;
    Vector waterCells[M] = { Vector(20,15), Vector(19,15), Vector(20,14), Vector(19,14), 
    Vector(20,13), Vector(19,13),  Vector(20,12), Vector(19,12),  Vector(20,11), Vector(19,11),
    Vector(20,10), Vector(19,10), Vector(18,10), Vector(18,11), Vector(17,10), Vector(17,11),
    Vector(16,10), Vector(16,11), Vector(15,10), Vector(15,11) };

    for (int x = 0; x < width; x++)
    {
        for (int y = 0; y < height; y++) // ������ �� ������ ������ ����
        {
            field[x][y].FillParams();

            for (int c = 0; c < N; c++)
            {
                if (noFuelCells[c].X == x && noFuelCells[c].Y == y)
                {
                    field[x][y].fuelMass = 0;
                    break;
                }
            }
            for (int c = 0; c < M; c++)
            {
                if (waterCells[c].X == x && waterCells[c].Y == y)
                {
                    field[x][y].SetHumidity(80);
                    field[x][y].fuelMass = 0;
                    break;
                }
            }

            updatedField[x][y].CopyTempsFrom(field[x][y]);
            updatedField[x][y].CopyConstantsFrom(field[x][y]);
        }
    }
}

    void Module::Exit()
    {
        // ������ �� ���� �������
        for (int x = 0; x < width; x++)
        {
            // �������� �����
            delete[] field[x];       
            delete[] updatedField[x];
        }

        // �������� ��������
        delete[] field;
        delete[] updatedField;
    }
