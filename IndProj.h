#pragma once
#include "resource.h"
#include "framework.h"
#include <string>
#include <cmath>
#include <windows.h>
#include <map>

using namespace std;

void Flush();
void Enter();
void LogStr(string message);
void Log(bool message);
void Log(int message);
void Log(float message);

struct Vector
{
public:
    float X;
    float Y;

    Vector(float x, float y)
    {
        this->X = x;
        this->Y = y;
    }
    Vector()
    {
        X = 0;
        Y = 0;
    }

    float Length(bool squared)
    {
        return squared? (X*X + Y*Y) : sqrt(X*X + Y*Y);
    }
    void Normalize()
    {
        if (this->Length(true) == 1.0) return;
        float n = sqrt(1.0 / (X*X + Y*Y));
        X *= n;
        Y *= n;
    }

    bool operator ==(Vector other)
    {
        return X == other.X && Y == other.Y;
    }
};

struct Cell
{
    bool burning;
    float fuelMass;
    float capacity;
    float burnSpeed;
    float combustionHeat;
    float temp;
    float fireTemp;
    float height;

    const float WaterCapacity = 4200.0;
    const float VaporizationHeat = 2260000.0;
    const float VaporizationTemp = 100.0;
    float waterMass;
    float waterTemp;

    void CopyTempsFrom(Cell other);
    void CopyConstantsFrom(Cell other);

    void SetHumidity(float percent);
    float GetHumidity();

    Cell() { }

    void FillParams();
    void Update(float heat);
    COLORREF GetColor();
};

class Module
{
private:
    Cell** field;

    int width;
    int height;

    int WindowWidth;
    int WindowHeight;

    int fireSourceX;
    int fireSourceY;

    Vector windDir = Vector(0, 0);
    float windSpeed = 1;
    bool renderWind = false;

public:
    Cell** updatedField;
    void Initialize();
    void CopyField(bool toNewField);
    void GetWindowSize(int width, int height);
    bool InBounds(Vector cord);
    Cell GetCell(Vector cord);
    void BurnNearCells(int x, int y);
    void Update();
    void Render(HDC hdc);
    void FillWithRandom();
    void Exit();
};