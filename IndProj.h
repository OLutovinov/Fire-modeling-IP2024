#pragma once
#include "resource.h"
#include "framework.h"
#include <string>
#include <cmath>
#include <windows.h>
#include <map>

using namespace std;


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
    int fuel;
    string type;

    Cell(string type = "dirt", int fuel = 0, bool burning = false)
    {
        this->type = type;
        this->fuel = fuel;
        this->burning = burning;
    }
};

class Module
{
public:
    Cell** field;

    int width;
    int height;

    int WindowWidth;
    int WindowHeight;

    int fireSourceX;
    int fireSourceY;

    float burnProbability = 0.65;

    Vector windDir = Vector(1, 1);
    float windSpeed = 1;
    bool renderWind = false;

public:
    Cell** updatedField;
    void Initialize();
    void CopyField(bool toNewField);
    void GetWindowSize(int width, int height);
    bool RandomProbability(float percent);
    void BurnNearCells(int x, int y);
    void Update();
    void Render(HDC hdc);
    void FillWithRandom();
    void Exit();
};