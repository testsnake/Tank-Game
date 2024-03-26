// GameWindow.cpp
#include "GameWindow.h"

GameWindow::GameWindow(std::wstring ipAddress, std::wstring port) :
    m_ipAddress(ipAddress), m_port(port), m_hWnd(NULL) {}

GameWindow::~GameWindow() {
    // Clean up window resources if needed
}

void GameWindow::Create(HWND parentWnd) {
    // Register the window class if it hasn't been registered already
    WNDCLASSEX wcex = {};
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.lpfnWndProc = WindowProc;
    wcex.hInstance = GetModuleHandle(NULL);
    wcex.lpszClassName = L"GameWindowClass";
    RegisterClassEx(&wcex);

    // Attempt to create the window
    m_hWnd = CreateWindowEx(0, L"GameWindowClass", L"Game Window",
        WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 800, 800,
        parentWnd, NULL, GetModuleHandle(NULL), this);

    if (m_hWnd == NULL) {
        MessageBox(parentWnd, L"Failed to create window. Exiting...", L"Error", MB_OK | MB_ICONERROR);
        return;
    }

    // If the window creation succeeds, show and update the window
    ShowWindow(m_hWnd, SW_SHOW);
    UpdateWindow(m_hWnd);
}



LRESULT CALLBACK GameWindow::WindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    GameWindow* pGameWindow;

    if (message == WM_NCCREATE) {
        CREATESTRUCT* pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
        pGameWindow = static_cast<GameWindow*>(pCreate->lpCreateParams);
        SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pGameWindow));
    }
    else {
        pGameWindow = reinterpret_cast<GameWindow*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
    }

    if (pGameWindow) {
        switch (message) {
        case WM_CREATE:
            // Handle creation
            return 0;
        case WM_PAINT:
            // Draw the ball when the window needs repainting
            pGameWindow->DrawBall(hwnd);
            return 0;
        case WM_DESTROY:
            // Handle destruction
            PostQuitMessage(0);
            return 0;
        default:
            return DefWindowProc(hwnd, message, wParam, lParam);
        }
    }

    return DefWindowProc(hwnd, message, wParam, lParam);
}


void GameWindow::DrawBall(HWND hwnd) {
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hwnd, &ps);

    // Draw a red ellipse (ball) at the center of the window
    HBRUSH hBrush = CreateSolidBrush(RGB(255, 0, 0)); // Red brush
    HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);

    int centerX = 200; // Center X coordinate
    int centerY = 200; // Center Y coordinate
    int radius = 50;   // Ball radius

    Ellipse(hdc, centerX - radius, centerY - radius, centerX + radius, centerY + radius);

    // Cleanup
    SelectObject(hdc, hOldBrush);
    DeleteObject(hBrush);

    EndPaint(hwnd, &ps);
}