#pragma once
#include <windows.h>

class ClipboardMonitor
{
public:
  ClipboardMonitor(HWND hwnd);
  void StartMonitoring();
  void StopMonitoring();
  void HandleClipboardUpdate();

private:
  HWND hwnd; // Window handle for the clipboard viewer
};