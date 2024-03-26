//// GameWindow.h
//#pragma once
//#include <Windows.h>
//#include <windows.h>
//#include <string>
//
//class GameWindow {
//public:
//    GameWindow(std::wstring ipAddress, std::wstring port);
//    ~GameWindow();
//
//    void Create(HWND parentWnd);
//
//    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
//
//private:
//    HWND m_hWnd;
//    std::wstring m_ipAddress;
//    std::wstring m_port;
//
//    void DrawBall(HWND hwnd); // Declaration of DrawBall method
//};
// GameWindow.h
#pragma once
#include <Windows.h>
#include <string>

class GameWindow {
public:
    GameWindow(std::wstring ipAddress, std::wstring port);
    ~GameWindow();

    void Create(HWND parentWnd);
    void MoveCircle(int dx, int dy); // Declaration of MoveCircle method

    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

private:
    HWND m_hWnd;
    std::wstring m_ipAddress;
    std::wstring m_port;
    int m_circleX = 200; // Initial X position of the circle
    int m_circleY = 200; // Initial Y position of the circle

    void DrawBall(HWND hwnd); // Declaration of DrawBall method
};
