#include <windows.h>
#include <iostream>
#include "ClipboardMonitor.h"

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  static ClipboardMonitor *monitor = nullptr;

  switch (uMsg)
  {
  case WM_CREATE:
    // Create and start monitoring clipboard events
    monitor = new ClipboardMonitor(hwnd);
    monitor->StartMonitoring();
    return 0;

  case WM_DRAWCLIPBOARD:
    // Handle clipboard update
    if (monitor)
      monitor->HandleClipboardUpdate();
    return 0;

  case WM_DESTROY:
    // Stop monitoring and clean up
    if (monitor)
    {
      monitor->StopMonitoring();
      delete monitor;
      monitor = nullptr; // Clear the pointer to prevent dangling reference
    }
    PostQuitMessage(0);
    return 0;

  case WM_CLOSE:
    // Ensure cleanup if the window is closed unexpectedly
    if (monitor)
    {
      monitor->StopMonitoring();
      delete monitor;
      monitor = nullptr;
    }
    return 0;
  }

  return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int main()
{
  LPCWSTR className = L"ClipboardMonitorWindow";
  WNDCLASS wc = {};
  wc.lpfnWndProc = WindowProc;
  wc.hInstance = GetModuleHandle(nullptr);
  wc.lpszClassName = className;

  if (!RegisterClass(&wc))
  {
    std::cerr << "Failed to register window class." << std::endl;
    return -1;
  }

  HWND hwnd = CreateWindowExW(
      0, className, L"Clipboard Monitor", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
      300, 200, nullptr, nullptr, wc.hInstance, nullptr);

  if (hwnd == nullptr)
  {
    std::cerr << "Failed to create window." << std::endl;
    return -1;
  }

  ShowWindow(hwnd, SW_SHOWNORMAL);
  UpdateWindow(hwnd);

  // Run message loop
  MSG msg = {};
  while (GetMessage(&msg, nullptr, 0, 0))
  {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }

  return 0;
}
