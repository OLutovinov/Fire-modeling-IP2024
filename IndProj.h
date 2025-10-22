#pragma once
#include "resource.h"
#include "framework.h"
#include <string>
#include <cmath>
#include <windows.h>
#include <map>
#include <vector>

using namespace std;

struct Vector;

void Flush();
void Enter();
void LogStr(string message);
void Log(bool message);
void Log(int message);
void Log(float message);
void Log(Vector message);

void Approach(float& value, float target, float maxMove);
void Triangle(HDC hdc, POINT a, POINT b, POINT c);

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

    float Length()
    {
        return sqrt(X*X + Y*Y);
    }
    void Normalize()
    {
        float n = sqrt(1.0 / (X*X + Y*Y));
        X *= n;
        Y *= n;
    }
    POINT Point()
    {
        return POINT{ (int)X, (int)Y };
    }

    bool operator == (Vector other)
    {
        return X == other.X && Y == other.Y;
    }
    Vector operator + (Vector second)
    {
        return Vector(X + second.X, Y + second.Y);
    }

    Vector operator - (Vector second)
    {
        return Vector(X - second.X, Y - second.Y);
    }
    
    Vector operator * (float scale)
    {
        return Vector(X * scale, Y * scale);
    }
    
    Vector operator / (float scale)
    {
        return Vector(X / scale, Y / scale);
    }

    void operator += (Vector other)
    {
        X += other.X;
        Y += other.Y;
    }
    void operator -= (Vector other)
    {
        X -= other.X;
        Y -= other.Y;
    }
    void operator *= (float scale)
    {
        X *= scale;
        Y *= scale;
    }
    void operator /= (float scale)
    {
        X /= scale;
        Y /= scale;
    }
};

struct Cell
{
    bool isWater = false;

    bool burning = false;
    float fuelMass = 0.;
    float capacity = 0.;

    float burnSpeed = 0.;
    float combustionHeat = 0.;
    float temp = 0.;
    float fireTemp = 0.;
    float height = 0.;

    const float VaporizationTemp = 100.0;
    const float WaterCapacity = 4200.0;
    const float VaporizationHeat = 2260000.0;
    
    float waterMass = 0.;
    float waterTemp = 0.;

    int colorR;
    int colorG;
    int colorB;

    void CopyTempsFrom(Cell other);
    void CopyConstantsFrom(Cell other);

    void SetHumidity(float percent);
    float GetHumidity();

    Cell() { }

    void FillParams();
    void Update(float heat, float deltaTime);
    COLORREF GetColor();

    void operator =(Cell other)
    {
        CopyTempsFrom(other);
        CopyConstantsFrom(other);
    }
    bool operator ==(Cell other)
    {
        return (burning == other.burning && fuelMass == other.fuelMass && temp == other.temp
            && waterMass == other.waterMass && waterTemp == other.waterTemp);
    }
};

class Button;
class Module;
struct ImportedData;

class Editor
{
public:
    Cell cellType;
    void RenderUI(HDC hdc);
    void ReloadType(int profile);

    Editor()
    {
        cellType.FillParams();
    }
};

class Camera
{
public:
    Vector Position;
    float Zoom;

    Camera()
    {
        Position = Vector(0, 0);
        Zoom = 0.9;
    }
    Camera(Vector Position)
    {
        this->Position = Position;
        Zoom = 0.9;
    }
    void WorldToScreen(Vector &worldPos);
    float WorldToScreenX(float posX);
    float WorldToScreenY(float posY);
    void ScreenToWorld(Module module, Vector &screenPos);

};

class Module
{
public:
    enum States
    {
        RunningSimulation,
        Paused,
        InEditor
    };

    Cell** startPosition;
    Cell** field;

    int width;
    int height;
    
    States state;

    Vector mousePosition;
    float ElapsedTime = 0.1;

    float Timer;
    float timerActive;

private:
    int WindowWidth;
    int WindowHeight;

    int fireSourceX;
    int fireSourceY;

    Vector windDir = Vector(0, 0);
    float windSpeed = 1;
    bool renderWind = false;

    int buttonCount = 0;
    Button** buttons;

    Camera camera;
    Editor editor;

public:
    Module() { Initialize(); }
    Cell** updatedField;
    void Initialize();
    void Initialize(ImportedData data);
    void Restart();
    void CopyField(bool toNewField);
    void GetWindowSize(int width, int height);
    bool InBounds(Vector cord);
    Cell GetCell(Vector cord);
    void BurnNearCells(int x, int y, float kHeat);
    void Update();
    void EditorOnMouseHold();
    float GetCellSize();
    void Render(HDC hdc);
    void RenderUI(HDC hdc);
    void FillWithRandom();
    void Exit();

    void MoveCamToMouse();
    void OnMousePressed();
    void OnScroll(int dir);
    bool mouseHeld = false;
    void DragCamera(Vector delta);
};

class Button
{
public:
    Vector Position;
    Vector size;

public:
    bool CheckClick(Vector clickPos);
    virtual void OnPress(Module &module) {};
    Button(Vector position, Vector size);
    Button();
    virtual void Update(Module module) {}
    virtual void Render(HDC hdc);
};

struct ImportedData
{
public:
    int width;
    int height;
    Cell** field;

    ImportedData(int width, int height)
    {
        this->width = width;
        this->height = height;
        field = nullptr;
    }
};

class IncreaseTimeButton : public Button
{
public:
    IncreaseTimeButton(Vector pos, Vector size)
    {
        Position = pos;
        this->size = size;
    }
    void Render(HDC hdc);
    void OnPress(Module& module) { Approach(module.ElapsedTime, 0.5, 0.1); }
};
class DecreaseTimeButton : public Button
{
public:
    DecreaseTimeButton(Vector pos, Vector size)
    {
        Position = pos;
        this->size = size;
    }
    void Render(HDC hdc);
    void OnPress(Module& module) { Approach(module.ElapsedTime, 0.0017, 0.1); }
};

class RestartSimulationButton : public Button
{
public:
    RestartSimulationButton(Vector pos, Vector size)
    {
        Position = pos;
        this->size = size;
    }
    void Render(HDC hdc);
    void OnPress(Module &module) {module.Restart();}
};
class PauseResumeButton : public Button
{
public:
    bool paused = false;
    PauseResumeButton(Vector pos, Vector size)
    {
        Position = pos;
        this->size = size;
    }
    void Render(HDC hdc);
    void OnPress(Module& module)
    {
        if (module.state == Module::States::RunningSimulation)
        {
            module.state = Module::States::Paused;
            paused = true;
        }
            
        
        else if (module.state == Module::States::Paused)
        {
            module.state = Module::States::RunningSimulation;
            paused = false;
        }
    }
};
class TimerButton : public Button
{
public:
    float timer = 0;
    bool timerActive;
    TimerButton(Vector pos, Vector size)
    {
        Position = pos;
        this->size = size;
    }
    void Update(Module module) { this->timer = module.Timer; this->timerActive = module.timerActive; }
    void Render(HDC hdc);
    void OnPress(Module& module)
    {
        module.timerActive = !module.timerActive;
        if (module.timerActive) module.Timer = timer = 0;
    }
};

class SaveManager
{
public:
    static void SaveData(Cell** field, int width, int height, string path);
    static void SaveData(Cell** field, int width, int height, TCHAR* path);
    static ImportedData LoadData(string path);
    static ImportedData LoadData(TCHAR* path);
};