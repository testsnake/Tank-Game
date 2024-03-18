#include <Windows.h>
#include <d3d11.h>

#pragma comment(lib, "d3d11.lib")

#include <d2d1_2.h> // Include Direct2D headers
#pragma comment(lib, "d2d1.lib") // Link Direct2D library

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

ID2D1Factory2* pFactory = NULL;
ID2D1HwndRenderTarget* pRenderTarget = NULL;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    // Register the window class
    const wchar_t CLASS_NAME[] = L"DirectXWindowClass";

    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.hbrBackground = NULL; // No background needed for Direct2D
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);

    RegisterClass(&wc);

    // Create the window
    HWND hwnd = CreateWindowEx(
        0,
        CLASS_NAME,
        L"DirectX Window",
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

    ShowWindow(hwnd, nCmdShow);

    // Initialize Direct2D
    D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &pFactory);
    RECT rc;
    GetClientRect(hwnd, &rc);
    pFactory->CreateHwndRenderTarget(
        D2D1::RenderTargetProperties(),
        D2D1::HwndRenderTargetProperties(hwnd, D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top)),
        &pRenderTarget
    );

    // Main message loop
    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // Cleanup Direct2D objects
    if (pRenderTarget) {
        pRenderTarget->Release();
    }
    if (pFactory) {
        pFactory->Release();
    }

    return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;

    case WM_PAINT: {
        if (pRenderTarget) {
            pRenderTarget->BeginDraw();
            pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));

            // Create a brush for filling the rectangle
            ID2D1SolidColorBrush* pBrush;
            pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Red), &pBrush);

            // Draw a rectangle
            D2D1_RECT_F rectangle = D2D1::RectF(100.0f, 100.0f, 300.0f, 300.0f);
            pRenderTarget->FillRectangle(&rectangle, pBrush);

            // Release the brush
            pBrush->Release();

            // End drawing
            pRenderTarget->EndDraw();
        }
        ValidateRect(hwnd, NULL); // Validate the entire client area
        return 0;
    }

    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
}
