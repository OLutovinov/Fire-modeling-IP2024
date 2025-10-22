#include "IndProj.h"
#include <iostream>
#include <fstream>

using namespace std;

void Approach(float& value, float target, float maxMove)
{
    if (abs(target - value) <= maxMove) value = target;
    else if (target > value) value += maxMove;
    else value -= maxMove;
}

COLORREF ColorMult(int r, int g, int b, float multiplier)
{
    return RGB((float)r * multiplier, (float)g * multiplier, (float)b * multiplier);
}   

void Module::Restart()
{
    for (int x = 0; x < width; x++)
    {
        for (int y = 0; y < height; y++)
        {
            field[x][y].CopyConstantsFrom(startPosition[x][y]);
            field[x][y].CopyTempsFrom(startPosition[x][y]);
        }
    }
    state = RunningSimulation;
    Timer = 0;
    timerActive = false;
}
void Module::Initialize()
{
    setlocale(LC_CTYPE, "rus");
    srand(time(0));
    
    width = 90;
    height = 60;

    WindowWidth = 1920; WindowHeight = 1080;

    windDir = Vector(0, 0);

    startPosition = new Cell * [width];
    field = new Cell * [width];
    updatedField = new Cell * [width];

    for (int x = 0; x < width; x++)
    {
        startPosition[x] = new Cell[height]();
        field[x] = new Cell[height]();
        updatedField[x] = new Cell[height]();
    }

    FillWithRandom();

    buttonCount = 5;
    buttons = new Button*[buttonCount]();
    buttons[0] = new DecreaseTimeButton(Vector(50, 80), Vector(40, 40));
    buttons[1] = new IncreaseTimeButton(Vector(100, 80), Vector(40, 40));
    buttons[2] = new RestartSimulationButton(Vector(30, 20), Vector(55, 55));
    buttons[3] = new PauseResumeButton(Vector(100, 20), Vector(55, 55));
    buttons[4] = new TimerButton(Vector(300, 20), Vector(55, 55));

    camera = Camera();
    camera.Position.Y -= GetCellSize() * height / 8;
    editor = Editor();

    state = Paused;
    Timer = 0;
    timerActive = false;
}
void Module::Initialize(ImportedData data)
{
    width = data.width;
    height = data.height;
    
    Exit();
    startPosition = new Cell * [width];
    field = new Cell * [width];
    updatedField = new Cell * [width];

    for (int x = 0; x < width; x++)
    {
        startPosition[x] = new Cell[height]();
        field[x] = new Cell[height]();
        updatedField[x] = new Cell[height]();
    }

    for (int x = 0; x < width; x++)
    {
        for (int y = 0; y < height; y++)
        {
            field[x][y].CopyTempsFrom(data.field[x][y]);
            field[x][y].CopyConstantsFrom(data.field[x][y]);
            updatedField[x][y].FillParams();
            updatedField[x][y].CopyConstantsFrom(data.field[x][y]);
            startPosition[x][y].CopyConstantsFrom(data.field[x][y]);
            startPosition[x][y].CopyTempsFrom(data.field[x][y]);
        }
    }
    state = Paused;
    Timer = 0;
    timerActive = false;
}

void Module::CopyField(bool toNewField)
{
    for (int x = 0; x < width; x++)
    {
        for (int y = 0; y < height; y++)
        {
            if (toNewField) updatedField[x][y].CopyTempsFrom(field[x][y]);
            else field[x][y].CopyTempsFrom(updatedField[x][y]);
        }
    }
}

void Module::GetWindowSize(int width, int height)
{
    WindowWidth = width;
    WindowHeight = height;
}

void Module::Update()
{
    if (state == InEditor)
    {
        for (int p = 0; p < 10; p++)
        {
            if (GetAsyncKeyState(48 + p))
            {
                editor.ReloadType(p);
                break;
            }
        }
    }

    for (int c = 0; c < buttonCount; c++)
        buttons[c]->Update(*this);

    if (state != RunningSimulation) return;


    CopyField(true);

    for (int x = 0; x < width; x++)
    {
        for (int y = 0; y < height; y++)
        {
            if (field[x][y].burning)
            {
                float cellHeat = field[x][y].burnSpeed * field[x][y].combustionHeat;
                BurnNearCells(x, y, cellHeat/2.);
                updatedField[x][y].Update(cellHeat/2., ElapsedTime);
            }
        }
    }

    CopyField(false);
    if(timerActive) Timer += ElapsedTime;
}
void Module::EditorOnMouseHold()
{
    Vector selected = mousePosition;
    camera.ScreenToWorld(*this, selected);
    if (!InBounds(selected)) return;

    startPosition[(int)selected.X][(int)selected.Y].CopyConstantsFrom(editor.cellType);
    startPosition[(int)selected.X][(int)selected.Y].CopyTempsFrom(editor.cellType);
}

bool Module::InBounds(Vector cord)
{
    return (cord.X >= 0 && cord.X < width && cord.Y >= 0 && cord.Y < height);
}

Cell Module::GetCell(Vector cord)
{
    return field[(int)cord.X][(int)cord.Y];
}

void Module::BurnNearCells(int fromX, int fromY, float kHeat)
{
    float baseHeight = field[fromX][fromY].height;
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
        checkPos += windDir;
        if (!InBounds(checkPos)) continue;

        Cell currentCell = GetCell(checkPos);

        if(!currentCell.burning && currentCell.fuelMass > 0 && !currentCell.isWater)
        {
            float heightMult = max(-pow(( ((currentCell.height - baseHeight) - 0.2) / 0.4), 2 ) + 1, 0);

            updatedField[(int)checkPos.X][(int)checkPos.Y].Update(
                kHeat / (Vector(fromX, fromY) - checkPos).Length() * heightMult, ElapsedTime);
        }
    }
}


void Editor::ReloadType(int profile)
{
    std::ifstream input("brushProfiles.txt");


    while (!input.eof())
    {
        string s;
        input >> s;
        if (s == "profile:")
        {
            int localProfile; input >> localProfile;
            if (localProfile == profile)
            {
                string s;
                input >> s; input >> cellType.isWater;
                input >> s; input >> cellType.burning;
                input >> s; input >> cellType.fuelMass;
                input >> s; input >> cellType.capacity;
                input >> s; input >> cellType.fireTemp;
                input >> s; input >> cellType.temp;

                float humidity; input >> s; input >> humidity;
                cellType.SetHumidity(humidity);
                input >> s; input >> cellType.burnSpeed;
                input >> s; input >> cellType.combustionHeat;
                input >> s; input >> cellType.height;
                input >> s; input >> cellType.colorR >> cellType.colorG >> cellType.colorB;
                
                return;
            }
        }
    }
}

void Module::MoveCamToMouse()
{
    camera.Position += mousePosition - (Vector(WindowWidth, WindowHeight) / 2);
}
void Module::OnMousePressed()
{
    bool flag = true;
    for (int c = 0; c < buttonCount; c++)
    {
        if ((*buttons[c]).CheckClick(mousePosition))
        {
            (*buttons[c]).OnPress(*this);
            flag = false;
            return;
        }
    }
}
void Module::OnScroll(int dir)
{
    camera.Position += (mousePosition - (Vector(WindowWidth, WindowHeight) / 2)) * dir * 0.1;
    Approach(camera.Zoom, dir > 0 ? 100 : 0.001, 0.1 );
}
void Module::DragCamera(Vector delta)
{
    camera.Position -= delta;
}

void Clear(HDC hdc, int width, int height)
{
    SelectObject(hdc, CreateSolidBrush(RGB(40, 40, 40)));
    Rectangle(hdc, 0, 0, width, height);
} 

void Module::RenderUI(HDC hdc)
{
    HBRUSH bg = CreateSolidBrush(RGB(60, 60, 60));
    SelectObject(hdc, bg);
    Rectangle(hdc, 0, 0, WindowWidth, 130);
    DeleteObject(bg);
    for (int c = 0; c < buttonCount; c++)
    {
        (*buttons[c]).Render(hdc);
    }
    
}

float Module::GetCellSize()
{
    float fullSize =
        ((float)WindowHeight / (float)height < (float)WindowWidth / (float)width) ?
        (WindowHeight) / (float)height : (WindowWidth) / (float)width;


    return fullSize;
}
void Module::Render(HDC hdc)
{
    Clear(hdc, WindowWidth, WindowHeight);

    float fullSize = GetCellSize();

    HBRUSH temporalBrush;
    HPEN pen = CreatePen(PS_NULL, 1, RGB(255, 0, 0));
    SelectObject(hdc, pen);
    for (int x = 0; x < width; x++)
    {
        for (int y = 0; y < height; y++)
        {
            if ((state == InEditor ? startPosition[x][y] : field[x][y]).fuelMass <= 0) continue;
            //if (!field[x][y].burning) continue;
            temporalBrush = CreateSolidBrush((state == InEditor ? startPosition[x][y] : field[x][y]).GetColor());
            pen = CreatePen(PS_INSIDEFRAME, 1, (state == InEditor ? startPosition[x][y] : field[x][y]).GetColor());
            SelectObject(hdc, temporalBrush);
            SelectObject(hdc, pen);

            Vector cellPos = Vector(x * fullSize, y * fullSize);
            camera.WorldToScreen(cellPos);

            Rectangle(hdc, (int)cellPos.X, (int)cellPos.Y,
                (int)cellPos.X + fullSize*camera.Zoom, (int)cellPos.Y + fullSize * camera.Zoom);

            DeleteObject(temporalBrush);
            DeleteObject(pen);
        }
        
    }
    pen = CreatePen(PS_NULL, 1, RGB(0, 0, 0));
    SelectObject(hdc, pen);
    if (renderWind)
    {
        for (int x = 0; x < width; x++)
        {
            for (int y = 0; y < height; y++)
            {
                Vector cellPos = Vector(x * fullSize, y * fullSize);
                MoveToEx(hdc, ((int)cellPos.X + fullSize / 2), ((int)cellPos.Y + fullSize / 2), 0);

                LineTo(hdc, ((int)cellPos.X + fullSize / 2) + (int)windDir.X * fullSize,
                    ((int)cellPos.Y + fullSize / 2) + (int)windDir.Y * fullSize);
            }
        }
    }
    
    RenderUI(hdc);

}


void Module::FillWithRandom()
{
    for (int x = 0; x < width; x++)
    {
        for (int y = 0; y < height; y++)
        {
            startPosition[x][y].FillParams();
            updatedField[x][y].CopyTempsFrom(startPosition[x][y]);
            updatedField[x][y].CopyConstantsFrom(startPosition[x][y]);
            field[x][y].CopyTempsFrom(startPosition[x][y]);
            field[x][y].CopyConstantsFrom(startPosition[x][y]);
        }
    }
}

    void Module::Exit()
    {
        for (int x = 0; x < width; x++)
        {
            delete[] field[x];       
            delete[] updatedField[x];
            delete[] startPosition[x];
        }

        delete[] field;
        delete[] updatedField;
        delete[] startPosition;
    }
