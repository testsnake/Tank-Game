// Source.cpp
#include <Windows.h>
#include <windows.h>
#include <d2d1.h>
#include <dwrite.h>
#include <string>
#include "GameWindow.h"

// Link with d2d1.lib and dwrite.lib
#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dwrite.lib")

#define ID_JOIN_GAME 1001   
#define IDC_PORT_INPUT 1002
#define IDC_IP_INPUT 1003


// Global variables
ID2D1Factory* pD2DFactory = NULL;
IDWriteFactory* pDWriteFactory = NULL;
IDWriteTextFormat* pTextFormat = NULL;
ID2D1HwndRenderTarget* pRT = NULL; // Declare pRT here

GameWindow* pGameWindow = nullptr;

// Variables to store IP and port
std::wstring ipAddress;
std::wstring port;

// Forward declarations
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
void InitializeDirect2D(HWND hwnd);
void InitializeDirectWrite();
void Render(HWND hwnd);
void CreateUI(HWND hwnd);
void OnJoinGame(HWND hwnd); // New function to handle join game button click
void Cleanup();
void SafeRelease(IUnknown** ppInterface);

// Entry point
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    // Register the window class
    const wchar_t CLASS_NAME[] = L"TankGameClass";

    WNDCLASS wc = {};

    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;

    RegisterClass(&wc);

    // Create the window
    HWND hwnd = CreateWindowEx(
        0,
        CLASS_NAME,
        L"Tank Game",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 800, 600,
        NULL,
        NULL,
        hInstance,
        NULL
    );

    if (hwnd == NULL) {
        return 0;
    }

    // Initialize Direct2D
    InitializeDirect2D(hwnd);
    InitializeDirectWrite();

    // Create UI elements
    CreateUI(hwnd);

    // Show the window
    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    // Message loop
    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // Clean up resources
    Cleanup();

    return 0;
}

// Window procedure
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_PAINT:
        Render(hwnd);
        ValidateRect(hwnd, NULL); // Validate the entire client area to prevent continuous repainting
        break;
    case WM_COMMAND:
        // Check if the join game button was clicked
        if (LOWORD(wParam) == ID_JOIN_GAME && HIWORD(wParam) == BN_CLICKED) {
            OnJoinGame(hwnd);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hwnd, message, wParam, lParam);
    }
    return 0;
}

// Initialize Direct2D 
void InitializeDirect2D(HWND hwnd) {
    D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &pD2DFactory);

    RECT rc;
    GetClientRect(hwnd, &rc);

    pD2DFactory->CreateHwndRenderTarget(
        D2D1::RenderTargetProperties(),
        D2D1::HwndRenderTargetProperties(hwnd, D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top)),
        &pRT
    );
}

// Initialize DirectWrite
void InitializeDirectWrite() {
    DWriteCreateFactory(
        DWRITE_FACTORY_TYPE_SHARED,
        __uuidof(IDWriteFactory),
        reinterpret_cast<IUnknown**>(&pDWriteFactory)
    );

    pDWriteFactory->CreateTextFormat(
        L"Arial",
        NULL,
        DWRITE_FONT_WEIGHT_NORMAL,
        DWRITE_FONT_STYLE_NORMAL,
        DWRITE_FONT_STRETCH_NORMAL,
        18.0f,
        L"en-US",
        &pTextFormat
    );
}

// Render function
void Render(HWND hwnd) {
    pRT->BeginDraw();

    // Clear the background
    pRT->Clear(D2D1::ColorF(D2D1::ColorF::White));

    // Create a solid color brush for text
    ID2D1SolidColorBrush* pBlackBrush = nullptr;
    pRT->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black), &pBlackBrush);

    // Render the title "Tank Game"
    D2D1_RECT_F titleRect = D2D1::RectF(50, 50, 200, 100);
    pRT->DrawText(
        L"Tank Game",
        wcslen(L"Tank Game"),
        pTextFormat,
        titleRect,
        pBlackBrush,
        D2D1_DRAW_TEXT_OPTIONS_NONE,
        DWRITE_MEASURING_MODE_NATURAL
    );

    // Render buttons ("Join Game", "Host Game", "Settings")
    D2D1_RECT_F joinButtonRect = D2D1::RectF(50, 150, 200, 200);
    pRT->DrawRectangle(joinButtonRect, pBlackBrush); // Use the same brush for rectangle and text rendering
    pRT->DrawText(
        L"Join Game",
        wcslen(L"Join Game"),
        pTextFormat,
        joinButtonRect,
        pBlackBrush,
        D2D1_DRAW_TEXT_OPTIONS_NONE,
        DWRITE_MEASURING_MODE_NATURAL
    );

    // Render fields for IP and Port input
    // IP Input Field
    D2D1_RECT_F ipInputRect = D2D1::RectF(300, 150, 500, 175);
    pRT->DrawRectangle(ipInputRect, pBlackBrush); // Use the same brush for rectangle and text rendering
    pRT->DrawText(
        L"IP Address:",
        wcslen(L"IP Address:"),
        pTextFormat,
        D2D1::RectF(300, 125, 500, 150),
        pBlackBrush,
        D2D1_DRAW_TEXT_OPTIONS_NONE,
        DWRITE_MEASURING_MODE_NATURAL
    );

    // Port Input Field
    D2D1_RECT_F portInputRect = D2D1::RectF(300, 200, 500, 225);
    pRT->DrawRectangle(portInputRect, pBlackBrush); // Use the same brush for rectangle and text rendering
    pRT->DrawText(
        L"Port:",
        wcslen(L"Port:"),
        pTextFormat,
        D2D1::RectF(300, 175, 500, 200),
        pBlackBrush,
        D2D1_DRAW_TEXT_OPTIONS_NONE,
        DWRITE_MEASURING_MODE_NATURAL
    );

    // Release the solid color brush
    SafeRelease(reinterpret_cast<IUnknown**>(&pBlackBrush));

    // End drawing
    HRESULT hr = pRT->EndDraw();
    if (FAILED(hr) || hr == D2DERR_RECREATE_TARGET) {
        // If EndDraw fails or if the render target needs to be recreated, recreate it
        SafeRelease(reinterpret_cast<IUnknown**>(&pRT));
        InitializeDirect2D(hwnd);
    }
}

// Create UI elements
void CreateUI(HWND hwnd) {
    // Create "Join Game" button
    CreateWindow(
        L"BUTTON",   // Predefined class; Unicode assumed
        L"Join Game",      // Button text
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  // Styles
        50,         // x position
        150,         // y position
        150,        // Button width
        50,        // Button height
        hwnd,     // Parent window
        (HMENU)ID_JOIN_GAME,       // No menu
        (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE),
        NULL
    );

    // Create IP address input field
    CreateWindow(
        L"EDIT",
        L"",
        WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL,
        300,
        150,
        200,
        25,
        hwnd,
        (HMENU)IDC_IP_INPUT,
        NULL,
        NULL
    );

    // Create Port input field
    CreateWindow(
        L"EDIT",
        L"",
        WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL,
        300,
        200,
        200,
        25,
        hwnd,
        (HMENU)IDC_PORT_INPUT,
        NULL,
        NULL
    );
}

// Handle "Join Game" button click
void OnJoinGame(HWND hwnd) {
    // Get IP address and port from input fields
    wchar_t ipBuffer[256];
    wchar_t portBuffer[256];

    GetDlgItemText(hwnd, IDC_IP_INPUT, ipBuffer, ARRAYSIZE(ipBuffer));
    GetDlgItemText(hwnd, IDC_PORT_INPUT, portBuffer, ARRAYSIZE(portBuffer));

    // Store IP address and port
    ipAddress = ipBuffer;
    port = portBuffer;

    // Output IP address and port (for demonstration)
    MessageBox(hwnd, (ipAddress + L":" + port).c_str(), L"Connection Info", MB_OK);


    if (pGameWindow == nullptr) {
        // Create the JoinGameWindow instance and pass IP and port
        pGameWindow = new GameWindow(ipAddress, port);
        pGameWindow->Create(hwnd);
    }

}

// Cleanup resources
void Cleanup() {
    delete pGameWindow; // Release the allocated memory
    pGameWindow = nullptr;
    SafeRelease(reinterpret_cast<IUnknown**>(&pTextFormat));
    SafeRelease(reinterpret_cast<IUnknown**>(&pDWriteFactory));
    SafeRelease(reinterpret_cast<IUnknown**>(&pRT));
    SafeRelease(reinterpret_cast<IUnknown**>(&pD2DFactory));
}

// Release a COM interface pointer safely
void SafeRelease(IUnknown** ppInterface) {
    if (*ppInterface != NULL) {
        (*ppInterface)->Release();
        *ppInterface = NULL;
    }
}
