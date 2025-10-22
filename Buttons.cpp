#include "IndProj.h"
#include <sstream>

Button::Button()
{
	this->Position = Vector(0, 0);
	this->size = Vector(200, 130);
}
Button::Button(Vector position, Vector size)
{
	this->Position = position;
	this->size = size;
}

bool Button::CheckClick(Vector mousePos)
{
	bool result = mousePos.X >= Position.X && mousePos.X <= Position.X + size.X && mousePos.Y >= Position.Y && mousePos.Y <= Position.Y + size.Y;
	return result;
}


void Button::Render(HDC hdc)
{
    HBRUSH temporalBrush = CreateSolidBrush(RGB(100, 100, 100));
    SelectObject(hdc, temporalBrush);

    Rectangle(hdc, (int)Position.X, (int)Position.Y,
        (int)Position.X + (int)size.X, (int)Position.Y + (int)size.Y);
	
    DeleteObject(temporalBrush);
}

void Triangle(HDC hdc, POINT a, POINT b, POINT c)
{
	SelectObject(hdc, CreatePen(PS_SOLID, 3, RGB(255, 255, 255)));
	MoveToEx(hdc, a.x, a.y, NULL);
	LineTo(hdc, b.x, b.y);
	LineTo(hdc, c.x, c.y);
	LineTo(hdc, a.x, a.y);
	SelectObject(hdc, CreatePen(PS_NULL, 3, RGB(255, 255, 255)));
}
void IncreaseTimeButton::Render(HDC hdc)
{
	HBRUSH temporalBrush = CreateSolidBrush(RGB(100, 100, 100));
	SelectObject(hdc, temporalBrush);
	Rectangle(hdc, Position.X, Position.Y, Position.X + size.X, Position.Y + size.Y);

	Triangle(hdc, (Position + size / 3).Point(), (Position + Vector(size.X / 1.5, size.Y / 2)).Point(), (Position + Vector(size.X / 3, size.Y * 0.66)).Point());
}
void DecreaseTimeButton::Render(HDC hdc)
{
	HBRUSH temporalBrush = CreateSolidBrush(RGB(100, 100, 100));
	SelectObject(hdc, temporalBrush);
	Rectangle(hdc, Position.X, Position.Y, Position.X + size.X, Position.Y + size.Y);

	Triangle(hdc, (Position + Vector(size.X / 3, size.Y / 2)).Point(), (Position + Vector(size.X * 0.66, size.Y / 3)).Point(), (Position + Vector(size.X * 0.66, size.Y * 0.66)).Point());
}

void RestartSimulationButton::Render(HDC hdc)
{
	HBRUSH temporalBrush = CreateSolidBrush(RGB(100, 100, 100));
	SelectObject(hdc, temporalBrush);
	Rectangle(hdc, Position.X, Position.Y, Position.X + size.X, Position.Y + size.Y);

	SelectObject(hdc, CreateSolidBrush(RGB(255, 255, 255)));
	Rectangle(hdc, Position.X + 10, Position.Y + 10, Position.X + size.X - 10, Position.Y + size.Y - 10);
}
void PauseResumeButton::Render(HDC hdc)
{
	HBRUSH temporalBrush = CreateSolidBrush(RGB(100, 100, 100));
	SelectObject(hdc, temporalBrush);
	Rectangle(hdc, Position.X, Position.Y, Position.X + size.X, Position.Y + size.Y);
	
	SelectObject(hdc, CreateSolidBrush(RGB(255, 255, 255)));
	
	if (paused)
	{
		Triangle(hdc, (Position + size / 3).Point(), (Position + Vector(size.X / 1.5, size.Y / 2)).Point(), (Position + Vector(size.X / 3, size.Y * 0.66)).Point());
	}
	else
	{
		Rectangle(hdc, Position.X + size.X / 5, Position.Y + size.Y / 5, Position.X + size.X / 5 * 2, Position.Y + size.Y / 5 * 4);
		Rectangle(hdc, Position.X + size.X / 5 * 3, Position.Y + size.Y / 5, Position.X + size.X / 5 * 4, Position.Y + size.Y / 5 * 4);
	}
}



const wchar_t* FloatToText(float num)
{
	std::wostringstream woss;
	woss << ' ';
	woss << num;
	std::wstring wideString = woss.str();

	const wchar_t* wideCharPointer = wideString.c_str();
	// Or copy to a modifiable buffer:
	std::vector<wchar_t> buffer(wideString.begin(), wideString.end());
	buffer.push_back(L'\0'); // Null-terminate
	wchar_t* modifiableWideCharPointer = buffer.data();

	return wideCharPointer;
}

void TimerButton::Render(HDC hdc)
{
	LOGFONT lf;
	ZeroMemory(&lf, sizeof(LOGFONT));
	lf.lfHeight = -MulDiv(22, GetDeviceCaps(hdc, LOGPIXELSY), 72); // 12-point font
	wcscpy_s(lf.lfFaceName, L"Arial"); // Example font face
	HFONT hFont = CreateFontIndirect(&lf);
	SelectObject(hdc, hFont);

	HBRUSH temporalBrush = CreateSolidBrush(RGB(100, 100, 100));
	SelectObject(hdc, temporalBrush);
	Rectangle(hdc, Position.X, Position.Y, Position.X + size.X, Position.Y + size.Y);

	SelectObject(hdc, CreateSolidBrush(RGB(255, 255, 255)));
	if (!timerActive)
	{
		Triangle(hdc, (Position + size / 3).Point(), (Position + Vector(size.X / 1.5, size.Y / 2)).Point(), (Position + Vector(size.X / 3, size.Y * 0.66)).Point());
	}
	else
	{
		Rectangle(hdc, Position.X + size.X / 5, Position.Y + size.Y / 5, Position.X + size.X / 5 * 4, Position.Y + size.Y / 5 * 4);
	}

	SetBkMode(hdc, TRANSPARENT);
	SetTextColor(hdc, RGB(240, 240, 240));
	
	const wchar_t* text = FloatToText(round(timer*10)/10);
	TextOut(hdc, Position.X + 70, Position.Y + 10, text, 12);
}