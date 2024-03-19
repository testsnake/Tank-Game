#include <Windows.h>
#include <windows.h>
#include <d2d1.h>
#include <dwrite.h>
#include <string>

#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dwrite.lib")

#define IDD_ENTER_IP_DIALOG 101
#define IDD_ENTER_PORT_DIALOG 102
#define IDC_IP_EDIT 1001
#define IDC_PORT_EDIT 1002


ID2D1Factory* pD2DFactory = NULL;
ID2D1HwndRenderTarget* pRT = NULL;
IDWriteFactory* pDWriteFactory = NULL;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

template <class T>
inline void SafeRelease(T** ppInterfaceToRelease) {
    if (*ppInterfaceToRelease != nullptr) {
        (*ppInterfaceToRelease)->Release();
        *ppInterfaceToRelease = nullptr;
    }
}

struct MenuItem {
    RECT rect;
    LPCWSTR text;
    int action; // You can define different actions for each menu item
};

void DrawMenu();
void HandleMouseClick(int x, int y);

// Function prototypes for dialog box handling
INT_PTR CALLBACK EnterIPDialogProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK EnterPortDialogProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam);

MenuItem menuItems[] = {
    { {200, 200, 600, 250}, L"Join Game", 1 },
    { {200, 260, 600, 310}, L"Host Game", 2 },
    { {200, 320, 600, 370}, L"Settings", 3 },
    { {200, 380, 600, 430}, L"Enter IP", 4 },
    { {200, 440, 600, 490}, L"Enter Port", 5 }
};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    // Register the window class
    const wchar_t CLASS_NAME[] = L"DirectX2DMenuWindowClass";

    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    RegisterClass(&wc);

    // Create the window
    HWND hwnd = CreateWindowEx(
        0,
        CLASS_NAME,
        L"DirectX 2D Menu Window",
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
    D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &pD2DFactory);

    // Run the message loop
    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // Clean up
    SafeRelease(&pD2DFactory);
    SafeRelease(&pRT);
    SafeRelease(&pDWriteFactory);

    return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_CREATE:
        if (FAILED(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &pD2DFactory))) {
            return -1;
        }
        if (FAILED(DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown**>(&pDWriteFactory)))) {
            return -1;
        }
        return 0;

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        if (SUCCEEDED(pD2DFactory->CreateHwndRenderTarget(
            D2D1::RenderTargetProperties(),
            D2D1::HwndRenderTargetProperties(hwnd, D2D1::SizeU(800, 600)),
            &pRT)))
        {
            pRT->BeginDraw();
            pRT->SetTransform(D2D1::Matrix3x2F::Identity());
            pRT->Clear(D2D1::ColorF(D2D1::ColorF::SkyBlue));

            // Draw menu items
            DrawMenu();

            pRT->EndDraw();
        }

        EndPaint(hwnd, &ps);
    }
    return 0;

    case WM_LBUTTONDOWN:
        // Handle left mouse button click
        HandleMouseClick(LOWORD(lParam), HIWORD(lParam));
        return 0;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;

    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}

void DrawMenu() {
    // Create a brush
    ID2D1SolidColorBrush* pBlackBrush = nullptr;
    pRT->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black), &pBlackBrush);

    // Create a text format
    IDWriteTextFormat* pTextFormat = nullptr;
    pDWriteFactory->CreateTextFormat(
        L"Arial",
        nullptr,
        DWRITE_FONT_WEIGHT_NORMAL,
        DWRITE_FONT_STYLE_NORMAL,
        DWRITE_FONT_STRETCH_NORMAL,
        20.0f,
        L"en-US",
        &pTextFormat
    );

    // Set text alignment
    pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
    pTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

    // Draw menu options
    for (const auto& menuItem : menuItems) {
        pRT->DrawText(
            menuItem.text,
            wcslen(menuItem.text),
            pTextFormat,
            D2D1::RectF(menuItem.rect.left, menuItem.rect.top, menuItem.rect.right, menuItem.rect.bottom),
            pBlackBrush
        );
    }

    // Release resources
    SafeRelease(&pBlackBrush);
    SafeRelease(&pTextFormat);
}

// Dialog procedure for Enter IP dialog
INT_PTR CALLBACK EnterIPDialogProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_INITDIALOG:
        return TRUE;
    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDOK: {
            wchar_t ipBuffer[256];
            GetDlgItemText(hwndDlg, IDC_IP_EDIT, ipBuffer, 256);
            MessageBox(NULL, ipBuffer, L"Entered IP", MB_OK);
            EndDialog(hwndDlg, wParam);
            break;
        }
        case IDCANCEL:
            EndDialog(hwndDlg, wParam);
            break;
        }
        break;
    default:
        return FALSE;
    }
    return TRUE;
}

// Dialog procedure for Enter Port dialog
INT_PTR CALLBACK EnterPortDialogProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_INITDIALOG:
        return TRUE;
    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDOK: {
            wchar_t portBuffer[256];
            GetDlgItemText(hwndDlg, IDC_PORT_EDIT, portBuffer, 256);
            MessageBox(NULL, portBuffer, L"Entered Port", MB_OK);
            EndDialog(hwndDlg, wParam);
            break;
        }
        case IDCANCEL:
            EndDialog(hwndDlg, wParam);
            break;
        }
        break;
    default:
        return FALSE;
    }
    return TRUE;
}


void HandleMouseClick(int x, int y) {
    // Check if the click coordinates fall within any menu option's boundaries
    for (const auto& menuItem : menuItems) {
        if (x >= menuItem.rect.left && x <= menuItem.rect.right &&
            y >= menuItem.rect.top && y <= menuItem.rect.bottom) {
            // Perform actions based on the menu item clicked
            switch (menuItem.action) {
            case 1: // Join Game
                MessageBox(NULL, L"Join Game clicked", L"Menu Item Clicked", MB_OK);
                break;
            case 2: // Host Game
                MessageBox(NULL, L"Host Game clicked", L"Menu Item Clicked", MB_OK);
                break;
            case 3: // Setting Game
                MessageBox(NULL, L"Setting Game clicked", L"Menu Item Clicked", MB_OK);
                break;
            case 4: // Enter IP
                DialogBox(NULL, MAKEINTRESOURCE(IDD_ENTER_IP_DIALOG), NULL, EnterIPDialogProc);
                break;
            case 5: // Enter Port
                DialogBox(NULL, MAKEINTRESOURCE(IDD_ENTER_PORT_DIALOG), NULL, EnterPortDialogProc);
                break;
            default:
                // Do nothing for other menu items
                break;
            }
            return;
        }
    }
}
