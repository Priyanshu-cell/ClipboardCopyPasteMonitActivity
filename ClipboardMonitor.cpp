#include "ClipboardMonitor.h"
#include <iostream>
#include <filesystem>
#include <string>
#include <windows.h>
#include <ShlObj.h> // For handling file paths,retrieve the path for folders such as Documents
#include <thread> //background tasks, clipboard monitoring without interrupting main projgram
#include <atomic> //a flag to start or stop the clipboard monitoring process

const std::wstring ROOT_FOLDER = L"C:\\Users\\priya\\Desktop\\projects\\clipboardCopyPaste\\root"; //where we want restriction, wide string data type must

const wchar_t *filePath = L"example.txt";

std::atomic<bool> monitoringActive(true); // chatgpt suggestion :( Flag to control monitoring 

ClipboardMonitor::ClipboardMonitor(HWND hwnd) : hwnd(hwnd) {}

void ClipboardMonitor::StartMonitoring()
{
  if (SetClipboardViewer(hwnd) == nullptr)
  {
    std::wcerr << L"Failed to register as clipboard viewer." << std::endl;
    return;
  }
  std::wcout << L"Clipboard monitoring started." << std::endl;

  try
  {
    std::wcout << L"Starting directory watch in a new thread..." << std::endl;
    std::thread(&ClipboardMonitor::WatchRootFolder, this).detach();
    std::wcout << L"Directory watch thread started." << std::endl;
  }
  catch (const std::exception &e)
  {
    std::wcerr << L"Error starting the directory watch thread: " << e.what() << std::endl;
  }
}

void ClipboardMonitor::StopMonitoring()
{
  HWND nextClipboardViewer = GetWindow(hwnd, GW_HWNDNEXT);

  if (nextClipboardViewer)
  {
    if (ChangeClipboardChain(hwnd, nextClipboardViewer) != 0)
    {
      std::wcerr << L"Failed to remove from clipboard viewer chain." << std::endl;
    }
  }
  else
  {
    std::wcerr << L"Failed to retrieve next clipboard viewer." << std::endl;
  }

  std::wcout << L"Clipboard monitoring stopped." << std::endl;
}

void ClipboardMonitor::HandleClipboardUpdate()
{
  try
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
          UINT fileCount = DragQueryFile(hDropData, 0xFFFFFFFF, nullptr, 0); // Get number of files

          for (UINT i = 0; i < fileCount; ++i)
          {
            wchar_t filePath[MAX_PATH];
            if (DragQueryFile(hDropData, i, filePath, MAX_PATH))
            {
              std::wcout << L"Copied file path: " << filePath << std::endl;
              std::wstring filePathStr(filePath);

              // Block copy-paste if the file is within the root folder
              if (filePathStr.find(ROOT_FOLDER) == 0)
              {
                std::wcout << L"File is within the root folder. Paste is blocked." << std::endl;
                MessageBox(nullptr, L"Copy-paste is blocked! You cannot copy files from the root folder.", L"Blocked", MB_OK);

                // Attempt to delete the pasted file, check if it's locked before deleting
                if (!DeleteFileW(filePathStr.c_str()))
                {
                  DWORD dwError = GetLastError();
                  std::wcerr << L"Failed to delete file: " << filePath << L", error code: " << dwError << std::endl;
                }
              }
              else
              {
                std::wcout << L"File is outside the root folder. Paste is allowed." << std::endl;
              }
            }
          }
          DragFinish(hDropData);
        }
        CloseClipboard(); // Always close after accessing clipboard
      }
    }
  }
  catch (const std::exception &e)
  {
    std::wcerr << L"Error handling clipboard update: " << e.what() << std::endl;
  }
  catch (...)
  {
    std::wcerr << L"Unknown error handling clipboard update." << std::endl;
  }
}

void ClipboardMonitor::WatchRootFolder()
{
  std::thread([this]()
              {
        HANDLE hDir = CreateFileW(
            ROOT_FOLDER.c_str(),
            FILE_LIST_DIRECTORY,
            FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
            nullptr,
            OPEN_EXISTING,
            FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED,
            nullptr);

        if (hDir == INVALID_HANDLE_VALUE)
        {
            std::wcerr << L"Failed to open directory for monitoring." << std::endl;
            return;
        }

        char buffer[1024];
        DWORD bytesReturned;
        FILE_NOTIFY_INFORMATION* pNotify;
        OVERLAPPED overlapped = {};
        overlapped.hEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);

        if (!overlapped.hEvent)
        {
            std::wcerr << L"Failed to create event for overlapped I/O." << std::endl;
            CloseHandle(hDir);
            return;
        }

        while (monitoringActive)
        {
            if (ReadDirectoryChangesW(
                    hDir, buffer, sizeof(buffer), TRUE,
                    FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME,
                    &bytesReturned, &overlapped, nullptr))
            {
                DWORD dwWaitResult = WaitForSingleObject(overlapped.hEvent, INFINITE);

                if (dwWaitResult == WAIT_OBJECT_0)
                {
                    pNotify = reinterpret_cast<FILE_NOTIFY_INFORMATION*>(buffer);
                    do
                    {
                        std::wstring fileName(pNotify->FileName, pNotify->FileNameLength / sizeof(WCHAR));

                        if (pNotify->Action == FILE_ACTION_ADDED)
                        {
                            std::wcout << L"Paste detected in root folder: " << fileName << std::endl;
                            MessageBoxW(nullptr, L"Pasting into the root folder is blocked.", L"Blocked", MB_OK);

                            std::wstring fullPath = ROOT_FOLDER + L"\\" + fileName;
                            DeleteFileW(fullPath.c_str());
                        }

                        pNotify = pNotify->NextEntryOffset ?
                            reinterpret_cast<FILE_NOTIFY_INFORMATION*>(reinterpret_cast<LPBYTE>(pNotify) + pNotify->NextEntryOffset) :
                            nullptr;
                    } while (pNotify);
                }
                else
                {
                    std::wcerr << L"Wait failed, error code: " << GetLastError() << std::endl;
                    break;
                }
            }
            else
            {
                std::wcerr << L"ReadDirectoryChangesW failed, error code: " << GetLastError() << std::endl;
                break;
            }

            ResetEvent(overlapped.hEvent);
        }

        CloseHandle(overlapped.hEvent);
        CloseHandle(hDir); })
      .detach();
}
