#include "ClipboardMonitor.h"
#include <iostream>
#include <string>
#include <windows.h>
#include <ShlObj.h> // For handling file paths

const std::wstring ROOT_FOLDER = L"C:\\Users\\priya\\Desktop\\projects\\clipboardCopyPaste\\root";

ClipboardMonitor::ClipboardMonitor(HWND hwnd) : hwnd(hwnd) {}

void ClipboardMonitor::StartMonitoring()
{
  // Register as clipboard viewer
  SetClipboardViewer(hwnd);
  std::cout << "Clipboard monitoring started." << std::endl;
}

void ClipboardMonitor::StopMonitoring()
{
  // Remove from clipboard viewer chain
  ChangeClipboardChain(hwnd, nullptr);
  std::cout << "Clipboard monitoring stopped." << std::endl;
}

void ClipboardMonitor::HandleClipboardUpdate()
{
  if (IsClipboardFormatAvailable(CF_TEXT))
  {
    if (OpenClipboard(nullptr))
    {
      HANDLE hData = GetClipboardData(CF_TEXT);
      if (hData)
      {
        char *pszText = static_cast<char *>(GlobalLock(hData));
        if (pszText)
        {
          std::string text(pszText);
          GlobalUnlock(hData);
          std::cout << "Copied text: " << text << std::endl;
        }
      }
      CloseClipboard();
    }
  }
  else if (IsClipboardFormatAvailable(CF_HDROP))
  {
    if (OpenClipboard(nullptr))
    {
      HANDLE hDrop = GetClipboardData(CF_HDROP);
      if (hDrop)
      {
        HDROP hDropData = static_cast<HDROP>(hDrop);
        wchar_t filePath[MAX_PATH];

        if (DragQueryFile(hDropData, 0, filePath, MAX_PATH))
        {
          std::wcout << L"Copied file path: " << filePath << std::endl;
          std::wstring filePathStr(filePath);

          // Block copy-paste if the file is within the root folder
          if (filePathStr.find(ROOT_FOLDER) == 0)
          {
            std::wcout << L"File is within the root folder. Paste is blocked." << std::endl;
            MessageBox(nullptr, L"Copy-paste is blocked! You cannot copy files from the root folder.", L"Blocked", MB_OK);
          }
          else
          {
            std::wcout << L"File is outside the root folder. Paste is allowed." << std::endl;
          }
        }
        DragFinish(hDropData);
      }
      CloseClipboard();
    }
  }
}
