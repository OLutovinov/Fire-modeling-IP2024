// IndProj.cpp : Defines the entry point for the application.
//
#include "framework.h"
#include "IndProj.h"
#include <commdlg.h>
#include <string>
#include <vector>
#include <windows.h>

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);


//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

Module Program;


//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void SetupConsole()
{
    AllocConsole();
    AttachConsole(GetCurrentProcessId());
    
    FILE* conout = stdout;
    freopen_s(&conout, "CONOUT$", "w", stdout);

    SetConsoleTitle(L"debug console");

    Flush();
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.

    //SetupConsole();

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_INDPROJ, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_INDPROJ));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_INDPROJ));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_INDPROJ);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   HWND hWnd = CreateWindowExW(WS_EX_COMPOSITED, szWindowClass, szTitle, WS_EX_COMPOSITED,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}


//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//

static TCHAR name[256] = _T("");
static OPENFILENAME file;
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
        file.lStructSize = sizeof(OPENFILENAME);
        file.hInstance = hInst;
        file.lpstrFilter = _T("Text\0*.txt");
        file.lpstrFile = name;
        file.nMaxFile = 256;
        file.lpstrInitialDir = _T(".\\");
        file.lpstrDefExt = _T("txt");
        
        SetTimer(hWnd, 1, 17, NULL);
        SetTimer(hWnd, 2, 300, NULL);
        Program.Initialize(); // âûçîâ ôóíêöèè èíèöèàëèçàöèè
        break;

    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                Program.Exit();
                DestroyWindow(hWnd);
                break;
            case ID_FILE_OPEN:
                file.lpstrTitle = _T("Открыть файл");
                file.Flags = OFN_HIDEREADONLY;
                if (!GetOpenFileName(&file)) return 1;
                
                Program.Initialize(SaveManager::LoadData(name));

                InvalidateRect(hWnd, NULL, 1);
                break;
            case ID_FILE_SAVE:
                file.lpstrTitle = _T("Сохранить файл");
                file.Flags = OFN_NOTESTFILECREATE;
                if (!GetSaveFileName(&file)) return 1;
                SaveManager::SaveData(Program.startPosition, Program.width, Program.height, name);
                break;

            case ID_FILE_EDIT:
                Program.Restart();
                Program.state = Program.InEditor;
                break;

            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_SIZE:
        RECT window; GetClientRect(hWnd, &window);
        Program.GetWindowSize(window.right, window.bottom);
        break;

    case WM_TIMER:
        if(wParam == 1) Program.Update();
        else InvalidateRect(hWnd, NULL, TRUE);
        break;

    case WM_MOUSEMOVE:
        if (Program.mouseHeld)
        {
            if (GetAsyncKeyState(VK_SPACE) & 0x8000)
                Program.DragCamera(Vector(LOWORD(lParam), HIWORD(lParam)) - Program.mousePosition);
            else if (Program.state == Program.InEditor) Program.EditorOnMouseHold();
        }
        Program.mousePosition = Vector(LOWORD(lParam), HIWORD(lParam));
        
        break;
    case WM_LBUTTONDOWN:
        Program.OnMousePressed();
        Program.mouseHeld = true;
        break;
    case WM_LBUTTONUP:
        Program.mouseHeld = false;
    case WM_MOUSEWHEEL:
        if (MK_CONTROL & wParam) Program.OnScroll((short)HIWORD(wParam) / WHEEL_DELTA);
        break;

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);

        Program.Render(hdc);
        EndPaint(hWnd, &ps);
        break;
    }

    case WM_DESTROY:
        Program.Exit();
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
