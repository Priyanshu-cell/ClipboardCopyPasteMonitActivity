# Clipboard Copy Paste and Monitor Activity

## Programming Language: C++

This project monitors clipboard activity and restricts certain copying actions, specifically preventing files from being copied from a designated **root** folder. It also logs any copied text to the console.

## Features

1. **Console the Result of Copied Text**: Logs any text copied to the clipboard to the console.
2. **Root Folder Setup**: Creates a folder named **root** in the main project directory, **clipboardCopyPaste**.
3. **Restrict File Copy from Root**: If a file is copied from the **root** folder, the program will prevent it from being pasted.
4. **Duplicate Prevention in Root**: If a file already exists in the **root** folder, copying it again is blocked.

---

## Project Folder Structure

Here’s the recommended folder structure:


---

## Setup Steps

### Step 1: Create the Folder Structure
1. Create the main project folder:
   - Name it **clipboardCopyPaste**.

2. Inside **clipboardCopyPaste**, create:
   - A folder named **root**.
   - A file named `ClipboardMonitor.h` for the class definition.
   - A file named `ClipboardMonitor.cpp` for the implementation of the clipboard monitoring and file restriction functions(actual logic).
   - A file named `main.cpp` as the main entry point of the program.

---

### Step 2: Coding the Project

#### `ClipboardMonitor.h` - Class Declaration
Define a class `ClipboardMonitor` that will handle clipboard events and restrict file copying actions.
It contains a class name **ClipboardMonitor** with 4 public member function,
1. StartMonitoring();
2. StopMonitoring();
3. HandleClipboardUpdate();
4. WatchRootFolder();
   
with a constructor **ClipboardMonitor(HWND hwnd)**;

and 1 private member for Window handle for the clipboard viewer: <br>
**HWND hwnd;**


#### `ClipboardMonitor.cpp` - Class Implementation
Implement the functions to monitor the clipboard and restrict file copying from the root folder.

#### `main.cpp` - Entry Point

---

### Step 3: Configure Build in VSCode
In clipboardCopyPaste/.vscode/tasks.json:

---

#### Step 4: Run the Program
After building, run the clipboardMonitor.exe executable created in the main project directory. The program should:

Log any text copied to the clipboard.
Prevent pasting files from outside the root folder, as described.

---

