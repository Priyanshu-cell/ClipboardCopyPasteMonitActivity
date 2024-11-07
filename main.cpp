#include <windows.h>
#include "ClipboardMonitor.h"

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  static ClipboardMonitor *monitor = nullptr;

  switch (uMsg)
  {
  case WM_CREATE:
    monitor = new ClipboardMonitor(hwnd);
    monitor->StartMonitoring();
    return 0;

  case WM_DRAWCLIPBOARD:
    if (monitor)
      monitor->HandleClipboardUpdate();
    return 0;

  case WM_DESTROY:
    if (monitor)
    {
      monitor->StopMonitoring();
      delete monitor;
    }
    PostQuitMessage(0);
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

  RegisterClass(&wc);

  HWND hwnd = CreateWindowEx(
      0, className, L"Clipboard Monitor", 0, 0, 0, 100, 100, nullptr, nullptr,
      wc.hInstance, nullptr);

  if (hwnd == nullptr)
    return -1;

  MSG msg = {};
  while (GetMessage(&msg, nullptr, 0, 0))
  {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }

  return 0;
}
