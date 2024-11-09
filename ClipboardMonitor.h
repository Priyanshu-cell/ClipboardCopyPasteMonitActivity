#pragma once // using this because i'm getting error of multiple compilation of .h file
#include <windows.h>

class ClipboardMonitor
{
public:
  ClipboardMonitor(HWND hwnd);  // constructor for viewing clipboard
  void StartMonitoring();       // allowing the object to receive notifications when the clipboard content changes.
  void StopMonitoring();        // same for deletion
  void HandleClipboardUpdate(); // when something changes this is responsible
  void WatchRootFolder();       // restriction for copy-paste in root folder

private:
  HWND hwnd; // this is assign by the upper constructor ClipboardMonitor(HWND hwnd);
};