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
        case WM_KEYDOWN:
            // Handle key down events (WASD for movement)
            switch (wParam) {
            case 'W':
                // Move the circle up
                pGameWindow->MoveCircle(0, -2);
                InvalidateRect(hwnd, NULL, TRUE); // Request a repaint
                break;
            case 'A':
                // Move the circle left
                pGameWindow->MoveCircle(-2, 0);
                InvalidateRect(hwnd, NULL, TRUE); // Request a repaint
                break;
            case 'S':
                // Move the circle down
                pGameWindow->MoveCircle(0, 2);
                InvalidateRect(hwnd, NULL, TRUE); // Request a repaint
                break;
            case 'D':
                // Move the circle right
                pGameWindow->MoveCircle(2, 0);
                InvalidateRect(hwnd, NULL, TRUE); // Request a repaint
                break;
            }
            return 0;
        }
    }

    return DefWindowProc(hwnd, message, wParam, lParam);
}

//void GameWindow::MoveCircle(int dx, int dy) {
//    // Update the position of the circle
//    m_circleX += dx;
//    m_circleY += dy;
//
//    // Request a repaint to refresh the window
//    InvalidateRect(m_hWnd, NULL, TRUE);
//}
void GameWindow::MoveCircle(int dx, int dy) {
    // Update the position of the circle
    m_circleX += dx;
    m_circleY += dy;

    // Get the dimensions of the client area
    RECT clientRect;
    GetClientRect(m_hWnd, &clientRect);

    // Adjust the circle position if it goes out of bounds
    if (m_circleX < 20) {
        m_circleX = 20;
    }
    else if (m_circleX > clientRect.right - 20) { // Adjust the value according to the circle size
        m_circleX = clientRect.right - 20;
    }

    if (m_circleY < 20) {
        m_circleY = 20;
    }
    else if (m_circleY > clientRect.bottom - 20) { // Adjust the value according to the circle size
        m_circleY = clientRect.bottom - 20;
    }

    // Request a repaint to refresh the window
    InvalidateRect(m_hWnd, NULL, TRUE);
}


void GameWindow::DrawBall(HWND hwnd) {
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hwnd, &ps);

    // Clear the drawing area by filling it with the background color (e.g., white)
    HBRUSH hBackgroundBrush = CreateSolidBrush(RGB(255, 255, 255)); // White brush
    RECT clientRect;
    GetClientRect(hwnd, &clientRect);
    FillRect(hdc, &clientRect, hBackgroundBrush);
    DeleteObject(hBackgroundBrush);

    // Draw a red ellipse (ball) at the specified position
    HBRUSH hBrush = CreateSolidBrush(RGB(255, 0, 0)); // Red brush
    HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);

    int centerX = m_circleX; // Center X coordinate
    int centerY = m_circleY; // Center Y coordinate
    int radius = 20;   // Ball radius

    Ellipse(hdc, centerX - radius, centerY - radius, centerX + radius, centerY + radius);

    // Cleanup
    SelectObject(hdc, hOldBrush);
    DeleteObject(hBrush);

    EndPaint(hwnd, &ps);
}
